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
######################################################################
# Unit tests
######################################################################
include(../../../../trunk_6742/trunk/hb/hb/hb.prf)

CONFIG += hb qtestlib
 LIBS += -lphoneengine \
            -lphoneuiutils \
            -lphoneuicontrol \
            -lphoneuiqtviewadapter \
            -lphoneuiqtview \
            -lFeatMgr
unix {
    test.commands = ./$(TARGET)
    autotest.commands = ./$(TARGET) -style plastique -xml -o ../$(QMAKE_TARGET).xml
} else:win32 {
    DESTDIR = ./
    test.CONFIG += recursive
    autotest.CONFIG += recursive
    build_pass {
        test.commands = $(TARGET)
        autotest.commands = $(TARGET) -xml -o ../$(QMAKE_TARGET).xml
    }
}
QMAKE_EXTRA_TARGETS += test autotest
