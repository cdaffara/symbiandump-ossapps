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

unix {
    test.commands = /epoc32/RELEASE/WINSCW/udeb/$${TARGET}.exe
    autotest.commands = /epoc32/RELEASE/WINSCW/udeb/$${TARGET}.exe -xml -o c:/$${TARGET}.xml
} else:win32 {
    test.CONFIG += recursive
    autotest.CONFIG += recursive
    build_pass {
        test.commands =/epoc32/RELEASE/WINSCW/udeb/$${TARGET}.exe
        autotest.commands =/epoc32/RELEASE/WINSCW/udeb/$${TARGET}.exe -xml -o c:/$${TARGET}.xml
        }
}
QMAKE_EXTRA_TARGETS += test autotest

symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_PLATFORM_EXPORT_PATH(cplogger)
    TARGET.CAPABILITY = ALL -TCB
}



