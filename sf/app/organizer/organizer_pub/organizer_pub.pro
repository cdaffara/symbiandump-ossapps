# Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Project specification file for organizer_plat
#

TEMPLATE = subdirs
CONFIG += ordered

include(calendar_interim_utils2_api/calendar_interim_utils2_api.pri)

symbian {
    headers.sources = $$PUBLIC_HEADERS
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" 
    for(header, headers.sources) {
        FILENAME = $$basename(header)
        BLD_INF_RULES.prj_exports += "$$header APP_LAYER_PLATFORM_EXPORT_PATH($$FILENAME)"
    }
}
