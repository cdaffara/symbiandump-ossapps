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
#
#
 

TEMPLATE = lib
CONFIG += plugin hb svg mobility
MOBILITY = serviceframework

TRANSLATIONS = operator_widget.ts

MOC_DIR = moc
RCC_DIR = rcc

symbian: {
LIBS += -lxqsettingsmanager \
    -lnetworkhandling \
    -lsatclient
}
   
INCLUDEPATH += ./inc \
    ./infowidget/inc 
    
symbian: { 
    load(data_caging_paths)
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    TARGET.UID3 = 0x20029F28
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    VERSION = 10.0
    deploy.path = c:
    
    DEPLOYMENT += exportheaders  
        
    BLD_INF_RULES.prj_exports +=  \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "./rom/infowidgetplugin.iby    CORE_APP_LAYER_IBY_EXPORT_PATH(infowidgetplugin.iby)" \
	 "./rom/infowidgetplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(infowidgetplugin_resources.iby)" 
}

DOCML += resource/infowidget.docml resource/settingsdialog.docml
RESOURCES = infowidgetplugin.qrc

symbian: PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/20029F28
OUTPUT_DIR = ./bin
SOURCE_DIR = ./inc
OBJECTS_DIR = $$OUTPUT_DIR/tmp/$$TARGET
DESTDIR = $$OUTPUT_DIR
MOC_DIR = $$OUTPUT_DIR/tmp/$$TARGET/moc
RCC_DIR = $$OUTPUT_DIR/tmp/$$TARGET/rcc
UI_DIR = $$OUTPUT_DIR/tmp/$$TARGET/ui
LIBS += -L$$DESTDIR
DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . $$SOURCE_DIR
INCLUDEPATH += $$MOC_DIR
plugin: !isEmpty(PLUGIN_SUBDIR): DESTDIR = $$OUTPUT_DIR/$$PLUGIN_SUBDIR

include(infowidgetplugin.pri)

exportResources(./resource/*.manifest, $$PLUGIN_SUBDIR)

BLD_INF_RULES.prj_exports += \
 "./rom/infowidgetplugin_stub.sis /epoc32/data/z/system/install/infowidgetplugin_stub.sis"

symbian:MMP_RULES += SMPSAFE
