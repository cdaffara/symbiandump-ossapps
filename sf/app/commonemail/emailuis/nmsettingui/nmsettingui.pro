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

TEMPLATE = lib

TARGET = nmsettingui

QT += core

DEPENDPATH += .

INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc

DEFINES += BUILD_NMAILSETTINGUI_DLL

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
                    ARMV5

# Input
HEADERS += inc/nmsettinguidef.h
HEADERS += inc/nmsettingsviewfactory.h
HEADERS += inc/nmmailboxsettingsmanager.h
HEADERS += inc/nmmailboxsettingview.h
HEADERS += inc/nmmailboxselectionview.h
HEADERS += inc/nmmailboxentryitem.h
HEADERS += inc/nmsettingsformcustomitems.h
HEADERS += inc/nmsettingsviewlauncher.h

SOURCES += src/nmsettingsviewfactory.cpp
SOURCES += src/nmmailboxsettingsmanager.cpp
SOURCES += src/nmmailboxsettingview.cpp
SOURCES += src/nmmailboxselectionview.cpp
SOURCES += src/nmmailboxentryitem.cpp
SOURCES += src/nmsettingsformcustomitems.cpp
SOURCES += src/nmsettingsviewlauncher.cpp

symbian: { 
    TARGET.EPOCALLOWDLLDATA = 1
    DEFINES += PLUGINUID3=0x2002C34A
    TARGET.UID3 = 0x2002C34A
    
    defBlock = \      
        "$${LITERAL_HASH}if defined(MARM)" \
        "DEFFILE  eabi/nmsettingui.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE  bwins/nmsettingui.def" \
        "$${LITERAL_HASH}endif"
        
    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
    
    # Build.inf rules
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/nmsettingui.iby CORE_APP_LAYER_IBY_EXPORT_PATH(nmsettingui.iby)"

    LIBS += -lcpframework
    LIBS += -lnmailuiengine
    LIBS += -lnmailbase
    LIBS += -leuser
    LIBS += -llibc
}

TARGET.CAPABILITY = All -TCB

RESOURCES += nmsettingui.qrc

DOCML += conf/nmmailboxsettingview.docml

# End of file.
