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
#     Zhiqiang Yang <zhiqiang.yang@nokia.com>
# 
# Description:
#     The project file of file manager
# 

CONFIG      += hb
TEMPLATE    = app
TARGET = filemngr
ICON = resources/qtg_large_filemgr.svg
RESOURCES += filemanager.qrc
INCLUDEPATH += ../fmbkupenginewrapper/inc
# remove this path when export plat header to app layer path
# INCLUDEPATH += ../../filemanager_plat/inc

symbian {
    include(../../rom/rom.pri)
}

include ( ../common.pri )
include ( ../inc/commoninc.pri )
include ( ../inc/commonutils.pri )
include ( filemanager.pri )

LIBS += -lfmbkupenginewrapper
LIBS += -lfmfiledialog

symbian {
    SKINICON = qtg_large_filemgr
    LIBS += -lplatformenv \
            -lapmime \
            -lxqservice \
            -lxqutils \
            -lshareui \
            -lefsrv \
            -lapgrfx \
    	    -lcone \
            -lsysutil \
            -lbafl \
            -leikcore \
            -lafservice

    TARGET.UID3 = 0x2002BCC0
    TARGET.EPOCHEAPSIZE = 0x020000 0x1000000
    TARGET.CAPABILITY = CAP_APPLICATION DiskAdmin AllFiles PowerMgmt

    BLD_INF_RULES.prj_exports += "./backup_registration.xml z:/private/2002BCC0/backup_registration.xml"
    BLD_INF_RULES.prj_exports += "./burconfig.xml z:/private/2002BCC0/burconfig.xml"	
}

symbian:MMP_RULES += SMPSAFE

TRANSLATIONS = filemanager.ts
