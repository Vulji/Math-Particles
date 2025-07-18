#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


struct Particle {
    glm::vec2 position;
    float     mass;
    float     elapsed;
    float     age;
    float     life_time;
};


void spawnParticleInDiskRejection(std::vector<Particle>& particles,
                                  const glm::vec2& center,
                                  float R)
{
    glm::vec2 pos;

    do {
        float x = utils::rand(-R, R);
        float y = utils::rand(-R, R);
        pos = center + glm::vec2(x, y);
    } while (glm::dot(pos - center, pos - center) > R*R);

    Particle p;
    p.position  = pos;
    p.elapsed   = utils::rand(0.f, 1.f);
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
    
    glm::vec2 center = glm::vec2(0.f, 0.f);
    float     R      = 0.8f;

    const int TOTAL_PARTICLES = 1500;
    std::vector<Particle> particles;
    particles.reserve(TOTAL_PARTICLES);

    for (int i = 0; i < TOTAL_PARTICLES; ++i)
        spawnParticleInDiskRejection(particles, center, R);

    
    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();
        glClear(GL_COLOR_BUFFER_BIT);

        const float beatFreq   = 1.5f;
        const float beatAmp    = 0.3f;
        const float baseRadius = 0.02f;

        for (auto& p : particles)
        {
            p.elapsed += dt;
            float scale = 1.f + beatAmp * std::sin(2.f * glm::pi<float>() * beatFreq * p.elapsed);
            float r     = baseRadius * scale;
            utils::draw_disk(p.position, r, {1,1,1,1});
        }
    }

    return 0;
}

