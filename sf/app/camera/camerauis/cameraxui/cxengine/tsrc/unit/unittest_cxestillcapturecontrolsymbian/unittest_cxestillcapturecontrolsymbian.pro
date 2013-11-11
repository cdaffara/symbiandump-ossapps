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

TARGET = unittest_cxestillcapturecontrolsymbian

LIBS *= -lecamadvsettings
LIBS *= -lecamsnapshot
LIBS *= -lecamdirectviewfinder
LIBS *= -lfbscli
LIBS *= -lbitgdi
LIBS *= -lsysutil
LIBS *= -lbafl
LIBS *= -lmediaclientaudio
LIBS *= -lecom
LIBS *= -lecam
LIBS *= -lecamsnapshot
LIBS *= -lecampluginsupport
LIBS *= -lmmfcontrollerframework
LIBS *= -lxqsettingsmanager
LIBS *= -lsensrvclient
LIBS *= -lsensrvutil
LIBS *= -lplatformenv
LIBS *= -lefsrv
LIBS *= -lcone

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxestillcapturecontrolsymbian.cpp \
    cxestillcapturecontrolsymbian.cpp \
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
    cxeimagedataqueuesymbian.cpp \
    cxefakefilenamegenerator.cpp \
    cxefakesensoreventhandler.cpp \
    cxestillimagesymbian.cpp \
    cxefakequalitypresets.cpp \
    cxesensoreventhandlersymbian.cpp \
    cxeimagedataitemsymbian.cpp \
    sensor/xqdeviceorientation.cpp \
    sensor/xqdeviceorientation_p.cpp \
    sensor/xqaccsensor.cpp \
    sensor/xqaccsensor_p.cpp \
    sensor/xqsensor.cpp \
    sensor/xqsensor_p.cpp \
    cxesettingsimp.cpp \
    cxefakeviewfindercontrol.cpp \
    cxesoundplayersymbian.cpp \
    cxesysutil.cpp \
    cxefakeautofocuscontrol.cpp \
    cxefakesettings.cpp \
    cxefilesavethread.cpp \
    cxediskmonitor.cpp \
    cxediskmonitorprivate.cpp \
    cxesnapshotcontrol.cpp \
    cxesnapshotcontrolprivate.cpp \
    cxefakefilesavethread.cpp \
    cxescenemodestore.cpp

HEADERS *= unittest_cxestillcapturecontrolsymbian.h \
    cxestillcapturecontrolsymbian.h \
    cxestate.h \
    cxestatemachine.h \
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
    cxeimagedataitem.h \
    cxeimagedataitemsymbian.h \
    cxeimagedataqueue.h \
    cxeimagedataqueuesymbian.h \
    cxestillcapturecontrol.h \
    cxefakefilenamegenerator.h \
    cxefakesensoreventhandler.h \
    cxestillimagesymbian.h \
    cxesensoreventhandler.h \
    cxesensoreventhandlersymbian.h \
    sensor/xqdeviceorientation.h \
    sensor/xqdeviceorientation_p.h \
    sensor/xqaccsensor.h \
    sensor/xqsensor.h \
    sensor/xqsensor_p.h \
    sensor/xqaccsensor_p.h \
    cxesettingsimp.h \
    cxesettings.h \
    cxecenrepkeys.h \
    cxefakeviewfindercontrol.h \
    cxeviewfindercontrol.h \
    cxesoundplayersymbian.h \
    cxesysutil.h \
    cxefakeautofocuscontrol.h \
    cxeautofocuscontrol.h \
    cxefakesettings.h \
    cxefakequalitypresets.h \
    cxefakefilesavethread.h \
    cxefilesavethread.h \
    cxediskmonitor.h \
    cxediskmonitorprivate.h \
    cxesnapshotcontrol.h \
    cxesnapshotcontrolprivate.h \
    cxequalitypresets.h \
    cxescenemodestore.h
