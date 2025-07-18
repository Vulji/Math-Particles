#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "poisson_disc.hpp"


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


int main()
{
    gl::init("Particles!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    glm::vec2 regionSize = {1.f, 1.f};
    float     radius     = 0.05f;    
    int       tries      = 30;       
    
    auto samplePoints = PoissonDisc::GeneratePoints(radius, regionSize, tries);


    std::vector<Particle> particles;
    particles.reserve(samplePoints.size());
    for (auto& sp : samplePoints) {
        Particle p;
        p.position  = sp;                              
        p.elapsed   = utils::rand(0.f, 1.f);           
        p.mass      = utils::rand(0.5f, 2.0f);
        p.age       = 0.f;
        p.life_time = utils::rand(2.f, 5.f);
        particles.emplace_back(p);
    }

    
    glm::vec2 offset = {-0.5f, -0.5f};

    
    const float beatFreq   = 1.5f;   
    const float beatAmp    = 0.3f;   
    const float baseRadius = 0.02f;  

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();
        glClear(GL_COLOR_BUFFER_BIT);

        
        utils::draw_line(offset + glm::vec2(0,0), offset + glm::vec2(1,0), 0.003f, {1,1,0,0.5f});
        utils::draw_line(offset + glm::vec2(1,0), offset + glm::vec2(1,1), 0.003f, {1,1,0,0.5f});
        utils::draw_line(offset + glm::vec2(1,1), offset + glm::vec2(0,1), 0.003f, {1,1,0,0.5f});
        utils::draw_line(offset + glm::vec2(0,1), offset + glm::vec2(0,0), 0.003f, {1,1,0,0.5f});

        
        for (auto& p : particles)
        {
            p.elapsed += dt;
            float scale = 1.f + beatAmp
                * std::sin(2.f * glm::pi<float>() * beatFreq * p.elapsed);
            float r = baseRadius * scale;

            glm::vec2 posNDC = offset + p.position;
            utils::draw_disk(posNDC, r, {1,1,1,1});
        }
    }

    return 0;
}

