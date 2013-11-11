#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Symbian Foundation License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Project file for calendar widget
#

TEMPLATE = lib
CONFIG += plugin hb mobility
MOBILITY = serviceframework

CONFIG(debug, debug|release) {
  DESTDIR = debug
  LIBS += -L../../../../bin/debug
}
else {
  DESTDIR = release
  LIBS += -L../../../../bin/release
}

HEADERS += ./inc/*.h

SOURCES += ./src/*.cpp \

RESOURCES += resources.qrc

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \

TRANSLATIONS = calwidget.ts

QT += xml              

symbian: {
    //TRANSLATIONS = loc/calendarwidget_en.ts 
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    LIBS += -lagendainterface
    LIBS += -lxqservice
    LIBS += -lxqserviceutil
    LIBS += -lxqsettingsmanager
    LIBS += -lbafl

    # get your own symbian uid    
    TARGET.UID3 = 0x2002C35A
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    
    # add your symbian uid here
    PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/2002C35A
    
    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_SUBDIR

    DEPLOYMENT += pluginstub
    
    MMP_RULES += SMPSAFE

    qtplugins.path = $$PLUGIN_SUBDIR
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
    qtplugins.sources += resource/$${TARGET}.manifest
    qtplugins.sources += resource/qtg_graf_hswidget_preview_calendar.svg
     
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin z:$$qtplugins.path/$$basename(qtplugin)"
    BLD_INF_RULES.prj_exports += "./resource/$${TARGET}.s60xml z:$$PLUGIN_SUBDIR/$${TARGET}.xml"

    XML_LOCATION = z:/resource/
    BLD_INF_RULES.prj_exports += "./gestures.xml $$XML_LOCATION"
}

win32 {
    # add platfrom API for windows
    TRANSLATIONS = loc/calendarwidget_en.ts 
    PLUGIN_SUBDIR = /hsresources/import/widgetregistry/2002C35A
}
