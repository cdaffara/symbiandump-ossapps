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
# Description: The project file of FmBkupEngineWrapper
#

CONFIG += hb
TARGET = 
TEMPLATE = lib
QT = core
DEFINES += FMBKUPENGINE_LIBRARY

include ( ../common.pri )
include ( ../inc/commoninc.pri )
include ( fmbkupenginewrapper.pri )

symbian {
    TARGET.UID3 = 0x2002BCC1
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL DiskAdmin AllFiles PowerMgmt
    TARGET.VID = VID_DEFAULT
    LIBS += -lfmbkupengine
    LIBS += -lefsrv
    LIBS += -lavkon
    LIBS += -leikctl
    LIBS += -leikcore
    LIBS += -lcone
    LIBS += -lsysutil
    LIBS += -lplatformenv
    LIBS += -lbafl
}



symbian:MMP_RULES += SMPSAFE
