#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

TMP_DIR_NAME = app
include(../buildflags.pri)

TEMPLATE        = app
TARGET          = fmradio
TRANSLATIONS    += fmradio.ts 

symbian: {
    TARGET.CAPABILITY       = CAP_APPLICATION MultimediaDD
    TARGET.UID3             = 0x2002FF4E
    TARGET.EPOCSTACKSIZE    = 0x14000
    TARGET.EPOCHEAPSIZE     = 0x020000 0x2400000
    SKINICON                = qtg_large_radio
    ICON                    = resources/qtg_large_radio.svg # Fallback icon in case theme doesn't have one

    LIBS                    += -lxqserviceutil
    LIBS                    += -lxqsettingsmanager

    BLD_INF_RULES.prj_exports += "resources/fmradio.docml       /epoc32/release/winscw/udeb/z/resource/hb/splashml/fmradio.docml" \
                                 "resources/fmradio.splashml    /epoc32/release/winscw/udeb/z/resource/hb/splashml/fmradio.splashml" \
                                 "resources/fmradio.docml       /epoc32/data/z/resource/hb/splashml/fmradio.docml" \
                                 "resources/fmradio.splashml    /epoc32/data/z/resource/hb/splashml/fmradio.splashml"
}

CONFIG      += hb service

# Service provider specific configuration.
SERVICE.FILE = resources/service_conf.xml

INCLUDEPATH += . inc
INCLUDEPATH += ../../common
INCLUDEPATH += ../radiouiengine/inc
INCLUDEPATH += ../radiowidgets/inc

LIBS        += -lfmradiouiengine
LIBS        += -lfmradiowidgets

# UI does not normally depend on the wrapper, but it does on two special cases
# 1. Logging is enabled. The logger is in the wrapper module
# 2. Radio is built for win32 target. The test window needs to access the wrapper
LOGGING_ENABLED:LIBS += -lfmradioenginewrapper

# Sources
HEADERS     += radioapplication.h

SOURCES     += main.cpp
SOURCES     += radioapplication.cpp

# Win32 specific stuff
win32: {
    LIBS += -lfmradioenginewrapper

    HEADERS += testwindow_win32.h

    SOURCES += testwindow_win32.cpp

    DEFINES += HB_RESOURCES_DIR=\"\\\"$${HB_RESOURCES_DIR}\\\"\"
}

DEPENDPATH  += $$INCLUDEPATH src
