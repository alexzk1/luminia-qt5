QT += opengl xml network script gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app

RESOURCES = lumina.qrc
DEFINES += QTSCRIPT
LIBS += -lGLEW

MOC_DIR = .build/moc
UI_DIR = .build/uic
OBJECTS_DIR = .build/obj
RCC_DIR = .build/qrc

exists($$[QT_INSTALL_HEADERS]/QtScript){
        message(Lumina configured for QT with QtScript)
        }


QMAKE_CXXFLAGS +=  -std=c++14 -Wall -frtti -fexceptions -Werror=return-type

#original code heavy uses overloaded virtuals
#QMAKE_CXXFLAGS +=-Werror=overloaded-virtual

QMAKE_CXXFLAGS +=  -Wctor-dtor-privacy -Werror=delete-non-virtual-dtor -fstrict-aliasing
QMAKE_CXXFLAGS +=  -Werror=strict-aliasing -Wstrict-aliasing=2

CONFIG += c++14

#using new C++ libs for macos http://blog.michael.kuron-germany.de/2013/02/using-c11-on-mac-os-x-10-8/
#that may not work with C++14 though, Apple is slow
macx: QMAKE_CXXFLAGS += -stdlib=libc++
macx: QMAKE_LFLAGS += -lc++
macx: QMAKE_CXXFLAGS += -mmacosx-version-min=10.10
macx: QMAKE_MACOSX_DEPLOYMENT_TARGET=10.10

CONFIG(debug) {
     message( "Building the DEBUG Version" )
     #lets optimize for CPU on debug, for release - packager should do
     QMAKE_CXXFLAGS +=  -march=native -O0 -g
     DEFINES += _DEBUG
     unix:!maxc:QMAKE_CXXFLAGS += -fsanitize=undefined -fsanitize=vptr
     unix:!maxc:LIBS += -lubsan
}
else {
    DEFINES += NDEBUG
    message( "Building the RELEASE Version" )
    QMAKE_CXXFLAGS += -O3
}

include($$PWD/src/base64/base64.pri)
PRE_TARGETDEPS += base64

HEADERS	=	src/mainwindow.h \
                src/glcam.h src/tree.h \
                src/item.h \
                src/item_image.h \
                src/item_virtual.h \
                src/item_mesh.h \
                src/item_index.h \
                src/item_component.h \
                src/item_texture.h \
                src/glwrapper.h \
                src/sourceedit.h \
                src/lumfile.h \
                src/timewidget.h \
                src/profiler.h \
                src/console.h \
                src/factory/dialog_factory_private.h \
                src/factory/file_factory_private.h \
                src/factory/network_factory_private.h \
                src/factory/glwidget_factory_private.h \
                src/factory/colorwidget_factory_private.h \
                src/factory/factory.h \
                src/importer/parsercodec.h \
                src/half.h \
                src/script_extender.h \
                src/script_extender_engine.h \
    src/incgl.h \
    src/saveable_widget.h \
    src/loaderpaths.h \
    src/file_loader.h \
    src/palgorithm.h \
    src/no_copy.h \
    src/script_header_parser.h \
    src/filterableitem.h \
    src/globals.h \
    src/dock_prep.h \
    src/prohibited_filter.h \
    src/texture2lum.h

SOURCES =	src/main.cpp \
                src/mainwindow.cpp \
                src/glcam.cpp \
                src/tree.cpp \
                src/item.cpp \
                src/item_world.cpp \
                src/item_matrix.cpp \
                src/item_node.cpp \
                src/item_edit.cpp \
                src/item_shader.cpp \
                src/item_cam.cpp \
                src/item_script.cpp \
                src/item_texture.cpp \
                src/item_index.cpp \
                src/item_component.cpp \
                src/item_mesh.cpp \
                src/item_buffer.cpp \
                src/item_uniform.cpp \
                src/item_image.cpp \
                src/item_virtual.cpp \
                src/sourceedit.cpp \
                src/lumfile.cpp \
                src/timewidget.cpp \
                src/item_bone.cpp \
                src/glwrapper.cpp \
                src/glwrapper_shader.cpp \
                src/glwrapper_framebuffer.cpp \
                src/profiler.cpp \
                src/console.cpp \
                src/script_extender.cpp \
                src/importer/genNormal.cpp \
                src/importer/genTangent.cpp \
                src/importer/genTexSpaceQuaternion.cpp \
                src/importer/importModel.cpp \
                src/importer/importMD2.cpp \
                src/importer/importMD3.cpp \
                src/importer/import3DS.cpp \
                src/importer/importOBJ.cpp \
                src/importer/importCR2.cpp \
                src/importer/importX.cpp \
                src/importer/importCMF.cpp \
                src/importer/parsercodec.cpp \
                src/factory/dialog_factory_private.cpp \
                src/factory/dialog_factory_dock_private.cpp \
                src/factory/file_factory_private.cpp \
                src/factory/colorwidget_factory_private.cpp \
                src/factory/factory.cpp \
    src/loaderpaths.cpp \
    src/script_extender_engine.cpp \
    src/script_header_parser.cpp \
    src/filterableitem.cpp

