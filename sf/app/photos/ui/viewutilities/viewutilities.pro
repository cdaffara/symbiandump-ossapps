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
TARGET = glxviewutilities

DEPENDPATH += . settingutiliy/inc settingutility/src
CONFIG += hb

LIBS += -lxqsettingsmanager

DEFINES += BUILD_GLXVIEWUTILITIES

INCLUDEPATH += . ../inc \
          ../../inc \
          ../../loggers/loggerqt/inc \
          ../../traces \
          /epoc32/include/mw/hgwidgets

symbian: { 
TARGET.UID3 = 0x2000718E
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = ALL -TCB 
MMP_RULES += SMPSAFE
}

# Input
HEADERS += settingutility/inc/glxsetting.h \
           settingutility/inc/glxsettinginterface.h \
           effectplugin/inc/glxeffectpluginbase.h \
           effectplugin/inc/glxfadeeffectplugin.h \
           effectplugin/inc/glxflipeffectplugin.h \
           effectplugin/inc/glxzoominoutplugin.h \
           effectplugin/inc/glxhelixeffectplugin.h \
           effectplugin/inc/glxeffectpluginresolver.h \
           effectengine/inc/glxtransitioneffect.h \
           effectengine/inc/glxeffectengine.h 
	   	     
SOURCES += settingutility/src/glxsetting.cpp \
           settingutility/src/glxsettinginterface.cpp \
           effectplugin/src/glxfadeeffectplugin.cpp \
           effectplugin/src/glxflipeffectplugin.cpp \
           effectplugin/src/glxzoominoutplugin.cpp \
           effectplugin/src/glxhelixeffectplugin.cpp \
           effectplugin/src/glxeffectpluginresolver.cpp \
           effectengine/src/glxtransitioneffect.cpp \
           effectengine/src/glxeffectengine.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT 

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxviewutilities.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxviewutilities.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
