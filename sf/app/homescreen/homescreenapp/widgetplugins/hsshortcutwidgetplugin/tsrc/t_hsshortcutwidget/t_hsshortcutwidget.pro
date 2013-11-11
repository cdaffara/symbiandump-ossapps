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

QT += testlib sql

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
  DESTDIR = debug
  LIBS += -L../../../../../../bin/debug -lhsutils
  LIBS += -L../../../../../../bin/debug -lcaclient
}
else {
  DESTDIR = release
  LIBS += -L../../../../../../bin/release -lhsutils
  LIBS += -L../../../../../../bin/release -lcaclient
}

CONFIG += hb mobility
MOBILITY = serviceframework

HB += hbfeedback

DEFINES += HOMESCREEN_TEST \
           HSDOMAINMODEL_TEST
           
coverage:DEFINES += COVERAGE_MEASUREMENT

exists(../../../../../../homescreensrv) {
INCLUDEPATH += ../../../../../../homescreensrv/homescreensrv_plat/contentstorage_api \
}
else {
INCLUDEPATH += $${EPOCROOT}sf/mw/homescreensrv/homescreensrv_plat/contentstorage_api \
}

INCLUDEPATH += ./inc \
               ../../inc \
               ../../../../inc \
               ../../../../hsdomainmodel/inc \               
               ../../../../hsutils/inc  

DEPENDPATH += ./inc \
              ./src \
              ../../inc \
              ../../src

include(t_hsshortcutwidget.pri)

symbian: {
    CONFIG += symbian_test
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    TARGET.UID3 = 0x20022F64
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    
    MMP_RULES += SMPSAFE
}

win32:include(t_hsshortcutwidget_installs_win32.pri)
