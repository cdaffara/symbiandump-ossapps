# ============================================================================
# Name : unittest_nmcontacthistorymodel
# Part of : nmutilities
# Description :
# Copyright (c) 2009 Nokia. All rights reserved.
# This material, including documentation and any related computer
# programs, is protected by copyright controlled by Nokia. All
# rights are reserved. Copying, including reproducing, storing,
# adapting or translating, any or all of this material requires the
# prior written consent of Nokia. This material also contains
# confidential information which may not be disclosed to others
# without the prior written consent of Nokia.
# ============================================================================
# ENTER HERE FILES NEEDED TO COMPILE WITH UNIT TESTS

DEFINES -= NMCHMAPI_LIBRARY
DEFINES += NMCHMAPI_LIBRARY_NO_LIBRARY

QT += testlib
QT += core

CONFIG += qtestlib
CONFIG += symbian_test

TEMPLATE = app
TARGET = unittest_nmcontacthistorymodel

MOC_DIR = tmp

INCLUDEPATH =   . \
                ../../../inc/ \
                $$MW_LAYER_SYSTEMINCLUDE

win32:DESTDIR = ../../bin

HEADERS +=  unittest_nmcontacthistorymodel.h \
            nmcontacthistorymodel_p.h \
            ../../../../../inc \
            ../../../../../email_plat/contact_history_model_api/inc/nmcontacthistorymodel.h

SOURCES +=  nmcnthistorystub.cpp
SOURCES +=  ../../../src/nmcontacthistorymodel.cpp
SOURCES +=  unittest_nmcontacthistorymodel.cpp

symbian*: { 
    TARGET.CAPABILITY = CAP_APPLICATION
    EPOCALLOWDLLDATA = 1
    MMP_RULES += "USERINCLUDE ."
}