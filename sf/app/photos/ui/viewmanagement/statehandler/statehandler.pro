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
TARGET = glxstatehandler
DEPENDPATH += . inc src

CONFIG += hb
CONFIG += mobility
MOBILITY += sensors

INCLUDEPATH += . \
            ../../inc \
            ../../../inc \
            ../../../loggers/loggerqt/inc \
            ../../../commonutilities/common/inc \
            ../../uiengine/medialists/inc \
            ../../uiengine/medialistwrapper/inc \
            ../../uiengine/model/mediamodel/inc \
            ../../uiengine/model/listmodel/inc \
            ../viewmanager/inc \
            ../../commandhandlers/commandhandlerbase/inc \
            ../../commandhandlers/commoncommandhandlers/inc
            
DEFINES += BUILD_STATEMANAGER

symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x20000A07
TARGET.CAPABILITY = ALL -TCB 
TARGET.EPOCALLOWDLLDATA = 1
MMP_RULES += SMPSAFE
}

LIBS += -lglxviewmanager.dll \
        -lglxmediamodel.dll \
        -lglxlistmodel.dll \
        -lglxexternalutility.dll \
        -lglxloggerqt.dll \
        -lglxcommoncommandhandlers.dll \
        -lglxlogging.dll \
        -lglxcommon.dll \
	-lglximageviewermanager.dll \
	-lafservice.dll


# Input
HEADERS += inc/glxbasestate.h \
           inc/glxfullscreenstate.h \
           inc/glxdetailstate.h \
           inc/glxgridstate.h \
           inc/glxslideshowsettingsstate.h \
           inc/glxliststate.h \
           inc/glxstatemanager.h \
           inc/glxslideshowstate.h \
           inc/glxactionhandler.h \
           inc/glxcommandhandlerfactory.h \
           inc/glxtnobserver.h \
           inc/glxorientationsensorfilter.h

SOURCES += src/glxbasestate.cpp \
           src/glxfullscreenstate.cpp \
           src/glxdetailstate.cpp \
           src/glxgridstate.cpp \
           src/glxslideshowsettingsstate.cpp \
           src/glxliststate.cpp \
           src/glxstatemanager.cpp \
           src/glxslideshowstate.cpp \
           src/glxactionhandler.cpp \
           src/glxcommandhandlerfactory.cpp \
           src/glxtnobserver.cpp \
           src/glxorientationsensorfilter.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxstatehandler.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxstatehandler.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

