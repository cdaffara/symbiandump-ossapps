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
# 
# Description:
#     Rom exports for demo_fmfiledialog
# 

symbian {
    DEMO_FMFILEDIALOG_IBY_DIR = $$section(PWD, ":", 1)

    exists(/epoc32/include/platform_paths.hrh) {
        BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    } else {
        BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <domain\osextensions\platform_paths.hrh>"
    }

    BLD_INF_RULES.prj_exports += "$$DEMO_FMFILEDIALOG_IBY_DIR/demo_fmfiledialog.iby CORE_APP_LAYER_IBY_EXPORT_PATH(demo_fmfiledialog.iby)"
}
