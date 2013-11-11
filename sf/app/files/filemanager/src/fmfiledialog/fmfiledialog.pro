# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
# The project file of FmFileDialog

TARGET = 
CONFIG += hb
TEMPLATE = lib
DEFINES += FMFILEDIALOG_LIBRARY
RESOURCES += fmfiledialog.qrc
include ( ../common.pri )
include ( ../inc/commoninc.pri )
include ( ../inc/commonutils.pri )
include ( fmfiledialog.pri )

symbian { 
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
            
    TARGET.UID3 = 0x2002BCC3
}


symbian:MMP_RULES += SMPSAFE
