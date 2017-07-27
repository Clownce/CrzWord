#-------------------------------------------------
#
# Project created by QtCreator 2017-07-25T17:22:47
#
#-------------------------------------------------

QT       += core gui printsupport

TRANSLATIONS = lang_zh.ts\
               lang_en.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CrzWord
TEMPLATE = app


SOURCES += main.cpp\
        myword.cpp \
    mychild.cpp

HEADERS  += myword.h \
    mychild.h

RESOURCES += \
    resource.qrc
