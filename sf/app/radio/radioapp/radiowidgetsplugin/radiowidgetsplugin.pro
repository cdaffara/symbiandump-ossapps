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

TMP_DIR_NAME = widgetsplugin
include(../buildflags.pri)

TEMPLATE    = lib
TARGET      = fmradiowidgetsplugin
symbian:TARGET.UID3 = 0x2002FF4D

CONFIG      += hb plugin
QT          += core

INCLUDEPATH += ../radiowidgets/inc
INCLUDEPATH += ../../common

symbian: {
    pluginstub.sources      = fmradiowidgetsplugin.dll
    pluginstub.path         = /resource/plugins
    DEPLOYMENT              += pluginstub
}

SOURCES     += main.cpp

LIBS        += -lfmradiowidgets
