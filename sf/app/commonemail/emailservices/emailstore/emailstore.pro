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
#

TEMPLATE = subdirs

symbian*: { 
    SUBDIRS = \
	message_store/debuglog \
	message_store/PointSecMonitor \
	message_store/server \
        message_store/client \
    	base_plugin \
        message_store/exe \
        message_store/MsgStoreRfsPlugin \
        preinstall
        
    BLD_INF_RULES.prj_exports += \
        "rom/emailstore.iby $$CORE_APP_LAYER_IBY_EXPORT_PATH(emailstore.iby)" \
        "sis/emailstore_stub.sis             /epoc32/data/z/system/install/emailstore_stub.sis"     
 
}
CONFIG += ordered
