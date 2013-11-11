#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
    LIBS += -L../../../../../../bin/debug -lhsdomainmodel
    LIBS += -L../../../../../../bin/debug -lhsutils
    DESTDIR = debug
    
}
else {    
    LIBS += -L../../../../../../bin/release -lhsdomainmodel
    LIBS += -L../../../../../../bin/release -lhsutils
    DESTDIR = release
}

CONFIG += debug_and_release \
          console \
          hb \
          mobility

MOBILITY = serviceframework publishsubscribe

QT += testlib \
      xml \
      sql

DEFINES += HOMESCREEN_TEST \
           HSDOMAINMODEL_TEST \
           HSUTILS_TEST

coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH += .\
              ./src \
              ../../../src \
              ../../../inc

INCLUDEPATH += .\
                ./inc \
                ../../../inc \
                ../../../../inc \
                ../../../../hsutils/inc \
                ../../../../hsdomainmodel/inc
                
win32: {
    INCLUDEPATH += ../../../../../../homescreensrv/homescreensrv_plat/homescreen_information_api/inc
}                

symbian: {
    CONFIG += symbian_test
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    SYMBIAN_PLATFORMS = WINSCW ARMV5

    TARGET.UID3 = 0x20022F5F
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    
    INCLUDEPATH += $${EPOCROOT}sf/mw/homescreensrv/homescreensrv_plat/contentstorage_api \
               $${EPOCROOT}sf/mw/homescreensrv/contentstorage/caclient/inc \
               $${EPOCROOT}sf/mw/homescreensrv/contentstorage/caclient/stub/inc
    LIBS +=  -lxqservice -lxqserviceutil -lXQKeyCapture -labclient -lxqsettingsmanager

    include(installs_symbian.pri)
    
    MMP_RULES += SMPSAFE
}

include(t_hsapplicationexe.pri)

win32:include(installs_win32.pri)
symbian:include(installs_symbian.pri)

