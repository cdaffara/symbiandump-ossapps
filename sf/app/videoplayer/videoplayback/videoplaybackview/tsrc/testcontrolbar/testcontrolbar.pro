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
# Description: Project file for building testcontrolbar
#
#
# Version : %version:   5 %

TEMPLATE = app
TARGET = testcontrolbar
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += inc src stub/inc stub/src
                
# Input
HEADERS += testcontrolbar.h \
           videoplaybackcontrolscontroller.h \
           videoplaybackviewfiledetails.h \
           videoplaybackprogressbar.h \
           videoplaybacktoolbar.h \
           ../../controlinc/videoplaybackcontrolbar.h
  
SOURCES += testcontrolbar.cpp \
           videoplaybackcontrolscontroller.cpp \
           videoplaybackviewfiledetails.cpp \
           videoplaybackprogressbar.cpp \
           videoplaybacktoolbar.cpp \
           ../../controlsrc/videoplaybackcontrolbar.cpp
