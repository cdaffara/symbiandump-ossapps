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

QT += testlib
CONFIG += hb qtestlib
CONFIG += symbian_test
TEMPLATE = app
TARGET = ut_fmfiletyperecognizer

include ( ../../../src/common.pri )
include ( ../../../src/inc/commoninc.pri )
include ( ../../../src/inc/commonutils.pri )
include ( ../ut_common.pri )

DEPENDPATH += .
INCLUDEPATH += .

TARGET.CAPABILITY = ALL -TCB

symbian:MMP_RULES += SMPSAFE

SOURCES += src/ut_fmfiletyperecognizer.cpp  
