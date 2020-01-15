LIBS += ../Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

CONFIG(release,debug|release) {
  DEFINES += NDEBUG
}

INCLUDEPATH += \
    ../Urho3D/include \
    ../Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    mastercontrol.cpp \
    inputmaster.cpp \
    bird.cpp \
    finchycam.cpp \
    birdfactory.cpp \
    helper.cpp \
    timeline.cpp

HEADERS += \
    mastercontrol.h \
    inputmaster.h \
    bird.h \
    finchycam.h \
    birdfactory.h \
    helper.h \
    timeline.h
