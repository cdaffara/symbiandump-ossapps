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
TARGET = phoneindicatorplugin
CONFIG += plugin hb

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc

symbian {
    load(data_caging_paths)
    include(./inc/api_headers.pri)

    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    headers.sources = $$PHONEINDICATORPLUGIN_API_HEADERS
    headers.path = |../../inc
    for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$headers.path/$$basename(header)"

    BLD_INF_RULES.prj_exports += \
    "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "./rom/phoneindicatorplugin.iby    CORE_APP_LAYER_IBY_EXPORT_PATH(phoneindicatorplugin.iby)"

    LIBS += -lxqservice \
            -lxqserviceutil \
            -lcpframework

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002E6B1
    VERSION = 10.0
    
    pluginstub.sources = phoneindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub

} else:win32 {

    # Put it to Hb/lib because it is in path
    DESTDIR = c:/hb/lib
    DLLDESTDIR = c:/hb/bin
    INCLUDEPATH += c:/hb/include/hbcore \
        c:/hb/include/hbwidgets \
        c:/hb/include/hbtools
}

BLD_INF_RULES.prj_exports += \
 "./rom/phoneindicatorplugin_stub.sis /epoc32/data/z/system/install/phoneindicatorplugin_stub.sis"


OUTPUT_DIR = $$PWD/bin
SOURCE_DIR = $$PWD/inc
OBJECTS_DIR = $$OUTPUT_DIR/tmp/$$TARGET
DESTDIR = $$OUTPUT_DIR
MOC_DIR = $$OUTPUT_DIR/tmp/$$TARGET/moc
RCC_DIR = $$OUTPUT_DIR/tmp/$$TARGET/rcc
UI_DIR = $$OUTPUT_DIR/tmp/$$TARGET/ui
DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . $$SOURCE_DIR
INCLUDEPATH += $$MOC_DIR

include(phoneindicatorplugin.pri)




symbian:MMP_RULES += SMPSAFE
