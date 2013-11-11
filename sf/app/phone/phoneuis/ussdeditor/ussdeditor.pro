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
# Description:
#
#
#

TEMPLATE = app

TARGET = ussdeditor
DEPENDPATH += .
HEADERS += $$PUBLIC_HEADERS
MOC_DIR = moc

CONFIG += hb

symbian: {

TARGET.CAPABILITY = CAP_APPLICATION NetworkControl
TARGET.UID3 = 0x10005955

RSS_RULES = "hidden = KAppIsHidden;"

INCLUDEPATH += . 
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += MOC_DIR

LIBS += -lphoneclient
LIBS += -lflogger

SOURCES += src/main.cpp
SOURCES += src/ussdeditorquery.cpp
SOURCES += src/ussdcomms.cpp 

HEADERS += inc/ussdeditorquery.h 
HEADERS += inc/ussdcomms.h
HEADERS += inc/tflogger.h

BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             "rom/ussdeditor.iby CORE_APP_LAYER_IBY_EXPORT_PATH(ussdeditor.iby)" \
                             "rom/ussdeditorresources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(ussdeditorresources.iby)" \
                             "rom/ussdeditor_stub.sis /epoc32/data/z/system/install/ussdeditor_stub.sis"

TRANSLATIONS = ussd.ts
}
