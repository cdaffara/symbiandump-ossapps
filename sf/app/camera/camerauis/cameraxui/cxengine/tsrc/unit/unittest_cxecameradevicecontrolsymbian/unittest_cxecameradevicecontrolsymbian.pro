#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

include(../unittest.pri)

TARGET = unittest_cxecameradevicecontrolsymbian

LIBS *= -lplatformenv
LIBS *= -lecamadvsettings
LIBS *= -lecamsnapshot
LIBS *= -lecam
LIBS *= -lecampluginsupport
LIBS *= -lfbscli
LIBS *= -lbitgdi
LIBS *= -lsysutil
LIBS *= -lefsrv
LIBS *= -lcone

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxecameradevicecontrolsymbian.cpp \
           cxecameradevicecontrolsymbian.cpp \
           cxecameradevicecontrolunit.cpp \
           cxecameradevice.cpp \
           cxefakecameradevice.cpp \
           cxefilenamegeneratorsymbian.cpp \
           cxeerrormappingsymbian.cpp \
           cxestatemachine.cpp \
           cxestate.cpp \
           cxestatemachinebase.cpp \
           cxedummycamera.cpp \
           cxesysutil.cpp

HEADERS *= unittest_cxecameradevicecontrolsymbian.h \
           cxecameradevicecontrolsymbian.h \
           cxecameradevicecontrolunit.h \
           cxecameradevicecontrol.h \
           cxecameradevice.h \
           cxefakecameradevice.h \
           cxestatemachine.h \
           cxestate.h \
           cxestatemachinebase.h \
           cxefilenamegenerator.h \
           cxesysutil.h




