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
#* Description:
#*
#*/ 
TEMPLATE = lib
TARGET = glxmedialistwrapper
DEPENDPATH += . inc src
INCLUDEPATH += . ../../inc \
		 ../../../inc \
		 ../../../commonui/inc \
		 ../../../commonutilities/common/inc \
             ../../../loggers\loggerqt\inc \
		 ../medialists/inc  \
		 ../drmutility/inc  \
	         /photos/engine/collectionframework/plugins/glxcollectionpluginall/inc          
CONFIG += hb
LIBS += -lglxmedialists.dll \
	-lglxdrmutility.dll \
	-lglxlogging.dll \
	-lmpxcommon.dll \
	-lfbscli.dll \ 
	-laknicon.dll \
	-lglxcommon.dll \
      -lglxloggerqt.dll

DEFINES += BUILD_MLWRAPPER

symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x20000A0B
TARGET.CAPABILITY = ALL -TCB 
TARGET.EPOCALLOWDLLDATA = 1
MMP_RULES += SMPSAFE 

}	
# Input
HEADERS += inc/glxmlgenericobserver.h inc/glxmlwrapper.h inc/glxmlwrapper_p.h inc/glxattributeretriever.h inc/glxtitlefetcher.h
SOURCES += src/glxmlgenericobserver.cpp \
           src/glxmlwrapper.cpp \
           src/glxmlwrapper_p.cpp \
	       src/glxattributeretriever.cpp \
		   src/glxtitlefetcher.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxmedialistwrapper.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxmedialistwrapper.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
