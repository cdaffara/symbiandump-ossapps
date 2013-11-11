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
TARGET = ut_fminternaldrivemodel

QT += testlib
CONFIG += hb qtestlib
CONFIG += symbian_test

include ( ../../../src/common.pri )
include ( ../../../src/inc/commoninc.pri )
include ( ../../../src/inc/commonutils.pri )
include ( ../ut_common.pri )

symbian:MMP_RULES += SMPSAFE

DEPENDPATH += .
INCLUDEPATH += .

#TARGET.CAPABILITY = ALL -TCB
TARGET.CAPABILITY = CAP_APPLICATION DiskAdmin AllFiles PowerMgmt

HEADERS += src/testclassdatafmdrivelistprovider.h \
           src/ut_fminternaldrivemodel.h
SOURCES += src/ut_fminternaldrivemodel.cpp
