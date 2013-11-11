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
# Description: Project file for building testdocumentloader
#
#
# Version : %version:  7 %

TEMPLATE = app
TARGET = testdocumentloader
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += inc src stub/inc stub/src
                
# Input
HEADERS += testdocumentloader.h \
           videoplaybackcontrolscontroller.h \
           videoplaybackfiledetailswidget.h \
           videoplaybackdetailsplaybackwindow.h \
           videoplaybackcontrolbar.h \
           ../../controlinc/videoplaybackdocumentloader.h
  
SOURCES += testdocumentloader.cpp \
           videoplaybackcontrolscontroller.cpp \
           videoplaybackfiledetailswidget.cpp \
           videoplaybackdetailsplaybackwindow.cpp \
           videoplaybackcontrolbar.cpp \
           ../../controlsrc/videoplaybackdocumentloader.cpp
