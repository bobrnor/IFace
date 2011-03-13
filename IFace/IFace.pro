#-------------------------------------------------
#
# Project created by QtCreator 2011-03-07T12:22:13
#
#-------------------------------------------------

QT       += core gui

TARGET = IFace
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Project.cpp \
    lib_json/json_writer.cpp \
    lib_json/json_valueiterator.inl \
    lib_json/json_value.cpp \
    lib_json/json_reader.cpp \
    lib_json/json_internalmap.inl \
    lib_json/json_internalarray.inl \
    ProjectManager.cpp \
    CodeEditor.cpp \
    LeftArea.cpp \
    TabsHelper.cpp \
    ProjectFile.cpp

HEADERS  += MainWindow.h \
    Project.h \
    lib_json/json/writer.h \
    lib_json/json/value.h \
    lib_json/json/reader.h \
    lib_json/json/json.h \
    lib_json/json/forwards.h \
    lib_json/json/features.h \
    lib_json/json/config.h \
    lib_json/json/autolink.h \
    lib_json/json_tool.h \
    lib_json/json_batchallocator.h \
    ProjectManager.h \
    CodeEditor.h \
    LeftArea.h \
    TabsHelper.h \
    ProjectFile.h

FORMS    += MainWindow.ui
