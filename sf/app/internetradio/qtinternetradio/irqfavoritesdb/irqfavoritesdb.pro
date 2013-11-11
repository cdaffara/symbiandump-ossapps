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

TARGET = irqfavorites
TARGET.CAPABILITY += CAP_GENERAL_DLL
QT -= gui
CONFIG += dll
DEFINES += BUILD_IRQFAVORITES_DLL
defFilePath = ..

MOC_DIR = moc
DEPENDPATH = src

INCLUDEPATH += ../irqcommon/inc \
               ../irdb/inc
               
LIBS += -lirdb \
        -lirqutility

symbian{
  TARGET.UID3 = 0x2002FFBC
  TARGET.EPOCALLOWDLLDATA = 1
  MMP_RULES += SMPSAFE
}


#header files list
HEADERS += inc/irqfavoritesdb.h  \
           inc/irqfavoritesdbexporter.h
           

#source files list
SOURCES += irqfavoritesdb.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }