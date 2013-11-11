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

TARGET = irqisdsclient
TARGET.CAPABILITY += CAP_GENERAL_DLL

DEFINES += BUILD_IRQISDSCLIENT_DLL
defFilePath = ..
MOC_DIR = moc
DEPENDPATH = src

INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(libc)}
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(ecom)}
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(http)}
INCLUDEPATH += $${MW_LAYER_PUBLIC_EXPORT_PATH(http)}       
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(mmf/common)}
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(xml)}

INCLUDEPATH += ../../internetradio2.0/ircommon/inc \
               ../../internetradio2.0/irfavoritesdb/inc \
               ../../internetradio2.0/irnetworkcontroller/inc \
               ../../internetradio2.0/irisdsclient/inc  \
               ../../internetradio2.0/resources \
               ../../internetradio2.0/inc  \
               ../../internetradio2.0/irdataprovider/inc \
               ../../internetradio2.0/ircachemgmt/inc \
               ../../internetradio2.0/irxmlparser/inc \
               ../../internetradio2.0/irsessionlog/inc \
               ../irqcommon/inc \
               ../irqfavoritesdb/inc \
               ../irqlogger/inc \
               ../irqsettings/inc \
               ../../presetserver/include \
               ../../presetserver/clientinc \
               ../../presetserver/serverinc

LIBS += -lirdatastructures \
        -lirfavoritesdb \
        -lirqfavorites \
        -lirnetworkcontroller \
        -lirqsettings \
        -lisdsclientinterface \
        -lirdataprovider \
        -lflogger \
        -lirqlogger \
        -lirqutility       
  

symbian{
  TARGET.UID3 = 0x2002FFBD
  TARGET.EPOCALLOWDLLDATA = 1
  MMP_RULES += SMPSAFE
}

#input

#header files list
HEADERS += inc/irqisdsclient.h \
           inc/irqisdsclientexporter.h \
           inc/irqabstractisdsclientimpl.h

#source files list
SOURCES += irqisdsclient.cpp

symbian{
HEADERS += inc/irqisdsclientimpl_symbian.h
SOURCES += irqisdsclientimpl_symbian.cpp
}

SYMBIAN_PLATFORMS = WINSCW ARMV5

include(../common.pri)

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }