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

TEMPLATE = app
TARGET = nmailagent
QT += core
CONFIG += hb 
CONFIG += console

load(hb.prf)
symbian:CONFIG -= symbian_i18n

MOC_DIR = moc

INCLUDEPATH += ../../inc

HEADERS   += inc/nmmailagent.h \
             inc/nmmailagentheaders.h \
             inc/ssastartupwatcher.h
    
SOURCES   += src/main.cpp \
             src/nmmailagent.cpp \
             src/ssastartupwatcher.cpp

LIBS += -lnmailbase
LIBS += -lnmailuiengine
LIBS += -lnmutilities
LIBS += -llibc

symbian*: { 
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    BLD_INF_RULES.prj_exports += "rom/nmailagent.iby $$CORE_APP_LAYER_IBY_EXPORT_PATH(nmailagent.iby)"

    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             "conf/2002C326.txt                  /epoc32/release/winscw/udeb/z/private/10202BE9/2002C326.txt" \
                             "conf/2002C326.txt                  /epoc32/release/winscw/urel/z/private/10202BE9/2002C326.txt" \
                             "conf/2002C326.txt                  /epoc32/data/z/private/10202BE9/2002C326.txt"

    RSS_RULES += "hidden = KAppIsHidden;"

    TARGET.EPOCHEAPSIZE = 0x1000 0x100000 // MAX 1MB

    LIBS += -ldomaincli
    LIBS += -lxqservice
    LIBS += -lxqsettingsmanager
    LIBS += -lxqsystemtoneservice
    LIBS += -lhwrmvibraclient

    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x2002C326
    TARGET.CAPABILITY = CAP_APPLICATION
    
    MMP_RULES += SMPSAFE
}

win32 {
    DESTDIR = ../../bin
}



