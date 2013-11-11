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
# Description: Example of home screen widget
#

TEMPLATE = lib
CONFIG += plugin mobility hb
MOBILITY = serviceframework

QT      += webkit

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

INCLUDEPATH += ./inc               

symbian: {

    WIDGET_DIR = /private/20022F35/import/widgetregistry/20022F6F
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    TARGET.UID3 = 0x20022F6F
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    
    plugins.path = /resource/qt/plugins/homescreen
    plugins.sources = $${TARGET}.dll 
    
    CONFIG += qtservice
    QTSERVICE.DESCRIPTOR = resource/$${TARGET}.xml
    
    widgetResources.path = $${WIDGET_DIR}
    widgetResources.sources += resource/$${TARGET}.manifest
    widgetResources.sources += resource/$${TARGET}.xml
    widgetResources.sources += resource/$${TARGET}preview.png
    widgetResources.sources += resource/$${TARGET}.svg
        
    DEPLOYMENT += plugins \
                  widgetResources
                  
    MMP_RULES += SMPSAFE
}

win32: {

    CONFIG(debug, debug|release) {
      TARGET_DIR = debug
    } else {
      TARGET_DIR = release
    }        
    
    HOMESCREEN_DIR = $$PWD/../../../../bin/$${TARGET_DIR}
    
    PLUGIN_DIR = $${HOMESCREEN_DIR}/resource/qt/plugins/homescreen
    WIDGET_DIR = $${HOMESCREEN_DIR}/private/20022F35/import/widgetregistry/20022F6F
        
    DESTDIR = $${PLUGIN_DIR}

    widgetResources.path = $${WIDGET_DIR}
    widgetResources.files = ./resource/*.manifest \
                            ./resource/*.xml \
                            ./resource/*.png \
                            ./resource/*.svg
    
    INSTALLS += widgetResources    
    
}
