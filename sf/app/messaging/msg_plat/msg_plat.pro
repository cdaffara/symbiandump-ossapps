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

SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += ordered

include(messaging_media_resolver_api/messaging_media_resolver_api.pri)
include(mms_application_api/mms_application_api.pri)
include(mms_codec_client_api/mms_codec_client_api.pri)
include(mms_conformance_api/mms_conformance_api.pri)
include(mms_engine_settings_api/mms_engine_settings_api.pri)
include(mms_server_settings_api/mms_server_settings_api.pri)
include(conversation_services_client_api/conversation_services_client_api.pri)
include(conversation_services_plugin_api/conversation_services_plugin_api.pri)
include(conversation_services_utilities_api/conversation_services_utilities_api.pri)
include(shareui_api/shareui_api.pri)
include(messaging_history_api/messaging_history_api.pri)

symbian {
    headers.sources = $$PUBLIC_HEADERS
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" 
    for(header, headers.sources) {
        FILENAME = $$basename(header)
        BLD_INF_RULES.prj_exports += "$$header APP_LAYER_PLATFORM_EXPORT_PATH($$FILENAME)"
    }
}

