#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T14:38:52
#
#-------------------------------------------------


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4)
{
QT += widgets
}

message(GiraffeCV Dir = $$(GIRAFFECV_DIR))

LIBS += -L$$(GIRAFFECV_DIR)/build/Linux/X64/lib/
debug: LIBS += -L$$(GIRAFFECV_DIR)/build/Linux/X64/lib/Debug
debug: LIBS += -L$$(GIRAFFECV_DIR)/build/3rd_party/Libs/Linux/X64/lib/Debug
release: LIBS += -L$$(GIRAFFECV_DIR)/build/Linux/X64/lib/Release
release: LIBS += -L$$(GIRAFFECV_DIR)/build/3rd_party/Libs/Linux/X64/lib/Release
LIBS += -lopencv_core
LIBS += -lopencv_imgcodecs
LIBS += -lopencv_videoio
LIBS += -lopencv_imgproc
LIBS += -lPanoRenderOGL
LIBS += -lEGL

TARGET = PanoPlayer
TEMPLATE = app

INCLUDEPATH += $$(GIRAFFECV_DIR)/build/3rd_party/OpenCV/Include
INCLUDEPATH += $$(GIRAFFECV_DIR)/Projects/Include
INCLUDEPATH += $$(GIRAFFECV_DIR)/Projects/Include/VideoStitching

SOURCES += main.cpp\
        mainwindow.cpp \
    GPanoRenderQWidget.cpp

HEADERS  += mainwindow.h \
    GPanoRenderQWidget.h

FORMS    += mainwindow.ui
