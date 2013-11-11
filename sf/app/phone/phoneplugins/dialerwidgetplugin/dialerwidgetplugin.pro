# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = lib
CONFIG += plugin \
    hb \
    mobility
HB += hbfeedback
MOBILITY = serviceframework

win32:PLUGIN_SUBDIR = /hsresources/import/widgetregistry/20022F44
symbian:PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/20022F44
TARGET_DIR = /private/20022F35/import/widgetregistry/20022F7E  

DEPENDPATH += ./inc \
    ./src
INCLUDEPATH += ./inc \
    ../../inc
symbian: { 
    LIBS += -lxqservice \
        -lxqserviceutil \
        -lxqsettingsmanager
    TARGET.UID3 = 0x20022F44
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    VERSION = 10.0
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    BLD_INF_RULES.prj_exports +=  \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "./rom/dialerwidgetplugin.iby    CORE_APP_LAYER_IBY_EXPORT_PATH(dialerwidgetplugin.iby)"
     
    TARGET_DIR = /private/20022F35/import/widgetregistry/20022F44
    widgetResources.path = $${TARGET_DIR}
    widgetResources.sources += resource/$${TARGET}.xml
    widgetResources.sources += resource/$${TARGET}.manifest
    DEPLOYMENT += widgetResources
}
OUTPUT_DIR = $$PWD/bin
SOURCE_DIR = $$PWD/inc
OBJECTS_DIR = $$OUTPUT_DIR/tmp/$$TARGET
DESTDIR = $$OUTPUT_DIR
MOC_DIR = $$OUTPUT_DIR/tmp/$$TARGET/moc
RCC_DIR = $$OUTPUT_DIR/tmp/$$TARGET/rcc
UI_DIR = $$OUTPUT_DIR/tmp/$$TARGET/ui
LIBS += -L$$DESTDIR
DEPENDPATH += . \
    $$SOURCE_DIR
INCLUDEPATH += . \
    $$SOURCE_DIR
INCLUDEPATH += $$MOC_DIR
plugin:!isEmpty(PLUGIN_SUBDIR):DESTDIR = $$OUTPUT_DIR/$$PLUGIN_SUBDIR
include(dialerwidgetplugin.pri)
exportResources(./resource/*.manifest, $$PLUGIN_SUBDIR)
RESOURCES += dialerwidgetplugin.qrc

BLD_INF_RULES.prj_exports += \
 "./rom/dialerwidgetplugin_stub.sis /epoc32/data/z/system/install/dialerwidgetplugin_stub.sis"

symbian:MMP_RULES += SMPSAFE
