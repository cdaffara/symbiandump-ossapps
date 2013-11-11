# ============================================================================
# Name : unitest_emailmailboxinfo.cpp
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

DEFINES -= XQSETTINGSMANAGER_LIBRARY
DEFINES += XQSETTINGSMANAGER_NO_LIBRARY

QT += testlib
QT += core

CONFIG += qtestlib
CONFIG += symbian_test

TEMPLATE = app
TARGET = unittest_emailmru

MOC_DIR = tmp

INCLUDEPATH =   . \
                $$MW_LAYER_SYSTEMINCLUDE
  
win32:DESTDIR = ../../bin

HEADERS +=  emailtrace.h \
            /epoc32/include/platform/mw/xqcentralrepositorysearchcriteria.h \
            /epoc32/include/platform/mw/xqcentralrepositoryutils.h \
            /epoc32/include/platform/mw/xqsettingsmanager.h \
            unittest_emailmru.h \
            ../../../inc/emailmru.h

SOURCES +=  xqstubs.cpp 
SOURCES +=  ../../../src/emailmru.cpp
SOURCES +=  unittest_emailmru.cpp

symbian*: { 
    TARGET.CAPABILITY = CAP_APPLICATION
    EPOCALLOWDLLDATA = 1
    MMP_RULES += "USERINCLUDE ."
}