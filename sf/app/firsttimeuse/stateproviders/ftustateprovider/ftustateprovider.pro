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
# Description:  FTU state provider project file
#

TEMPLATE = lib

DEFINES += FTUSTATEPROVIDER_LIB
#DEFINES += LOGGING

symbian {
    CONFIG(debug, debug|release) {
      DESTDIR = debug      
    } else {
      DESTDIR = release
    }
}

win32 {
    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }
    DESTDIR = $$PWD/../../../bin/$$SUBDIRPART/fturesources/plugins/stateproviders
    LIBS += -L$$PWD/../../../bin/debug
}

LIBS += -lftuwizardmodel
LIBS += -lftuwizardprovider
LIBS += -lfturuntimeservices
LIBS += -lxqsettingsmanager

CONFIG += debug_and_release

coverage:DEFINES += COVERAGE_MEASUREMENT

CONFIG += hb

QT += xml

nft:DEFINES += NFT

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \
               ../../inc \
               ../../wizardproviders/ftuwizardprovider/inc
               
#TRANSLATIONS=ftustateprovider.ts

symbian {
    TARGET.UID2 = 0x20004C45
    TARGET.UID3 = 0x20026F99
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    
    #include(ftustateprovider_installs_symbian.pri)
    
    BLD_INF_RULES.prj_exports += "./inc/ftustateprovider_global.h |../../inc/" \                             
                             "./inc/ftustateprovider.h |../../inc/" \
                             "conf/ftustateprovider.confml            APP_LAYER_CONFML(ftustateprovider.confml)" \
                             "conf/Ftustateprovider_20026f99.crml     APP_LAYER_CRML(Ftustateprovider_20026f99.crml)" 


     #Add default symbian libs required by the application (redundant/duplicate entries to be removed)
     LIBS += -lcone -leikcore -lmediaclientaudio -leikcoctl -leiksrv -lapparc -lavkon
     LIBS += -lefsrv -lcharconv -lws32 -lhal -lgdi -lapgrfx
}

win32:include(ftustateprovider_installs_win32.pri)

symbian:unix:include(ftustateprovider_installs_unix.pri)

include(ftustateprovider.pri)

#ONLY FOR DEVELOPMENT! REMOVE THIS BEFORE EACH HS_Domain RELEASE!



symbian:MMP_RULES += SMPSAFE
