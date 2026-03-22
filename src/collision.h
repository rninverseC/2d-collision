#pragma once

#include <glm/glm.hpp>

#include "physics.h"

float distanceBetween(const vec2& a, const vec2& b);
vec2 getNormal(const vec2& from, const vec2& to);
bool checkCollision(const Ball& a, const Ball& b);
void resolveCollision(Ball& a, Ball& b);
