# 
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Symbian Foundation License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
#  
# Initial Contributors:
# Nokia Corporation - initial contribution.
# 
# Contributors: 
# 
# Description:
# This is the project specification file notes test code
#

TEMPLATE = subdirs
SUBDIRS += \
		unittest_notesmodel \
		unittest_noteseditor
CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += ordered

symbian: {
	BLD_INF_RULES.prj_testexports += \
	"./rom/unit_notes.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unit_notes.iby)" \
	"./rom/unit_noteseditor.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unit_noteseditor.iby)"
}

# End of file	--Don't remove this
