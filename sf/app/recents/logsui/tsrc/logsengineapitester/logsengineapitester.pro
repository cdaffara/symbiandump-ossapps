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

SERVICEAPP=app
TARGET=logsengineapitester

CONFIG += hb

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

include(src/logsengineapitester.pri)

LIBS+=-llogsengine -lcentralrepository

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xE0014E73
    TARGET.CAPABILITY = CAP_APPLICATION
}


libFiles.sources = logsengineapitester.exe 
libFiles.path = "!:\sys\bin"
DEPLOYMENT += libFiles



symbian:MMP_RULES += SMPSAFE
