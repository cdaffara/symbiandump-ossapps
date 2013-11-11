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

TARGET = irqnetworkcontroller
TARGET.CAPABILITY = CAP_GENERAL_DLL

MOC_DIR = moc
DEPENDPATH += src
DEFINES += BUILD_IRQNETWORKCONTROLLER_DLL
defFilePath = ..

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(http)}
INCLUDEPATH += $${MW_LAYER_PUBLIC_EXPORT_PATH(http)}

INCLUDEPATH += inc \
               ../irqcommon/inc \
               ../../internetradio2.0/irnetworkcontroller/inc    

LIBS += -lirnetworkcontroller -lnetmeta

symbian{
  TARGET.UID3 = 0x2002FFC0
  TARGET.EPOCALLOWDLLDATA = 1
  MMP_RULES += SMPSAFE
}

# Input
HEADERS += inc/irqnetworkcontroller.h inc/irqnetworkcontroller_p.h
SOURCES += irqnetworkcontroller.cpp irqnetworkcontroller_p.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }