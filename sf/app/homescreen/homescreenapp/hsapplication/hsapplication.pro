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

include(../common.pri)

CONFIG += console

#QT += xml

CONFIG += hb mobility
MOBILITY = serviceframework publishsubscribe

fute:DEFINES += FUTE

DEPENDPATH += . \
              ./inc \
              ./src

INCLUDEPATH += . \
               ./inc \
                ../hsutils/inc \
                ../hsdomainmodel/inc

TRANSLATIONS = homescreen.ts

LIBS += -lhsutils \
        -lhsdomainmodel \
	-lcaclient


symbian: {
    TARGET.UID3 = 0x20022F35
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles TrustedUI
    TARGET.EPOCHEAPSIZE = 0x20000 0x1600000 // 128kB - 23MB
    ICON = resource/qgn_menu_appsgrid.svg

    LIBS +=  -lapgrfx \
             -lcone \
             -lefsrv
    include(installs_symbian.pri)
    include(hsapplication_exports_to_rom.pri)    

    CONFIG += service
    LIBS +=  -lxqservice -lxqserviceutil -lXQKeyCapture
  
    SERVICE.FILE = ipc_service_conf.xml
    
    MMP_RULES += SMPSAFE
}

!symbian: {
    include(installs_win.pri)
}

include(hsapplication.pri)

exportResources(./*.qm, resource/qt/translations)
