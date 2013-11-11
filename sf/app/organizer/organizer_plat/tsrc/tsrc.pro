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
# This is the project specification file for test code under organizer_plat.
#

TEMPLATE = subdirs
SUBDIRS +=  ../agenda_interface_api/tsrc \
			#../calendar_editor_api/tsrc \ TODO : uncomment after fixing build issues
			../calendar_launcher_api/tsrc \
			../notes_editor_api/tsrc \
			../clock_settingsview_plugin_api/tsrc

CONFIG += ordered
CONFIG += symbian_test

# End of file	--Don't remove this
