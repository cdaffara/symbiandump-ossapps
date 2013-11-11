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


# Deploy stub SIS and IBY files
symbian {

    CAMERAX_IBY_DIR = $$section(PWD,":",1)
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    BLD_INF_RULES.prj_exports += "$$CAMERAX_IBY_DIR/camerax.iby           CORE_APP_LAYER_IBY_EXPORT_PATH(camerax.iby)"
    BLD_INF_RULES.prj_exports += "$$CAMERAX_IBY_DIR/cameraxresources.iby  LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cameraxresources.iby)"
    BLD_INF_RULES.prj_exports += "$$CAMERAX_IBY_DIR/camerax_stub.sis      /epoc32/data/z/system/install/camerax_stub.sis"

}
