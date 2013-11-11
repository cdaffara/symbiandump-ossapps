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
TARGET = cxuitest
TARGET.UID3 = 0xE013A865
DEPENDPATH += .
VPATH += ../src \
         ../inc
INCLUDEPATH += . \
               ../../cxengine/inc/api \
               ../../cxengine/inc \
               ../../cxengine/src/dummyengine \
               ../inc \
               ./cxui \
               ../traces
QT += testlib
CONFIG += qtestlib hb
LIBS += -lcxengine \
        -lcommonui \
        -lapmime \
        -lws32 \
        -lbitgdi \
        -lfbscli
TARGET.CAPABILITY = ALL -TCB -DRM

# Input
SOURCES += cxuitest.cpp \
    cxuiviewmanagertest.cpp \
    cxuifakeengine.cpp \
    cxuiautofocusreticule.cpp \
    cxuiprecaptureview.cpp \
    cxuivideoprecaptureview.cpp \
    cxuistillprecaptureview.cpp \
    cxuipostcaptureview.cpp \
    cxuicapturekeyhandler.cpp \
    cxuiviewmanager.cpp \
    cxuidocumentloader.cpp \
    cxuisettingbutton.cpp \
    cxuisettingslider.cpp \
    cxuisettingradiobuttonlist.cpp \
    cxuisettingscenemoderadiobuttonlist.cpp \
    cxuisettingbuttoncontainer.cpp \
    cxuifakestillcapturecontrol.cpp \
    cxuifakecameradevicecontrol.cpp \
    cxuifakeautofocuscontrol.cpp \
    cxuifakeviewfindercontrol.cpp
HEADERS += cxuitest.h \
    cxuiviewmanagertest.h \
    cxuifakeengine.h \
    cxuiautofocusreticule.h \
    cxuiprecaptureview.h \
    cxuistillprecaptureview.h \
    cxuivideoprecaptureview.h \
    cxuipostcaptureview.h \
    cxuicapturekeyhandler.h \
    cxuiviewmanager.h \
    cxuisettingbutton.h \
    cxuisettingslider.h \
    cxuisettingradiobuttonlist.h \
    cxuisettingscenemoderadiobuttonlist.h \
    cxuisettingbuttoncontainer.h\
   	cxuidocumentloader.h \
    cxuienums.h \
    cxuifakestillcapturecontrol.h \
    cxuifakecameradevicecontrol.h \
    cxuifakeautofocuscontrol.h \
    cxuifakeviewfindercontrol.h

RESOURCES += ../cxui.qrc

