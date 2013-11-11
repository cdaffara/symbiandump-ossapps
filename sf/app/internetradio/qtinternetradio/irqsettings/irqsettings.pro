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

TARGET = irqsettings
TARGET.CAPABILITY = CAP_GENERAL_DLL

MOC_DIR = moc
DEPENDPATH += src
DEFINES += BUILD_IRQSETTINGS_DLL
defFilePath = ..

INCLUDEPATH += inc \
               ../irqcommon/inc \
               ../../internetradio2.0/inc

LIBS += -lxqsettingsmanager

symbian{
  TARGET.UID3 = 0x2002FFC2
  TARGET.EPOCALLOWDLLDATA = 1
  MMP_RULES += SMPSAFE
}

# Input
HEADERS += inc/irqsettings.h inc/irqsettings_p.h
SOURCES += irqsettings.cpp irqsettings_p.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }