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
#

include(../common.pri)

TEMPLATE    =  lib
TARGET      =  irqlogger
CONFIG      += dll
DEFINES     += BUILD_IRQLOGGER_DLL
defFilePath = ..


MOC_DIR = moc
DEPENDPATH = src

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

INCLUDEPATH += inc \
               ../../internetradio2.0/inc

LIBS += -lflogger
        
symbian{
  TARGET.UID3 = 0x2002FFBE
  TARGET.EPOCALLOWDLLDATA = 1
  TARGET.CAPABILITY += CAP_GENERAL_DLL
  MMP_RULES += SMPSAFE
}
       
#header files list
HEADERS += inc/irqlogger.h \
           inc/irqloggerexport.h
                       

#source files list
SOURCES += irqlogger.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }