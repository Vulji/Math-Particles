#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/gtc/constants.hpp>

struct Particle
{
    glm::vec2 position;
    glm::vec2 velocity;
};

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    const float minSpeed = 0.2f;
    const float maxSpeed = 1.0f;

    std::vector<Particle> particles;
    particles.reserve(100);

    for (int i = 0; i < 100; ++i) {
        Particle p;
        p.position = {
            utils::rand(-gl::window_aspect_ratio(), gl::window_aspect_ratio()),
            utils::rand(-1.f, +1.f)
        };
        float angle = utils::rand(0.f, 2.f * glm::pi<float>());
        float speed = utils::rand(minSpeed, maxSpeed);
        p.velocity = glm::vec2(std::cos(angle), std::sin(angle)) * speed;
        particles.push_back(p);
    }

    glm::vec4 color = {1.f, 0.f, 0.f, 1.f};

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& p : particles) {
            p.position += p.velocity * dt;
            if (p.position.x >  gl::window_aspect_ratio())  p.position.x = -gl::window_aspect_ratio();
            if (p.position.x < -gl::window_aspect_ratio())  p.position.x =  gl::window_aspect_ratio();
            if (p.position.y >  1.f)                        p.position.y = -1.f;
            if (p.position.y < -1.f)                        p.position.y =  1.f;
            utils::draw_disk(p.position, 0.05f, color);
        }
    }

    return 0;
}
