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

include(buildflags.pri)

TEMPLATE = lib
TARGET = fmradiohswidget
CONFIG += plugin
CONFIG += hb
CONFIG += mobility
MOBILITY = serviceframework

LIBS += -lxqsettingsmanager
LIBS += -lxqservice
LIBS += -lxqserviceutil

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp
SOURCES += ../common/radiologger.cpp

INCLUDEPATH += ./inc
INCLUDEPATH += ../common

UID = 2002E6D6

RESOURCES += resources.qrc
TRANSLATIONS = fmradiohswidget.ts

symbian: {

    DESTDIR = /private/20022F35/import/widgetregistry/$${UID}

    TARGET.UID3 = 0x$${UID}
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    
    plugins.path = $${DESTDIR}
    plugins.sources = $${TARGET}.dll 
    
    widgetResources.path = $${DESTDIR}
    widgetResources.sources += resource/$${TARGET}.xml
    widgetResources.sources += resource/$${TARGET}.manifest
	
    localisedFiles.path = /resource/qt/translations
    localisedFiles.sources += ./locales/*.qm
        
    DEPLOYMENT += plugins
    DEPLOYMENT += widgetResources
    DEPLOYMENT += localisedFiles
    
    MMP_RULES += SMPSAFE
    
    include(radiohswidget_exports_to_rom.pri)
}

win32: {

    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }    
    
    PLUGIN_SUBDIR = /hsresources/import/widgetregistry/$${UID}
    
    DESTDIR = $$PWD/../../../../../bin/$${SUBDIRPART}/$${PLUGIN_SUBDIR}

    manifest.path = $${DESTDIR}
    manifest.files = ./resource/*.manifest ./resource/*.xml ./resource/*.css
    
    widgetLocalisation.path = $$PWD/../../../../../bin/$${SUBDIRPART}/resource/qt/translations
    widgetLocalisation.files += ./locales/*.qm
    
    INSTALLS += manifest    
}

