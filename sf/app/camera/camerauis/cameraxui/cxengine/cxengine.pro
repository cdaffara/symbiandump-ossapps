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

include(../camerax.pri)
!symbian {
    CONFIG += staticlib
    CONFIG += HB
    HB += hbcore hbwidgets
} else {
    CONFIG  += dll
    DEFINES += CAMERAX_ENGINE_LIBRARY
    VERSION = 10.0.2
}

TEMPLATE = lib
TARGET = 
DEPENDPATH += inc inc/api src/dummyengine
INCLUDEPATH += inc \
               inc/sensor \
               inc/api \
               src/dummyengine \
               traces  \
               ../extensions/inc

VPATH += src
MOC_DIR = tmp

symbian {
    TARGET.CAPABILITY = ALL -TCB -DRM
    TARGET.UID3 = 0x20027016
    # Fix for QMake translating INCLUDEPATH to SYSTEMINCLUDE
    # and TraceCompiler needing USERINCLUDE.
    MMP_RULES           += "USERINCLUDE traces"
    MMP_RULES           += SMPSAFE

    # Libs
    LIBS += -lecam
    LIBS += -lecamadvsettings
    LIBS += -lecamsnapshot
    LIBS += -lfbscli
    LIBS += -lbitgdi
    LIBS += -lgdi
    LIBS += -lws32
    LIBS += -lcone
    LIBS += -lecamdirectviewfinder
    LIBS += -lmediaclientvideo
    LIBS += -lefsrv
    LIBS += -lsysutil
    LIBS += -lbafl
    LIBS += -lmediaclientaudio
    LIBS += -lecom
    LIBS += -lecampluginsupport
    LIBS += -lmmfcontrollerframework
    LIBS += -lplatformenv
    LIBS += -lxqsettingsmanager
    LIBS += -lsensrvclient
    LIBS += -lsensrvutil
    LIBS += -limagingconfigmanager
    LIBS += -lharvesterclient
    LIBS += -lthumbnailmanagerqt
    LIBS += -lxqserviceutil
    LIBS += -loommonitor
    LIBS += -lhal
    LIBS += -llocationmanager

    TARGET.EPOCALLOWDLLDATA = 1 // TODO: check this out??
}

# dependencies
#cxeenginecenrep.sources = cenrep/20027017.txt cenrep/20027018.txt
#cxeenginecenrep.path = /private/10202be9
# deploy it
#DEPLOYMENT += cxeenginecenrep


# Sound files
symbian {
    CXENGINE_DIR = $$section(PWD,":",1)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/data/autoFocus.wav    /epoc32/data/z/system/sounds/digital/autoFocus.wav"
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/data/capture.wav      /epoc32/data/z/system/sounds/digital/capture.wav"
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/data/videoStart.wav   /epoc32/data/z/system/sounds/digital/videoStart.wav"
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/data/videoStop.wav    /epoc32/data/z/system/sounds/digital/videoStop.wav"
}

# confml, crml files, backup registration definition
symbian {
    CXENGINE_DIR = $$section(PWD,":",1)
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/camerax.confml                    APP_LAYER_CONFML(camerax.confml)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/camerax_20027017.crml             APP_LAYER_CRML(camerax_20027017.crml)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/cameraxvariation.confml           APP_LAYER_CONFML(cameraxvariation.confml)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/cameraxvariation_20027018.crml    APP_LAYER_CRML(cameraxvariation_20027018.crml)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/backup_registration.xml           /epoc32/data/z/private/20027017/backup_registration.xml"
}


# Input
HEADERS += cxengine_global.h \
           cxememorymonitor.h \
           cxememorymonitorprivate.h \
           cxecameradevicecontrol.h \
           cxeengine.h \
           cxutils.h \
           cxeerror.h \
           cxenamespace.h \
           cxesettings.h \
           cxesettingsimp.h \
           cxefeaturemanager.h \
           cxefeaturemanagerimp.h \
           cxestatemachine.h \
           cxestate.h \
           cxestatemachinebase.h \
           cxestillimage.h \
           cxestillcapturecontrol.h \
           cxefilesavethread.h \
           cxesensoreventhandler.h \
           cxesettingsstore.h \
           cxequalitydetails.h \
           cxequalitypresets.h \
           cxeautofocuscontrol.h \
           cxevideocapturecontrol.h \
           cxeimagedataitem.h \
           cxeimagedataqueue.h \
           cxezoomcontrol.h \
           cxeviewfindercontrol.h \
           cxegeotaggingtrail.h \
           cxeexception.h \
           cxescenemodestore.h

symbian {

HEADERS+=  cxequalitypresetssymbian.h \
           cxecameradevicecontrolsymbian.h \
           cxecameradevice.h \
           cxeviewfindercontrolsymbian.h \
           cxevideocapturecontrolsymbian.h \
           cxesettingscontrolsymbian.h \
           cxestillcapturecontrolsymbian.h \
           cxeenginesymbian.h \
           cxeautofocuscontrolsymbian.h \
           cxezoomcontrolsymbian.h \
           cxeimagedataqueuesymbian.h \
           cxeimagedataitemsymbian.h \
           cxefilenamegeneratorsymbian.h \
           cxeerrormappingsymbian.h \
           cxesoundplayersymbian.h \
           cxevideorecorderutility.h \
           cxevideorecorderutilitysymbian.h \
           cxestillimagesymbian.h \
           cxesettingsmappersymbian.h \
           cxefilesavethreadsymbian.h \
           cxesensoreventhandlersymbian.h \
           cxesettingscenrepstore.h \
           cxesysutil.h \
           cxevideocontainer.h \
           cxedummycamera.h \
           cxeharvestercontrolsymbian.h \
           cxethumbnailmanagersymbian.h \
           cxediskmonitor.h \
           cxediskmonitorprivate.h \
           cxesnapshotcontrol.h \
           cxesnapshotcontrolprivate.h \
           cxegeotaggingtrail_symbian_p.h \
           sensor/xqsensor.h \
           sensor/xqaccsensor.h \
           sensor/xqdeviceorientation.h \
           sensor/xqdeviceorientation_p.h \
           sensor/xqsensor_p.h \
           sensor/xqaccsensor_p.h \
           traces/OstTraceDefinitions.h
           
} else {
HEADERS += cxeautofocuscontroldesktop.h \
           cxecameradevicecontroldesktop.h \
           cxeviewfindercontroldesktop.h \
           cxezoomcontroldesktop.h \
           cxefilenamegeneratordesktop.h \
           cxefilesavethreaddesktop.h \
           cxeimagedataitemdesktop.h \
           cxeimagedataqueuedesktop.h \
           cxevideocapturecontroldesktop.h \
           cxestillimagedesktop.h \
           cxestillcapturecontroldesktop.h \
           cxequalitypresets.h \
           cxequalitypresetsdesktop.h \
           cxesensoreventhandlerdesktop.h \
           cxesettingsstoredesktop.h \
           cxecameradevicedesktop.h \
           cxeviewfinderwidgetdesktop.h \
           cxegeotaggingtrail_desktop_p.h \
           cxeenginedesktop.h
}

SOURCES += cxeengine.cpp \
           cxefeaturemanagerimp.cpp \
           cxememorymonitor.cpp \
           cxememorymonitorprivate.cpp \
           cxestatemachine.cpp \
           cxestate.cpp \
           cxestatemachinebase.cpp \
           cxefilesavethread.cpp \
           cxegeotaggingtrail.cpp \
           cxesettingsimp.cpp \
           cxescenemodestore.cpp

symbian {
SOURCES += cxedummycamera.cpp \
           cxesysutil.cpp \
           cxevideocontainer.cpp \
           cxeenginesymbian.cpp \
           cxeviewfindercontrolsymbian.cpp \
           cxestillcapturecontrolsymbian.cpp \
           cxevideocapturecontrolsymbian.cpp \
           cxesettingscontrolsymbian.cpp \
           cxeautofocuscontrolsymbian.cpp \
           cxezoomcontrolsymbian.cpp \
           cxeimagedataqueuesymbian.cpp \
           cxeimagedataitemsymbian.cpp \
           cxefilenamegeneratorsymbian.cpp \
           cxecameradevice.cpp \
           cxesoundplayersymbian.cpp \
           cxecameradevicecontrolsymbian.cpp \
           cxeerrormappingsymbian.cpp \
           cxestillimagesymbian.cpp \
           cxevideorecorderutilitysymbian.cpp \
           cxesettingsmappersymbian.cpp \
           cxefilesavethreadsymbian.cpp \
           cxesensoreventhandlersymbian.cpp \
           cxesettingscenrepstore.cpp \
           cxequalitypresetssymbian.cpp \
           cxeharvestercontrolsymbian.cpp \
           cxethumbnailmanagersymbian.cpp \
           cxediskmonitor.cpp \
           cxediskmonitorprivate.cpp \
           cxesnapshotcontrol.cpp \
           cxesnapshotcontrolprivate.cpp \
           cxegeotaggingtrail_symbian_p.cpp \
           sensor/xqsensor.cpp \
           sensor/xqaccsensor.cpp \
           sensor/xqdeviceorientation.cpp \
           sensor/xqdeviceorientation_p.cpp \
           sensor/xqsensor_p.cpp \
           sensor/xqaccsensor_p.cpp
           
} else {
SOURCES += cxeautofocuscontroldesktop.cpp \
           cxecameradevicecontroldesktop.cpp \
           cxeviewfindercontroldesktop.cpp \
           cxezoomcontroldesktop.cpp \
           cxefilenamegeneratordesktop.cpp \
           cxefilesavethreaddesktop.cpp \
           cxeimagedataqueuedesktop.cpp \
           cxeimagedataitemdesktop.cpp \
           cxevideocapturecontroldesktop.cpp \
           cxestillimagedesktop.cpp \
           cxestillcapturecontroldesktop.cpp \
           cxequalitypresetsdesktop.cpp \
           cxesensoreventhandlerdesktop.cpp \
           cxesettingsstoredesktop.cpp \
           cxecameradevicedesktop.cpp \
           cxeviewfinderwidgetdesktop.cpp \
           cxegeotaggingtrail_desktop_p.cpp \
           cxeenginedesktop.cpp
}
