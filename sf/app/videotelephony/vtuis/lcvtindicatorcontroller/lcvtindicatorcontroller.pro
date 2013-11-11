# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = lib
TARGET = lcvtindicatorcontroller
CONFIG += hb dll svg
    
QT += core
    
INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += LCVTINDICATORCONTROLLER_LIB

libFiles.sources = lcvtindicatorcontroller.dll
libFiles.path = "!:/sys/bin"
DEPLOYMENT += libFiles

symbian: {
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x200315EC

    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lflogger -lxqplugins -lxqservice -lxqserviceutil
    defFiles = "$${LITERAL_HASH}ifdef WINS" \
        "DEFFILE bwins/lcvtindicatorcontroller.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE eabi/lcvtindicatorcontroller.def" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += defFiles
    :BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"               
    :BLD_INF_RULES.prj_exports += "rom/lcvtindctr.iby CORE_APP_LAYER_IBY_EXPORT_PATH(lcvtindctr.iby)"
}
# Input
include(lcvtindicatorcontroller.pri)