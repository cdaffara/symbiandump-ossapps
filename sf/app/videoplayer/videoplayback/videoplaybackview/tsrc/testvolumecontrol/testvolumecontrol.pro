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
# Description: Project file for building testvolumecontrol
#
#
# Version : %version:  1 %

TEMPLATE = app
TARGET = testvolumecontrol
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += stub/inc stub/src inc src 
                
# Input
HEADERS += testvolumecontrol.h \
           videoplaybackcontrolscontroller.h \
           videoplaybackviewfiledetails.h \
           hbvolumesliderpopup.h \
           ../../controlinc/videoplaybackvolumecontrol.h
  
SOURCES += testvolumecontrol.cpp \
           videoplaybackcontrolscontroller.cpp \
           videoplaybackviewfiledetails.cpp \
           hbvolumesliderpopup.cpp \
           ../../controlsrc/videoplaybackvolumecontrol.cpp
