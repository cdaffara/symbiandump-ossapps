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
# Description: display plugin source files
#
include (../common.pri)
include (ringtoneview.pri)
TARGET = cpringtoneview
TEMPLATE = lib

CONFIG += hb
DEFINES += CPRINGTONEVIEW_LIBRARY
symbian {
    LIBS +=  -lcpprofilewrapper -lcplogger -lcpframework -lxqservice -lxqserviceutil
    TARGET.UID3 = 0X2002873A
    TARGET.CAPABILITY = All -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}


symbian:MMP_RULES += SMPSAFE
