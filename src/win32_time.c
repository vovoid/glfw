//========================================================================
// GLFW - An OpenGL library
// Platform:    Win32
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


//========================================================================
// Return raw time
//========================================================================

static GLFWuint64 getRawTime(void)
{
    GLFWuint64 time;

    if (_glfwLibrary.Win32.timer.hasQPC)
        QueryPerformanceCounter((LARGE_INTEGER*) &time);
    else
        time = _glfw_timeGetTime();

    return time;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

//========================================================================
// Initialise timer
//========================================================================

void _glfwInitTimer(void)
{
    if (QueryPerformanceFrequency((LARGE_INTEGER*) &_glfwLibrary.Win32.timer.freq))
        _glfwLibrary.Win32.timer.hasQPC = GL_TRUE;
    else
    {
        _glfwLibrary.Win32.timer.hasQPC = GL_FALSE;
        _glfwLibrary.Win32.timer.freq = 1000; // timeGetTime is always in ms
    }

    _glfwLibrary.Win32.timer.base = getRawTime();
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

//========================================================================
// Return timer value in seconds
//========================================================================

GLFWuint64 _glfwPlatformGetTime(void)
{
    const GLFWuint64 billion = 1000000000UL;

    const GLFWuint64 time = getRawTime() - _glfwLibrary.Win32.timer.base;
    const GLFWuint64 freq = _glfwLibrary.Win32.timer.freq;

    // If we use the naive solution and multiply time by scale before dividing
    // by freq, we will quickly run out of room even in a 64-bit uint, so we
    // split it and thus won't run out of room unless freq > (2^64 / 1e9),
    // which comes out to over 18GHz.  This should work for now (TM).
    return (time / freq) * billion + ((time % freq) * billion) / freq;
}


//========================================================================
// Set timer value in seconds
//========================================================================

void _glfwPlatformSetTime(GLFWuint64 t)
{
}

