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

TEMPLATE = lib
TARGET   = irhswidgetplugin
TRANSLATIONS += iradhswidget.ts
CONFIG += plugin mobility hb
MOBILITY = serviceframework

LIBS += -lxqsettingsmanager \
        -lxqservice \
        -lxqserviceutil \
        -lirqlogger 

MOC_DIR = moc
        
HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

INCLUDEPATH += ./inc               
INCLUDEPATH += ../irqcommon/inc
INCLUDEPATH += ../irqlogger/inc 

DOCML += resource/irhswidgetmetadatarow.docml
DOCML += resource/irhswidgettitlerow.docml

UID = 2002FFE4
RESOURCES = resources.qrc

symbian: {

    DESTDIR = /private/20022F35/import/widgetregistry/$${UID}
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
    MMP_RULES += SMPSAFE
}

win32: {

    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }    
    
    PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/$${UID}
    
    DESTDIR = $$PWD/../../../../bin/$${SUBDIRPART}/$${PLUGIN_SUBDIR}

    manifest.path = $${DESTDIR}
    manifest.files = ./resource/*.manifest ./resource/*.xml
    
    INSTALLS += manifest    
    
}

symbian:include(irhswidgetplugin_rom.pri)
