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
TARGET = glxviewmanager
DEPENDPATH += . inc src
INCLUDEPATH += . ../../inc \
                 ../../../inc \
                 ../../views/viewsfactory/inc \
                 ../../views/viewbase/inc \
                 ../../views/effectengine/inc \
                 ../../../loggers/loggerqt/inc \
                 ../../../commonutilities/externalutility/inc \
                 ../../viewutilities/effectengine/inc \
                 ../../viewutilities/settingutility/inc 
        
CONFIG += hb

LIBS +=  -lglxexternalutility.dll \
         -lglxviews.dll \
         -lglxloggerqt.dll \
         -lglxviewutilities.dll \
	 -lfeatdiscovery.dll \
	 -lxqsettingsmanager

DEFINES += BUILD_VIEWMANAGER
symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x20000A03
TARGET.CAPABILITY = ALL -TCB 
TARGET.EPOCALLOWDLLDATA = 1
MMP_RULES += SMPSAFE
}
# Input
HEADERS += inc/glxviewmanager.h \
           inc/glxmenumanager.h \
	   inc/glxmainwindoweventfilter.h
SOURCES += src/glxviewmanager.cpp\
           src/glxmenumanager.cpp 

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT           

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxviewmanager.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxviewmanager.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

