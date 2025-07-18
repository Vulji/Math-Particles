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

void draw_parametric(std::function<glm::vec2(float)> const& p,
                     int segments = 128,
                     float thickness = 0.003f,
                     const glm::vec4& color = {1,1,1,1})
{

    glm::vec2 prev = p(0.f);

    for (int i = 1; i <= segments; ++i) {
        float t    = float(i) / float(segments);
        glm::vec2 curr = p(t);
        utils::draw_line(prev, curr, thickness, color);
        prev = curr;
    }
}

int main()
{
    gl::init("Parametric Curve");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glm::vec2 center = {0.f, 0.f};
    float     scale  = 0.5f;   


    while (gl::window_is_open())
    {
        glClear(GL_COLOR_BUFFER_BIT);

       draw_parametric(
            [&](float t) -> glm::vec2 {
                
                float θ = t * 2.f * glm::pi<float>();
                
                float x = std::pow(std::sin(θ), 3.f);
                
                float y = (13.f*std::cos(θ)
                         - 5.f*std::cos(2.f*θ)
                         - 2.f*std::cos(3.f*θ)
                         -     std::cos(4.f*θ)
                        ) / 16.f;
                return center + scale * glm::vec2(x, y);
            },
            512,
            0.003f,
            {1,1,1,1}
        );
    }

    return 0;
}

