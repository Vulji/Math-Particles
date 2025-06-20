#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

float bounceEase(float t)
{
    if (t < 4.0f/11.0f)        return (121.0f*t*t)/16.0f;
    else if (t < 8.0f/11.0f)   return (363.0f/40.0f*t*t) - (99.0f/10.0f*t) + 17.0f/5.0f;
    else if (t < 9.0f/10.0f)   return (4356.0f/361.0f*t*t) - (35442.0f/1805.0f*t) + 16061.0f/1805.0f;
    else                       return (54.0f/5.0f*t*t) - (513.0f/25.0f*t) + 268.0f/25.0f;
}

float easeInOutPower(float t, float power)
{
    if (t < 0.5f) return 0.5f * std::pow(2.0f*t, power);
    else           return 1.0f - 0.5f * std::pow(2.0f*(1.0f - t), power);
}

struct Particle
{
    glm::vec2 position;
    glm::vec2 velocity;
    float     mass;
    float     age;
    float     life_time;
};

bool segment_intersect(const glm::vec2& p1, const glm::vec2& p2,
                               const glm::vec2& q1, const glm::vec2& q2,
                               glm::vec2& intersection)
        {
            glm::vec2 r = p2 - p1;
            glm::vec2 s = q2 - q1;
            float denom = r.x * s.y - r.y * s.x;
            if (std::abs(denom) < 1e-8f) return false; 

            glm::vec2 qp = q1 - p1;
            float t = (qp.x * s.y - qp.y * s.x) / denom;
            float u = (qp.x * r.y - qp.y * r.x) / denom;

            if (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f) {
                intersection = p1 + t * r;
                return true;
            }
            return false;
        }

int main()
{
    gl::init("Particules – easing");
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

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 segA1 = {-gl::window_aspect_ratio(), 0.f};
        glm::vec2 segA2 = {gl::window_aspect_ratio(), 0.f};
        glm::vec2 segB1 = {0.f, -1.f};
        glm::vec2 segB2 = gl::mouse_position();

        utils::draw_line(segA1, segA2, 0.01f, {1.f, 1.f, 1.f, 0.5f});
        utils::draw_line(segB1, segB2, 0.01f, {1.f, 1.f, 1.f, 0.5f});

        glm::vec2 inter;
        if (segment_intersect(segA1, segA2, segB1, segB2, inter)) {
            utils::draw_disk(inter, 0.03f, {1.f, 0.f, 0.f, 1.f});
        }


        auto it = particles.begin();
        while (it != particles.end()) {
            it->age += dt;
            if (it->age > it->life_time) {
                it = particles.erase(it);
            } else {
                it->position += it->velocity * dt;
                float tColorRaw = glm::clamp(it->age / it->life_time, 0.f, 1.f);
                float tColorEased = easeInOutPower(tColorRaw, 2.f);
                glm::vec4 color = glm::mix(startColor, endColor, tColorEased);
                float tShrinkRaw = glm::clamp((it->age - (it->life_time - 2.f)) / 2.f, 0.f, 1.f);
                float tShrinkBounced = bounceEase(tShrinkRaw);
                float radius = initialRadius * (1.f - tShrinkBounced);
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
