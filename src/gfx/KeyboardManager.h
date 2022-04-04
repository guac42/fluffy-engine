#ifndef PATHTRACER_KEYBOARDMANAGER_H
#define PATHTRACER_KEYBOARDMANAGER_H

#include <GLFW/glfw3.h>

class KeyboardManager {
private:
    // Wacky bitwise shit
    // Not sure why I did this, but I did :/
    //0x00000001-current 0x00000010-previous
    unsigned char keys[GLFW_KEY_LAST]{0x0};

public:
    inline void update(int key, int action) {
        keys[key] = (unsigned char)(action == GLFW_PRESS || action == GLFW_REPEAT) | ((keys[key] & 0x00000001) << 1);
    }

    inline bool isKeyTouched(int key) {
        return !(keys[key] ^ 0x00000010);
    }

    inline bool isKeyDown(int key) {
        return keys[key] & 0x00000001;
    }
};

#endif //PATHTRACER_KEYBOARDMANAGER_H
