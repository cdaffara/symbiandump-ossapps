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

include(../../../../homescreenapp/common.pri)

CONFIG += debug_and_release \
          console \
          hb \
          mobility

MOBILITY = serviceframework

coverage:DEFINES += COVERAGE_MEASUREMENT

QT += testlib
LIBS += -lcaclient

DEFINES += HSDOMAINMODEL_TEST\
           HSMENUCLIENTPLUGIN_UNITTEST

win32: {
DEFINES += CACLIENT_TEST
}

DEPENDPATH += .\
              ./src \
              ./inc \
              ../../../../homescreenapp/hsmenuclientplugin/src \
              ../../../../homescreenapp/hsmenuclientplugin/inc

INCLUDEPATH += .\
               ./inc \
               ../../../../homescreenapp/hsmenuclientplugin/inc \
               ../../inc/

symbian: {
    ### get rid of localization warnings
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    ###
    CONFIG += symbian_test
    coverage: CONFIG -= symbian_test
    TARGET.UID3 = 20028716
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
}

include(t_hsmenuclientplugin.pri)


symbian:MMP_RULES += SMPSAFE
