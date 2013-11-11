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
TARGET = glxexternalutility
DEPENDPATH += . inc src
INCLUDEPATH += . ../../inc 

CONFIG += hb

DEFINES += BUILD_EXTERNALUTILITY
symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x20007192
TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = ALL -TCB
MMP_RULES += SMPSAFE  
}
exportfile = "inc/glxexternalutility.h APP_LAYER_PLATFORM_EXPORT_PATH(glxexternalutility.h)"
BLD_INF_RULES.prj_exports += exportfile

# Input
HEADERS += inc/glxexternalutility.h 
SOURCES += src/glxexternalutility.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxexternalutility.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxexternalutility.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
