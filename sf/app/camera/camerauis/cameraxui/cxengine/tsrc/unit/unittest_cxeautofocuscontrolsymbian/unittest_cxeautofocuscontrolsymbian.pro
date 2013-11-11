# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

include(../unittest.pri)

TARGET = unittest_cxeautofocuscontrolsymbian

LIBS *= -lecamadvsettings
LIBS *= -lecam
LIBS *= -lecampluginsupport
LIBS *= -lfbscli
LIBS *= -lbitgdi
LIBS *= -lecamsnapshot

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxeautofocuscontrolsymbian.cpp \
    cxeautofocuscontrolsymbian.cpp \
    cxeautofocuscontrolsymbianunit.cpp \
    cxestatemachine.cpp \
    cxestate.cpp \
    cxestatemachinebase.cpp \
    cxeerrormappingsymbian.cpp \
    cxesettingsmappersymbian.cpp \
    cxedummycamera.cpp \
    cxecameradevice.cpp \
    cxefakecameradevice.cpp \
    cxecameradevicecontrolsymbian.cpp \
    cxefakecameradevicecontrol.cpp \
    cxefakesettings.cpp

HEADERS *= unittest_cxeautofocuscontrolsymbian.h \
    cxeautofocuscontrol.h \
    cxeautofocuscontrolsymbian.h \
    cxeautofocuscontrolsymbianunit.h \
    cxestatemachine.h \
    cxestate.h \
    cxestatemachinebase.h \
    cxeerrormappingsymbian.h \
    cxesettingsmappersymbian.h \
    cxedummycamera.h \
    cxecameradevice.h \
    cxefakecameradevice.h \
    cxecameradevicecontrol.h \
    cxecameradevicecontrolsymbian.h \
    cxefakecameradevicecontrol.h \
    cxutils.h \
    cxefakesettings.h \
    cxesettings.h \
    cxenamespace.h
