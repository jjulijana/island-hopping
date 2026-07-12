QT += core gui widgets
CONFIG += c++17 link_pkgconfig
TEMPLATE = app
TARGET = island-hopping

PKGCONFIG += cgal

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    canvas.cpp \
    solver.cpp

HEADERS += \
    mainwindow.h \
    canvas.h \
    solver.h \
    components.h
