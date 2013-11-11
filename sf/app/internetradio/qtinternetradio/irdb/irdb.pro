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
TARGET = irdb
TARGET.CAPABILITY = CAP_GENERAL_DLL

INCLUDEPATH += inc

defFilePath = ..

MOC_DIR = moc
QT += sql
DEFINES += BUILD_IRDB_DLL

symbian {
    TARGET.UID3 = 0x2002FFFF
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY += CAP_GENERAL_DLL
    MMP_RULES += SMPSAFE
}

HEADERS += inc/irsqlstr.h \
           inc/irdb.h \
           inc/irdbwrapper.h \
           inc/irdbviewstruct.h \
           inc/channelhistorywrapper.h \
           inc/searchrltwrapper.h \
           inc/songhistorywrapper.h \
           inc/favoriteswrapper.h \
           inc/imgwrapper.h \
           inc/urlinfowrapper.h
	
SOURCES += src/irdb.cpp \
           src/irdbwrapper.cpp \
           src/channelhistorywrapper.cpp \
           src/searchrltwrapper.cpp \
           src/songhistorywrapper.cpp \
           src/favoriteswrapper.cpp \
           src/imgwrapper.cpp \
           src/urlinfowrapper.cpp
					 
SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }