
#warning! for target ARM you must reconfigure all manually ..
#https://github.com/aklomp/base64

lbase64.target=base64
lbase64.commands=AVX2_CFLAGS=-mavx2 SSSE3_CFLAGS=-mssse3 SSE41_CFLAGS=-msse4.1 SSE42_CFLAGS=-msse4.2 AVX_CFLAGS=-mavx cd $$PWD/base64 &&  make lib/libbase64.o
QMAKE_EXTRA_TARGETS += lbase64

INCLUDEPATH += $$PWD/base64/include
LIBS += $$PWD/base64/lib/libbase64.o
