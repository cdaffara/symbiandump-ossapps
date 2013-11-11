#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).

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
#

TEMPLATE = app
CONFIG += hb \
          qt \
		  service
TARGET = deviceupdatesqtsp
symbian: {

	TARGET.CAPABILITY = CAP_APPLICATION 
    TARGET.EPOCALLOWDLLDATA = 1  
    TARGET.UID3 = 0x1028339D	
	BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
	BLD_INF_RULES.prj_exports += "./rom/deviceupdatesqtsp.iby CORE_APP_LAYER_IBY_EXPORT_PATH(deviceupdatesqtsp.iby)"
	BLD_INF_RULES.prj_exports += "./rom/deviceupdatesqtsp_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(deviceupdatesqtsp_resources.iby)"
	LIBS += -lws32	\
			apparc.lib
}			   
symbian: INCLUDEPATH += /orbit/hb/include/hbservices \
			   /orbit/hb/include/hbservices/private \
			   /orbit/hb/include/hbcore \
			   /orbit/hb/include/hbcore/private	
			   
RSS_RULES += "hidden = KAppIsHidden;"
			   
INCLUDEPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE			   

HEADERS += ./inc/DmTelServiceHandler.h 

SOURCES += ./src/main.cpp
SOURCES += ./src/DmTelServiceHandler.cpp

LIBS += -lhbcore \
        -lxqservice \
        -lxqserviceutil

SERVICE.FILE += ./resources/service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

libFiles.sources = xqservice.dll
libFiles.path = "!:\sys\bin"
DEPLOYMENT += libFiles

MOC_DIR = moc
