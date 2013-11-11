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
# Description: Example home screen widget
#

TEMPLATE = lib
CONFIG += plugin mobility hb
MOBILITY = serviceframework

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

INCLUDEPATH += ./inc               

symbian: {
	
	WIDGET_DIR = /private/20022F35/import/widgetregistry/20022F80	    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    TARGET.UID3 = 0x20022F80
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
            
    plugins.path = /resource/qt/plugins/homescreen
    plugins.sources = $${TARGET}.dll 
    
    CONFIG += qtservice
    QTSERVICE.DESCRIPTOR = resource/$${TARGET}.xml
    
    widgetResources.path = $${WIDGET_DIR}
    widgetResources.sources += resource/$${TARGET}.manifest
    widgetResources.sources += resource/$${TARGET}.xml
    widgetResources.sources += resource/$${TARGET}.png
    
    widgetTranslations.path = /resource/qt/translations
    widgetTranslations.sources += ./locales/*.qm 
    
    DEPLOYMENT += plugins \
                  widgetResources \
                  widgetTranslations
                  
    BLD_INF_RULES.prj_exports += \
      "./rom/localisedhellowidgetplugin.iby            CORE_APP_LAYER_IBY_EXPORT_PATH(localisedhellowidgetplugin.iby)" \
      "./rom/localisedhellowidgetpluginresources.iby   LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(localisedhellowidgetpluginresources.iby)"
      
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
    WIDGET_DIR = $${HOMESCREEN_DIR}/private/20022F35/import/widgetregistry/20022F80
    TRANSLATIONS_DIR = $${HOMESCREEN_DIR}/resource/qt/translations
    
    DESTDIR = $${PLUGIN_DIR}
    
    widgetResources.path = $${WIDGET_DIR}
    widgetResources.files = ./resource/*.manifest \
                            ./resource/*.xml \
                            ./resource/*.png
    
    widgetTranslations.path = $${TRANSLATIONS_DIR}
    widgetTranslations.files += ./locales/*.qm 
        
    INSTALLS += widgetResources \
                widgetTranslations

}
