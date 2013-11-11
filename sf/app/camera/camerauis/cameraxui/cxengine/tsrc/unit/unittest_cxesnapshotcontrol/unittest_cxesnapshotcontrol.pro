# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

DEFINES *= CXE_USE_DUMMY_CAMERA

include(../unittest.pri)

TARGET = unittest_cxesnapshotcontrol

LIBS    *= -lecamsnapshot
LIBS    *= -lecamadvsettings
LIBS    *= -lecam
LIBS    *= -lecampluginsupport
LIBS    *= -lfbscli
LIBS    *= -lbitgdi

SOURCES *= unittest_cxesnapshotcontrol.cpp
SOURCES *= cxesnapshotcontrol.cpp
SOURCES *= cxesnapshotcontrolprivate.cpp
SOURCES *= cxedummycamera.cpp
SOURCES *= cxecameradevice.cpp
SOURCES *= cxefakecameradevice.cpp
SOURCES *= cxefakecameradevicecontrol.cpp
SOURCES *= cxeerrormappingsymbian.cpp
SOURCES *= cxestate.cpp
SOURCES *= cxestatemachine.cpp
SOURCES *= cxestatemachinebase.cpp

HEADERS *= unittest_cxesnapshotcontrol.h
HEADERS *= cxesnapshotcontrol.h
HEADERS *= cxesnapshotcontrolprivate.h
HEADERS *= cxedummycamera.h
HEADERS *= cxecameradevice.h
HEADERS *= cxefakecameradevice.h
HEADERS *= cxecameradevicecontrol.h
HEADERS *= cxefakecameradevicecontrol.h
HEADERS *= cxeerrormappingsymbian.h
HEADERS *= cxestate.h
HEADERS *= cxestatemachine.h
HEADERS *= cxestatemachinebase.h
