#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    float     mass;
    float     age;
    float     life_time;
};


void spawnParticleInRectangle(std::vector<Particle>& particles,
                              const glm::vec2& min, const glm::vec2& max)
{
    Particle p;
    p.position = { utils::rand(min.x, max.x),
                   utils::rand(min.y, max.y) };
    float ang   = utils::rand(0.f, 2.f * glm::pi<float>());
    float spd   = utils::rand(0.2f, 1.0f);
    p.velocity  = glm::vec2(std::cos(ang), std::sin(ang)) * spd;
    p.mass      = utils::rand(0.5f, 2.0f);
    p.age       = 0.f;
    p.life_time = utils::rand(2.f, 5.f);
    particles.emplace_back(p);
}

int main()
{
    gl::init("Particles!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);


    const glm::vec2 rectMin      = {-0.5f, -0.5f};
    const glm::vec2 rectMax      = { 0.5f,  0.5f};
    const glm::vec2 circleCenter = { 0.f,    0.f};
    const float     circleRadius = 0.4f;
    const float     particleR    = 0.02f;
    const glm::vec4 startColor   = {1.f, 1.f, 1.f, 1.f};
    const glm::vec4 rectColor    = {1.f, 1.f, 0.f, 0.5f};

    std::vector<Particle> particles;
    particles.reserve(500);


    for (int i = 0; i < 100; ++i)
        spawnParticleInRectangle(particles, rectMin, rectMax);

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);




        float thickness = 0.005f;
        glm::vec2 bl = { rectMin.x, rectMin.y };
        glm::vec2 br = { rectMax.x, rectMin.y };
        glm::vec2 tr = { rectMax.x, rectMax.y };
        glm::vec2 tl = { rectMin.x, rectMax.y };
        utils::draw_line(bl, br, thickness, rectColor);
        utils::draw_line(br, tr, thickness, rectColor);
        utils::draw_line(tr, tl, thickness, rectColor);
        utils::draw_line(tl, bl, thickness, rectColor);

        spawnParticleInRectangle(particles, rectMin, rectMax);

        for (auto& p : particles)
        {
            glm::vec2 oldPos = p.position;
            glm::vec2 newPos = oldPos + p.velocity * dt;
            glm::vec2 hit;

            float ar = gl::window_aspect_ratio();
            if (p.position.x >  ar) p.position.x = -ar;
            if (p.position.x < -ar) p.position.x =  ar;
            if (p.position.y >  1.f) p.position.y = -1.f;
            if (p.position.y < -1.f) p.position.y =  1.f;

            utils::draw_disk(p.position, particleR, startColor);
        }
    }

    return 0;
}
