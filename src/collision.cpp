#include "collision.h"
#include <algorithm>
#include <cmath>

using glm::vec2;
using glm::vec4;
using std::clamp;
using std::max;
using std::min;

namespace {
constexpr float kMinimumDistance = 0.00001f;
constexpr float kPenetrationSlop = 0.0001f;
constexpr float kPenetrationPercent = 0.9f;
}

float distanceBetween(const vec2& a, const vec2& b) {
    const vec2 delta = a - b;
    return sqrt(dot(delta, delta));
}

vec2 getNormal(const vec2& from, const vec2& to) {
    const vec2 offset = to - from;
    const float length = distanceBetween(from, to);

    if (length <= kMinimumDistance) {
        return {1.0f, 0.0f};
    }

    return offset / length;
}

bool checkCollision(const Ball& a, const Ball& b) {
    return distanceBetween(a.position, b.position) <= (a.radius + b.radius);
}

void resolveCollision(Ball& a, Ball& b) {
    vec2 normal = getNormal(a.position, b.position);
    vec2 relativeVelocity = b.velocity - a.velocity;
    const float velocityAlongNormal = dot(relativeVelocity, normal);

    if (velocityAlongNormal > 0.0f) {
        return;
    }

    const float inverseMassA = (a.mass > 0.0f) ? (1.0f / a.mass) : 0.0f;
    const float inverseMassB = (b.mass > 0.0f) ? (1.0f / b.mass) : 0.0f;
    const float inverseMassSum = inverseMassA + inverseMassB;
    if (inverseMassSum <= 0.0f) {
        return;
    }

    const float elasticity = min(a.restitution, b.restitution);
    const float impulseMagnitude = -(1.0f + elasticity) * velocityAlongNormal / inverseMassSum;
    const vec2 impulse = impulseMagnitude * normal;

    a.velocity -= impulse * inverseMassA;
    b.velocity += impulse * inverseMassB;

    const float distance = distanceBetween(a.position, b.position);
    const float penetration = (a.radius + b.radius) - distance;
    if (penetration > 0.0f) {
        const float correctionMagnitude =
            max(penetration - kPenetrationSlop, 0.0f) / inverseMassSum * kPenetrationPercent;
        const vec2 correction = correctionMagnitude * normal;
        a.position -= correction * inverseMassA;
        b.position += correction * inverseMassB;
    }
}
