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
TARGET = glximagedecoderwrapper
DEPENDPATH += . inc src
INCLUDEPATH += . inc
		
		
		
CONFIG += hb
LIBS += -limageconversion.dll \
	-lbitmaptransforms.dll \
	-liclextjpegapi.dll \
	-lfbscli.dll \
	-lefsrv.dll \
	-lapmime.dll \
	-lapgrfx.dll

DEFINES += BUILD_IMAGEWRAPPER
symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x2000718D 
TARGET.CAPABILITY = ALL -TCB
TARGET.EPOCALLOWDLLDATA = 1
MMP_RULES += SMPSAFE 
}


# Input
HEADERS += inc/glximagedecoder.h inc/glximagedecoderwrapper.h
SOURCES += src/glximagedecoder.cpp src/glximagedecoderwrapper.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glximagedecoderwrapper.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glximagedecoderwrapper.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
