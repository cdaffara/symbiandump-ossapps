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
# Description: Project file for calenregionalutil module.
#

TEMPLATE = lib
CONFIG += hb
TARGET = calenregionalutil

DEPENDPATH +=  ./inc \
							 ./src \
							 ./data
							 
INCLUDEPATH +=  ./inc 
INCLUDEPATH += ../inc \
			   ../../inc \
			   ./traces

# Input
HEADERS += 	./inc/calenextrarowformatter.h \
						./inc/calenlunarinfo.h \
						./inc/calenlunarinfoprovider.h \
						./inc/calenlunarlocalizedinfo.h \
						./inc/calenlunarlocalizer.h \
						./inc/calensolarterms.h \
						./inc/calenregionalpluginuids.h \
						./inc/calenlunarpaths.h \
						./inc/calenlunarpanic.h
 

SOURCES +=  ./src/calenextrarowformatter.cpp \
						./src/calenlunarinfo.cpp \
						./src/calenlunarinfoprovider.cpp \
						./src/calenlunarlocalizedinfo.cpp \
						./src/calenlunarlocalizer.cpp \
						./src/calensolarterms.cpp
       
       

            
RESOURCES += calenregionalutil.qrc
 
symbian: {

	TARGET.UID2 = 0x1000008d 
	TARGET.UID3 = 0x20022EDB
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.EPOCALLOWDLLDATA = 1
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	BLD_INF_RULES.prj_exports += \
		"./inc/calenextrarowformatter.h			|../inc/calenextrarowformatter.h" \
		"./inc/calenlunarinfo.h							|../inc/calenlunarinfo.h" \
		"./inc/calenlunarinfoprovider.h     |../inc/calenlunarinfoprovider.h" \
		"./inc/calenlunarlocalizedinfo.h   	|../inc/calenlunarlocalizedinfo.h" \
		"./inc/calenlunarlocalizer.h   			|../inc/calenlunarlocalizer.h" \
		"./inc/calenlunarpanic.h   					|../inc/calenlunarpanic.h" \
		"./inc/CalenLunarPaths.h						|../inc/CalenLunarPaths.h" \
		"./inc/CalenRegionalPluginUids.h		|../inc/CalenRegionalPluginUids.h" \
		"./inc/CalenSolarTerms.h						|../inc/CalenSolarTerms.h"
		
		LIBS +=    	-lcalinterimapi \
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
            -lCdlEngine
}


# End of file	--Don't remove this.