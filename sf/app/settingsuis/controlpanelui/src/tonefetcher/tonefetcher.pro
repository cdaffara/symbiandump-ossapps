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

TEMPLATE = app
TARGET = tonefetcher
CONFIG += hb service

include( common.pri )
include( tonefetcher.pri )
LIBS += -lxqservice \
    -lxqserviceutil \
    -lcplogger \
    -lcpframework \
    -lxqutils
symbian {
    LIBS += -lcafutils \
            -lcaf \
            -lmdeclient \
            -lcentralrepository \
            -lProfileEng \
            -lpeninputClient \
            -lmediaclientaudio \
            -lDrmAudioPlayUtility \
            -lmediaclientvideo \
            -lDRMCommon \
            -lDrmRights \
            -lDrmHelper \
            -ldrmutility \
            -lapmime \
            -lecom \
            -lcone \
            -lapgrfx

            TARGET.UID3 = 0x2002BCCA
            TARGET.CAPABILITY = ALL -TCB
            TARGET.VID = VID_DEFAULT
            BLD_INF_RULES.prj_exports += "./service_conf.xml z:/private/2002BCCA/service_conf.xml"
}
symbian {
    include(rom/rom.pri)
}

             
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden     
libFiles.sources = xqservice.dll
libFiles.path = "!:\sys\bin"
DEPLOYMENT += libFiles


symbian:MMP_RULES += SMPSAFE
