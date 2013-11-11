#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
#
#
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

TEMPLATE=app
TARGET=Cpqtsp

symbian: {
	TARGET.CAPABILITY = CAP_APPLICATION 
  TARGET.EPOCALLOWDLLDATA = 1  
  TARGET.UID3 = 0x2002DD2B	
	BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
	BLD_INF_RULES.prj_exports += "rom/cpqtsp.iby CORE_APP_LAYER_IBY_EXPORT_PATH(cpqtsp.iby)"
	BLD_INF_RULES.prj_exports += "rom/cpsqtsp_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cpsqtsp_resources.iby)"
	SYMBIAN_PLATFORMS = WINSCW \
    ARMV5
}

CONFIG += hb service

RSS_RULES += "hidden = KAppIsHidden;"

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
include(Cpqtsp.pri)
LIBS+=-lxqservice -lxqserviceutil -lflogger
MMP_RULES += "LIBRARY msgs.lib centralrepository.lib provisioningengine.lib featmgr.lib smcm.lib gsmu.lib"

TRANSLATIONS += deviceupdates.ts

SERVICE.FILE = resources/service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

libFiles.sources = xqservice.dll
libFiles.path = "!:\sys\bin"
DEPLOYMENT += libFiles

MOC_DIR = moc

