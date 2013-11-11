#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).

#
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
#



TEMPLATE = app
TARGET = phonebook

#DEFINES += BUILD_QTCONTACTS

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../inc
INCLUDEPATH += ../cntcommonui/core
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

MOC_DIR = moc

CONFIG += hb
CONFIG += service

LIBS += -lhbcore \
        -lcntcommonui \
        -lxqservice \
        -lxqserviceutil
        
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable

# Input
HEADERS += inc/cntapplication.h
HEADERS += inc/cntinterface.h
HEADERS += inc/cntappservicehandler.h

SOURCES += src/main.cpp
SOURCES += src/cntappservicehandler.cpp

# capability
TARGET.CAPABILITY = CAP_APPLICATION NetworkControl

TRANSLATIONS = contacts.ts
    
symbian: {
        # Skip the UID2/3 thing
        TARGET.UID3 = 0x20022EF9
        TARGET.EPOCSTACKSIZE = 0x14000
        TARGET.EPOCHEAPSIZE = 0x1000 0xA00000
        
        SKINICON = qtg_large_phonebook 

        :BLD_INF_RULES.prj_exports += "resources/phonebook.splashml /epoc32/release/winscw/udeb/z/resource/hb/splashml/phonebook.splashml
        :BLD_INF_RULES.prj_exports += "resources/phonebook.splashml /epoc32/data/z/resource/hb/splashml/phonebook.splashml
        :BLD_INF_RULES.prj_exports += "resources/phonebook.docml /epoc32/release/winscw/udeb/z/resource/hb/splashml/phonebook.docml
        :BLD_INF_RULES.prj_exports += "resources/phonebook.docml /epoc32/data/z/resource/hb/splashml/phonebook.docml
}
symbian:MMP_RULES += SMPSAFE