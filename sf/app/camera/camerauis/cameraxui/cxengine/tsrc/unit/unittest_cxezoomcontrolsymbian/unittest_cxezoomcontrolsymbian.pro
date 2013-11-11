# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

TARGET = unittest_cxezoomcontrolsymbian

LIBS *= -lecamsnapshot
LIBS *= -lecamadvsettings
LIBS *= -lecam
LIBS *= -lecampluginsupport
LIBS *= -lfbscli
LIBS *= -lbitgdi

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxezoomcontrolsymbian.cpp \
    cxezoomcontrolsymbian.cpp \
    cxestate.cpp \
    cxestatemachine.cpp \
    cxestatemachinebase.cpp \
    cxesettingsmappersymbian.cpp \
    cxedummycamera.cpp \
    cxecameradevice.cpp \
    cxefakecameradevice.cpp \
    cxefakesettings.cpp \
    cxefakefeaturemanager.cpp \
    cxecameradevicecontrolsymbian.cpp \
    cxefakecameradevicecontrol.cpp \
    cxeerrormappingsymbian.cpp

HEADERS *= unittest_cxezoomcontrolsymbian.h \
    cxezoomcontrolsymbian.h \
    cxezoomcontrol.h \
    cxestate.h \
    cxestatemachine.h \
    cxestatemachinebase.h \
    cxeerrormappingsymbian.h \
    cxesettingsmappersymbian.h \
    cxedummycamera.h \
    cxecameradevice.h \
    cxefakecameradevice.h \
    cxefeaturemanager.h \
    cxefakefeaturemanager.h \
    cxesettings.h \
    cxefakesettings.h \
    cxecameradevicecontrol.h \
    cxecameradevicecontrolsymbian.h \
    cxefakecameradevicecontrol.h \
    cxutils.h \
    cxetestutils.h \
    cxeerrormappingsymbian.h
