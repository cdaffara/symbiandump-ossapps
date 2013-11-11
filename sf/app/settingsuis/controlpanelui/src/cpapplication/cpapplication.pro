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

TEMPLATE = app
TARGET = ControlPanel

ICON = resources/qtg_large_settings.svg

include ( ../common.pri )
include ( cpapplication.pri )

CONFIG += hb

RESOURCES += cpapplication.qrc

LIBS += -lcplogger -lcpframework -lcpcategorymodel

TRANSLATIONS = control_panel.ts

symbian: { 
    SKINICON = qtg_large_settings
    TARGET.UID3 = 0x20025FD9
    TARGET.EPOCHEAPSIZE = 0x020000 0x1000000
	
    deploy.path = C:
    headers.sources += data/mainview.cpcfg
    headers.path = /resource/qt/plugins/controlpanel/config
    DEPLOYMENT += exportheaders
    
    for(header, headers.sources):BLD_INF_RULES.prj_exports += "./$$header $$deploy.path$$headers.path/$$basename(header)"
}

symbian:MMP_RULES += SMPSAFE
