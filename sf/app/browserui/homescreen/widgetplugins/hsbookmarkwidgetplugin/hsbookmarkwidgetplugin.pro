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
# Description: Homescreen bookmark plugin widget
#

TEMPLATE = lib
CONFIG += plugin mobility hb
MOBILITY = serviceframework

HB += hbfeedback

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

INCLUDEPATH += ./inc  

RESOURCES += hsbookmarkwidgetplugin.qrc         

symbian: {

    DESTDIR = /private/20022F35/import/widgetregistry/200267BE
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    TARGET.UID3 = 0x200267BE
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    
    plugins.path = $${DESTDIR}
    plugins.sources = $${TARGET}.dll 
    
    widgetResources.path = $${DESTDIR}
    widgetResources.sources += resource/$${TARGET}.xml    
    widgetResources.sources += resource/$${TARGET}.manifest
    widgetResources.sources += resource/nokiabrowser-bm-icon_70x70_ico.png
    widgetResources.sources += resource/nokiabrowser-bm-icon_70x70_no_ico.png
    widgetResources.sources += resource/nokiabrowser-bm-icon_50x50.png
        
    DEPLOYMENT += plugins \
                  widgetResources
}

win32: {

    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }    
    
    PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/20102F7E
    
    DESTDIR = $$PWD/../../../../bin/$${SUBDIRPART}/$${PLUGIN_SUBDIR}

    manifest.path = $${DESTDIR}
    manifest.files = ./resource/*.manifest ./resource/*.xml ./resource/*.png ./resource/*.svg
    
    INSTALLS += manifest        
}
