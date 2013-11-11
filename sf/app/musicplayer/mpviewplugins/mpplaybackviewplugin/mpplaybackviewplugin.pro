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
TARGET = mpplaybackviewplugin
symbian: { 
    TARGET.UID3 = 0x10207C64
    TARGET.CAPABILITY = All -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES += SMPSAFE
}

SERVICE.INTERFACE_NAME = org.nokia.mmdt.MpxViewPlugin/1.0
SERVICE.CONFIGURATION = "<t>0x101FFCA0</t><p>0x101FFC06;0x101FFC3A;0xE419BEEE</p><i>EMPXViewPluginPriorityNormal</i><f>0x00000002</f>"

DEPENDPATH += .
INCLUDEPATH += . \
               inc \
               ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
     
LIBS += -lmpxplaybackutility \
        -lmpxcommon \
        -lestor \
        -lmpxviewframeworkqt \
        -laudioequalizerutility \
        -lmpsettingsmanager \
        -lmpengine \
        -lmpdata \
        -lmpalbumcoverwidget \
        -lhwrmlightclient

# Input
HEADERS += ../../inc/mpviewbase.h \
           inc/mpplaybackviewplugin.h \
           inc/mpplaybackview.h \
           inc/mpplaybackwidget.h \
           inc/mpequalizerwidget.h \
           inc/mpplaybackdocumentloader.h \
           inc/mplightmonitor_p.h \
           inc/mplightmonitor.h

SOURCES += src/mpplaybackviewplugin.cpp \
          src/mpplaybackview.cpp \
          src/mpplaybackwidget.cpp \
          src/mpequalizerwidget.cpp \
          src/mpplaybackdocumentloader.cpp \
          src/mplightmonitor_p.cpp \
          src/mplightmonitor.cpp
       
RESOURCES += resources/mpplaybackviewpluginresources.qrc

DOCML += resources/playbackwidget.docml
