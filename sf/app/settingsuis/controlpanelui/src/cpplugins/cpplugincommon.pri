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
# Description:  controlpanel plugin project qmake common settings
#
include (../common.pri)

LIBS += -lcplogger -lcpframework -lcpcategorymodel

symbian: plugin { # copy qtstub and manifest
	
		PLUGIN_STUB_PATH = /resource/qt/plugins/controlpanel
		
		deploy.path = C:
		pluginstub.sources = $${TARGET}.dll
		pluginstub.path = $$PLUGIN_STUB_PATH
		DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_STUB_PATH
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
     
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin  $$deploy.path$$qtplugins.path/$$basename(qtplugin)" 
}
