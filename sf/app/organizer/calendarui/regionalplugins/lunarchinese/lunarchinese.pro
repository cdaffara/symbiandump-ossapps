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
# Description: Project file for LunarChinese plugin module.
#

TEMPLATE = lib
CONFIG += hb
TARGET = calenlunarchineseplugin

DEPENDPATH +=  ./inc ./src
INCLUDEPATH +=  ./inc 
INCLUDEPATH += ../inc ../../inc \
				./traces


# Input
HEADERS += ./inc/CalenLunarChinesePlugin.h \
           ./inc/CalenRegionalInfoData.h

SOURCES +=  ./src/CalenLunarChinesePlugin.cpp \
            ./src/CalenLunarChinesePluginProxy.cpp \
            ./src/CalenRegionalInfoData.cpp 
          
LIBS +=    -lcalinterimapi \
			-lcone \
            -lecom \
            -leikcdlg \
            -leikctl \
            -leikcoctl \
            -leikcore \
            -leuser \
            -lgdi \
            -lbafl \
            -lplatformenv \
            -lefsrv \
            -lCommonEngine \
            -lccon \
            -lestor \
            -lcentralrepository \
            -lCdlEngine \
            -laknskins \
	    -laknicon \
	    -legul \
	    -lflogger \	 
            -lcalenregionalutil

symbian: {

INCLUDEPATH += /epoc32/include/ecom 

TARGET.UID2 = 0x10009D8D #SYMBIAN_ECOM_PLUGIN_UID 
TARGET.UID3 = 0x2002EA3C #CALENLUNARCHINESEPLUGIN_DLL_UID 
TARGET.CAPABILITY = CAP_ECOM_PLUGIN
TARGET.EPOCALLOWDLLDATA = 1

BLD_INF_RULES.prj_exports += \
"./data/solaritems  /epoc32/data/z/private/10005901/solaritems" \
"./data/solaritems  /epoc32/release/winscw/udeb/z/private/10005901/solaritems" \
"./data/solaritems  /epoc32/release/winscw/urel/z/private/10005901/solaritems" 

# MMP Rules
mmpRules = \
"$${LITERAL_HASH}include <platform_paths.hrh>" \
#"$${LITERAL_HASH}include <../inc/CalenRegionalPluginUids.h>" \
"$${LITERAL_HASH}include <data_caging_paths.hrh>" \
"APP_LAYER_SYSTEMINCLUDE" \
 \
"SOURCEPATH      ./data" \
"START RESOURCE 2002EA3C.rss" \
"TARGET calenlunarchineseplugin.rsc" \
"TARGETPATH ECOM_RESOURCE_DIR" \
"LANGUAGE_IDS" \
"END" \
\
"TARGETTYPE plugin"


MMP_RULES +=mmpRules

MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl -D__QT_NOEFFECTMACRO_DONOTUSE"

}

TRANSLATIONS += calenregional.ts

# End of file	--Don't remove this.