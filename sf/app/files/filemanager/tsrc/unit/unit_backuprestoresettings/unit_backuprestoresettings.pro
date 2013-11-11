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
CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
TEMPLATE = app
TARGET = 
include ( ../../../src/inc/commoninc.pri )
include ( ../../../src/common.pri )
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../../src/fmbkupenginewrapper/inc
INCLUDEPATH += ../../../src/inc
TARGET.CAPABILITY = ALL -TCB
# Input
SOURCES += src/unit_backuprestoresettings.cpp
LIBS += -lfmbkupenginewrapper

unix {
    test.commands = /epoc32/RELEASE/WINSCW/udeb/unit_backuprestoresettings.exe
    autotest.commands = /epoc32/RELEASE/WINSCW/udeb/unit_backuprestoresettings.exe -xml -o c:/unit_backuprestoresettings.xml
} else:win32 {
    test.CONFIG += recursive
    autotest.CONFIG += recursive
    build_pass {
        test.commands =/epoc32/RELEASE/WINSCW/udeb/unit_backuprestoresettings.exe
        autotest.commands =/epoc32/RELEASE/WINSCW/udeb/unit_backuprestoresettings -xml -o c:/unit_backuprestoresettings.xml
        }
}
QMAKE_EXTRA_TARGETS += test autotest

symbian:MMP_RULES += SMPSAFE
