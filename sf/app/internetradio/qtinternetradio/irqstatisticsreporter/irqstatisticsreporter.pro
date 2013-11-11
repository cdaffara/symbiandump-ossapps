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
# Description:  Project definition file for internet radio statisticsreport

TEMPLATE = lib
TARGET = irqstatisticsreporter
QT -= gui
CONFIG += dll
DEFINES += BUILD_IRQSTATISTICSREPORTER_DLL
defFilePath = ..

symbian{
  TARGET.UID3 = 0x2002FFC4
  TARGET.EPOCALLOWDLLDATA = 1
  TARGET.CAPABILITY = CAP_GENERAL_DLL
  SYMBIAN_PLATFORMS = WINSCW ARMV5
  MMP_RULES += SMPSAFE
}

########## Dependency Path ##############################
MOC_DIR = moc
DEPENDPATH += src
INCLUDEPATH += /epoc32/include/platform \
               inc \
               ../irqcommon/inc \
               ../irqisdsclient/inc \
               ../irqnetworkcontroller/inc \
               ../irqnwkinfoobserver/inc \
               ../irqlogger/inc \
               ../irqsettings/inc
symbian{
INCLUDEPATH += ../../internetradio2.0/irsessionlog/inc
}


########## Dependency Libs ##############################

LIBS += -lirqnetworkcontroller \
        -lirqisdsclient \
        -lirqnwkinfoobserver \
        -lirqlogger \
        -lirqsettings

symbian{
LIBS += -lirsessionlog
}

########## Input ##############################
HEADERS += inc/irqstatisticsreporter.h \
           inc/irqstatisticsreporterexporter.h
SOURCES += irqstatisticsreporter.cpp

symbian{
HEADERS += inc/irqstatisticsreporter_symbian_p.h
SOURCES += irqstatisticsreporter_symbian_p.cpp
}

########## Others ##############################
!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }