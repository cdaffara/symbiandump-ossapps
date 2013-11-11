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
#
symbian*: {
    # Build.inf rules
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
            "email_client_api/inc/emailinterfacefactory.h       APP_LAYER_PLATFORM_EXPORT_PATH(emailinterfacefactory.h)" \
            "email_client_api/inc/emailinterfacefactory.inl     APP_LAYER_PLATFORM_EXPORT_PATH(emailinterfacefactory.inl)" \
            "email_client_api/inc/emailclientapi.hrh            APP_LAYER_PLATFORM_EXPORT_PATH(emailclientapi.hrh)" \
            "email_client_api/inc/emailapidefs.h                APP_LAYER_PLATFORM_EXPORT_PATH(emailapidefs.h)" \
            "email_client_api/inc/emailsorting.h                APP_LAYER_PLATFORM_EXPORT_PATH(emailsorting.h)" \
            "email_client_api/inc/mmessageiterator.h            APP_LAYER_PLATFORM_EXPORT_PATH(mmessageiterator.h)" \
            "email_client_api/inc/memailclientapi.h             APP_LAYER_PLATFORM_EXPORT_PATH(memailclientapi.h)" \
            "email_client_api/inc/memailmailbox.h               APP_LAYER_PLATFORM_EXPORT_PATH(memailmailbox.h)" \
            "email_client_api/inc/memailmessage.h               APP_LAYER_PLATFORM_EXPORT_PATH(memailmessage.h)" \
            "email_client_api/inc/memailfolder.h                APP_LAYER_PLATFORM_EXPORT_PATH(memailfolder.h)" \
            "email_client_api/inc/memailaddress.h               APP_LAYER_PLATFORM_EXPORT_PATH(memailaddress.h)" \
            "email_client_api/inc/memailcontent.h               APP_LAYER_PLATFORM_EXPORT_PATH(memailcontent.h)" \
            "email_client_api/inc/memailmessagesearch.h         APP_LAYER_PLATFORM_EXPORT_PATH(memailmessagesearch.h)" \
            "email_client_api/inc/mmailboxsyncobserver.h        APP_LAYER_PLATFORM_EXPORT_PATH(mmailboxsyncobserver.h)" \
            "email_client_api/inc/mmailboxcontentobserver.h     APP_LAYER_PLATFORM_EXPORT_PATH(mmailboxcontentobserver.h)"
}
