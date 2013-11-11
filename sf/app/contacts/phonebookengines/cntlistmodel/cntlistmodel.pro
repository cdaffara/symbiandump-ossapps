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
TARGET = cntlistmodel
DEFINES += dll \
    BUILD_CNTLISTMODEL

MOC_DIR = moc

CONFIG += hb

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x20026FC3

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += inc
INCLUDEPATH += ../../inc

HEADERS += inc/cntlistmodelglobal.h \
    inc/cntlistmodel.h \
    inc/cntlistmodel_p.h \
    inc/cntcache.h \
    inc/cntcache_p.h \
    inc/cntnamefetcher.h \
    inc/cntdefaultinfoprovider.h \
    inc/cntpresenceinfoprovider.h \
    inc/cntdisplaytextformatter.h \
    ../../inc/cntdebug.h

SOURCES += src/cntlistmodel.cpp \
    src/cntcache.cpp \
    src/cntcache_p.cpp \
    src/cntnamefetcher.cpp \
    src/cntdefaultinfoprovider.cpp \
    src/cntpresenceinfoprovider.cpp \
    src/cntdisplaytextformatter.cpp

LIBS += -lQtContacts \
    -lhbcore \
    -lthumbnailmanagerqt \
    -lpresencecacheqt \
    -lxqsettingsmanager \
    -lestor \
    -lefsrv \
    -lxqutils

DEPLOYMENT += exportheaders

:BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntinfoproviderfactory.h APP_LAYER_PLATFORM_EXPORT_PATH(cntinfoproviderfactory.h)"
:BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntinfoprovider.h APP_LAYER_PLATFORM_EXPORT_PATH(cntinfoprovider.h)"

defBlock = "$${LITERAL_HASH}if defined(EABI)" \
    "DEFFILE ../eabi/cntlistmodel.def" \
    "$${LITERAL_HASH}else" \
    "DEFFILE ../bwins/cntlistmodel.def" \
    "$${LITERAL_HASH}endif"
MMP_RULES += defBlock
