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
# Description:
TEMPLATE = lib

TARGET = irqutility
TARGET.CAPABILITY += CAP_GENERAL_DLL

MOC_DIR = moc
DEPENDPATH = src
DEFINES += BUILD_IRQCOMMMON_DLL
defFilePath = ..
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

INCLUDEPATH += ../../internetradio2.0/ircommon/inc \
               ../../internetradio2.0/irpresetplugin/inc \
               ../../presetserver/include \
               ../../presetserver/clientinc \
               ../../presetserver/serverinc

LIBS += -lirdatastructures \
        -lfbscli \
        -lcone \
        -lws32 \
        -lapgrfx \
        -lxqsettingsmanager \
        -lapmime \
        -lapparc \
        -lpsclient100
        
symbian{
  TARGET.UID3 = 0x2002FFBB
  TARGET.EPOCALLOWDLLDATA = 1
  MMP_RULES += SMPSAFE
}
       
#input

#header files list
HEADERS += inc/irqisdsdatastructure.h \
           inc/irqutility.h \
           inc/irservicedata.h \
           inc/irservicedef.h
                       

#source files list
SOURCES += irqisdsdatastructure.cpp \
           irqutility.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }