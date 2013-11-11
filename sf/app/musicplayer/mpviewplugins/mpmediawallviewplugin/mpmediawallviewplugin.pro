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
HB += hbfeedback
TARGET = mpmediawallviewplugin
symbian: { 
    TARGET.UID3 = 0x10207C94
    TARGET.CAPABILITY = All -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES += SMPSAFE
}

SERVICE.INTERFACE_NAME = org.nokia.mmdt.MpxViewPlugin/1.0
SERVICE.CONFIGURATION = "<i>EMPXViewPluginPriorityNormal</i>"

DEPENDPATH += .
INCLUDEPATH += . \
               inc \
               ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_PUBLIC_EXPORT_PATH(hgwidgets)
     
LIBS += -lestor \
        -lmpxviewframeworkqt \
        -lganeswidgets \
        -lmpengine \
        -lmpdata \
        -lmpalbumcoverwidget
	
# Input
HEADERS += ../../inc/mpviewbase.h \
           inc/mpmediawallviewplugin.h \
           inc/mpmediawallview.h \
           inc/mpmediawalldocumentloader.h \
           inc/mptracklistwidget.h
SOURCES += src/mpmediawallviewplugin.cpp \
           src/mpmediawallview.cpp \
           src/mpmediawalldocumentloader.cpp \
           src/mptracklistwidget.cpp
RESOURCES += resources/mpmediawallviewresources.qrc

DOCML += resources/mediawall.docml
