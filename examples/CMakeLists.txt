
link_libraries(glfw ${OPENGL_glu_LIBRARY})

if (BUILD_SHARED_LIBS)
    add_definitions(-DGLFW_DLL)
    link_libraries(${OPENGL_gl_LIBRARY} ${MATH_LIBRARY})
else()
    link_libraries(${glfw_LIBRARIES})
endif()

include_directories(${GLFW_SOURCE_DIR}/include
                    ${GLFW_SOURCE_DIR}/support
                    ${OPENGL_INCLUDE_DIR})

set(GETOPT ${GLFW_SOURCE_DIR}/support/getopt.h
           ${GLFW_SOURCE_DIR}/support/getopt.c)
set(TINYCTHREAD ${GLFW_SOURCE_DIR}/support/tinycthread.h
                ${GLFW_SOURCE_DIR}/support/tinycthread.c)

if (APPLE)
    # Set fancy names for bundles
    add_executable("Abstracted Events" MACOSX_BUNDLE absevent.c ${TINYCTHREAD})
    add_executable(Boing MACOSX_BUNDLE boing.c)
    add_executable(Gears MACOSX_BUNDLE gears.c)
    add_executable("Split View" MACOSX_BUNDLE splitview.c)
    add_executable(Triangle MACOSX_BUNDLE triangle.c)
    add_executable(Wave MACOSX_BUNDLE wave.c)

    set_target_properties(Boing PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Boing")
    set_target_properties(Gears PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Gears")
    set_target_properties("Split View" PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Split View")
    set_target_properties(Triangle PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Triangle")
    set_target_properties(Wave PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Wave")
else()
    # Set boring names for executables
    add_executable(absevent WIN32 absevent.cpp ${TINYCTHREAD})
    add_executable(boing WIN32 boing.c)
    add_executable(gears WIN32 gears.c)
    add_executable(heightmap WIN32 heightmap.c ${GETOPT})
    add_executable(splitview WIN32 splitview.c)
    add_executable(triangle WIN32 triangle.c)
    add_executable(wave WIN32 wave.c)
endif()

if (MSVC)
    set(WINDOWS_BINARIES boing gears heightmap splitview triangle wave)

    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (APPLE)
    set(BUNDLE_BINARIES Boing Gears "Split View" Triangle Wave)

    set_target_properties(${BUNDLE_BINARIES} PROPERTIES
                          MACOSX_BUNDLE_SHORT_VERSION_STRING ${GLFW_VERSION}
                          MACOSX_BUNDLE_LONG_VERSION_STRING ${GLFW_VERSION_FULL})
endif()

