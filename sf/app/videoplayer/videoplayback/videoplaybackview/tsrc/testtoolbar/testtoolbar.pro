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
# Description: Project file for building testtoolbar
#
#
# Version : %version:  3 %

TEMPLATE = app
TARGET = testtoolbar
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += inc src stub/inc stub/src

LIBS += -lws32.dll \
        -lcone.dll


# Input
HEADERS += testtoolbar.h \
           videoplaybackcontrolscontroller.h \
           videoplaybackviewfiledetails.h \
           videobaseplaybackview.h \
           hbtoolbar.h \
           hbtoolbutton.h \
           ../../controlinc/videoplaybacktoolbar.h
  
SOURCES += testtoolbar.cpp \
           videoplaybackcontrolscontroller.cpp \
           videoplaybackviewfiledetails.cpp \
           videobaseplaybackview.cpp \
           hbtoolbar.cpp \
           hbtoolbutton.cpp \
           ../../controlsrc/videoplaybacktoolbar.cpp
