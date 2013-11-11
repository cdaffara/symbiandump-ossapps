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

TARGET = unittest_cxevideocapturecontrolsymbian

LIBS *= -lecamadvsettings
LIBS *= -lecam
LIBS *= -lecampluginsupport
LIBS *= -lfbscli
LIBS *= -lbitgdi
LIBS *= -lecamdirectviewfinder
LIBS *= -lecamsnapshot
LIBS *= -lmediaclientvideo
LIBS *= -lmediaclientaudio
LIBS *= -lmmfcontrollerframework
LIBS *= -lplatformenv
LIBS *= -lsysutil
LIBS *= -lefsrv
LIBS *= -lcone

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxevideocapturecontrolsymbian.cpp \
    cxevideocapturecontrolsymbian.cpp \
    cxevideocapturecontrolsymbianunit.cpp \
    cxefakevideorecorderutility.cpp \
    cxevideorecorderutilitysymbian.cpp \
    cxefakefilenamegenerator.cpp \
    cxedummycamera.cpp \
    cxefakecameradevice.cpp \
    cxecameradevice.cpp \
    cxefakeviewfindercontrol.cpp \
    cxefakecameradevicecontrol.cpp \
    cxefakesettings.cpp \
    cxesettingsimp.cpp \
    cxestatemachine.cpp \
    cxestate.cpp \
    cxestatemachinebase.cpp \
    cxesettingsmappersymbian.cpp \
    cxesoundplayersymbian.cpp \
    cxeerrormappingsymbian.cpp \
    cxefakequalitypresets.cpp \
    cxediskmonitor.cpp \
    cxediskmonitorprivate.cpp \
    cxesnapshotcontrol.cpp \
    cxesnapshotcontrolprivate.cpp \
    cxesysutil.cpp \
    cxescenemodestore.cpp

HEADERS *= unittest_cxevideocapturecontrolsymbian.h \
    cxevideocapturecontrolsymbian.h \
    cxevideocapturecontrolsymbianunit.h \
    cxevideorecorderutility.h \
    cxefakevideorecorderutility.h \
    cxevideorecorderutilitysymbian.h \
    cxevideocapturecontrol.h \
    cxefakefilenamegenerator.h \
    cxedummycamera.h \
    cxefakecameradevice.h \
    cxecameradevice.h \
    cxefakeviewfindercontrol.h \
    cxeviewfindercontrol.h \
    cxecameradevicecontrol.h \
    cxefakecameradevicecontrol.h \
    cxesettingsimp.h \
    cxesettings.h \
    cxefakesettings.h \
    cxestatemachine.h \
    cxestate.h \
    cxeerrormappingsymbian.h \
    cxestatemachinebase.h \
    cxesettingsmappersymbian.h \
    cxesoundplayersymbian.h \
    cxefakequalitypresets.h \
    cxutils.h \
    cxediskmonitor.h \
    cxediskmonitorprivate.h \
    cxesnapshotcontrol.h \
    cxesnapshotcontrolprivate.h \
    cxesysutil.h  \
    cxescenemodestore.h
