#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/color_space.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Particle
{
    glm::vec2 position;
    glm::vec2 velocity;
    float     mass;
    float     age;
    float     life_time;
};

int main()
{
    gl::init("Particules – durée de vie + couleur/taille");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    const float minSpeed      = 0.2f;
    const float maxSpeed      = 1.0f;
    const float minMass       = 0.5f;
    const float maxMass       = 2.0f;
    const float minLife       = 2.0f;
    const float maxLife       = 5.0f;
    const float initialRadius = 0.05f;
    const glm::vec4 startColor = {1.f, 0.f, 0.f, 1.f};
    const glm::vec4 endColor   = {0.f, 0.f, 1.f, 1.f};

    std::vector<Particle> particles;
    particles.reserve(100);

    for (int i = 0; i < 100; ++i) {
        Particle p;
        p.position = {
            utils::rand(-gl::window_aspect_ratio(), gl::window_aspect_ratio()),
            utils::rand(-1.f, 1.f)
        };
        float angle = utils::rand(0.f, 2.f * glm::pi<float>());
        float speed = utils::rand(minSpeed, maxSpeed);
        p.velocity  = glm::vec2(std::cos(angle), std::sin(angle)) * speed;
        p.mass      = utils::rand(minMass, maxMass);
        p.age       = 0.f;
        p.life_time = utils::rand(minLife, maxLife);
        particles.push_back(p);
    }

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto it = particles.begin();
        while (it != particles.end()) {
            it->age += dt;
            if (it->age > it->life_time) {
                it = particles.erase(it);
            } else {
                it->position += it->velocity * dt;
                float tColor = glm::clamp(it->age / it->life_time, 0.f, 1.f);
                glm::vec4 color = glm::mix(startColor, endColor, tColor);
                float tShrink = glm::clamp((it->age - (it->life_time - 2.f)) / 2.f, 0.f, 1.f);
                float radius = initialRadius * (1.f - tShrink);
                if (it->position.x >  gl::window_aspect_ratio())  it->position.x = -gl::window_aspect_ratio();
                if (it->position.x < -gl::window_aspect_ratio())  it->position.x =  gl::window_aspect_ratio();
                if (it->position.y >  1.f)                        it->position.y = -1.f;
                if (it->position.y < -1.f)                        it->position.y =  1.f;
                utils::draw_disk(it->position, radius, color);
                ++it;
            }
        }
    }

    return 0;
}
