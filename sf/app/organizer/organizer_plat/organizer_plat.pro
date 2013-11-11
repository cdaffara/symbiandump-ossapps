# Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Project specification file for organizer_plat
#

TEMPLATE = subdirs
CONFIG += ordered

include(agenda_versit_2_api/agenda_versit_2_api.pri)
include(calendar_exporter_api/calendar_exporter_api.pri)
include(calendar_importer_api/calendar_importer_api.pri)
include(calendar_launcher_api/calendar_launcher_api.pri)
include(agenda_interface_api/agenda_interface_api.pri)
include(calendar_editor_api/calendar_editor_api.pri)
include(notes_editor_api/notes_editor_api.pri)
include(eventviewer_plugin_api/eventviewer_plugin_api.pri)
include(clock_settings_keys_api/clock_settings_keys_api.pri)
include(calendar_settings_keys_api/calendar_settings_keys_api.pri)
include(clock_settingsview_plugin_api/clock_settingsview_plugin_api.pri)

symbian {
    headers.sources = $$PUBLIC_HEADERS
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" 
    for(header, headers.sources) {
        FILENAME = $$basename(header)
        BLD_INF_RULES.prj_exports += "$$header APP_LAYER_PLATFORM_EXPORT_PATH($$FILENAME)"
    }
}
