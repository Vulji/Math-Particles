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
    
    glm::mat2 M(r, -s);

    float det = glm::determinant(M);
    if (std::abs(det) < 1e-8f) return false;

    glm::vec2 qp = q1 - p1;
    glm::vec2 tu = glm::inverse(M) * qp;

    float t = tu.x;
    float u = tu.y;

    if (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f) {
        intersection = p1 + t * r;
        return true;
    }
    return false;
}

bool segment_circle_intersect(const glm::vec2& p1, const glm::vec2& p2,
                              const glm::vec2& center, float radius,
                              glm::vec2& intersection)
{
    glm::vec2 d = p2 - p1;
    glm::vec2 f = p1 - center;

    float a = glm::dot(d, d);
    float b = 2 * glm::dot(f, d);
    float c = glm::dot(f, f) - radius * radius;

    float discriminant = b*b - 4*a*c;

    if (discriminant < 0) {
 
        return false;
    }

    discriminant = std::sqrt(discriminant);
    float t1 = (-b - discriminant) / (2*a);
    float t2 = (-b + discriminant) / (2*a);

    if (t1 >= 0 && t1 <= 1) {
        intersection = p1 + t1 * d;
        return true;
    }

    if (t2 >= 0 && t2 <= 1) {
        intersection = p1 + t2 * d;
        return true;
    }

    return false; 
}


int main()
{
    gl::init("Particules - circle");
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
    const glm::vec2 segA1 = {utils::rand(-1.f, 1.f), utils::rand(-1.f, 1.f)};
    const glm::vec2 segA2 = {utils::rand(-1.f, 1.f), utils::rand(-1.f, 1.f)};
    glm::vec2 circleCenter = {0.0f, 0.0f};
    float circleRadius = 0.4f;

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

    utils::draw_line(segA1, segA2, 0.01f, {1.f, 1.f, 1.f, 0.5f});
    utils::draw_disk(circleCenter, circleRadius, {0.f, 1.f, 0.f, 0.3f});

auto it = particles.begin();
while (it != particles.end()) {
    glm::vec2 old_position = it->position;
    glm::vec2 displacement = it->velocity * dt;
    glm::vec2 new_position = old_position + displacement;

    glm::vec2 inter;

    bool hit = segment_circle_intersect(old_position, new_position, circleCenter, circleRadius, inter);

    if (hit) {
        float distToHit = glm::distance(old_position, inter);
        float totalDist = glm::length(displacement);
        float distRemaining = totalDist - distToHit;

        glm::vec2 normal = glm::normalize(inter - circleCenter);
        it->velocity = glm::reflect(it->velocity, normal);

        it->position = inter + glm::normalize(it->velocity) * distRemaining;
    } else {
        it->position = new_position;
    }

    if (it->position.x >  gl::window_aspect_ratio())  it->position.x = -gl::window_aspect_ratio();
    if (it->position.x < -gl::window_aspect_ratio())  it->position.x =  gl::window_aspect_ratio();
    if (it->position.y >  1.f)                        it->position.y = -1.f;
    if (it->position.y < -1.f)                        it->position.y =  1.f;

    utils::draw_disk(it->position, initialRadius, startColor);
    ++it;
}
}

    return 0;
}
