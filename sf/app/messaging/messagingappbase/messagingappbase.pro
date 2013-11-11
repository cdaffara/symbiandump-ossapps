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

CONFIG += ordered

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

SUBDIRS += mtmregistry/mtmregistry.pro

# Build.inf rules

symbian {
     BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \  
#### MSGMEDIA ####
     "./msgmedia/rom/MsgMedia.iby                     CORE_APP_LAYER_IBY_EXPORT_PATH(MsgMedia.iby)" \
     ":zip ./msgmedia/help/data/xhtml.zip             /epoc32/data/z/resource/ overwrite" \
     ":zip ./msgmedia/help/data/xhtml.zip             /epoc32/winscw/c/resource/ overwrite" \
     "./msgmedia/help/inc/msg.hlp.hrh                 MW_LAYER_PLATFORM_EXPORT_PATH(csxhelp/msg.hlp.hrh)" \
     "./msgmedia/help/rom/msgmediahelps_variant.iby   CUSTOMER_APP_LAYER_IBY_EXPORT_PATH(msgmediahelps_variant.iby)" 
}

                 
symbian {
BLD_INF_RULES.prj_mmpfiles = "./msgmedia/group/MsgMedia.mmp"
}
