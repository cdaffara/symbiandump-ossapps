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
# Description: This is the project specification file for the notes project.
#

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += notesui

symbian: {
	BLD_INF_RULES.prj_exports += \
	"./rom/notes.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(notes.iby)" \
	"./rom/notesresources.iby           LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(notesresources.iby)" \
	# stubsis is added to provide IAD
	"./stubsis/notes_stub.sis             /epoc32/data/z/system/install/notes_stub.sis"
}

# End of file	--Don't remove this.
