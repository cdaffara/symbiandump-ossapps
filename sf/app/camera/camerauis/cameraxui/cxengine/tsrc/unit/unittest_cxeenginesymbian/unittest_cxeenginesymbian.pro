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

TARGET = unittest_cxeenginesymbian

DEFINES *= CXE_USE_DUMMY_CAMERA

INCLUDEPATH *= ../../../inc/sensor

LIBS *= -lecam
LIBS *= -lecamadvsettings
LIBS *= -lecamsnapshot
LIBS *= -lecamdirectviewfinder
LIBS *= -lfbscli
LIBS *= -lbitgdi
LIBS *= -lxqsettingsmanager
LIBS *= -lecampluginsupport
LIBS *= -lmediaclientvideo
LIBS *= -lefsrv
LIBS *= -lsysutil
LIBS *= -lbafl
LIBS *= -lmediaclientaudio
LIBS *= -lecom
LIBS *= -lmmfcontrollerframework
LIBS *= -lplatformenv
LIBS *= -lsensrvclient
LIBS *= -lsensrvutil
LIBS *= -limagingconfigmanager
LIBS *= -lxqserviceutil
LIBS *= -loommonitor
LIBS += -llocationmanager
LIBS += -lcone
LIBS *= -lws32
LIBS += -lgdi
LIBS += -lhal


SOURCES *= unittest_cxeenginesymbian.cpp \
           cxeenginesymbianunit.cpp \
           cxeenginesymbian.cpp \
           cxeengine.cpp \
           cxesettingscontrolsymbian.cpp \
           cxefakeautofocuscontrol.cpp \
           cxeautofocuscontrolsymbian.cpp \
           cxevideorecorderutilitysymbian.cpp \
           cxecameradevicecontrolsymbian.cpp \
           cxefakecameradevicecontrol.cpp \
           cxefakecameradevice.cpp \
           cxecameradevice.cpp \
           cxefakestillcapturecontrol.cpp \
           cxestillcapturecontrolsymbian.cpp \
           cxefakeviewfindercontrol.cpp \
           cxeviewfindercontrolsymbian.cpp \
           cxevideocontainer.cpp \
           cxefakevideocapturecontrol.cpp \
           cxestatemachine.cpp \
           cxestatemachinebase.cpp \
           cxestate.cpp \
           cxesettingscenrepstore.cpp \
           cxefilenamegeneratorsymbian.cpp \
           cxefakefilenamegenerator.cpp \
           cxezoomcontrolsymbian.cpp \
           cxefakezoomcontrol.cpp \
           cxevideocapturecontrolsymbian.cpp \
           cxefeaturemanagerimp.cpp \
           cxefakefeaturemanager.cpp \
           cxesettingsimp.cpp \
           cxesettingsmappersymbian.cpp \
           cxefakesettings.cpp \
           cxesoundplayersymbian.cpp \
           cxesensoreventhandlersymbian.cpp \
           cxefakesensoreventhandler.cpp \
           cxeerrormappingsymbian.cpp \
           cxefilesavethread.cpp \
           cxefilesavethreadfactoryunit.cpp \
           cxeimagedataitemsymbian.cpp \
           cxeimagedataqueuesymbian.cpp \
           cxestillimagesymbian.cpp \
           cxedummycamera.cpp \
           cxesysutil.cpp \
           sensor/xqsensor.cpp \
           sensor/xqaccsensor.cpp \
           sensor/xqdeviceorientation.cpp \
           sensor/xqdeviceorientation_p.cpp \
           sensor/xqsensor_p.cpp \
           sensor/xqaccsensor_p.cpp \
           cxefakequalitypresets.cpp \
           cxequalitypresetssymbian.cpp \
           cxediskmonitor.cpp \
           cxediskmonitorprivate.cpp \
           cxememorymonitor.cpp \
           cxememorymonitorprivate.cpp \
           cxesnapshotcontrol.cpp \
           cxesnapshotcontrolprivate.cpp \
           cxegeotaggingtrail.cpp \
           cxegeotaggingtrail_symbian_p.cpp \
           cxescenemodestore.cpp          



HEADERS *= unittest_cxeenginesymbian.h \
           cxeenginesymbianunit.h \
           cxeenginesymbian.h \
           cxengine_global.h \
           cxeengine.h \
           cxeautofocuscontrol.h \
           cxeautofocuscontrolsymbian.h \
           cxefakeautofocuscontrol.h \
           cxevideorecorderutility.h \
           cxevideorecorderutilitysymbian.h \
           cxecameradevice.h \
           cxecameradevicecontrol.h \
           cxefakecameradevicecontrol.h \
           cxecameradevicecontrolsymbian.h \
           cxefakecameradevice.h \
           cxestillcapturecontrol.h \
           cxefakestillcapturecontrol.h \
           cxestillcapturecontrolsymbian.h \
           cxeviewfindercontrol.h \
           cxefakeviewfindercontrol.h \
           cxeviewfindercontrolsymbian.h \
           cxevideocontainer.h \
           cxevideocapturecontrol.h \
           cxefakevideocapturecontrol.h \
           cxevideocapturecontrolsymbian.h \
           cxestatemachine.h \
           cxestatemachinebase.h \
           cxestate.h \
           cxesettings.h \
           cxesettingsimp.h \
           cxefakesettings.h \
           cxesettingscenrepstore.h \
           cxesettingsmappersymbian.h \
           cxesettingscontrolsymbian.h \
           cxefilenamegeneratorsymbian.h \
           cxefakefilenamegenerator.h \
           cxezoomcontrolsymbian.h \
           cxefakezoomcontrol.h \
           cxezoomcontrol.h \
           cxefeaturemanager.h \
           cxefeaturemanagerimp.h \
           cxefakefeaturemanager.h \
           cxesoundplayersymbian.h \
           cxesensoreventhandlersymbian.h \
           cxesensoreventhandler.h \
           cxefakesensoreventhandler.h \
           cxeerrormappingsymbian.h \
           cxefilesavethread.h \
           cxeimagedataitemsymbian.h \
           cxeimagedataitem.h \
           cxeimagedataqueuesymbian.h \
           cxeimagedataqueue.h \
           cxestillimagesymbian.h \
           cxestillimage.h \
           cxedummycamera.h \
           cxesysutil.h \
           sensor/xqsensor.h \
           sensor/xqaccsensor.h \
           sensor/xqdeviceorientation.h \
           sensor/xqdeviceorientation_p.h \
           sensor/xqsensor_p.h \
           sensor/xqaccsensor_p.h \
           cxequalitypresets.h \
           cxefakequalitypresets.h \
           cxequalitypresetssymbian.h \
           cxediskmonitor.h \
           cxediskmonitorprivate.h \
           cxememorymonitor.h \
           cxememorymonitorprivate.h \
           cxesnapshotcontrol.h \
           cxesnapshotcontrolprivate.h \
           cxegeotaggingtrail.h \
           cxegeotaggingtrail_symbian_p.h \
           cxescenemodestore.h
