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

QT += testlib
CONFIG += hb qtestlib
CONFIG += symbian_test

TEMPLATE = app
TARGET = ut_fminternalfmutils

include ( ../ut_common.pri )
include ( ../../../src/common.pri )
include ( ../../../src/inc/commoninc.pri )
include ( ../../../src/inc/commonutils.pri )

DEPENDPATH += .
INCLUDEPATH += .

#TARGET.CAPABILITY = ALL -TCB
TARGET.CAPABILITY = CAP_APPLICATION DiskAdmin AllFiles PowerMgmt

HEADERS += src/*.h
SOURCES += src/*.cpp

symbian:MMP_RULES += SMPSAFE

symbian { 
    deploy.path = C:
    files.sources += data/Image.jpg \
                     data/Tone.mp3 \
                     data/unknow.unknow \
                     data/Imageformove.jpg
    files.path = /data/resource/fmtestdata
    DEPLOYMENT += files
    
    # This is for new exporting system coming in garden
    for(file, files.sources):BLD_INF_RULES.prj_exports += "./$$file $$deploy.path$$files.path/$$basename(file)"
}
