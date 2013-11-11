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
# Contributors: XENT-MV
#
# Description: Stuff for all unit tests
#

QT += testlib
CONFIG += qtestlib hb


win32 {
    INCLUDEPATH += .
    INCLUDEPATH += ../../../src/inc
    INCLUDEPATH += ../../../src/cpframework/src
    INCLUDEPATH += ../../../controlpanel_plat/inc
}

unix {
     test.commands = ./$(TARGET)
     autotest.commands = ./$(TARGET) -xml -o ../$(QMAKE_TARGET).xml
} else:win32 {
     test.CONFIG += recursive
     autotest.CONFIG += recursive
     build_pass {
     		test.commands =/epoc32/RELEASE/WINSCW/udeb/$(QMAKE_TARGET).exe
		autotest.commands =/epoc32/RELEASE/WINSCW/udeb/$(QMAKE_TARGET).exe -xml -o c:/$(QMAKE_TARGET).xml
     }
}

QMAKE_EXTRA_TARGETS += test autotest

symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MOC_DIR
    LIBS += -lcentralrepository
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}

	   