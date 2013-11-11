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
TARGET = cxuitestbat
TARGET.UID3 = 0xE0115615
TARGET.EPOCHEAPSIZE = 0x020000 \
    0x1000000
DEPENDPATH += .

VPATH += ../src \
         ../inc

INCLUDEPATH += . \
               ../../cxengine/inc/api \
               ../../cxengine/inc \
               ../inc \
               ./cxui \
               ../traces

QT += testlib
CONFIG += qtestlib hb
HB += hbcore hbwidgets hbtools hbfeedback

LIBS += -lcxengine \
        -lcommonui \
        -lapmime \
        -lws32 \
        -lbitgdi \
        -lfbscli \
        -lxqsettingsmanager \
        -lxqutils

TARGET.CAPABILITY = ALL -TCB -DRM

# Input
SOURCES += cxuitestbat.cpp \
    cxuiautofocusreticule.cpp \
    cxuiprecaptureview.cpp \
    cxuivideoprecaptureview.cpp \
    cxuistillprecaptureview.cpp \
    cxuipostcaptureview.cpp \
    cxuicapturekeyhandler.cpp \
    cxuiviewmanager.cpp \
    cxuidocumentloader.cpp \
    cxuidisplaypropertyhandler.cpp \
    cxuisettingslider.cpp \
    cxuisettingradiobuttonlist.cpp \
    cxuisettingscenemoderadiobuttonlist.cpp \
    cxuiselftimer.cpp \
	cxuierrormanager.cpp \
    cxuistandby.cpp

HEADERS += cxuitestbat.h \
    cxuiautofocusreticule.h \
    cxuiprecaptureview.h \
    cxuistillprecaptureview.h \
    cxuivideoprecaptureview.h \
    cxuipostcaptureview.h \
    cxuicapturekeyhandler.h \
    cxuiviewmanager.h \
    cxuienums.h \
    cxuidocumentloader.h \
    cxuidisplaypropertyhandler.h \
    cxuisettingslider.h \
    cxuisettingradiobuttonlist.h \
    cxuisettingscenemoderadiobuttonlist.h \
    cxuiselftimer.h \
    cxuierrormanager.h \
    cxuistandby.h \
    ../traces/OstTraceDefinitions.h

RESOURCES += ../cxui.qrc

