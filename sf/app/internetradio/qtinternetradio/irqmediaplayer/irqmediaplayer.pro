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
TARGET = irqmediaplayer
TARGET.CAPABILITY = CAP_GENERAL_DLL

defFilePath = ..
DEFINES += MMFADAPTER BUILD_IRQMEDIAPLAYER_DLL

DEPENDPATH += src
LIBS += -lmediaclientvideo \
        -lmmfcontrollerframework \
        -lstereowideningeffect  \
        -lws32 \
        -lirqlogger

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

INCLUDEPATH += inc  \
               ../irqcommon/inc \
               ../irqlogger/inc            
                
                        
                  
MOC_DIR = moc

symbian{
  TARGET.UID3 = 0x2002FFBF
  TARGET.EPOCALLOWDLLDATA = 1
  TARGET.VID = VID_DEFAULT
  MMP_RULES += SMPSAFE
}

# Input
HEADERS += inc/irqplayeradapterinterface.h \
           inc/irqmediaplayer.h  \
           inc/irqmmfadapter.h  \
           inc/irqphononadapter.h

SOURCES += irqmediaplayer.cpp \
           irqmmfadapter.cpp \
           irqphononadapter.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5 

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }