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
#

TEMPLATE = lib
TARGET = phoneservices
CONFIG += hb

symbian {
    TARGET.UID3 = 0x20029F88
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA =1
    DEFINES += BUILD_PHONESERVICES
    VERSION = 10.0
    INCLUDEPATH += ../../inc \
                   ../phonemodel/inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    servicesDefFiles = \
        "$${LITERAL_HASH}ifdef WINSCW" \
         "DEFFILE bwins/phoneservices.def" \
         "$${LITERAL_HASH}else" \
         "DEFFILE eabi/phoneservices.def" \
         "$${LITERAL_HASH}endif"
    
    MMP_RULES += servicesDefFiles

    LIBS += -lxqservice \
            -lxqserviceutil \
            -lfeatmgr \
            -lxqsettingsmanager
}

# Input
#include(\ext\mw\qthighway\xqservicebase.pri)
include(./phoneservices.pri)

headers.sources = $$PHONESERVICES_API_HEADERS
headers.path = |../../inc
# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$headers.path/$$basename(header)"       

BLD_INF_RULES.prj_exports += \
 "./rom/phoneservices_stub.sis /epoc32/data/z/system/install/phoneservices_stub.sis"