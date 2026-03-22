#include "physics.h"
#include <algorithm>
#include "collision.h"

using glm::vec2;
using glm::vec4;
using std::clamp;
using std::max;
using std::min;
namespace {
constexpr float kMaxFrameTime = 1.0f / 30.0f;
constexpr float kDefaultRestitution = 0.95f;
}

PhysicsEngine::PhysicsEngine()
    : boundsMin(-1.6f, -0.9f),
      boundsMax(1.6f, 0.9f),
      fixedTimeStep(1.0f / 240.0f),
      accumulatedTime(0.0f) {}

void PhysicsEngine::addBall(
    const vec2& pos,
    const vec2& vel,
    float rad,
    float mass,
    float restitution,
    const vec3& color
) {
    balls.push_back({pos, vel, rad, mass, restitution, color});
}

void PhysicsEngine::setBounds(const vec2& minBounds, const vec2& maxBounds) {
    boundsMin = minBounds;
    boundsMax = maxBounds;
}

void PhysicsEngine::update(float deltaTime) {
    accumulatedTime += min(deltaTime, kMaxFrameTime);

    while (accumulatedTime >= fixedTimeStep) {
        step(fixedTimeStep);
        accumulatedTime -= fixedTimeStep;
    }
}

const vector<Ball>& PhysicsEngine::getBalls() const {
    return balls;
}
//boundry collision
void PhysicsEngine::step(float deltaTime) {
    for (auto& ball : balls) {
        ball.position += ball.velocity * deltaTime;
        handleBoundaryCollision(ball);
    }

    handleCollisions();

    for (auto& ball : balls) {
        handleBoundaryCollision(ball);
    }
}

void PhysicsEngine::handleBoundaryCollision(Ball& ball) {
    if (ball.position.x - ball.radius < boundsMin.x) {
        ball.position.x = boundsMin.x + ball.radius;
        if (ball.velocity.x < 0.0f) {
            ball.velocity.x = -ball.velocity.x * min(ball.restitution, kDefaultRestitution);
        }
    }

    if (ball.position.x + ball.radius > boundsMax.x) {
        ball.position.x = boundsMax.x - ball.radius;
        if (ball.velocity.x > 0.0f) {
            ball.velocity.x = -ball.velocity.x * min(ball.restitution, kDefaultRestitution);
        }
    }

    if (ball.position.y - ball.radius < boundsMin.y) {
        ball.position.y = boundsMin.y + ball.radius;
        if (ball.velocity.y < 0.0f) {
            ball.velocity.y = -ball.velocity.y * min(ball.restitution, kDefaultRestitution);
        }
    }

    if (ball.position.y + ball.radius > boundsMax.y) {
        ball.position.y = boundsMax.y - ball.radius;
        if (ball.velocity.y > 0.0f) {
            ball.velocity.y = -ball.velocity.y * min(ball.restitution, kDefaultRestitution);
        }
    }
}

void PhysicsEngine::handleCollisions() {
    if (balls.size() < 2) {
        return;
    }

    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            if (checkCollision(balls[i], balls[j])) {
                resolveCollision(balls[i], balls[j]);
            }
        }
    }
}
