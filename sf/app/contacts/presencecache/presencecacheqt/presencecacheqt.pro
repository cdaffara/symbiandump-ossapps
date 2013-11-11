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
TARGET = presencecacheqt
DEFINES += BUILD_PRESENCECACHE
symbian { 
    load(data_caging_paths)
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3=0x2002FFAA
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += .
		INCLUDEPATH += ./inc
		INCLUDEPATH += ../inc

BLD_INF_RULES.prj_exports += \
        "../../contacts_plat/presence_cache_api/inc/prcpresencereader_qt.h      APP_LAYER_PLATFORM_EXPORT_PATH(prcpresencereader_qt.h)" \
        "../../contacts_plat/presence_cache_api/inc/prcpresencewriter_qt.h      APP_LAYER_PLATFORM_EXPORT_PATH(prcpresencewriter_qt.h)" \
		"../../contacts_plat/presence_cache_api/inc/prcpresencebuddyinfo_qt.h  APP_LAYER_PLATFORM_EXPORT_PATH(prcpresencebuddyinfo_qt.h)" \ 
		"../../contacts_plat/presence_cache_api/inc/presencecacheglobal_qt.h APP_LAYER_PLATFORM_EXPORT_PATH(presencecacheglobal_qt.h)" \
        "./rom/presencecacheqt.iby                                           CORE_APP_LAYER_IBY_EXPORT_PATH(presencecacheqt.iby)" 
}


# Input
LIBS +=     -lpresencecacheutils \
            -lpresencecacheclient2 \
            -lfbscli.dll \
            -lbafl \


SOURCES +=  src/prcpresencereader_qt.cpp  \
			src/prcpresencewriter_qt.cpp \
			src/prcpresencereaderprivate_qt.cpp \
			src/prcpresencewriterprivate_qt.cpp \
			 src/prcpresencecachebuddyinfo_qt.cpp \
            src/prcpresencecachebuddyinfoprivate_qt.cpp
          
HEADERS +=  ../../contacts_plat/presence_cache_api/inc/prcpresencereader_qt.h \
			../../contacts_plat/presence_cache_api/inc/prcpresencewriter_qt.h \
			../../contacts_plat/presence_cache_api/inc/prcpresencebuddyinfo_qt.h \
			../../contacts_plat/presence_cache_api/inc/presencecacheglobal_qt.h \
			inc/prcpresencereaderprivate_qt.h \
			inc/prcpresencewriterprivate_qt.h \
            inc/prcpresencecachebuddyinfo_qt.h \
            inc/prcpresencecachebuddyinfoprivate_qt.h 
              
            



myDefInclude = "NOSTRICTDEF" \
	"$${LITERAL_HASH}if defined(WINS)"\
	"DEFFILE  bwins/ "\
	"$${LITERAL_HASH}else "\
	"DEFFILE  eabi/ "\
	"$${LITERAL_HASH}endif"
	MMP_RULES += myDefInclude

symbian:MMP_RULES += SMPSAFE