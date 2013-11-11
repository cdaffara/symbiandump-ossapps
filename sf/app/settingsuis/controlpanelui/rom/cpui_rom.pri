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
#

symbian {
		BLD_INF_RULES.prj_exports += \
				"$${LITERAL_HASH}include<platform_paths.hrh>" \
				"rom/controlpanelui.iby CORE_APP_LAYER_IBY_EXPORT_PATH(controlpanelui.iby)"
		
		
		BLD_INF_RULES.prj_exports += \
				"$${LITERAL_HASH}include<platform_paths.hrh>" \
				"rom/controlpanelui_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(controlpanelui_resources.iby)"
}