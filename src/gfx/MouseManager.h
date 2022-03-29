#ifndef PATHTRACER_MOUSEMANAGER_H
#define PATHTRACER_MOUSEMANAGER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class MouseManager {
private:
    //0x00000001-current 0x00000010-previous
    unsigned char buttons[GLFW_MOUSE_BUTTON_LAST]{0x0};

public:
    glm::vec2 position, delta;

    inline void update(int button, int action) {
        this->buttons[button] = (unsigned char)(action == GLFW_PRESS) | ((buttons[button] & 0x00000001) << 1);
    }

    inline bool isButtonDown(int button) {
        return buttons[button] & 0x00000001;
    }
};

#endif //PATHTRACER_MOUSEMANAGER_H
