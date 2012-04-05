//========================================================================
// Window mode test
// Copyright (c) Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
//
// This test provides an interface to window mode switching
//
//========================================================================

#include <GL/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

static int window_mode = GLFW_WINDOWED;

static void window_size_callback(GLFWwindow window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow window, int key, int action)
{
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_SPACE)
    {
        if (window_mode == GLFW_FULLSCREEN)
            window_mode = GLFW_WINDOWED;
        else
            window_mode = GLFW_FULLSCREEN;

        glfwSetWindowMode(window, 800, 600, window_mode);
    }
}

int main(void)
{
    GLFWwindow window;

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    window = glfwOpenWindow(0, 0, window_mode, "Window Mode Test", NULL);
    if (!window)
    {
        glfwTerminate();

        fprintf(stderr, "Failed to open GLFW window\n");
        exit(EXIT_FAILURE);
    }

    glfwSetWindowSizeCallback(window_size_callback);
    glfwSetKeyCallback(key_callback);
    glfwSwapInterval(1);

    while (glfwIsWindow(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers();
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

