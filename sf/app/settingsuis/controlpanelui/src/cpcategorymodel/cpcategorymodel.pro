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
TARGET = cpcategorymodel

include ( ../common.pri )
include ( ../inc/inc.pri )
include ( ./cpcategorymodel.pri )

CONFIG += Hb xml
DEFINES += BUILD_CPCATEGORY_GLOBAL
    
LIBS += -lcplogger -lcpframework

symbian: { 
    TARGET.UID3 = 0X20028736
}




symbian:MMP_RULES += SMPSAFE
