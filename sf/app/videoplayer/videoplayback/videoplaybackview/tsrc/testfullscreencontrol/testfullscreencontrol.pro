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
# Description: Project file for building testfullscreencontrol
#
#
# Version : %version: 4 %


TEMPLATE = app
TARGET = testfullscreencontrol
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
HEADERS += ../../../controlinc/videoplaybackfullscreencontrol.h \      
           inc/testfullscreencontrol.h \            
           videoplaybackcontrolscontroller.h \
           videoplaybackviewfiledetails.h \
           videoplaybackcontrolbar.h \
           videoplaybackfiledetailswidget.h \
           videoplaybackdetailsplaybackwindow.h     
					 
               
SOURCES += ../../../controlsrc/videoplaybackfullscreencontrol.cpp \
           src/testfullscreencontrol.cpp \
           videoplaybackcontrolscontroller.cpp \
           videoplaybackviewfiledetails.cpp \
           videoplaybackcontrolbar.cpp \
           videoplaybackfiledetailswidget.cpp \
           videoplaybackdetailsplaybackwindow.cpp                    
