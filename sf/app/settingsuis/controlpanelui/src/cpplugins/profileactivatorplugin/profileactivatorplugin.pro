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

TEMPLATE = lib
TARGET = cpprofileactivator

CONFIG += hb plugin


include ( ../cpplugincommon.pri )
include ( profileactivatorplugin.pri )
LIBS +=  -lcpprofilewrapper
symbian { 
    TARGET.UID3 = 0X20028739
    TARGET.CAPABILITY = All -TCB 
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS +=  -lprofileeng
}


symbian:MMP_RULES += SMPSAFE
