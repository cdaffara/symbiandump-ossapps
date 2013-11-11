#
# Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
#
# Contributors:
#
# Description: 
#     Pro file of devicemgmtdialogsplugin
#
#

TEMPLATE = lib
TARGET = devicemanagementnotifiersplugin
CONFIG += hb \
          plugin        
INCLUDEPATH += .
DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DESTDIR = $${HB_BUILD_DIR}/plugins/devicedialogs

SOURCES += src/devicemanagementnotifierplugin.cpp
SOURCES += src/devicemanagementnotifierwidget.cpp
SOURCES += src/devicemanagementnotifierutils.cpp
SOURCES += src/syncmlnotifierprivate.cpp
SOURCES += src/omacppinquerydialog.cpp
SOURCES += src/syncmlconnectnotifier.cpp
SOURCES += src/fotadevicedialogs.cpp

HEADERS += inc/devicemanagementnotifierplugin_p.h
HEADERS += inc/devicemanagementnotifierwidget_p.h
HEADERS += inc/devicemanagementnotifierutils.h 
HEADERS += inc/syncmlnotifierprivate.h
HEADERS += inc/omacppinquerydialog.h
HEADERS += inc/fotadevicedialogs.h

RESOURCES = devicemanagementnotifierdevman.qrc
#TRANSLATIONS += deviceupdates_en.ts

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002DD20

    hblib.sources = Hb.dll
    hblib.path = \sys\bin
    hblib.depends = "(0xEEF9EA38), 1, 0, 0, {\"Hb\"}"

    pluginstub.sources = devicemanagementnotifiersplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub
    MMP_RULES += "LIBRARY syncmlclientapi.lib centralrepository.lib flogger.lib"
    
    BLD_INF_RULES.prj_exports += \
  	"$${LITERAL_HASH}include <platform_paths.hrh>" \
  	"rom/devicemgmtdialogsplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(devicemgmtdialogsplugin.iby)"
}
!local {
    target.path = $${HB_PLUGINS_DIR}/devicedialogs
    INSTALLS += target
}

MOC_DIR = moc
