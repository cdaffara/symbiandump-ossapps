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

TEMPLATE = subdirs

SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += ordered

include(mms_client_mtm_api/mms_client_mtm_api.pri)

symbian {
    headers.sources = $$PUBLIC_HEADERS
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" 
    for(header, headers.sources) {
        FILENAME = $$basename(header)
        BLD_INF_RULES.prj_exports += "$$header APP_LAYER_PUBLIC_EXPORT_PATH($$FILENAME)"
    }
}

symbian {
    headers.sources = $$PUBLIC_HEADERS
    BLD_INF_RULES.prj_exports += "./msg_smil_dtd_api/inc/smildtd.h      /epoc32/include/smildtd.h" \
    "./msg_smil_dtd_api/inc/smildtdenum.h     /epoc32/include/smildtdenum.h"
     
}