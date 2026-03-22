#pragma once
#include <glm/glm.hpp>
#include <vector>

using glm::vec2;
using glm::vec3;
using std::vector;

struct Ball {
    vec2 position;
    vec2 velocity;
    float radius;
    float mass;
    float restitution;
    vec3 color;
};

class PhysicsEngine {
public:
    PhysicsEngine();

    void addBall(
        const vec2& pos,
        const vec2& vel,
        float rad,
        float mass = 1.0f,
        float restitution = 0.98f,
        const vec3& color = {0.85f, 0.32f, 0.26f}
    );
    void setBounds(const vec2& minBounds, const vec2& maxBounds);
    void update(float deltaTime);
    const vector<Ball>& getBalls() const;

private:
    vector<Ball> balls;
    vec2 boundsMin;
    vec2 boundsMax;
    float fixedTimeStep;
    float accumulatedTime;

    void step(float deltaTime);
    void handleBoundaryCollision(Ball& ball);
    void handleCollisions();
};
