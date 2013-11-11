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
# Description:
#
#

TEMPLATE = lib

TARGET = nmipssettings

QT += core

DEPENDPATH += .

INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb
CONFIG += plugin

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
                    ARMV5

# Input
HEADERS += inc/nmipssettingsplugin.h \
           inc/nmipssettingshelper.h \
           inc/nmipssettingsmanagerbase.h \
           inc/nmipsimap4settingsmanager.h \
           inc/nmipspop3settingsmanager.h \
           inc/nmipssettingsmanagerfactory.h \
           inc/nmipsextendedsettingsmanager.h \
           inc/nmipssettingscustomitem.h \
           inc/nmipssettingslabeledcombobox.h \
           inc/nmipssettingstimeeditor.h \
           inc/nmipssettingsmultiselectionitem.h

SOURCES += src/nmipssettingsplugin.cpp \
           src/nmipssettingshelper.cpp \
           src/nmipssettingsmanagerbase.cpp \
           src/nmipsimap4settingsmanager.cpp \
           src/nmipspop3settingsmanager.cpp \
           src/nmipssettingsmanagerfactory.cpp \
           src/nmipsextendedsettingsmanager.cpp \
           src/nmipssettingscustomitem.cpp \
           src/nmipssettingslabeledcombobox.cpp \
           src/nmipssettingstimeeditor.cpp \
           src/nmipssettingsmultiselectionitem.cpp

symbian: { 
    TARGET.EPOCALLOWDLLDATA = 1
    DEFINES += PLUGINUID3=0x2002C30D
    TARGET.UID3 = 0x2002C30D
    
    LIBS += -lcpframework
    LIBS += -limcm
    LIBS += -lxqutils
    LIBS += -lxqsettingsmanager
    LIBS += -leuser
    LIBS += -llibc
    LIBS += -lcmapplsettingsui
    LIBS += -lAlwaysOnlineManagerClient
    
    CONFIG += mobility
    MOBILITY += bearer serviceframework
    
    MMP_RULES += SMPSAFE
}

symbian: plugin { # copy qtstub and manifest

    PLUGIN_STUB_PATH = /resource/qt/plugins/nmail/settings
    
    deploy.path = C:
    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_STUB_PATH
    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_STUB_PATH
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"
}

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             "rom/nmipssettings.iby                 CORE_APP_LAYER_IBY_EXPORT_PATH(nmipssettings.iby)" \
                             "rom/nmipssettingslanguage.iby         LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(nmipssettingslanguage.iby)" \
                             "inc/ipssettingkeys.h                  |../inc/ipssettingkeys.h" \
                             "inc/nmipssettingitems.h               |../inc/nmipssettingitems.h" \
                             "conf/2000E53D.txt                     /epoc32/release/winscw/udeb/z/private/10202BE9/2000E53D.txt" \
                             "conf/2000E53D.txt                     /epoc32/release/winscw/urel/z/private/10202BE9/2000E53D.txt" \
                             "conf/2000E53D.txt                     /epoc32/data/z/private/10202BE9/2000E53D.txt" \
                             "conf/nmipssettings.confml             APP_LAYER_CONFML(nmipssettings.confml)" \
                             "conf/nmipssettings_2002C30D.crml      APP_LAYER_CRML(nmipssettings_2002C30D.crml)"

 
 
TARGET.CAPABILITY = All -TCB
plugin.sources = $${TARGET}.dll
plugin.path = $$PLUGIN_STUB_PATH
DEPLOYMENT += plugin
    
TRANSLATIONS = mailips.ts
