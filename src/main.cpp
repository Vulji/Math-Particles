#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/gtc/constants.hpp>

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
    gl::init("Particules – ressort + friction + durée de vie");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    const float minSpeed        = 0.2f;
    const float maxSpeed        = 1.0f;
    const float minMass         = 0.5f;
    const float maxMass         = 2.0f;
    const float springK         = 5.0f;
    const float dragCoefficient = 0.5f;
    const float minLife         = 2.0f;
    const float maxLife         = 5.0f;

    std::vector<Particle> particles;
    particles.reserve(100);

    for (int i = 0; i < 100; ++i) {
        Particle p;
        p.position   = { utils::rand(-gl::window_aspect_ratio(), gl::window_aspect_ratio()),
                         utils::rand(-1.f, 1.f) };
        float angle   = utils::rand(0.f, 2.f * glm::pi<float>());
        float speed   = utils::rand(minSpeed, maxSpeed);
        p.velocity    = glm::vec2(std::cos(angle), std::sin(angle)) * speed;
        p.mass        = utils::rand(minMass, maxMass);
        p.age         = 0.f;
        p.life_time   = utils::rand(minLife, maxLife);
        particles.push_back(p);
    }

    glm::vec4 color = {1.f, 0.f, 0.f, 1.f};

    while (gl::window_is_open())
    {
        float dt    = gl::delta_time_in_seconds();
        glm::vec2 m = gl::mouse_position();

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto it = particles.begin();
        while (it != particles.end()) {
            it->age += dt;
            if (it->age > it->life_time) {
                it = particles.erase(it);
            } else {
                glm::vec2 springForce = springK * (m - it->position);
                glm::vec2 dragForce   = -dragCoefficient * it->velocity;
                glm::vec2 force       = springForce + dragForce;
                glm::vec2 accel       = force / it->mass;
                it->velocity += accel * dt;
                it->position += it->velocity * dt;
                if (it->position.x >  gl::window_aspect_ratio())  it->position.x = -gl::window_aspect_ratio();
                if (it->position.x < -gl::window_aspect_ratio())  it->position.x =  gl::window_aspect_ratio();
                if (it->position.y >  1.f)                        it->position.y = -1.f;
                if (it->position.y < -1.f)                        it->position.y =  1.f;
                utils::draw_disk(it->position, 0.05f, color);
                ++it;
            }
        }
    }

    return 0;
}
