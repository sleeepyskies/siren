#define GLM_ENABLE_EXPERIMENTAL
#define SOBJ_USE_SLOG

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "Buffer.hpp"
#include "Scene.hpp"
#include "Shaders.hpp"
#include "Window.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "slog.hpp"
#include "sobj.hpp"
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

    // ReSharper disable once CppCStyleCast
    const int glVersion =
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // load opengl functions
    if (!glVersion) {
        err("OpenGl could not load correctly");
        glfwTerminate();
        return -1;
    }

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    nfo("Loaded OpenGl version {}.{}", major, minor);

    glViewport(0, 0, window.getWidth(), window.getHeight()); // specify what part of the window
                                                             // OpenGL should render into

    sobj::OBJLoader loader{};

    if (!loader.load("../resources/models/obj/demogorgon/Demogorgon.obj")) return -1;
    sobj::OBJData objData = loader.steal();

    Scene scene{ objData, STATIC, window.getWidth(), window.getHeight() };
    scene.scale(0.05);

    window.registerCallback([&scene](GLFWwindow* win, const int width, const int height) {
        scene.onWindowResize(width, height);
        glViewport(0, 0, width, height);
    });

    const Shaders shaderProgram("../shaders/vertex.vert", "../shaders/fragment.frag");

    glEnable(GL_DEPTH_TEST); // depth testing

    window.setVsyncEnabled(true);

    float lastFrame  = glfwGetTime();
    float lastUpdate = lastFrame;
    window.setTitle("FPS: ");
    while (!window.shouldClose()) {
        if (Input::isKeyPressed(ESC)) window.setShouldClose(true);

        // TODO: put inside some time class
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

        scene.onUpdate(delta, window);
        scene.draw(shaderProgram);

        window.swapBuffers();
    }

    return 0;
}
