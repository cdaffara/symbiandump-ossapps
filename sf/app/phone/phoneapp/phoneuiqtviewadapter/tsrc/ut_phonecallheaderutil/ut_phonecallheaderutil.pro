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
# Description: Project file for building unit test component
#
#

TEMPLATE = app
TARGET.UID3 = 0xE37786C5
DEPENDPATH += . \
    ../../src
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += . \
    ../../inc \
    ../../../phoneuiutils/inc \
    ../../../phoneui2/srcdata \
    ../../../../tsrc/common \
    ../../../../inc \
    ../../../../phoneengine/inc
    
CONFIG += qtestlib engineinfo

LIBS += -lengineinfo -lphoneuiutils \
        -lmocklib -lsymbianmock 

SOURCES += unit_tests.cpp \
           ../../src/phonecallheaderutil.cpp \
           ../../../internal/mocks/mock_stringloader.cpp \
           ../../../internal/mocks/phoneuiutils/mock_cphonelogger.cpp \
           ../../../internal/mocks/phoneuiutils/mock_cphonemainresourceresolver.cpp \
           ../../../internal/mocks/phoneuiutils/mock_cphonecenrepproxy.cpp \
           ut_phonecallheaderutilstubs.cpp \
           
           
include(../../../../tsrc/common/phonetest.pri)
