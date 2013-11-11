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

DEFINES *= CXE_USE_DUMMY_CAMERA

TARGET  = unittest_cxesettingscontrolsymbian

LIBS    *= -lbitgdi
LIBS    *= -lfbscli
LIBS    *= -lecam
LIBS    *= -lecamadvsettings
LIBS    *= -lecamsnapshot
LIBS    *= -lecamdirectviewfinder
LIBS    *= -lecampluginsupport

HEADERS *= unittest_cxesettingscontrolsymbian.h \
           cxesettingscontrolsymbian.h \
           cxesettings.h \
           cxefakesettings.h \
           cxecameradevice.h \
           cxefakecameradevice.h \
           cxecameradevicecontrol.h \
           cxefakecameradevicecontrol.h \
           cxesettingsmappersymbian.h \
           cxeerrormappingsymbian.h \
           cxedummycamera.h

SOURCES *= unittest_cxesettingscontrolsymbian.cpp \
           cxesettingscontrolsymbian.cpp \
           cxefakesettings.cpp \
           cxecameradevice.cpp \
           cxefakecameradevice.cpp \
           cxefakecameradevicecontrol.cpp \
           cxesettingsmappersymbian.cpp \
           cxeerrormappingsymbian.cpp \
           cxedummycamera.cpp