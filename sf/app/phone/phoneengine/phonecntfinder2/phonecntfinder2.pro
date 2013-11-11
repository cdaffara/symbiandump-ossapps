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
TARGET = phonecntfinder
CONFIG += qtgui
CONFIG += qtcore

TARGET.EPOCALLOWDLLDATA =1
TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0x10005998
TARGET.VID = VID_DEFAULT

VERSION = 10.0

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../inc 
                   
defFiles = \
    "$${LITERAL_HASH}ifdef WINSCW" \
     "DEFFILE bwins/phonecntfinder.def" \
     "$${LITERAL_HASH}else" \
     "DEFFILE eabi/phonecntfinder.def" \
     "$${LITERAL_HASH}endif"

MMP_RULES += defFiles

LIBS += -leuser \
            -lrfs \
            -fbscli.lib \
            -lprofileeng \
            -lcenrepnotifhandler \
            -lcentralrepository \
            -lecom \
            -lbafl \
            -lfeatmgr \
            -lcharconv \
            -lqtcontacts


HEADERS += ./inc/cphcntfactoryimpl2.h
HEADERS += ./inc/phcntpanic.h
HEADERS += ./inc/cphcntmatch2.h
HEADERS    += ./inc/cphcntcontactidimpl2.h            
HEADERS += ./inc/cphcntmatcherimpl2.h


SOURCES += ./src/cphcntfactoryimpl2.cpp
SOURCES  += ./src/phcntpanic.cpp
SOURCES  += ./src/cphcntmatch2.cpp
SOURCES    += ./src/cphcntcontactidimpl2.cpp
SOURCES  += ./src/cphcntmatcherimpl2.cpp

include(api_headers.pri)

headers.sources = $$CNTFINDER2_API_HEADERS
headers.path = |../../inc
# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$headers.path/$$basename(header)"       

BLD_INF_RULES.prj_exports += \
 "$${LITERAL_HASH}include <platform_paths.hrh>" \
 "./rom/phonecntfinder.iby    CORE_APP_LAYER_IBY_EXPORT_PATH(phonecntfinder.iby)"
 
BLD_INF_RULES.prj_exports += \
 "./rom/phonecntfinder_stub.sis /epoc32/data/z/system/install/phonecntfinder_stub.sis"


symbian:MMP_RULES += SMPSAFE