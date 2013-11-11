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
# Description:  This is the project file defnition for deviceupdatesplugin.
#
#



TEMPLATE = lib
TARGET = deviceupdatesplugin
DEPENDPATH += . \
			  ./inc \
			  ./src
INCLUDEPATH += . \
               ./inc

CONFIG += hb plugin

LIBS += -lcpframework \
					-lcentralrepository \
					-lws32 \
					-lapgrfx

# Input
HEADERS += deviceupdatesplugin.h \
		   deviceupdatesdata.h
		   
SOURCES += deviceupdatesplugin.cpp \
		   deviceupdatesdata.cpp

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1  
    TARGET.UID3 = 0X2002DD04
	
	LIBS += -ldeviceupdatesplugin 
	
						SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

}

symbian {
    deploy.path = C:
    headers.sources = qmakepluginstubs/deviceupdatesplugin.qtplugin
    headers.path = /resource/qt/plugins/controlpanel

    # This is for new exporting system coming in garden
    for(header, headers.sources):BLD_INF_RULES.prj_exports += "./$$header $$deploy.path$$headers.path/$$basename(header)"
}
symbian: INCLUDEPATH += /orbit/hb/include/hbservices \
			   /orbit/hb/include/hbservices/private \
			   /orbit/hb/include/hbcore \
			   /orbit/hb/include/hbcore/private
			   
TARGET.CAPABILITY = CAP_GENERAL_DLL

plugin.sources = deviceupdatesplugin.dll
plugin.path = \resource\qt\plugins\controlpanel
DEPLOYMENT += plugin
RESOURCES += ./resources/deviceupdatesplugin.qrc

# End of file	--Don't remove this.