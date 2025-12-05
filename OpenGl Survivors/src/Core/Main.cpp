#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Game.h"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

int main()
{
    Game game(SCR_WIDTH,SCR_HEIGHT,"OpenGl Survivors");
    if (game.Init())
    {
        game.Run();
    }
    else
    {
        std::cout << "CRITICAL ERROR: Game failed to initialize." << std::endl;
        return -1;
    }
    return 0;
}


