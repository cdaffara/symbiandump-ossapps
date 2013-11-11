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
TARGET.UID3 = 0xE37686c5
DEPENDPATH += . \
    ../../src
INCLUDEPATH += . \
    ../../inc \
    ../../../../tsrc/common
CONFIG += hb
CONFIG += qtestlib
symbian:
else:win32:DESTDIR = ./

# CONFIG(debug,debug|release)
# {
# QMAKE_CXX = ctc -i d g++
# LIBS += $$(CTCHOME)\lib\ctcmsnt.lib
# }
SOURCES += unit_tests.cpp \
    ../../src/phoneuiqtbuttonscontroller.cpp
HEADERS += ../../inc/phoneuiqtbuttonscontroller.h

include(../../../../tsrc/common/phonetest.pri)
