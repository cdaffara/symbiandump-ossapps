#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Project definition file for project clock
#

TEMPLATE = subdirs

SUBDIRS += \
		clockengines \
		clockmw \
		clockui
		
CONFIG += ordered

symbian: {
    BLD_INF_RULES.prj_exports += \
    "./rom/clock.iby                    CORE_APP_LAYER_IBY_EXPORT_PATH(clock.iby)" \
    "./rom/clockengines.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(clockengines.iby)" \
	"./rom/clockresources.iby           LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(clockresources.iby)" \
	\
    "./conf/clockapp.confml             APP_LAYER_CONFML(clockapp.confml)" \	
    "./conf/clockapp_101F874D.crml      APP_LAYER_CRML(clockapp_101F874D.crml)" \
    "./conf/clockapp_101F8876.crml      APP_LAYER_CRML(clockapp_101F8876.crml)" \
	# splashml
	"data/xml/clockapplication.docml  			/epoc32/data/z/resource/hb/splashml/clockapplication.docml" \
    "data/xml/clockapplication.splashml  		/epoc32/data/z/resource/hb/splashml/clockapplication.splashml" \
	# stubsis is added to provide IAD
	"./stubsis/clock_stub.sis           /epoc32/data/z/system/install/clock_stub.sis"  \
	# Backup registration file
# Date Time Settings
"data/backup_registration.xml   /epoc32/data/z/private/10005906/backup_registration.xml" \
"data/backup_registration.xml   /epoc32/release/winscw/udeb/z/private/10005906/backup_registration.xml" \
"data/backup_registration.xml   /epoc32/release/winscw/urel/z/private/10005906/backup_registration.xml" \
 # Clockserver
"data/backup_registration.xml   /epoc32/data/z/private/200159A2/backup_registration.xml"  \
"data/backup_registration.xml   /epoc32/release/winscw/udeb/z/private/200159A2/backup_registration.xml" \
"data/backup_registration.xml   /epoc32/release/winscw/urel/z/private/200159A2/backup_registration.xml" \
 # Clock
"data/backup_registration.xml   /epoc32/data/z/private/10005903/backup_registration.xml" \
"data/backup_registration.xml   /epoc32/release/winscw/udeb/z/private/10005903/backup_registration.xml" \
"data/backup_registration.xml   /epoc32/release/winscw/urel/z/private/10005903/backup_registration.xml
}

# End of file   --Don't remove this.
