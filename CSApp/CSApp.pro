QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatform.cpp \
    chattingclient.cpp \
    chattingserver.cpp \
    clienthandlerform.cpp \
    clientinformaiton.cpp \
    main.cpp \
    cs_app.cpp \
    orderhandlerform.cpp \
    orderinformaiton.cpp \
    producthandlerform.cpp \
    productinformaiton.cpp

HEADERS += \
    chatform.h \
    chattingclient.h \
    chattingserver.h \
    clienthandlerform.h \
    clientinformaiton.h \
    cs_app.h \
    orderhandlerform.h \
    orderinformaiton.h \
    producthandlerform.h \
    productinformaiton.h

FORMS += \
    chatform.ui \
    clienthandlerform.ui \
    cs_app.ui \
    orderhandlerform.ui \
    producthandlerform.ui

TRANSLATIONS += \
    CSApp_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
