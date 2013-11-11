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

BLD_INF_RULES.prj_exports += \
"$${LITERAL_HASH}include <platform_paths.hrh>"

symbian {
MP_IBY_DIR = $$section(PWD,":",1)

#iby exports to core
BLD_INF_RULES.prj_exports += "$$MP_IBY_DIR/musicwidgetplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(musicwidgetplugin.iby)"

#iby exports to language
BLD_INF_RULES.prj_exports += "$$MP_IBY_DIR/musicwidgetplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(musicwidgetplugin_resources.iby)"
}
