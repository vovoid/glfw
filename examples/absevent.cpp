//========================================================================
// Abstracted event passing from main thread to a child thread
//
// Copyright (c) Camilla Berglund <elmindreda@elmindreda.org>
// Copyright (c) Jonatan Wallmander, Vovoid Media Technologies AB
//
//========================================================================
// Description:
//   The rendering runs very slow to simulate a massive and hopefully
//   temporary framedrop in a game for instance.
//
//   Press random characters on the keyboard and check in the console
//   for printf output.
//
//   The main thread collecting the keys feeds the char events into the
//   rendering thread while it runs via the event pool.
//
//   You can extend this example by implementing a class to carry the data
//   instead - for instance a class holding mouse x, y and button.
//
//========================================================================
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
// This test is intended to verify whether the OpenGL context part of
// the GLFW API is able to be used from multiple threads
//
//========================================================================

extern "C" {
  #include "tinycthread.h"
}

#include <GL/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <vector>

// how many time to loop - each round we'll consume keys
#define LOOPS_PER_FRAME 10
// delay in microseconds
#define DELAY_PER_LOOP 100000
// extra delay - out of the loop - per frame
#define EXTRA_DELAY_OUT_OF_LOOP_PER_FRAME 0

// thread safe event queue implementation
template < typename T >
class event_pool
{
private:
  // our mutex
  mtx_t mutex;
  // the actual events are stored here
  typename std::vector< T > events;
  // iterator to keep track of consumption
  typename std::vector< T >::iterator consumption_iterator;

public:
    event_pool()
    {
        mtx_init( &mutex, mtx_plain ); // create our mutex
    }

    ~event_pool()
    {
        mtx_destroy( &mutex ); // destroy our mutex
    }

    void begin_consume()
    {
        consumption_iterator = events.begin();
        mtx_lock( &mutex );
    }

    // returns 0x0 when there's nothing left in the queue
    bool consume( T* destination )
    {
        // do we even have any events to deliver?
        if ( !events.size() ) return false;

        // are we not at the end of the event pool?
        if ( events.end() == consumption_iterator) return false;

        // all should be OK, return the value
        *destination = (*consumption_iterator);
        consumption_iterator++;
        return true;
    }

    void end_consume()
    {
        // check if all items were consumed, if yes - empty the events vector
        if (
            events.end() == consumption_iterator
        )
        {
            events.clear();
        }
        // otherwise delete them one by one
        else
        {
            events.erase( events.begin(), consumption_iterator );
        }
        mtx_unlock( &mutex );
    }

    void add( T &t )
    {
        mtx_lock( &mutex );
        events.push_back(t);
        mtx_unlock( &mutex );
    }
};


// declare an event pool to hold our characters
event_pool< int > char_event_pool;


typedef struct
{
    GLFWwindow window;
    const char* title;
    float r, g, b;
    thrd_t id;
} Thread;

static volatile GLboolean running = GL_TRUE;

static int thread_main(void* data)
{
    const Thread* thread = (const Thread*) data;

    glfwMakeContextCurrent(thread->window);
    assert(glfwGetCurrentContext() == thread->window);

    glfwSwapInterval(1);

    while (running)
    {
        const float v = (float) fabs(sin(glfwGetTime() * 2.f));

        // random color
        glClearColor(
              rand()%1000 * 0.001f,
              rand()%1000 * 0.001f,
              rand()%1000 * 0.001f,
              0.f
              );

        // loop several times to simulate low FPS
        for (int it = 0; it < LOOPS_PER_FRAME; it++)
        {

            // time to consume some characters
            char_event_pool.begin_consume();
            int c;
            while ( char_event_pool.consume(&c) )
            {
                printf("got character with code: %d\n", c);
            }
            char_event_pool.end_consume();
            usleep( DELAY_PER_LOOP ); // sleep a 1/10 of a second
        }
        // extra sleep
        usleep( EXTRA_DELAY_OUT_OF_LOOP_PER_FRAME );

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(thread->window);
    }

    glfwMakeContextCurrent(NULL);
    return 0;
}


// this will get called from main thread
void key_char_event( GLFWwindow window, int character )
{
    char_event_pool.add( character );
}


int main(void)
{
    int i, result;
    Thread threads[] =
    {
        { NULL, "Main", 1.f, 0.f, 0.f, 0 }
    };
    const int count = sizeof(threads) / sizeof(Thread);

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW: %s\n",
                glfwErrorString(glfwGetError()));
        exit(EXIT_FAILURE);
    }

    for (i = 0;  i < count;  i++)
    {
        glfwWindowHint(GLFW_POSITION_X, 200 + 250 * i);
        glfwWindowHint(GLFW_POSITION_Y, 200);
        threads[i].window = glfwCreateWindow(200, 200,
                                             GLFW_WINDOWED,
                                             threads[i].title,
                                             NULL);
        if (!threads[i].window)
        {
            fprintf(stderr, "Failed to open GLFW window: %s\n",
                    glfwErrorString(glfwGetError()));
            exit(EXIT_FAILURE);
        }

        if (thrd_create(&threads[i].id, thread_main, threads + i) !=
            thrd_success)
        {
            fprintf(stderr, "Failed to create secondary thread\n");
            exit(EXIT_FAILURE);
        }
        glfwSetCharCallback(threads[i].window, key_char_event);
    }

    while (running)
    {
        assert(glfwGetCurrentContext() == NULL);

        glfwWaitEvents();

        for (i = 0;  i < count;  i++)
        {
            if (glfwGetWindowParam(threads[i].window, GLFW_CLOSE_REQUESTED))
                running = GL_FALSE;
        }
    }

    for (i = 0;  i < count;  i++)
        thrd_join(threads[i].id, &result);

    exit(EXIT_SUCCESS);
}

