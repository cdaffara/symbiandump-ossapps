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
TARGET = unittest_nmcontacthistorymodel_p

DEPENDPATH += . \
              $$MW_LAYER_SYSTEMINCLUDE \
              x:/epoc32/include/platform/mw
              
INCLUDEPATH += . \
               ../../../../../email_plat/contact_history_model_api/inc \
               ../../../../../inc \
               /epoc32/include/mw/QtTest \
               $$MW_LAYER_SYSTEMINCLUDE \
               /epoc32/include/platform/mw

QT += core

CONFIG += qtestlib 
CONFIG += symbian_test

# Input
HEADERS += qtcontacts.h 
HEADERS += ../../../inc/nmcontacthistorymodel_p.h
HEADERS += unittest_nmcontacthistorymodel_p.h         

SOURCES += ../../../src/nmcontacthistorymodelitem.cpp 
SOURCES += ../../../src/nmcontacthistorymodel_p.cpp 
SOURCES += emailmru_stub.cpp
SOURCES += unittest_nmcontacthistorymodel_p.cpp 


symbian*: { 
    TARGET.CAPABILITY = CAP_APPLICATION
    EPOCALLOWDLLDATA = 1
}
