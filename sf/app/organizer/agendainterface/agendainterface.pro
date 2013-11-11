#  
#  Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
#  All rights reserved.
#  This component and the accompanying materials are made available
#  under the terms of "Eclipse Public License v1.0"
#  which accompanies this distribution, and is available
#  at the URL "http://www.eclipse.org/legal/epl-v10.html".
#  
#  Initial Contributors:
#  Nokia Corporation - initial contribution.
#  
#  Contributors: 
#  
#  Description: Project definition file for agendainterface.
#  

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += agendautil

symbian: {
	BLD_INF_RULES.prj_exports += \
	"./rom/agendainterface.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(agendainterface.iby)"
}
		   
# End of file	--Don't remove this.
