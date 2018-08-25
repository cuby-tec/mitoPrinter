#-------------------------------------------------
#
# Project created by QtCreator 2018-08-25T19:21:54
#
#-------------------------------------------------

QT       += core gui widgets printsupport

TARGET = mitoPrinter
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    myglobal.cpp \
    rightarea.cpp \
    thermoplot.cpp \
    exchange/cnccommand.cpp \
    exchange/exchange.cpp \
    exchange/thermothread.cpp \
    exchange/threadarc.cpp \
    exchange/threadexchange.cpp \
    exchange/usbexchange.cpp \
    aboutwindow.cpp \
    gparcer/coordinatus.cpp \
    profiles/profile.cpp \
    profiles/profiledialog.cpp \
    profiles/thermopiddialog.cpp \
    step_motor/accelerationtable.cpp \
    step_motor/controller.cpp \
    step_motor/stepmotor.cpp \
    thermolog/thermologdialog.cpp \
    thermolog/thermologview.cpp \
    links/json_sintax.c \
    links/parson.c \
    qcustomplot.cpp


HEADERS += \
        mainwindow.h \
    myglobal.h \
    rightarea.h \
    thermoplot.h \
    exchange/cnccommand.h \
    exchange/exchange.h \
    exchange/thermothread.h \
    exchange/threadarc.h \
    exchange/threadexchange.h \
    exchange/usbexchange.h \
    aboutwindow.h \
    gparcer/coordinatus.h \
    links/exchange/eModelstate.h \
    links/msmotor/msport.h \
    links/msmotor/sControl.h \
    links/msmotor/sSegment.h \
    links/ComDataReq_t.h \
    links/eModelstate.h \
    links/json_sintax.h \
    links/modelState.h \
    links/parson.h \
    links/sHotendControl.h \
    links/sModelCompState.h \
    links/status.h \
    links/Status_t.h \
    profiles/profile.h \
    profiles/profiledialog.h \
    profiles/thermopiddialog.h \
    step_motor/accelerationtable.h \
    step_motor/AccelerationTable_t.h \
    step_motor/block_state_t.h \
    step_motor/controller.cpp.orig \
    step_motor/controller.h \
    step_motor/countertime_t.h \
    step_motor/ProfileData.h \
    step_motor/Recalculate_flag.h \
    step_motor/stepmotor.h \
    step_motor/trapeze_t.h \
    thermolog/thermologdialog.h \
    thermolog/thermologview.h \
    qcustomplot.h


FORMS += \
        mainwindow.ui \
    rightArea.ui \
    aboutwindow.ui \
    profiles/profiledialog.ui \
    profiles/thermopiddialog.ui \
    thermolog/thermologdialog.ui \
    profiles/profiledialog.ui \
    profiles/thermopiddialog.ui \
    thermolog/thermologdialog.ui \
    thermoplot.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    profile/Profile1.json \
    profile/Profile10.json \
    profile/Profile2.json \
    profile/Profile3.json \
    profile/Profile4.json \
    profile/Profile5.json \
    profile/Profile6.json \
    profile/Profile7.json \
    profile/Profile8.json \
    profile/Profile9.json \
    profile/profiles.json \
    profile/aboutcomment.txt \
    profile/Profile1.json \
    profile/Profile10.json \
    profile/Profile2.json \
    profile/Profile3.json \
    profile/Profile4.json \
    profile/Profile5.json \
    profile/Profile6.json \
    profile/Profile7.json \
    profile/Profile8.json \
    profile/Profile9.json \
    profile/profiles.json \
    profile/aboutcomment.txt

RESOURCES += \
    resource.qrc
