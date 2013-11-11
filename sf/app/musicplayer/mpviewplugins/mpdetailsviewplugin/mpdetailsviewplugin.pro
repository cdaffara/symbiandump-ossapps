#
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
#

TEMPLATE = lib
CONFIG += hb qt ecomplugin
QT += webkit \
      network \
      xml
TARGET = mpdetailsviewplugin
MOBILITY += systeminfo
symbian: { 
    TARGET.UID3 = 0x2002D0AA
    TARGET.CAPABILITY = All -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}

SERVICE.INTERFACE_NAME = org.nokia.mmdt.MpxViewPlugin/1.0
SERVICE.CONFIGURATION = ""

DEPENDPATH += .
INCLUDEPATH += . \
               inc \
               ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxviewframeworkqt \
        -lmpxplaybackutility \
        -lmpxcommon \
        -lthumbnailmanagerqt \
        -lmpsettingsmanager \
        -lmpengine \
        -lmpdata \
        -lxqsysinfo

# Input
HEADERS += ../../inc/mpviewbase.h \
           inc/mpdetailsviewplugin.h \
           inc/mpdetailsview.h \
           inc/mpquerymanager.h
           
SOURCES += src/mpdetailsviewplugin.cpp \
           src/mpdetailsview.cpp \
           src/mpquerymanager.cpp

RESOURCES += resources/mpdetailsviewresources.qrc

DOCML += resources/mpdetailsview.docml