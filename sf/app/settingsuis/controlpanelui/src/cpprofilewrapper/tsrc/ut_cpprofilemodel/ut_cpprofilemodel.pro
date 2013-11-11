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
TARGET = ut_cpprofilemodel

QT += testlib
CONFIG += hb qtestlib
CONFIG += symbian_test

include (../unit_common.pri)

HEADERS += src/*.h
SOURCES += src/*.cpp

QMAKE_EXTRA_TARGETS += test autotest

DEPENDPATH += .

INCLUDEPATH += . ../../src\
	        ../../../inc 

LIBS += -lcpframework 
LIBS += -lcpprofilewrapper \
        -lprofileeng  \
        -lcentralrepository \
        -lcharconv
symbian { 
    deploy.path = C:
    soundfiles.sources += data/testsound.aac \
                          data/testsound2.aac
    soundfiles.path = /resource/cptestdata/sounds
    DEPLOYMENT += soundfiles
    
    # This is for new exporting system coming in garden
    for(soundfile, soundfiles.sources):BLD_INF_RULES.prj_exports += "./$$soundfile $$deploy.path$$soundfiles.path/$$basename(soundfile)"
}

symbian:MMP_RULES += SMPSAFE
