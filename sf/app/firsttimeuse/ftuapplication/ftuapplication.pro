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
# Description:  First Time Use application project file
#

TEMPLATE = app

TARGET = ftuapplication 

win32 {
    LIBS += -L$$PWD/../../bin/debug
    
    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }
    DESTDIR = $$PWD/../../bin/$$SUBDIRPART
}

QT += xml 

CONFIG += hb mobility
MOBILITY = serviceframework

CONFIG += hb    
CONFIG += console
#DEFINES += LOGGING
nft:DEFINES += NFT

DEPENDPATH += . \
              ./inc \
              ./src

INCLUDEPATH += . \
               ./inc \
               ../inc

symbian {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x20026F95
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles TrustedUI
    TARGET.EPOCSTACKSIZE = 0x14000 // 80kB
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    rom:DEFINES += ROM
    ICON = resources/ftu_app_icon.svg
    icon.sources = resources/ftu_app_icon.svg
    icon.path = /fturesources
   
    DEPLOYMENT += icon
 
    LIBS += -lxqsettingsmanager 
    RSS_RULES += "hidden = KAppIsHidden;"


     #Add default symbian libs required by the application (redundant/duplicate entries to be removed)
     LIBS += -lcone -leikcore -lmediaclientaudio -leikcoctl -leiksrv -lapparc -lavkon
     LIBS += -lefsrv -lcharconv -lws32 -lhal -lgdi -lapgrfx
}

win32 {
include(ftuapplication_installs_win32.pri)
}

include(ftuapplication.pri)


symbian:MMP_RULES += SMPSAFE
