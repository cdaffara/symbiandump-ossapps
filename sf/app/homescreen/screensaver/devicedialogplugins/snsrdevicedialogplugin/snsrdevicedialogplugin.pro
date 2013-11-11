#
# Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: snsrdevicedialogplugin.pro
#

TEMPLATE = lib
CONFIG += hb plugin mobility
MOBILITY = serviceframework

include (../../common.pri)
include (snsrdevicedialogplugin.pri)

LIBS += -lscreensavermodel

symbian: {
    LIBS += -lsnsrdisplaycontrolclient
    LIBS += -lxqsettingsmanager

    TARGET.UID3 = 0x2002704E
    TARGET.CAPABILITY = CAP_ECOM_PLUGIN
    
    include(snsrdevicedialogplugin_exports_to_rom.pri)

    pluginstub.sources = snsrdevicedialogplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub
    MMP_RULES += SMPSAFE
}

win32: {
    include ($$[QMAKE_MKSPECS]/features/hb_install.prf)
    DESTDIR = $$HB_PLUGINS_DIR/devicedialogs
}
