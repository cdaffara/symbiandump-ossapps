#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Example of home screen content publishing client
#

TEMPLATE = app

CONFIG +=  mobility console debug

MOBILITY = serviceframework

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

INCLUDEPATH += ./inc

QT += xml

symbian: {
    SYMBIAN_PLATFORMS = WINSCW ARMV5
    # this should remove localization for test application
    load(hb.prf)
    CONFIG -= symbian_i18n
    TARGET.UID3 = 0x20022F74
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles TrustedUI
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    LIBS += -lefsrv
    
    testwallpaperimages.path = /data/images
    testwallpaperimages.sources += ./resource/*.jpg \
                                   ./resource/*.png
    
    DEPLOYMENT += testwallpaperimages
    
    MMP_RULES += SMPSAFE
    
} else {
    error("Only Symbian supported!")
}