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
# Description: Project file for building testdetailsplaybackwindow
#
#
# Version : %version:  4 %

TEMPLATE = app
TARGET = testdetailsplaybackwindow
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += inc src stub/inc stub/src
                
# Input
HEADERS += testdetailsplaybackwindow.h \
           videoplaybackcontrolscontroller.h \
           videoplaybackdocumentloader.h \
           videoplaybackviewfiledetails.h \
           hbpushbutton.h \
           ../../controlinc/videoplaybackdetailsplaybackwindow.h
  
SOURCES += testdetailsplaybackwindow.cpp \
           videoplaybackcontrolscontroller.cpp \
           videoplaybackdocumentloader.cpp \
           videoplaybackviewfiledetails.cpp \
           hbpushbutton.cpp \
           ../../controlsrc/videoplaybackdetailsplaybackwindow.cpp
