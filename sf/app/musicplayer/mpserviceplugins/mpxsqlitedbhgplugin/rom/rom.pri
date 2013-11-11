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
MP_IBY_DIR = $$section(PWD,":",1)

BLD_INF_RULES.prj_exports += "$$MP_IBY_DIR/mpxsqlitedbhgplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(mpxsqlitedbhgplugin.iby)" \
                             "$$MP_IBY_DIR/mpxsqlitedbhgpluginrsc.iby CORE_APP_LAYER_IBY_EXPORT_PATH(mpxsqlitedbhgpluginrsc.iby)"
}