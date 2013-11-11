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
# 
# Description:
#     
# 

CONFIG      += hb
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
symbian {
    include(rom.pri)
}

# Input
HEADERS += mainwindow.h
SOURCES += main.cpp mainwindow.cpp

LIBS += -lfmfiledialog

symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MOC_DIR
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES += SMPSAFE
}
