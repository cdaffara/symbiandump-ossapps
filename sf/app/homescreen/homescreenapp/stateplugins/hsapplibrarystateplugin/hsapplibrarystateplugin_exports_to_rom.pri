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
    "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "conf/hsapplibrary.confml           APP_LAYER_CONFML(hsapplibrary.confml)" \
    "conf/CI_hsapplibrary.confml        APP_LAYER_CONFML(CI_hsapplibrary.confml)" \
    "conf/hsapplibrary.implml           APP_LAYER_CRML(hsapplibrary.implml)" \
    "conf/hsapplibrary_20022F97.crml    APP_LAYER_CRML(hsapplibrary_20022F97.crml)"

