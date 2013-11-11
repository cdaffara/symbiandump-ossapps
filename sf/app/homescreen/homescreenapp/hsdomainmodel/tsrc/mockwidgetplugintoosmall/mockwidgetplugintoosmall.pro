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

TEMPLATE = lib
CONFIG += plugin hb mobility
MOBILITY = serviceframework

DEFINES += HSDOMAINMODEL_TEST

DEPENDPATH += .\
              ./src \
              ./inc \
              ../mockwidget/inc/ \
              ../mockwidget/src/

INCLUDEPATH += .\
               ./inc \
              ../mockwidget/inc/

symbian: {

CONFIG += symbian_test

TARGET.UID3 = 0x20022F75
TARGET.CAPABILITY = CAP_APPLICATION AllFiles
TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
TARGET.EPOCALLOWDLLDATA=1

MMP_RULES += SMPSAFE

}

include(mockwidgetplugintoosmall.pri)
win32:include(installs_win32.pri)