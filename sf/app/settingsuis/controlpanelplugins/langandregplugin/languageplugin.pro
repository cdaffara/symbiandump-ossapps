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

TEMPLATE = lib
TARGET = cplanguageplugin

CONFIG += hb plugin

include ( languageplugin.pri )
include (rom/languageplugin_rom.pri)

LIBS += -lcpframework
LIBS += -lstarterclient

TRANSLATIONS = control_panel.ts

CONFIG += debug_and_release 


MOC_DIR = moc
OBJECT_DIR = obj
RCC_DIR = rcc

# On win32 and mac, debug and release libraries are named differently.
# We must follow the debug and release settings Qt was compiled with:
# build debug iff Qt built debug, build release iff Qt built release.

win32|mac {
    !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug):  CONFIG+=debug
        contains(QT_CONFIG,release):CONFIG+=release
    }
}

CONFIG(debug, debug|release) {
    SUBDIRPART = debug
} else {
    SUBDIRPART = release
}

win32 { 
    DESTDIR = C:/ControlPanel/$$SUBDIRPART/bin
    OBJECTS_DIR = $$PWD/$$SUBDIRPART/tmp/$$TARGET
    # add platfrom API for windows
    INCLUDEPATH += $$PWD/../../../controlpanel_plat/inc
}

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR
LIBS += -lcplogger

RESOURCES += languageplugin.qrc

#DEFINES += ENABLE_CPLANG_LOG

#For some reason the default include path doesn't include MOC_DIR on symbian
symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_PLATFORM_EXPORT_PATH(cplogger)
    INCLUDEPATH += $$MOC_DIR
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0X2002873C    
    
    INCLUDEPATH += /sf/app/organizer/organizer_plat/clock_settingsview_plugin_api/inc
}

symbian: plugin { # copy qtstub and manifest
    
    PLUGIN_STUB_PATH = /resource/qt/plugins/controlpanel
        
    deploy.path = C:
    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_STUB_PATH
    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_STUB_PATH
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
     
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin  $$deploy.path$$qtplugins.path/$$basename(qtplugin)" 
}
symbian:MMP_RULES += SMPSAFE
               
# End of file   --Don't remove this.

