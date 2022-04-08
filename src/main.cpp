#include "Main.h"

#ifdef WIN32
#ifndef DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

int main() {
    GameWindow game;
    game.Run();
}
