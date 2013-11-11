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
# Description: 
#

TEMPLATE = app
TARGET = testshareui

DEPENDPATH += . inc src 
INCLUDEPATH += inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb



TARGET.UID3 = 0x2002DD18
TARGET.CAPABILITY = All -TCB
TARGET.EPOCHEAPSIZE = 0x20000 0x1000000
  
# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" 
     

# Input
HEADERS += testshareuimainwindow.h \
           testshareuiview.h 

SOURCES += main.cpp \
          testshareuimainwindow.cpp \
          testshareuiview.cpp  

# Libs
LIBS += -lshareui		


symbian:MMP_RULES += SMPSAFE
