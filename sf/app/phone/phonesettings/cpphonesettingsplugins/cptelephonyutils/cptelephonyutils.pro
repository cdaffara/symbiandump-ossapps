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

TEMPLATE = lib
TARGET = cptelephonyutils
CONFIG += hb mobility
MOBILITY += systeminfo
MOC_DIR = moc

INCLUDEPATH += inc
SOURCEPATH += src

# Input
HEADERS += inc/cpplugincommon.h \
           inc/cpphonelocalisation.h 
           
SOURCES += src/cpphonelocalisation.cpp 
           
symbian: {
    SOURCES += src/cpplugincommon.cpp 
    }
else: {
    SOURCES += src/cpplugincommon_s.cpp
    }

TRANSLATIONS = telephone_cp.ts

DEFINES += BUILD_TELEPHONYUTILS

symbian: {
    load(data_caging_paths)
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
    INCLUDEPATH +=  ../../../inc

    LIBS += -lsssettings    \
            -lphonesettings \
            -lxqsysinfo \
            -lxqsettingsmanager

    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    TARGET.EPOCALLOWDLLDATA = 1 
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0X20029F1F
    VERSION = 10.0
    defFiles = \
    "$${LITERAL_HASH}ifdef WINS" \
        "DEFFILE bwins/cptelephonyutils.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE eabi/cptelephonyutils.def" \
    "$${LITERAL_HASH}endif"
    MMP_RULES += defFiles

    # For sis file
    dllfile.sources = $${TARGET}.dll
    dllfile.path = $$SHARED_LIB_DIR
    DEPLOYMENT += dllfile
} else: {
    INCLUDEPATH += ../../../../../mw/phonesrv/phonesrv_plat/phone_settings_api/inc
    INCLUDEPATH += ../../../../../mw/gsprofilesrv/controlpanel/controlpanel_plat/inc
    DESTDIR = c:\hb\lib
    DLLDESTDIR = c:\hb\bin
}

BLD_INF_RULES.prj_exports += "./rom/cptelephonyutils_stub.sis /epoc32/data/z/system/install/cptelephonyutils_stub.sis"
