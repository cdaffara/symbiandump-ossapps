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

HEADERS += ./inc/*.h

SOURCES += ./src/*.cpp

defineTest(exportResources) {
    symbian {
        for(subdirs, 1) {
            entries = $$files($$subdirs)
            for(entry, entries) : BLD_INF_RULES.prj_exports += "./$$entry z:/$$replace(2, ^/,)/$$basename(entry)"
        }
        export ( BLD_INF_RULES.prj_exports)
    }
}

symbian: plugin { # copy qtstub and manifest

    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_SUBDIR

    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_SUBDIR
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin

    !contains(MOBILITY, serviceframework):qtplugins.sources += resource/$${TARGET}.manifest
    contains(MOBILITY, serviceframework):BLD_INF_RULES.prj_exports += "resource/$${TARGET}.xml z:$$qtplugins.path/$${TARGET}.xml"
    
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin z:$$qtplugins.path/$$basename(qtplugin)"
}