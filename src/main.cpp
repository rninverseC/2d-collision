#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "physics.h"

using glm::vec2;
using glm::vec4;
using std::cerr;
using std::clamp;
using std::endl;
using std::max;
using std::min;

namespace {
constexpr int kInitialWindowWidth = 1100;
constexpr int kInitialWindowHeight = 700;
constexpr float kWorldLeft = 0.0f;
constexpr float kWorldBottom = 0.0f;

float ballRadiusForWindow(float width, float height) {
    return clamp(min(width, height) * 0.055f, 24.0f, 42.0f);
}

void configureScene(float worldWidth, float worldHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(kWorldLeft, worldWidth, kWorldBottom, worldHeight, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawDisk(
    const vec2& center,
    float radius,
    const vec4& centerColor,
    const vec4& edgeColor
) {
    const int segments = 72;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(centerColor.r, centerColor.g, centerColor.b, centerColor.a);
    glVertex2f(center.x, center.y);

    glColor4f(edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a);
    for (int i = 0; i <= segments; ++i) {
        const float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * 3.14159265f;
        glVertex2f(center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius);
    }
    glEnd();
}

void drawBall(const Ball& ball) {
    drawDisk(ball.position, ball.radius, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
}
//setting the boundry
void syncWindowState(GLFWwindow* window, PhysicsEngine& engine, float& worldWidth, float& worldHeight) {
    int windowWidth = kInitialWindowWidth;
    int windowHeight = kInitialWindowHeight;
    int framebufferWidth = kInitialWindowWidth;
    int framebufferHeight = kInitialWindowHeight;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    worldWidth = max(static_cast<float>(windowWidth), 1.0f);
    worldHeight = max(static_cast<float>(windowHeight), 1.0f);
    glViewport(0, 0, max(framebufferWidth, 1), max(framebufferHeight, 1));
    engine.setBounds({kWorldLeft, kWorldBottom}, {worldWidth, worldHeight});
}

void seedScene(PhysicsEngine& engine, float worldWidth, float worldHeight) {
    constexpr float mass = 10.0f;
    constexpr float restitution = 0.92f;
    const float radius = ballRadiusForWindow(worldWidth, worldHeight);

    engine.setBounds({kWorldLeft, kWorldBottom}, {worldWidth, worldHeight});
    engine.addBall(
        {worldWidth * 0.22f, worldHeight * 0.30f},
        {worldWidth * 0.26f, worldHeight * 0.23f},
        radius,
        mass,
        restitution,
        {1.0f, 1.0f, 1.0f}
    );
    engine.addBall(
        {worldWidth * 0.72f, worldHeight * 0.68f},
        {-worldWidth * 0.21f, -worldHeight * 0.18f},
        radius,
        mass,
        restitution,
        {1.0f, 1.0f, 1.0f}
    );
}
} 

int main() {
    if (!glfwInit()) {
        cerr << "GLFW init failed" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(
        kInitialWindowWidth,
        kInitialWindowHeight,
        "Collision Orz",
        nullptr,
        nullptr
    );
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    PhysicsEngine engine;
    float worldWidth = static_cast<float>(kInitialWindowWidth);
    float worldHeight = static_cast<float>(kInitialWindowHeight);
    syncWindowState(window, engine, worldWidth, worldHeight);
    seedScene(engine, worldWidth, worldHeight);

    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;

        syncWindowState(window, engine, worldWidth, worldHeight);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            engine = PhysicsEngine();
            syncWindowState(window, engine, worldWidth, worldHeight);
            seedScene(engine, worldWidth, worldHeight);
            previousTime = glfwGetTime();
        }

        engine.update(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT);
        configureScene(worldWidth, worldHeight);

        for (const auto& ball : engine.getBalls()) {
            drawBall(ball);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

//cmake --build build --target run
