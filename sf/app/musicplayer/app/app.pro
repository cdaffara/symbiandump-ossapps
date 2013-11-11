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

SERVICEAPP = app
TARGET = musicplayer
CONFIG += hb
symbian: {
    TARGET.UID2 = 0x100039CE 
    TARGET.UID3 = 0x10207C62
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x1F00000
    TARGET.CAPABILITY = CAP_APPLICATION NetworkControl DRM
    SKINICON = qtg_large_music

    BLD_INF_RULES.prj_exports += \
        "../sis/musicplayer_stub.sis          z:/system/install/musicplayer_stub.sis" \
        "resources/musicplayer.docml          z:/resource/hb/splashml/musicplayer.docml" \
        "resources/musicplayer.splashml       z:/resource/hb/splashml/musicplayer.splashml"
    MMP_RULES += SMPSAFE
}

# Service provider specific configuration.
# If in-process plugin specific implementation
# will be possible in future, service configuration
# can be located in musicservices library's pro file.
SERVICE.FILE = data/service_conf.xml
SERVICE.OPTIONS = embeddable
CONFIG += service
# Service provider specific configuration ends

DEPENDPATH += .
INCLUDEPATH += . \
               inc \
               ../inc \
               ../musicservices/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lecom \
        -lestor  \
        -lfbscli \
        -lxqplugins \
        -lmpxviewframeworkqt \
        -lxqservice \
        -lxqserviceutil \
        -lmusicservices \
        -lmpsettingsmanager \
        -lmpengine \
        -lmpmediacontroller \
        -lafservice

# Input
HEADERS += inc/mpmainwindow.h \
           inc/mpglobalpopuphandler.h \
           inc/mpmtpinfolink.h

SOURCES += src/main.cpp \
           src/mpmainwindow.cpp \
           src/mpglobalpopuphandler.cpp \
           src/mpmtpinfolink.cpp


TRANSLATIONS = musicplayer.ts

RSS_RULES += \
"    datatype_list =" \
"        {" \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-midi\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/midi\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/sp-midi\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-beatnik-rmf\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-rmf\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/rmf\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/mobile-xmf\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/vnd.nokia.mobile-xmf\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-mrv.xml\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-mrv.wbxml\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-ms-wma\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"x-caf-audio/x-ms-wma\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-pn-realaudio\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-realaudio\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/vnd.rn-realaudio\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"playlist/mpegurl\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/mpeg\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/mp3\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/m4a\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-mp3\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/3gpp\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/3gpp2\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/mp4\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/aac\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/basic\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/amr\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/amr-wb\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/awb\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-amr\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-epoc-wve\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-sibo-wve\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/wav\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-wav\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-au\"; }," \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/au\"; }, " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"application/vnd.nokia.ringing-tone\"; }, " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-mpegurl\"; }, " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/mpegurl\"; }" \
"        };" \     