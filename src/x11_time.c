//========================================================================
// GLFW - An OpenGL library
// Platform:    X11
// API version: 3.0
// WWW:         http://www.glfw.org/
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2010 Camilla Berglund <elmindreda@elmindreda.org>
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

#include "internal.h"

#include <sys/time.h>
#include <time.h>


//========================================================================
// Return raw time
//========================================================================

static GLFWuint64 getRawTime(void)
{
    const GLFWuint64 scale = 1000000000UL;

#if defined(CLOCK_MONOTONIC)
    if (_glfwLibrary.X11.timer.monotonic)
    {
        struct timespec ts;

        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * scale + ts.tv_nsec;
    }
    else
#endif
    {
        struct timeval tv;

        gettimeofday(&tv, NULL);
        return tv.tv_sec * scale + tv.tv_usec * 1000UL;
    }
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

//========================================================================
// Initialise timer
//========================================================================

void _glfwInitTimer(void)
{
#if defined(CLOCK_MONOTONIC)
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
        _glfwLibrary.X11.timer.monotonic = GL_TRUE;
#endif

    _glfwLibrary.X11.timer.base = getRawTime();
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

//========================================================================
// Return timer value in seconds
//========================================================================

GLFWuint64 _glfwPlatformGetTime(void)
{
    return getRawTime() - _glfwLibrary.X11.timer.base;
}


//========================================================================
// Set timer value in seconds
//========================================================================

void _glfwPlatformSetTime(GLFWuint64 time)
{
    _glfwLibrary.X11.timer.base = getRawTime() - time;
}

