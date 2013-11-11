#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Project file for building testcontrolconfiguration
#
#
# Version : %version: 3 %


TEMPLATE = app
TARGET = testcontrolconfiguration
CONFIG += qtestlib hb qt

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += stub/inc \
               ../inc \
               ../../inc \   
               ../../../inc \            
               ../../../../inc \

DEPENDPATH += stub/inc stub/src inc src  
        
# Input
HEADERS += ../../../controlinc/videoplaybackcontrolconfiguration.h \
           videoplaybackdocumentloader.h \           
           testcontrolconfiguration.h \            
           videoplaybackcontrolscontroller.h \           
           videoplaybackviewfiledetails.h	 
               
SOURCES += ../../../controlsrc/videoplaybackcontrolconfiguration.cpp \
           videoplaybackdocumentloader.cpp	\           
           testcontrolconfiguration.cpp \				 
           videoplaybackviewfiledetails.cpp \
           videoplaybackcontrolscontroller.cpp
