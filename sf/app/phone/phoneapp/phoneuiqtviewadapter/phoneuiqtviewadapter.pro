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
# Description: Project file for building qt view adapter component
#
#

TEMPLATE = lib
TARGET = phoneuiqtviewadapter
CONFIG += hb mobility
MOBILITY = publishsubscribe

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20029F86
    DEFINES += FT_SYMBIAN_INTEGRATION
    DEFINES += BUILD_PHONEUIQTVIEWADAPTER
    VERSION = 10.0
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += ../../inc \
                   ../phoneuiutils/inc \
                   ../phoneuiview2/inc \
                   ../phoneui2/srcdata \
                   ../phoneringingtoneplayer/inc \
                   ../phonemediatorcenter/inc

    LIBS += -lphoneuiutils \
        -lbubblemanager2 \
        -lphoneringingtoneplayer \
        -lphoneuiqtview \
        -lphonemediatorcenter \
        -lxqservice \
        -lxqserviceutil \
        -lserviceprovidersettings \
        -lxqsettingsmanager \
        -llogsengine \
        -lxqplugins \
        -lakncapserverclient \
        -llockclient \
        -ltelephonyservice \
        -lapgrfx \
        -lws32 \
        -lcone \
        -lengineinfo \
        -lphonestringloader \
        -lbafl \
        -lxqsystemtoneservice
        
        
    defFiles = "$${LITERAL_HASH}ifdef WINS" \
        "DEFFILE bwins/phoneuiqtviewadapter.def" \
        "$${LITERAL_HASH}else" \
            "DEFFILE eabi/phoneuiqtviewadapter.def" \
        "$${LITERAL_HASH}endif"
    
    MMP_RULES += defFiles

#qcrml export
    BLD_INF_RULES.prj_exports += "./conf/carmode.qcrml  c:/resource/qt/crml/carmode.qcrml "
    crml.sources = ./conf/carmode.qcrml
    crml.path = c:/resource/qt/crml
    DEPLOYMENT += crml

    BLD_INF_RULES.prj_exports += "./rom/phoneuiqtviewadapter_stub.sis /epoc32/data/z/system/install/phoneuiqtviewadapter_stub.sis"
}

# Input
include(phoneuiqtviewadapter.pri)


symbian:MMP_RULES += SMPSAFE
