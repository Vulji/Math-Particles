#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <functional>
#include <vector>
#include <limits>
#include <glm/gtx/norm.hpp>

inline glm::vec2 bezier3_bernstein(const glm::vec2& p0,
                                   const glm::vec2& p1,
                                   const glm::vec2& p2,
                                   const glm::vec2& p3,
                                   float t)
{
    float u = 1.f - t;
    return p0*(u*u*u)
         + p1*(3.f*u*u*t)
         + p2*(3.f*u*t*t)
         + p3*(t*t*t);
}

inline glm::vec2 bezier3_tangent(const glm::vec2& p0,
                                 const glm::vec2& p1,
                                 const glm::vec2& p2,
                                 const glm::vec2& p3,
                                 float t)
{
    float u = 1.f - t;
    return 3.f*(u*u)*(p1 - p0)
         + 6.f*u*t*(p2 - p1)
         + 3.f*(t*t)*(p3 - p2);
}

float findClosestT(const glm::vec2& p0,
                   const glm::vec2& p1,
                   const glm::vec2& p2,
                   const glm::vec2& p3,
                   const glm::vec2& P)
{
    const int SAMPLES = 20;
    float bestT = 0.f;
    float bestD = std::numeric_limits<float>::infinity();
    for (int i = 0; i <= SAMPLES; ++i) {
        float t = float(i)/float(SAMPLES);
        float d = glm::distance2(bezier3_bernstein(p0,p1,p2,p3,t), P);
        if (d < bestD) { bestD = d; bestT = t; }
    }
    float t = bestT;
    const float alpha = 0.2f;
    for (int iter = 0; iter < 30; ++iter) {
        glm::vec2 B = bezier3_bernstein(p0,p1,p2,p3,t);
        glm::vec2 T = bezier3_tangent(p0,p1,p2,p3,t);
        float grad = 2.f * glm::dot(B - P, T);
        t = glm::clamp(t - alpha * grad, 0.f, 1.f);
    }
    return t;
}

void draw_parametric(std::function<glm::vec2(float)> const& p,
                     int segments = 256,
                     float thickness = 0.004f,
                     glm::vec4 color = {1,1,1,1})
{
    glm::vec2 prev = p(0.f);
    for (int i = 1; i <= segments; ++i) {
        float t = float(i)/float(segments);
        glm::vec2 curr = p(t);
        utils::draw_line(prev, curr, thickness, color);
        prev = curr;
    }
}

void draw_bezier3(const glm::vec2& p0,
                  const glm::vec2& p1,
                  const glm::vec2& p2,
                  const glm::vec2& p3,
                  int segments = 256,
                  glm::vec4 color = {1,1,1,1})
{
    draw_parametric(
        [&](float t){ return bezier3_bernstein(p0,p1,p2,p3,t); },
        segments, 0.004f, color
    );
}

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    float     elapsed;
    float     mass;
    float     age;
    float     life_time;
};

int main()
{
    gl::init("Force Field Bézier");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    std::vector<glm::vec2> curve = {
        {-0.6f,  0.7f},
        {-0.2f, -0.2f},
        { 0.3f,  0.8f},
        { 0.8f,  0.3f}
    };
    int grabbedIndex = -1;
    const float pickRadius = 0.03f;
    const int TOTAL_PARTICLES = 800;
    std::vector<Particle> particles;
    particles.reserve(TOTAL_PARTICLES);
    float ar = gl::window_aspect_ratio();
    for (int i = 0; i < TOTAL_PARTICLES; ++i) {
        Particle p;
        p.position   = { utils::rand(-ar, ar), 1.1f };
        p.velocity   = { 0.f, 0.f };
        p.elapsed    = utils::rand(0.f, 1.f);
        p.mass       = utils::rand(0.5f, 2.0f);
        p.age        = 0.f;
        p.life_time  = utils::rand(2.f, 5.f);
        particles.emplace_back(p);
    }

    const glm::vec2 gravity   = { 0.f, -0.5f };
    const float     infRadius  = 0.1f;
    const float     forceK     = 25.0f;
    const float     beatFreq   = 1.5f;
    const float     beatAmp    = 0.3f;
    const float     baseRadius = 0.02f;

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 M = gl::mouse_position();
        if (grabbedIndex < 0) {
            for (int i = 0; i < 4; ++i) {
                if (glm::length(curve[i] - M) < pickRadius) {
                    grabbedIndex = i;
                    break;
                }
            }
        } else {
            if (glm::length(curve[grabbedIndex] - M) < pickRadius)
                curve[grabbedIndex] = M;
            else
                grabbedIndex = -1;
        }

        draw_bezier3(curve[0],curve[1],curve[2],curve[3],256,{1,1,1,1});
        for (auto& cp : curve)
            utils::draw_disk(cp, pickRadius*0.7f, {1,0,1,1});

        for (auto& pt : particles) {
            pt.velocity += gravity * dt;
            float tClosest = findClosestT(
                curve[0],curve[1],curve[2],curve[3], pt.position);
            glm::vec2 Pc = bezier3_bernstein(
                curve[0],curve[1],curve[2],curve[3], tClosest);
            glm::vec2 diff = pt.position - Pc;
            float d = glm::length(diff);
            if (d < infRadius && d > 1e-4f) {
                glm::vec2 n = diff / d;
                float mag = forceK * (1.f - d / infRadius);
                pt.velocity += n * mag * dt;
            }
            pt.position += pt.velocity * dt;
            if (pt.position.y < -1.1f) {
                pt.position   = { utils::rand(-ar, ar), 1.1f };
                pt.velocity   = {0,0};
                pt.elapsed    = utils::rand(0.f, 1.f);
                pt.mass       = utils::rand(0.5f, 2.0f);
                pt.age        = 0.f;
                pt.life_time  = utils::rand(2.f, 5.f);
            }
            pt.elapsed += dt;
            float scale = 1.f + beatAmp * std::sin(2.f * glm::pi<float>() * beatFreq * pt.elapsed);
            float r = baseRadius * scale;
            utils::draw_disk(pt.position, r, {1,1,1,1});
        }
    }

    return 0;
}
