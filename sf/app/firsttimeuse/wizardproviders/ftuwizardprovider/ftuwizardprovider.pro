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
# Description:  FTU wizard provider project file
#


TEMPLATE = lib
CONFIG += hb
 
INCLUDEPATH += ./inc                         


DEFINES += FTUWIZARDPROVIDER_LIB
#DEFINES += LOGGING

win32 {
    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }
    DESTDIR = $$PWD/../../../bin/$$SUBDIRPART
    LIBS += -L$$PWD/../../../bin/debug
}

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \
               ../../inc 

LIBS += -lftuwizardmodel

QT -= webkit
QT += xml

symbian {
        TARGET.UID3 = 0x20026F9B
        TARGET.CAPABILITY = ALL -TCB
        TARGET.EPOCALLOWDLLDATA=1        
        TARGET.EPOCHEAPSIZE =  0x020000 0x1F00000
        INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
        LIBS += -lxqsettingsmanager
	BLD_INF_RULES.prj_exports += \
		"conf/ftuwizardprovider.confml	APP_LAYER_CONFML(ftuwizardprovider.confml)" \
		"conf/ftuwizardprovider_20026F9B.crml     APP_LAYER_CRML(ftuwizardprovider_20026F9B.crml)" 


     #Add default symbian libs required by the application (redundant/duplicate entries to be removed)
     LIBS += -lcone -leikcore -lmediaclientaudio -leikcoctl -leiksrv -lapparc -lavkon
     LIBS += -lefsrv -lcharconv -lws32 -lhal -lgdi -lapgrfx
}

include(ftuwizardprovider.pri)

#ONLY FOR DEVELOPMENT! REMOVE THIS BEFORE EACH HS_Domain RELEASE!



symbian:MMP_RULES += SMPSAFE
