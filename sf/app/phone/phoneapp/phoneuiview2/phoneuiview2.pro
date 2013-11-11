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
# Description: Project file for building Phone qt view component
#
#

TEMPLATE = lib
TARGET = phoneuiqtview
CONFIG += hb mobility
MOBILITY += systeminfo

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA =1
    TARGET.UID3 = 0x101F4D18
    DEFINES += FT_SYMBIAN_INTEGRATION
    DEFINES += BUILD_PHONEUIQTVIEW
    VERSION = 10.0
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += ../../inc \
                   ../phoneuiutils/inc

    LIBS += -lphoneuiutils \
            -lbubblemanager2 \
            -lxqserviceutil \
            -ldialpad \
            -lxqkeycapture

    defFiles = \
        "$${LITERAL_HASH}ifdef WINS" \
            "DEFFILE bwins/phoneuiqtview.def" \
        "$${LITERAL_HASH}else" \
            "DEFFILE eabi/phoneuiqtview.def" \
        "$${LITERAL_HASH}endif"
    
    MMP_RULES += defFiles

    BLD_INF_RULES.prj_exports += "./rom/phoneuiqtview_stub.sis /epoc32/data/z/system/install/phoneuiqtview_stub.sis"
}

# Input
include(phoneuiqtview.pri)


symbian:MMP_RULES += SMPSAFE
