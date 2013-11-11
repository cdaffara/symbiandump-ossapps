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
include(../rom/camerax_rom.pri)

TEMPLATE = app
TARGET =
symbian {
    TARGET.CAPABILITY   = ALL -Tcb -DRM
    TARGET.UID3         = 0x101f857a
    TARGET.VID          = VID_DEFAULT
    TARGET.EPOCHEAPSIZE = 0x020000 0x1000000
    SKINICON            = qtg_large_camera
    # Fix for QMake translating INCLUDEPATH to SYSTEMINCLUDE
    # and TraceCompiler needing USERINCLUDE.
    MMP_RULES           += "USERINCLUDE traces"
    MMP_RULES           += SMPSAFE
    VERSION = 10.0.2
}

# export sound file
symbian {
    CXUI_DIR = $$section(PWD,":",1)
    BLD_INF_RULES.prj_exports += "$$CXUI_DIR/data/selftimer.wav    /epoc32/data/z/system/sounds/digital/selftimer.wav"
}

DEPENDPATH += ../cxengine/inc/api
VPATH += src \
    inc
INCLUDEPATH += inc \
               traces
!symbian {
    DEPENDPATH  += stubs_desktop/inc
    INCLUDEPATH += ../cxengine/inc/api \
                   stubs_desktop/inc
    DEFINES += CAMERAX_ENGINE_NO_LIBRARY
}

symbian {
    LIBS += -lecam  \
            -lecamsnapshot \
            -lws32 \
            -lbitgdi \
            -lfbscli \
            -lcxengine \
            -lapmime \
            -lcommonui \
            -lxqutils \
            -lxqservice \
            -lxqserviceutil \
            -lshareui \
            -lcone \
            -lefsrv \
            -lws32 \
            -lgdi \
            -lapgrfx \
            -lusbman \
            -lxqsettingsmanager \
            -lthumbnailmanagerqt \
            -lafservice
} else {
    win32 {
        debug {
            LIBS += ../cxengine/debug/libcxengine.a

        } else {
            LIBS += ../cxengine/release/libcxengine.a
        }        
    } else {
        LIBS += ../cxengine/libcxengine.a
    }
}

CONFIG += hb
CONFIG += service
QT += xml
SERVICE.FILE = cxui_service_conf.xml
SERVICE.OPTIONS = embeddable

HB += hbcore hbwidgets hbfeedback hbutils

# Input

DOCML += layouts/errornote_popup.docml \
    layouts/full_screen_popup.docml \
    layouts/setting.docml \
    layouts/setting_scenemode.docml \
    layouts/setting_slider.docml \
    layouts/view_postcapture.docml \
    layouts/view_still_precapture.docml \
    layouts/view_video_precapture.docml        

HEADERS += cxengine_global.h \
    cxuiapplication.h \
    cxuiapplicationframeworkmonitor.h \
    cxuiapplicationframeworkmonitorprivate.h \
    cxuiapplicationstate.h \
    cxuiview.h \
    cxuiprecaptureview.h \
    cxuistillprecaptureview.h \
    cxuivideoprecaptureview.h \
    cxuipostcaptureview.h \
    cxestillcapturecontrol.h \
    cxuicapturekeyhandler.h \
    cxuiviewmanager.h \
    cxuidocumentloader.h \
    cxuidisplaypropertyhandler.h \
    cxuisettingdialog.h \
    cxuisettingslider.h \
    cxuiselftimer.h \
    cxuisettingradiobuttonlist.h \
    cxuierrormanager.h \
    cxuisettingradiobuttonlistmodel.h \
    cxuisettingsinfo.h \
    cxuisettingxmlreader.h \
    cxuiserviceprovider.h \
    cxuiscenemodeview.h \
    cxuizoomslider.h \
    cxuifullscreenpopup.h \
    cxuieventlog.h 

symbian {
    HEADERS += traces/OstTraceDefinitions.h \
               cxuicapturekeyhandler_symbian_p.h
}
else {
    HEADERS += cxuimacrosdesktop.h \
               cxuicapturekeyhandler_desktop_p.h \
               shareui.h \
               thumbnailmanager_qt.h \
               xqappmgr.h \
               xqserviceprovider.h \
               XQUtils \
               afactivation.h \
               afactivities_global.h \
               afactivitystorage.h \
               xqrequestinfo.h
}

SOURCES += main.cpp \
    cxuiapplication.cpp \
    cxuiapplicationframeworkmonitor.cpp \
    cxuiapplicationframeworkmonitorprivate.cpp \
    cxuiapplicationstate.cpp \
    cxuiview.cpp \
    cxuiprecaptureview.cpp \
    cxuivideoprecaptureview.cpp \
    cxuistillprecaptureview.cpp \
    cxuipostcaptureview.cpp \
    cxuicapturekeyhandler.cpp \
    cxuiviewmanager.cpp \
    cxuidocumentloader.cpp \
    cxuidisplaypropertyhandler.cpp \
    cxuisettingdialog.cpp \
    cxuisettingslider.cpp \
    cxuiselftimer.cpp \
    cxuisettingradiobuttonlist.cpp \
    cxuierrormanager.cpp \
    cxuisettingradiobuttonlistmodel.cpp \
    cxuisettingsinfo.cpp \
    cxuisettingxmlreader.cpp \
    cxuiserviceprovider.cpp \
    cxuiscenemodeview.cpp \
    cxuizoomslider.cpp \
    cxuifullscreenpopup.cpp \
    cxuieventlog.cpp

symbian {
    SOURCES += cxuicapturekeyhandler_symbian_p.cpp
} else {
    SOURCES += cxuicapturekeyhandler_desktop_p.cpp
}

RESOURCES += cxui.qrc

# Variating internal and external icons for scene selection view
SCENEICONDIR = ../internal/icons
exists($$SCENEICONDIR) {
    RESOURCES += cxuiinternalsceneimages.qrc
}
else {
    RESOURCES += cxuiexternalsceneimages.qrc
}

TRANSLATIONS = camera.ts
