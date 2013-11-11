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
#

TEMPLATE = lib
TARGET = bubblemanager2
CONFIG += hb

INCLUDEPATH += . ../inc

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x100058FF
    VERSION = 10.0
    
    defFiles = \
    "$${LITERAL_HASH}ifdef WINS" \
        "DEFFILE bwins/bubblemanager2.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE eabi/bubblemanager2.def" \
    "$${LITERAL_HASH}endif"
    MMP_RULES += defFiles

    LIBS += -lthumbnailmanagerqt
    
    BLD_INF_RULES.prj_exports += \
    "./rom/bubblecore_stub.sis /epoc32/data/z/system/install/bubblecore_stub.sis"
}
else:win32 { 
    # Put it to Hb/lib because it is in path
    DESTDIR = c:/hb/lib
    DLLDESTDIR = c:/hb/bin
    INCLUDEPATH += c:/hb/include/hbcore \
        c:/hb/include/hbwidgets \
        c:/hb/include/hbtools
}
DEFINES += BUILD_BUBBLEMANAGER

# Input
include(src/bubblecore.pri)
include(xml/bubbledocml.pri)
RESOURCES += bubblecore.qrc


symbian:MMP_RULES += SMPSAFE
