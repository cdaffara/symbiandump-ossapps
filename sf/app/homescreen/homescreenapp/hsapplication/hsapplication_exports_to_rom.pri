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

# configuration file exports
BLD_INF_RULES.prj_exports += \
    "conf/confml/CI_homescreendb.confml                APP_LAYER_CONFML(CI_homescreendb.confml)" \
    "conf/confml/homescreendb.confml                   APP_LAYER_CONFML(homescreendb.confml)" \
    "conf/implml/homescreendb.implml                   APP_LAYER_CRML(homescreendb.implml)" \
    "conf/implml/homescreendb_templates/create.sql     APP_LAYER_CRML(homescreendb_templates/create.sql)" \
    "conf/implml/homescreendb_templates/fill.sql       APP_LAYER_CRML(homescreendb_templates/fill.sql)"
    