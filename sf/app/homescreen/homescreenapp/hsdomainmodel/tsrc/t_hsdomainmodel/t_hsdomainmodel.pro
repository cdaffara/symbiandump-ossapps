#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

TEMPLATE = app

CONFIG(debug, debug|release) {
    LIBS += -L./../../../../../bin/debug -lcaclient
    DESTDIR = debug
}
else {
    LIBS += -L./../../../../../bin/release  -lcaclient
    DESTDIR = release
}



CONFIG += debug_and_release \
          console

CONFIG += hb mobility
MOBILITY = serviceframework

HB += hbfeedback

coverage:DEFINES += COVERAGE_MEASUREMENT

QT += testlib \
      xml \
      sql

DEFINES += HOMESCREEN_TEST \
           HSDOMAINMODEL_TEST \
           HSUTILS_TEST

DEPENDPATH += .\
              ./src \
              ./inc

INCLUDEPATH += .\
               ./inc \
               ../../inc \
               ../../../inc \
               ../../../hsutils/inc \
               ../../../hsapplication/inc

exists(../../../../../homescreensrv) {
INCLUDEPATH += ../../../../../homescreensrv/homescreensrv_plat/contentstorage_api

} else {
INCLUDEPATH += $${EPOCROOT}sf/mw/homescreensrv/homescreensrv_plat/contentstorage_api
}

RESOURCES += ../../hsdomainmodel_win.qrc

symbian: {
    CONFIG += symbian_test
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    TARGET.UID3 = 0x20022F59
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    TARGET.EPOCALLOWDLLDATA=1

    LIBS += -lxqsettingsmanager

    INCLUDEPATH +=$${EPOCROOT}epoc32/include/connect
    include(installs_symbian.pri)
    
    MMP_RULES += SMPSAFE
}

include(t_hsdomainmodel.pri)

win32:include(installs_win32.pri)

