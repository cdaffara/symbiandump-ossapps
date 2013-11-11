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
# Music Player home screen widget
#

TEMPLATE = lib
TARGET   = musicwidgetplugin
TRANSLATIONS += musicwidgetplugin.ts
CONFIG += plugin mobility hb
MOBILITY = serviceframework

LIBS += -lxqservice \
        -lxqserviceutil \
        -lmpengine \
        -lmpdata \
        -lmpalbumcoverwidget \
        -lmpapplicationmonitor

INCLUDEPATH += inc \
               ../inc

HEADERS += inc/musicwidget.h \
           inc/musicwidgetdocumentloader.h \
           inc/musicwidgetplugin.h
           
SOURCES += src/musicwidget.cpp \
           src/musicwidgetdocumentloader.cpp \
           src/musicwidgetplugin.cpp


RESOURCES = resource/musicwidgetui.qrc

UID = 200286FA

symbian: {

    DESTDIR = $${PRIVATE}/private/20022F35/import/widgetregistry/$${UID}
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    TARGET.UID3 = 0x$${UID}
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
        
    plugins.path = $${DESTDIR}
    plugins.sources = $${TARGET}.dll 
    
    widgetResources.path = $${DESTDIR}
    widgetResources.sources += resource/$${TARGET}.xml
    widgetResources.sources += resource/$${TARGET}.manifest
    
    DEPLOYMENT += plugins \
                  widgetResources
}

DOCML += resource/music_widget.docml