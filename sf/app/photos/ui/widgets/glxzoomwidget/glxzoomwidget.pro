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
TARGET = glxzoomwidget

CONFIG += hb

DEFINES += GLXZOOMWIDGET_LIBRARY

LIBS += -lglxmodelwrapper.dll \
        -lglximagedecoderwrapper.dll \

DEPENDPATH += . inc src
INCLUDEPATH += . \
            ../../../commonutilities/imagedecoderwrapper/inc \
            ../uiengine/model/modelwrapper/inc \
            ../../inc \
            ../../../inc 

symbian: { 
TARGET.UID3 = 0x200104E4
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = ALL -TCB
MMP_RULES += SMPSAFE 
}

exportfile = "inc/glxzoomwidget.h APP_LAYER_PLATFORM_EXPORT_PATH(glxzoomwidget.h)"
exportfile += "inc/glxzoomwidget_global.h APP_LAYER_PLATFORM_EXPORT_PATH(glxzoomwidget_global.h)"
BLD_INF_RULES.prj_exports += exportfile

# Input
HEADERS += inc/glxzoomwidget.h inc/glxzoomwidget_global.h
SOURCES += src/glxzoomwidget.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxzoomwidget.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxzoomwidget.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
