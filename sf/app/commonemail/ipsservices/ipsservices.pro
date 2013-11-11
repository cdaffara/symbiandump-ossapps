#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#

TEMPLATE = subdirs

win32: { 
    
    
}

symbian*: { 
    BLD_INF_RULES.prj_exports += \
	    "ipssosplugin/inc/ipsplgcommon.h |inc/ipsplgcommon.h" \
	    "rom/FreestyleIpsServices.iby       CORE_APP_LAYER_IBY_EXPORT_PATH(FreestyleIpsServices.iby)" \
        "rom/nmimapclientplugin.iby       CORE_APP_LAYER_IBY_EXPORT_PATH(nmimapclientplugin.iby)" \
        "rom/nmpopclientplugin.iby       CORE_APP_LAYER_IBY_EXPORT_PATH(nmpopclientplugin.iby)"
    SUBDIRS = \
    ipssosplugin \
    ipssosaoplugin \
    nmimapclientplugin \
    nmpopclientplugin \
    nmipsmtms \
    nmipssettings
    
    
}
CONFIG += ordered
