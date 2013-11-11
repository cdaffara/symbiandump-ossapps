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
TARGET.UID3 = 0xEFF8FEA9
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += hb
CONFIG += qtestlib

symbian {

    INCLUDEPATH += \epoc32\include\platform\mw \
                   \epoc32\include\platform \
                   \epoc32\include\platform\app \
	                 \epoc32\include\mw \
                   \sf\app\phone\inc \
                   ../../../phoneuiutils/inc/ \
                   ../../../phonemediatorcenter/inc/ \
                   ../../../phoneuiview2/inc \
                   ../../../../inc \
                   ../../../phoneui2/srcdata \
                   ../../../../tsrc/common


    LIBS += -lphoneuiutils
    LIBS += -lphoneuiqtview
    LIBS += -lserviceprovidersettings

    TARGET.CAPABILITY = ALL -TCB
    DEFINES += FT_SYMBIAN_INTEGRATION
    DEFINES += BUILD_PHONEUIQTVIEWADAPTER
}

# Input
HEADERS += ../../inc/phoneuicommandcontroller.h
SOURCES += ../../src/phoneuicommandcontroller.cpp
HEADERS += ../../inc/phoneresourceadapter.h
SOURCES += ../../src/phoneresourceadapter.cpp
HEADERS += ../../inc/phoneuiqtbuttonscontroller.h
HEADERS += ../../inc/phonecommandextensionwrapper.h
SOURCES += phoneuiqtbuttonscontroller_stub.cpp
SOURCES += phonecommandextensionwrapper_stub.cpp
SOURCES += unit_tests.cpp
SOURCES += hb_stub.cpp


include(../../../../tsrc/common/phonetest.pri)
symbian:MMP_RULES += SMPSAFE
