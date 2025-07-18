#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


struct Particle {
    glm::vec2 position;
    float     mass;
    float elapsed;
    float     age;
    float     life_time;
};


void spawnParticleInParallelogram(std::vector<Particle>& particles,
                                  const glm::vec2& offset,
                                  const glm::vec2& v1,
                                  const glm::vec2& v2)
{
    float u = utils::rand(0.f, 1.f);
    float v = utils::rand(0.f, 1.f);

    glm::vec2 pos = offset + u * v1 + v * v2;

    Particle p;
    p.position  = pos;
    float ang    = utils::rand(0.f, 2.f * glm::pi<float>());
    float spd    = utils::rand(0.2f, 1.0f);
    p.elapsed   = 0.f + utils::rand(0.f, 1.f);
    p.mass       = utils::rand(0.5f, 2.0f);
    p.age        = 0.f;
    p.life_time  = utils::rand(2.f, 5.f);

    particles.emplace_back(p);
}

int main()
{
    gl::init("Particles!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);


    glm::vec2 offset = {-0.5f, -0.5f};
    glm::vec2 side1  = { 1.0f,  0.0f};
    glm::vec2 side2  = { 0.5f,  0.8f};


    const int TOTAL_PARTICLES = 1500;

    std::vector<Particle> particles;
    particles.reserve(TOTAL_PARTICLES);


    for (int i = 0; i < TOTAL_PARTICLES; ++i)
        spawnParticleInParallelogram(particles, offset, side1, side2);


    while (gl::window_is_open())
    {
    float dt = gl::delta_time_in_seconds();

    glClear(GL_COLOR_BUFFER_BIT);


        float thickness = 0.005f;
        glm::vec2 p0 = offset;
        glm::vec2 p1 = offset + side1;
        glm::vec2 p2 = offset + side1 + side2;
        glm::vec2 p3 = offset + side2;
        glm::vec4 rectColor = {1,1,0,0.5f};
        utils::draw_line(p0,p1,thickness,rectColor);
        utils::draw_line(p1,p2,thickness,rectColor);
        utils::draw_line(p2,p3,thickness,rectColor);
        utils::draw_line(p3,p0,thickness,rectColor);

        const float beatFreq   = 1.5f;
        const float beatAmp    = 0.3f;
        const float baseRadius = 0.02f;

        for (auto& p : particles)
        {
            p.elapsed += dt;
            float scale = 1.0f + beatAmp * std::sin(2.f * glm::pi<float>() * beatFreq * p.elapsed);
            float r     = baseRadius * scale;
            utils::draw_disk(p.position, r, {1,1,1,1});
        }
    }

    return 0;
}

