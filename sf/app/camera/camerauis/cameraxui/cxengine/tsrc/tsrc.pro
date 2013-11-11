#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

include(../../camerax.pri)

TEMPLATE = app
TARGET = cxenginetest
DEPENDPATH += .
INCLUDEPATH += . ../inc/api
QT += testlib
LIBS += -lcxengine -lcommonui
CONFIG += hb
HB = hbcore hbwidgets

TARGET.CAPABILITY = ALL -TCB -DRM

# Input
SOURCES += cxeenginetest.cpp
HEADERS += cxeenginetest.h

