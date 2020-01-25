QT += core
QT -= gui

CONFIG += c++11

TARGET = test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../quazip

LIBS += ../quazip/libquazip_fabaria.so

HEADERS += decompress.h

SOURCES += decompress.cpp \
	   main.cpp
