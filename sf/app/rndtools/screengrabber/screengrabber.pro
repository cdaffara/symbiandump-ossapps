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
#


TEMPLATE = app
TARGET = ScreenGrabber
DEPENDPATH += src
INCLUDEPATH += inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
load(hb.prf)
symbian:CONFIG -= symbian_i18n

HEADERS += inc/mainview.h \
					 inc/settingsview.h \
					 inc/application.h \
					 inc/notifications.h

SOURCES += main.cpp \
					 mainview.cpp \
					 application.cpp \
					 settingsview.cpp \
					 notifications.cpp 

symbian: {
    BLD_INF_RULES.prj_exports += "./rom/screengrabber.iby CORE_IBY_EXPORT_PATH(tools,screengrabber.iby)
    BLD_INF_RULES.prj_exports += "./rom/backup_registration.xml     Z:/private/101FB751/backup_registration.xml"
    
    screengrabber_buildstubsis_extension = \
        "$${LITERAL_HASH}ifdef MARM" \
        "    START EXTENSION app-services/buildstubsis" \
        "    OPTION SRCDIR sis" \
        "    OPTION SISNAME screengrabber_stub" \
        "    END" \
        "$${LITERAL_HASH}endif" 
        
    BLD_INF_RULES.prj_extensions += screengrabber_buildstubsis_extension
        
		LIBS += -lestor -lws32 -lPlatformEnv -limageconversion  -lapgrfx -lcommonengine  -lfbscli -lgdi -leikcore -lbafl -lefsrv -lapparc -lcone

		HEADERS += inc/sgengine.h \
   				   inc/enginewrapper.h \
				   inc/gifanimator.h

		SOURCES += sgengine.cpp \
				   enginewrapper.cpp \
				   gifanimator.cpp

		RSS_RULES += "group_name = \"RnD Tools\"";

    TARGET.CAPABILITY = WriteDeviceData SwEvent

    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x101FB751
    TARGET.SID = 0x101FB751
    TARGET.VID = 0x101FB657 // Nokia
	
	
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000  // Min 128Kb, Max 16Mb

    ICON = ./icons/qgn_menu_screengrabber.svg 

} else {
    error("Only Symbian supported!")
}
