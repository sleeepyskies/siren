#define GLM_ENABLE_EXPERIMENTAL
#define SOBJ_USE_SLOG

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Shaders.hpp"
#include "Window.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "serialization/sobj.hpp"
#include "slog.hpp"
#include "sobj.hpp"
#include "stb_image.hpp"
#include <cmath>
#include <glm/vec3.hpp>

// opengl has a right-handed coordinate system, so:
//  +x = right
//  +y = up
//  +z = behind
//
// opengl uses column major ordering for matrices, meaning we store a matrices data with columns
// packed together [col1, col2, col3, col4, ...]

int main()
{
    slog::logLevel = slog::Level::TRACE;

    Window window{};
    if (!window.valid()) { return -1; }
    window.setBackgroundColor({ 0.11f, 0.12f, 0.25, 1.f });
    Camera camera{ window.getWidth(), window.getHeight() };

    // ReSharper disable once CppCStyleCast
    const int glVersion =
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // load opengl functions
    if (!glVersion) {
        err("OpenGl could not load correctly");
        glfwTerminate();
        return -1;
    }

    window.registerCallback([&camera](GLFWwindow* win, const int width, const int height) {
        camera.setViewportWidth(width);
        camera.setViewportHeight(height);
        glViewport(0, 0, width, height);
    });

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    nfo("Loaded OpenGl version {}.{}", major, minor);

    glViewport(0, 0, window.getWidth(), window.getHeight()); // specify what part of the window
                                                             // OpenGL should render into

    sobj::OBJLoader loader{};

    if (!loader.load("../resources/models/obj/coffee_cup/coffee_cup.obj")) return -1;
    sobj::OBJData objData = loader.steal();

    Model object{ objData, STATIC };

    const Shaders shaderProgram("../shaders/vertex.vert", "../shaders/fragment.frag");

    glEnable(GL_DEPTH_TEST); // depth testing

    // connect texture to slot 0
    shaderProgram.registerModel(object);

    glm::vec4 lightColor{ 1.0, 1.0, 1.0, 1.0 };

    window.setVsyncEnabled(true);

    constexpr float rotationSpeed = glm::radians(0.25f);

    float lastFrame  = glfwGetTime();
    float lastUpdate = lastFrame;
    while (!window.shouldClose()) {
        if (Input::isKeyPressed(ESC)) window.setShouldClose(true);

        const float now   = glfwGetTime();
        const float delta = now - lastFrame;
        lastFrame         = now;

        // update title every 1s
        if (now - lastUpdate > 1.f) {
            window.setTitle("FPS: " + std::to_string(1 / delta));
            lastUpdate = now;
        }

        glfwPollEvents();

        window.clearBuffers();

        camera.onUpdate(delta, window);

        // rotate the pyramid slowly
        object.rotateRad(glm::vec3{ 0, 1, 0 }, rotationSpeed);

        // upload matrices to the gpu
        shaderProgram.use();
        shaderProgram.setUniformImage("tex0", 0);
        shaderProgram.setUniformMat4("model", object.getTransform());
        shaderProgram.setUniformMat4("projView", camera.projectionViewMatrix());
        shaderProgram.setUniformVec4("lightColor", lightColor);
        shaderProgram.setUniformVec3("lightPos", glm::vec3(1));
        shaderProgram.setUniformVec3("camPos", glm::vec3(0, 0, 0));
        object.draw();

        window.swapBuffers();
    }

    return 0;
}
