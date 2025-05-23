#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>

struct Particle
{
    glm::vec2 position;
    // glm::vec2 velocity;
    // float size;
    // float life_time;
    // float age;
};

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    Particle pa;
    pa.position = {utils::rand(0.5f, 1)};

    std::vector<Particle> particles;

for (int i = 0; i < 100; ++i) {
    Particle p;
    p.position = glm::vec2(utils::rand(-gl::window_aspect_ratio(), gl::window_aspect_ratio()), utils::rand(-1, +1));
    particles.push_back(p);
}

    glm::vec4 color = {1.f, 0.f, 0.f, 1.f};

    // TODO: create an array of particles

     while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(int i = 0; i < particles.size(); ++i) {
            Particle& p = particles[i];
            p.position += glm::vec2(utils::rand(-0.01f, 0.01f), utils::rand(-0.01f, 0.01f));
            utils::draw_disk(p.position, 0.05f, color);
        }
    }

    

}
