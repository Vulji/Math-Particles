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

inline glm::vec2 bezier1_bernstein(const glm::vec2& p0, const glm::vec2& p1, float t)
{
    float u = 1.f - t;
    return p0 * u + p1 * t;
}

inline glm::vec2 bezier2_bernstein(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, float t)
{
    float u = 1.f - t;
    return p0 * (u*u)
         + p1 * (2.f * u * t)
         + p2 * (t*t);
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

void draw_bezier1(const glm::vec2& p0, const glm::vec2& p1, int segments = 64, glm::vec4 color = {1,0,0,1})
{
    draw_parametric([&](float t){ return bezier1_bernstein(p0, p1, t); }, segments, 0.004f, color);
}

void draw_bezier2(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, int segments = 128, glm::vec4 color = {0,1,0,1})
{
    draw_parametric([&](float t){ return bezier2_bernstein(p0, p1, p2, t); }, segments, 0.004f, color);
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

    std::vector<glm::vec2> curve1 = {{-0.8f, -0.5f}, {-0.3f,  0.2f}};
    std::vector<glm::vec2> curve2 = {{ 0.1f, -0.4f}, { 0.4f,  0.6f}, { 0.7f, -0.1f}};
    std::vector<glm::vec2> curve3 = {{-0.6f,  0.7f}, {-0.2f, -0.2f}, { 0.3f,  0.8f}, {0.8f, 0.3f}};

    int grabbedCurve = -1;
    int grabbedIndex = -1;
    float pickRadius = 0.03f;

    while (gl::window_is_open())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 m = gl::mouse_position();

        if (grabbedCurve < 0)
        {
            auto tryHover = [&](auto& c, int cid){
                for (int i = 0; i < (int)c.size(); ++i)
                    if (glm::length(c[i] - m) < pickRadius)
                    {
                        grabbedCurve = cid;
                        grabbedIndex = i;
                        return true;
                    }
                return false;
            };
            if      (tryHover(curve1,1)){}
            else if (tryHover(curve2,2)){}
            else if (tryHover(curve3,3)){}
        }
        else
        {
            auto& c = (grabbedCurve==1?curve1:grabbedCurve==2?curve2:curve3);
            if (glm::length(c[grabbedIndex] - m) < pickRadius)
                c[grabbedIndex] = m;
            else
                grabbedCurve = grabbedIndex = -1;
        }

        draw_bezier1(curve1[0], curve1[1], 64,   {1,0,0,1});
        draw_bezier2(curve2[0], curve2[1], curve2[2], 128, {0,1,0,1});
        draw_bezier3(curve3[0], curve3[1], curve3[2], curve3[3], 256, {0,0,1,1});

        for (auto& p : curve1) utils::draw_disk(p, pickRadius*0.7f, {1,0,0,1});
        for (auto& p : curve2) utils::draw_disk(p, pickRadius*0.7f, {0,1,0,1});
        for (auto& p : curve3) utils::draw_disk(p, pickRadius*0.7f, {0,0,1,1});
    }

    return 0;
}

