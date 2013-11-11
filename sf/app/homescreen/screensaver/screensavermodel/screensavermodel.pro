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
# Description: screensavermodel.pro
#

TEMPLATE = lib

include (../common.pri)

symbian: {
TARGET.UID3 = 0x20027051
MMP_RULES += SMPSAFE
}

DEFINES += SREENSAVERMODEL_LIB

include(screensavermodel.pri)

BLD_INF_RULES.prj_exports *= "inc/screensaver.h             |../inc/screensaver.h"
BLD_INF_RULES.prj_exports *= "inc/screensaverfactory.h      |../inc/screensaverfactory.h"
BLD_INF_RULES.prj_exports *= "inc/screensavermodel_global.h |../inc/screensavermodel_global.h"
