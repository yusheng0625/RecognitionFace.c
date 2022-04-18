#-------------------------------------------------
#
# Project created by QtCreator 2018-04-25T23:22:44
#
#-------------------------------------------------

QT       += core gui sql multimedia multimediawidgets printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AcerFaceDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    loginwindow.cpp \
    matchwidget.cpp \
    mismatchwidget.cpp \
    checkoutwidget.cpp \
    thankudialog.cpp \
    settingsdialog.cpp \
    switch.cpp \
    dbmanagement.cpp \
    global.cpp \
    insertdialog.cpp \
    usermanagedialog.cpp \
    visitorinfo.cpp \
    signincamera.cpp \
    floordlg.cpp \
    privacydlg.cpp \
    userinfo.cpp \
    thankyouwidget.cpp \
    splashform.cpp \
    matchtop3widget.cpp \
    transactionmgr.cpp \
    frmsecurity.cpp

HEADERS  += mainwindow.h \
    loginwindow.h \
    matchwidget.h \
    mismatchwidget.h \
    checkoutwidget.h \
    thankudialog.h \
    settingsdialog.h \
    switch.h \
    dbmanagement.h \
    global.h \
    insertdialog.h \
    usermanagedialog.h \
    visitorinfo.h \
    signincamera.h \
    floordlg.h \
    privacydlg.h \
    userinfo.h \
    thankyouwidget.h \
    splashform.h \
    matchtop3widget.h \
    transactionmgr.h \
    frmsecurity.h

FORMS    += mainwindow.ui \
    loginwindow.ui \
    matchwidget.ui \
    mismatchwidget.ui \
    checkoutwidget.ui \
    thankudialog.ui \
    settingsdialog.ui \
    insertdialog.ui \
    usermanagedialog.ui \
    visitorinfo.ui \
    signincamera.ui \
    floordlg.ui \
    licensedlg.ui \
    privacydlg.ui \
    userinfo.ui \
    thankyouwidget.ui \
    splashform.ui \
    matchtop3widget.ui \
    frmsecurity.ui

RESOURCES += \
    demo.qrc

LIBS += -L$$PWD/opencv/lib/ -lopencv_world300
INCLUDEPATH += $$PWD/opencv/include
DEPENDPATH += $$PWD/opencv/include

LIBS += -L$$PWD/Faceengine/lib/ -ldetection

INCLUDEPATH += $$PWD/Faceengine/include
DEPENDPATH += $$PWD/Faceengine/include

LIBS += -L$$PWD/Faceengine/lib/ -lface_cnn

INCLUDEPATH += $$PWD/Faceengine/include
DEPENDPATH += $$PWD/Faceengine/include

