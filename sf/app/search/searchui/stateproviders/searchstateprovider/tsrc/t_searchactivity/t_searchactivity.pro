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
# Description:  Search stateprovider unit test project file
#

TEMPLATE = app

CONFIG += hb console mobility

MOBILITY = serviceframework

symbian {         
CONFIG += symbian_test               
TARGET.UID2 = 0x100039CE

TARGET.CAPABILITY = ALL -TCB -DRM
TARGET.EPOCSTACKSIZE = 0x14000 // 80kB
TARGET.EPOCHEAPSIZE = 0x20000 0x800000 // 128kB - 16MB
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

include(t_searchactivity.pri)

