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

TARGET = unittest_cxecameradevice

LIBS *= -lecamadvsettings
LIBS *= -lecamsnapshot
LIBS *= -lecam
LIBS *= -lecampluginsupport
LIBS *= -lfbscli
LIBS *= -lbitgdi

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxecameradevice.cpp \
    cxestate.cpp \
    cxestatemachinebase.cpp \
    cxestatemachine.cpp \
    cxedummycamera.cpp \
    cxecameradevice.cpp \
    cxeerrormappingsymbian.cpp
HEADERS *= unittest_cxecameradevice.h \
    cxestate.h \
    cxestatemachinebase.h \
    cxestatemachine.h \
    cxedummycamera.h \
    cxecameradevice.h \
    cxutils.h \
    cxeerrormappingsymbian.h
