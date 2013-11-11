#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

TEMPLATE = lib
TARGET = msgerrorwatcher

CONFIG += hb

DEFINES += MSGERRORWATCHER_DLL

DEPENDPATH +=  ./inc ./src
INCLUDEPATH +=  ./inc 
INCLUDEPATH += ../../../inc 
INCLUDEPATH += ../../msgsettings/settingsview/inc 
INCLUDEPATH += ../../msgui/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
HEADERS += ./inc/msgerrorwatcher.h \
           ./inc/msgerrorwatcher_p.h \
			./inc/msgerrorroamingobserver.h \
			./inc/msgerrorstartupobserver.h  \
			./inc/msgerrorsmsdiskspaceobserver.h \
			./inc/msgerrorextsmsdiskspaceobserver.h \
			./inc/msgcenrepobserver.h \
			./inc/msgerrorcommdbobserver.h\
			./inc/msgerrordiskspaceobserver.h 

SOURCES +=  ./src/msgerrorwatcher.cpp \
            ./src/msgerrorwatcher_p.cpp \
           ./src/msgerrorroamingobserver.cpp \
			./src/msgerrorstartupobserver.cpp \
			./src/msgerrorsmsdiskspaceobserver.cpp \
			./src/msgerrorextsmsdiskspaceobserver.cpp \
			./src/msgcenrepobserver.cpp \
			./src/msgerrorcommdbobserver.cpp \
			./src/msgerrordiskspaceobserver.cpp 

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/msgerrorwatcher.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/msgerrorwatcher.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock			
LIBS +=   -lecom \
          -leuser \
          -lcommonengine \
          -lefsrv\
          -letel \
          -letelmm \   
          -lmsgs \
          -lcentralrepository \
          -lcenrepnotifhandler \   
          -llogcli \
          -llogwrap \
          -lcommdb \
          -lconnmon \
          -lHbCore \
          -lHbWidgets \
          -lHbUtils \
          -lQtCore \
          -lxqutils \
          -lflogger \
          -lcone \
          -leikcdlg \
          -leikctl \
          -leikcoctl \
          -leikcore \
          -lgdi \
          -lbafl \
          -lplatformenv \
          -lccon \
          -lestor \
          -lCdlEngine \
          -laknskins \
	        -laknicon \
	        -legul \
	        -lxqservice 
           

TARGET.UID3 = 0x2001FE78 #MSGERRORWATCHER_DLL_UID 
TARGET.CAPABILITY =  All -TCB
TARGET.EPOCALLOWDLLDATA = 1  

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/msgerrorwatcher.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(msgerrorwatcher.iby)"

# End of file	--Don't remove this.

