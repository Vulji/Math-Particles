#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "poisson_disc.hpp"
#include <vector>


struct Particle {
    glm::vec2 position;
    float     t;
    float     mass;
    float     elapsed;
    float     age;
    float     life_time;
};

void spawnParticleInDiskAnalytic(std::vector<Particle>& particles,
                                 const glm::vec2& center,
                                 float R)
{
    float theta = utils::rand(0.f, 2.f * glm::pi<float>());

    float u = utils::rand(0.f, 1.f);
    float r = R * std::sqrt(u);

    glm::vec2 pos = center + glm::vec2(std::cos(theta),
                                       std::sin(theta)) * r;

    Particle p;
    p.position  = pos;
    p.elapsed   = utils::rand(0.f, 1.f);
    p.mass      = utils::rand(0.5f, 2.0f);
    p.age       = 0.f;
    p.life_time = utils::rand(2.f, 5.f);
    particles.emplace_back(p);
}

inline glm::vec2 bezier3_bernstein(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, float t)
{
    float u = 1.f - t;
    return p0 * (u*u*u)
         + p1 * (3.f * u*u * t)
         + p2 * (3.f * u * t*t)
         + p3 * (t*t*t);
}

void draw_parametric(std::function<glm::vec2(float)> const& p, int segments = 128, float thickness = 0.003f, glm::vec4 color = {1,1,1,1})
{
    glm::vec2 prev = p(0.f);
    for (int i = 1; i <= segments; ++i)
    {
        float t = float(i) / float(segments);
        glm::vec2 curr = p(t);
        utils::draw_line(prev, curr, thickness, color);
        prev = curr;
    }
}

void draw_bezier3(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, int segments = 256, glm::vec4 color = {0,0,1,1})
{
    draw_parametric([&](float t){ return bezier3_bernstein(p0, p1, p2, p3, t); }, segments, 0.004f, color);
}

int main()
{
    gl::init("Interactive Bézier ");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    std::vector<glm::vec2> curve = {
        {-0.6f,  0.7f},
        {-0.2f, -0.2f},
        { 0.3f,  0.8f},
        { 0.8f,  0.3f}
    };

    int grabbedCurve = 3;
    int grabbedIndex = -1;
    const float pickRadius = 0.03f;

    const int TOTAL_PARTICLES = 50;
    std::vector<Particle> particles;
    particles.reserve(TOTAL_PARTICLES);

    for (int i = 0; i < TOTAL_PARTICLES; ++i) {
        Particle p;
        p.t         = float(i) / float(TOTAL_PARTICLES - 1);
        p.elapsed   = utils::rand(0.f, 1.f);
        p.mass      = utils::rand(0.5f, 2.0f);
        p.age       = 0.f;
        p.life_time = utils::rand(2.f, 5.f);
        particles.emplace_back(p);
    }

    const float beatFreq   = 1.5f;
    const float beatAmp    = 0.3f;
    const float baseRadius = 0.02f;

    while (gl::window_is_open())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 m = gl::mouse_position();

        if (grabbedIndex < 0) {
            for (int i = 0; i < 4; ++i) {
                if (glm::length(curve[i] - m) < pickRadius) {
                    grabbedIndex = i;
                    break;
                }
            }
        }
        else {
            if (glm::length(curve[grabbedIndex] - m) < pickRadius)
                curve[grabbedIndex] = m;
            else
                grabbedIndex = -1;
        }

        draw_bezier3(curve[0], curve[1], curve[2], curve[3], 256, {0,0,1,1});
        for (auto& p : curve) utils::draw_disk(p, pickRadius*0.7f, {0,0,1,1});

        for (auto& pt : particles) {
            pt.elapsed += gl::delta_time_in_seconds();
            glm::vec2 pos = bezier3_bernstein(
                curve[0], curve[1], curve[2], curve[3], pt.t
            );
            float scale = 1.f + beatAmp
                * std::sin(2.f * glm::pi<float>() * beatFreq * pt.elapsed);
            float r = baseRadius * scale;
            utils::draw_disk(pos, r, {1,1,1,1});
        }
    }

    return 0;
}

