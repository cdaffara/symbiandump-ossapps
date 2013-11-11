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

symbian: {
    load(data_caging_paths)
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    TARGET.EPOCALLOWDLLDATA = 1  
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    
    exists($$OUT_PWD/data/$${TARGET}.cpcfg) {
        cpcfg.sources = data/$${TARGET}.cpcfg
        cpcfg.path = $$QT_PLUGINS_BASE_DIR/controlpanel/config
    }
    plugin.sources = $${TARGET}.dll
    plugin.path = $$QT_PLUGINS_BASE_DIR/controlpanel
    DEPLOYMENT += plugin cpcfg
    
    # For armv5 rom build
    qtplugin.sources += qmakepluginstubs/$${TARGET}.qtplugin
    qtplugin.path = $$QT_PLUGINS_BASE_DIR/controlpanel

    for(cpcfg, cpcfg.sources): \
        BLD_INF_RULES.prj_exports += "./$$cpcfg /$$HW_ZDIR$$RESOURCE_FILES_DIR/qt/plugins/controlpanel/config/$$basename(cpcfg)"
    for(qtplugin, qtplugin.sources): \
        BLD_INF_RULES.prj_exports += "./$$qtplugin /$$HW_ZDIR$$RESOURCE_FILES_DIR/qt/plugins/controlpanel/$$basename(qtplugin)" 
} else: {
    INCLUDEPATH += ../../../../../mw/phonesrv/phonesrv_plat/phone_settings_api/inc
    INCLUDEPATH += ../../../../../mw/phonesrv/phonesrv_plat/ss_settings_api/inc
    INCLUDEPATH += ../../../../../mw/gsprofilesrv/controlpanel/controlpanel_plat/inc
    INCLUDEPATH += ../../../../../../epoc32/include
    INCLUDEPATH += ../../../../../../epoc32/include/mw
    LIBS += -LC:/ControlPanel/debug/bin
        DESTDIR = C:/ControlPanel/debug/bin
}
