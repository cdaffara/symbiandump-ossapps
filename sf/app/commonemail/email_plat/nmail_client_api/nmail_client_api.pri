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
            "nmail_client_api/inc/nmapicommon.h            APP_LAYER_PLATFORM_EXPORT_PATH(nmapicommon.h)" \
            "nmail_client_api/inc/nmapiemailaddress.h      APP_LAYER_PLATFORM_EXPORT_PATH(nmapiemailaddress.h)" \
            "nmail_client_api/inc/nmapiemailservice.h      APP_LAYER_PLATFORM_EXPORT_PATH(nmapiemailservice.h)" \
            "nmail_client_api/inc/nmapidef.h               APP_LAYER_PLATFORM_EXPORT_PATH(nmapidef.h)" \
            "nmail_client_api/inc/nmapienvelopelisting.h   APP_LAYER_PLATFORM_EXPORT_PATH(nmapienvelopelisting.h)" \
            "nmail_client_api/inc/nmapieventnotifier.h     APP_LAYER_PLATFORM_EXPORT_PATH(nmapieventnotifier.h)" \
            "nmail_client_api/inc/nmapifolder.h            APP_LAYER_PLATFORM_EXPORT_PATH(nmapifolder.h)" \
            "nmail_client_api/inc/nmapifolderlisting.h     APP_LAYER_PLATFORM_EXPORT_PATH(nmapifolderlisting.h)" \
            "nmail_client_api/inc/nmapimailbox.h           APP_LAYER_PLATFORM_EXPORT_PATH(nmapimailbox.h)" \
            "nmail_client_api/inc/nmapimailboxlisting.h    APP_LAYER_PLATFORM_EXPORT_PATH(nmapimailboxlisting.h)" \
            "nmail_client_api/inc/nmapimessagebody.h       APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessagebody.h)" \
            "nmail_client_api/inc/nmapimessageenvelope.h   APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessageenvelope.h)" \
            "nmail_client_api/inc/nmapimessagetask.h       APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessagetask.h)" \
            "nmail_client_api/inc/nmapimessagemanager.h    APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessagemanager.h)" \
            "nmail_client_api/inc/nmapioperation.h         APP_LAYER_PLATFORM_EXPORT_PATH(nmapioperation.h)" \
            "nmail_client_api/inc/nmapimessagesearch.h     APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessagesearch.h)" \
            "nmail_client_api/inc/nmapimessage.h           APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessage.h)" \
            "nmail_client_api/inc/nmapimessagecontent.h    APP_LAYER_PLATFORM_EXPORT_PATH(nmapimessagecontent.h)" \
            "nmail_client_api/inc/nmapitextcontent.h       APP_LAYER_PLATFORM_EXPORT_PATH(nmapitextcontent.h)" \
            "nmail_client_api/inc/nmapiattachment.h        APP_LAYER_PLATFORM_EXPORT_PATH(nmapiattachment.h)"

}
