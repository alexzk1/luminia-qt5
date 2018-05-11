#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdio>
#define GL_CHECK_ERROR()                        \
({                                               \
    GLenum error = glGetError();                \
    if (error != GL_NO_ERROR)                   \
        fprintf(stderr, "GL_ERROR: %s(%d): %s 0x%X\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, error);    \
    error; \
})


#define FATAL_ERROR(TEXT)                           \
{                                               \
    fprintf(stderr, "FATAL_ERROR: %s(%d): %s\n%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, TEXT);    \
    exit(1); \
}


#include <QDebug>





#endif // GLOBALS_H
