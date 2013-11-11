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
# Description:  `Search application project file
#

TEMPLATE = app

TARGET = Searchapplication 

win32 {
    LIBS += -L$$PWD/../../bin/debug
    
    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }
    DESTDIR = $$PWD/../../bin/$$SUBDIRPART
}

LIBS +=  -lsearchruntimeprovider

QT += xml 

CONFIG += hb    
CONFIG += console
CONFIG += hb console mobility 

TRANSLATIONS += search.ts 
nft:DEFINES += NFT

DEPENDPATH += . \
              ./inc \
              ./src

INCLUDEPATH += . \
               ./inc \
               ../inc \
              ../runtimeproviders/searchruntimeprovider/inc 

symbian {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x2002C377
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles TrustedUI
    TARGET.EPOCSTACKSIZE = 0x14000 // 80kB
    TARGET.EPOCHEAPSIZE = 0x20000 0x800000 // 128kB - 16MB
    rom:DEFINES += ROM
    SKINICON = qtg_large_search
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

win32 {
include(searchapplication_installs_win32.pri)
}

include(searchapplication.pri)
