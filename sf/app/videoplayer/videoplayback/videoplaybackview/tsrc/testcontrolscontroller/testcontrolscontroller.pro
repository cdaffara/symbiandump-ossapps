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
# Description: Project file for building testcontrolscontroller
#
#
# Version : %version: 10 %


TEMPLATE = app
TARGET = testcontrolscontroller
CONFIG += qtestlib hb qt

LIBS = -lefsrv.dll

INCLUDEPATH = hbstub
INCLUDEPATH += stub/inc \
               ../inc \
               ../../inc \
               ../../../inc \            
               ../../../../inc                                           

DEPENDPATH += stub/inc stub/src inc src  
        
# Input
HEADERS += videobaseplaybackview.h  \
           videoplaybackcontrolbar.h \
           videoplaybackcontrolpolicy.h \
           videoplaybackcontrolconfiguration.h \
           videoplaybackdetailsplaybackwindow.h \
           videoplaybackdocumentloader.h \
           videoplaybackfiledetailswidget.h \
           videoplaybackfullscreencontrol.h \
           videoplaybackstatuspanecontrol.h \
           videoplaybackviewfiledetails.h \
           mpxvideoviewwrapper.h \
           thumbnailmanager_qt.h \
           testcontrolscontroller.h \
           videoplaybackvolumecontrol.h \
           xqserviceutilxtra.h \
           xqserviceutil.h \
           videoservices.h \
           videoserviceurifetch.h \
           shareui.h \
           ../../inc/videoplaybackcontrolscontroller.h 
					 
               
SOURCES += videobaseplaybackview.cpp \ 
           videoplaybackcontrolbar.cpp \
           videoplaybackcontrolpolicy.cpp \
           videoplaybackcontrolconfiguration.cpp \
           videoplaybackdetailsplaybackwindow.cpp \
           videoplaybackdocumentloader.cpp \
           videoplaybackfiledetailswidget.cpp \
           videoplaybackfullscreencontrol.cpp \
           videoplaybackstatuspanecontrol.cpp \
           videoplaybackviewfiledetails.cpp \
           mpxvideoviewwrapper.cpp \
           thumbnailmanager_qt.cpp \
           testcontrolscontroller.cpp \
           videoplaybackvolumecontrol.cpp \
           xqserviceutilxtra.cpp \
           xqserviceutil.cpp \
           videoservices.cpp \
           videoserviceurifetch.cpp \
           shareui.cpp \
           ../../controlsrc/videoplaybackcontrolscontroller.cpp 
					 
           