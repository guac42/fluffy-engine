#ifndef GAMEFRAME_CONFIG_H
#define GAMEFRAME_CONFIG_H

class Config {
public:
    // Field of view, Range: 30 - 120
    static float FOV;

    // Mouse sensitivity, Rand: 0 - 1
    static float SENS;

};

float Config::FOV = 90.0f;
float Config::SENS = 0.3f;

#endif //GAMEFRAME_CONFIG_H
