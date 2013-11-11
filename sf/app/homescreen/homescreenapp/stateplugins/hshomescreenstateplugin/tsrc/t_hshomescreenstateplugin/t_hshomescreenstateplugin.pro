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
    DESTDIR = ./debug
} else {
    DESTDIR = ./release
}

CONFIG += debug_and_release \
          console
CONFIG += hb mobility

MOBILITY = serviceframework

QT += testlib \
      xml \
      sql
HB += hbfeedback

RESOURCES += t_hshomescreenstateplugin.qrc

CONFIG(debug, debug|release) {
    DESTDIR = ./debug
} else {
    DESTDIR = ./release
}

DEFINES += HOMESCREEN_TEST \
           HSDOMAINMODEL_TEST \
           HSUTILS_TEST \
           MENUSERVICE_TEST

coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH += .\
              ./src \
              ./inc \
              ../../src \
              ../../inc

INCLUDEPATH += .\
               ./inc \
               ../../inc \
               ../../../../../homescreenapp/inc \
               ../../../../hsutils/inc \
               ../../../../hsdomainmodel/inc \
               ../../../../hsapplication/inc \
               ../../../../serviceproviders/hsmenuserviceprovider/inc \
               ../../../../hsutils/tsrc/t_hsutils/inc

exists(../../../../../../homescreensrv) {
INCLUDEPATH += ../../../../../../homescreensrv/homescreensrv_plat/contentstorage_api \
               ../../../../../../homescreensrv/contentstorage/caclient/inc \
               ../../../../../../homescreensrv/contentstorage/caclient/stub/inc
} else {
INCLUDEPATH += $${EPOCROOT}sf/mw/homescreensrv/homescreensrv_plat/contentstorage_api \
               $${EPOCROOT}sf/mw/homescreensrv/contentstorage/caclient/inc \
               $${EPOCROOT}sf/mw/homescreensrv/contentstorage/caclient/stub/inc
}

symbian: {
    CONFIG += symbian_test
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    SYMBIAN_PLATFORMS = WINSCW ARMV5
    TARGET.UID3 = 0x20022F60
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB

    LIBS+=-lxqservice -lxqserviceutil -labclient -lxqsettingsmanager
    
    MMP_RULES += SMPSAFE
}

include(t_hshomescreenstateplugin.pri)

win32:include(installs_win32.pri)
