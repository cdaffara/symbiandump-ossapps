#/* 
#* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
#* All rights reserved.
#* This component and the accompanying materials are made available
#* under the terms of "Eclipse Public License v1.0"
#* which accompanies this distribution, and is available
#* at the URL "http://www.eclipse.org/legal/epl-v10.html".
#*
#* Initial Contributors:
#* Nokia Corporation - initial contribution.
#*
#* Contributors:
#* 
#* Description: Qt wrapper for DRM utility
#*
#*/ 
TEMPLATE = lib
TARGET = glxdrmutilitywrapper
DEPENDPATH += . inc src
INCLUDEPATH += . ../../inc \
			../../../inc \
			../drmutility/inc \
			./inc
			
CONFIG += hb
LIBS +=  -lglxdrmutility.dll \
		-lglxlogging.dll \

DEFINES += BUILD_DRMUTILITYWRAPPER

symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x2000A7BC
TARGET.CAPABILITY = ALL -TCB 
TARGET.EPOCALLOWDLLDATA = 1
MMP_RULES += SMPSAFE 

}	
# Input
HEADERS += inc/glxdrmutilitywrapper.h \
			inc/glxdrmutilitywrapper_p.h
			
SOURCES += src/glxdrmutilitywrapper.cpp \
			src/glxdrmutilitywrapper_p.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxdrmutilitywrapper.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxdrmutilitywrapper.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
