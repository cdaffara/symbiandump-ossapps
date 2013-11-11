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
TARGET = glxcommoncommandhandlers
DEPENDPATH += . inc src

CONFIG += hb

INCLUDEPATH += . \
            ../commandhandlerbase/inc \
            ../../../commonutilities/common/inc \
            ../commandexecutionwrapper/inc \
            ../../uiengine/medialists/inc \
            ../../../inc \
            ../../inc \
            ../../uiengine/model/mediamodel/inc \
            ../../uiengine/model/listmodel/inc \
            ../../../commonui/inc \
            ../../uiengine/medialistwrapper/inc \
            ../../../traces

DEFINES += BUILD_COMMONCOMMANDHANDLERS

symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x20000A0D
TARGET.CAPABILITY = ALL -TCB 
TARGET.EPOCALLOWDLLDATA = 1
MMP_RULES += SMPSAFE 
}

LIBS += -lglxcommandhandlerbase.dll \
        -lmpxcollectionutility.dll \
        -lmpxcommon.dll \
        -lglxcommon.dll \
       	-lglxlistmodel.dll \
        -lglxmedialists.dll \
        -lglxcommonui.dll \
        -lshareui.dll \
	-lexiflib.dll \
	-lfbscli.dll \
	-lbafl.dll \
	-lefsrv.dll \
	-lcaf.dll \
        -lxqservice.dll \
	-lxqserviceutil.dll \
	-lglxmediamodel.dll


# Input
HEADERS += inc/glxcommandhandlerdelete.h
HEADERS += inc/glxcommandhandleraddtocontainer.h
HEADERS += inc/glxcommandhandlernewmedia.h
HEADERS += inc/glxcommandhandlerrotate.h
HEADERS += inc/glxcommandhandlersend.h
HEADERS += inc/glxcommandhandlerremovefrom.h
HEADERS += inc/glxcommandhandlerrename.h
HEADERS += inc/glxcommondialogs.h
HEADERS += inc/glxcommandhandlercomment.h
HEADERS += inc/glxcommandhandlereditimage.h


SOURCES += src/glxcommandhandlerdelete.cpp
SOURCES += src/glxcommandhandleraddtocontainer.cpp
SOURCES += src/glxcommandhandlernewmedia.cpp
SOURCES += src/glxcommandhandlerrotate.cpp
SOURCES += src/glxcommandhandlersend.cpp
SOURCES += src/glxcommandhandlerremovefrom.cpp
SOURCES += src/glxcommandhandlerrename.cpp
SOURCES += src/glxcommondialogs.cpp
SOURCES += src/glxcommandhandlercomment.cpp
SOURCES += src/glxcommandhandlereditimage.cpp


DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxcommoncommandhandlers.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxcommoncommandhandlers.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

