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
# Description: Project file for building teststatuspanecontrol
#
#
# Version : %version: 5 %


TEMPLATE = app
TARGET = teststatuspanecontrol
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
HEADERS += ../../../controlinc/videoplaybackstatuspanecontrol.h \       
           inc/teststatuspanecontrol.h \            
           videoplaybackcontrolscontroller.h \
           videoplaybackviewfiledetails.h \
           videobaseplaybackview.h \
           videoplaybackdocumentloader.h \
           videoplaybackfullscreencontrol.h \
           hblabel.h \
           hbgroupbox.h

SOURCES += ../../../controlsrc/videoplaybackstatuspanecontrol.cpp \
           src/teststatuspanecontrol.cpp \				 
           videoplaybackcontrolscontroller.cpp \
           videoplaybackviewfiledetails.cpp \
           videobaseplaybackview.cpp \
           videoplaybackdocumentloader.cpp \
           videoplaybackfullscreencontrol.cpp \
           hblabel.cpp \
           hbgroupbox.cpp
           