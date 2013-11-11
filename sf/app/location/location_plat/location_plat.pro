#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"

include(location_picker_service_api/location_picker_service_api.pri)
include(location_data_harvester_api/location_data_harvester_api.pri)


for(filename,PLATFORM_HEADERS){
    BLD_INF_RULES.prj_exports *= "$$filename APP_LAYER_PLATFORM_EXPORT_PATH($$basename(filename))"
  }


#BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include \"group/bld.inf\""


symbian:MMP_RULES += SMPSAFE
