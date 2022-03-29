#ifndef GAMEGL_WINDOW_H
#define GAMEGL_WINDOW_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "KeyboardManager.h"
#include "MouseManager.h"
#include "Util.h"

#ifndef TICKS_PER_SECOND
#define TICKS_PER_SECOND (60)
#endif
#ifndef VSYNC
#define VSYNC (true)
#endif
#ifndef KEYBIND_FULLSCREEN
#define KEYBIND_FULLSCREEN GLFW_KEY_F
#endif
#ifndef KEYBIND_QUIT
#define KEYBIND_QUIT GLFW_KEY_ESCAPE
#endif
#ifndef KEYBIND_TOGGLE_CURSOR
#define KEYBIND_TOGGLE_CURSOR GLFW_KEY_T
#endif

#define GetWindow(handle) ((Window*)glfwGetWindowUserPointer(handle))

class Window {
public:
    GLFWwindow *handle;
    int width, height;
    bool focused;
    KeyboardManager keyboardManager;
    MouseManager mouseManager;

    unsigned long long last_second = 0,
            frames = 0, fps = 0, last_frame = 0, frame_delta = 0,
            ticks = 0, tps = 0, tick_remainder = 0;

private:
    // Internal Callbacks
    static void _error_callback(int error, const char *description) {
        fprintf(stderr, "Error %d: %s\n", error, description);
    }
    
    static void _key_callback(GLFWwindow *handle, int key, int scancode, int action, int mods) {
        if (key < 0) return;
        Window *window = GetWindow(handle);
        switch (key) {
            // Fullscreen
            case KEYBIND_FULLSCREEN:
                if (action == GLFW_PRESS) {
                    if (glfwGetWindowAttrib(handle, GLFW_MAXIMIZED)) {
                        glfwRestoreWindow(handle);
                        break;
                    }
                    glfwMaximizeWindow(handle);
                }
                break;
            // Exit
            case KEYBIND_QUIT:
                if (action == GLFW_PRESS)
                    glfwSetWindowShouldClose(handle, GLFW_TRUE);
                break;
            // Lock cursor
            case KEYBIND_TOGGLE_CURSOR:
                if (action == GLFW_PRESS)
                    glfwSetInputMode(handle, GLFW_CURSOR,
                                     glfwGetInputMode(handle, GLFW_CURSOR) == GLFW_CURSOR_NORMAL
                                     ? GLFW_CURSOR_DISABLED
                                     : GLFW_CURSOR_NORMAL);
                break;
            default:
                break;
        }
        // Key states
        window->keyboardManager.update(key, action);
        window->OnKeyPress();
    }

    static void _cursor_callback(GLFWwindow *handle, double xp, double yp) {
        Window *window = GetWindow(handle);
        glm::vec2 p{xp, yp};
        window->mouseManager.delta.y = p.y - window->mouseManager.position.y;
        window->mouseManager.delta.x = p.x - window->mouseManager.position.x;
        window->mouseManager.delta = glm::clamp(window->mouseManager.delta, -100.0f, 100.0f);
        window->mouseManager.position = p;
    }

    static void _mouse_callback(GLFWwindow *handle, int button, int action, int mods) {
        if (button < 0) return;
        GetWindow(handle)->mouseManager.update(button, action);
    }

    static void _size_callback(GLFWwindow *handle, int width, int height) {
        glViewport(0, 0, width, height);
        Window *window = GetWindow(handle);
        window->width = width;
        window->height = height;
        window->OnResize();
    }

    static void _focus_callback(GLFWwindow *handle, int focused) {
        GetWindow(handle)->focused = focused;
    }
    // End Internal Callbacks

public:
    Window() : Window(832, 468) {}

    explicit Window(const std::string& title) : Window(832, 463, title) {}

    Window(int width, int height) : Window(width, height, "Window") {}

    Window(int width, int height, const std::string& title) : Window(width, height, 4, 5, title) {}

    /// Warning: Some features require opengl 4.5
    Window(int width, int height, int versionMajor, int versionMinor, const std::string& title) {
        this->width = width;
        this->height = height;

        this->last_frame = NOW();
        this->last_second = NOW();

        glfwSetErrorCallback(_error_callback);

        if (!glfwInit()) {
            fprintf(stderr, "%s", "error initializing GLFW\n");
            exit(1);
        }

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        this->handle = glfwCreateWindow(this->width, this->height, title.c_str(), nullptr, nullptr);
        if (this->handle == nullptr) {
            fprintf(stderr, "%s", "error creating window\n");
            glfwTerminate();
            exit(1);
        }

        glfwMakeContextCurrent(this->handle);
        glfwSetWindowUserPointer(this->handle, this);

        glfwSetWindowAspectRatio(this->handle, 16, 9);
        glfwSetInputMode(this->handle, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwSetWindowSizeLimits(this->handle, 640, 360, GLFW_DONT_CARE, GLFW_DONT_CARE);

        // callbacks
        glfwSetKeyCallback(this->handle, _key_callback);
        glfwSetCursorPosCallback(this->handle, _cursor_callback);
        glfwSetMouseButtonCallback(this->handle, _mouse_callback);
        glfwSetFramebufferSizeCallback(this->handle, _size_callback);
        glfwSetWindowFocusCallback(this->handle, _focus_callback);

        if (!gladLoadGL()) {
            fprintf(stderr, "%s", "error initializing GLAD\n");
            glfwTerminate();
            exit(1);
        }

        glfwGetFramebufferSize(this->handle, &this->width, &this->height);
        glViewport(0, 0, this->width, this->height);

        glfwSwapInterval(VSYNC);
    }

    void Run() {
        this->OnLoad();
        glfwShowWindow(this->handle);

        while (!glfwWindowShouldClose(this->handle)) {
            const unsigned long long start = NOW();

            this->frame_delta = start - this->last_frame;
            this->last_frame = start;

            if (start - this->last_second > NS_PER_SECOND) {
                this->fps = this->frames;
                this->tps = this->ticks;
                this->frames = 0;
                this->ticks = 0;
                this->last_second = start;

                printf("FPS: %lld | TPS: %lld\n", this->fps, this->tps);
            }

            // tick processing
            const unsigned long long NS_PER_TICK = (NS_PER_SECOND / TICKS_PER_SECOND);
            unsigned long long tick_time = this->frame_delta + this->tick_remainder;
            while (tick_time > NS_PER_TICK) {
                this->OnGameTick();
                tick_time -= NS_PER_TICK;
            }
            this->tick_remainder = std::max(tick_time, (unsigned long long) 0);

            this->OnUpdateFrame();
            this->OnRenderFrame();
            glfwSwapBuffers(this->handle);
            glfwPollEvents();

            //DELAY(NS_PER_TICK - (start - NOW()));
        }

        this->OnClose();
        glfwTerminate();
        exit(0);
    }

    inline float deltaTime() const {
        return (float)this->frame_delta / NS_PER_SECOND;
    }

protected:
    /// The window is not visible until after this function is called
    virtual void OnLoad() = 0;

    virtual void OnClose() = 0;

    /// Call the base OnUpdateFrame prior to the child implementation
    virtual void OnUpdateFrame() {
        this->mouseManager.delta = glm::vec2{0.0, 0.0};
    }

    /// Call the base OnRenderFrame after the child implementation
    virtual void OnRenderFrame() {
        this->frames++;
    }

    /// Call the base OnGameTick prior to the child implementation
    virtual void OnGameTick() {
        this->ticks++;
    }

    virtual void OnResize() = 0;

    virtual void OnKeyPress() = 0;

    void ToggleCursorLock() const {
        glfwSetInputMode(this->handle, GLFW_CURSOR,
                         glfwGetInputMode(this->handle, GLFW_CURSOR) == GLFW_CURSOR_NORMAL
                         ? GLFW_CURSOR_DISABLED
                         : GLFW_CURSOR_NORMAL);
    }
};

#undef GetWindow
#endif //GAMEGL_WINDOW_H
