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
		src/script_launcher.h \
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
		src/dqobject.h \
		src/script_extender.h \
		src/script_extender_engine.h \
		src/setup.h

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
		src/script_launcher.cpp \
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
		src/glew/glew.c


RESOURCES =	lumina.qrc
QT += opengl xml network script
CONFIG += console
DEFINES += QTSCRIPT GLEW_STATIC
INCLUDEPATH += src/glew

unix:HEADERS += src/x11/spaceball.h
unix:SOURCES += src/x11/spaceball.cpp
unix:LIBS  +=  -lXi


MOC_DIR = .build/moc 
UI_DIR = .build/uic 
OBJECTS_DIR = .build/obj 
RCC_DIR = .build/qrc 

exists($$[QT_INSTALL_HEADERS]/QtScript){
	message(Lumina configured for QT-4.3 with QtScript)
	}

