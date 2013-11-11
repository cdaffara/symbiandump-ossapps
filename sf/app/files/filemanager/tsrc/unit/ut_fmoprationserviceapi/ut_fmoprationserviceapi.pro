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
CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
TEMPLATE = app
TARGET = ut_fmoprationserviceapi

include ( ../../../src/common.pri )
include ( ../../../src/inc/commoninc.pri )
include ( ../../../src/inc/commonutils.pri )
include ( ut_fmoperationserviceapi.pri )

DEPENDPATH += .
INCLUDEPATH += ../../../src/fmbkupenginewrapper/inc
INCLUDEPATH += ../../../src/filemanager/src/operationservice
TARGET.CAPABILITY = ALL -TCB

QMAKE_EXTRA_TARGETS += test autotest

symbian { 
    deploy.path = C:
    files1.sources += data/dataforcopy.txt \
                      data/dataformove.txt 
    files2.sources += data/multifilesforcopy/copydata1.txt \
                      data/multifilesforcopy/copydata2.txt \
                      data/multifilesforcopy/copydata3.txt 
    files3.sources += data/multifilesformove/movedata1.txt \
                      data/multifilesformove/movedata2.txt \
                      data/multifilesformove/movedata3.txt 
    files1.path = /data/dataforfmoperation/data
    files2.path = /data/dataforfmoperation/data/multifilesforcopy
    files3.path = /data/dataforfmoperation/data/multifilesformove
    DEPLOYMENT += files1 files2 files3
}