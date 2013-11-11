#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: tests for Friend widget plugin
#

TEMPLATE = app

CONFIG += debug_and_release

QT += testlib

CONFIG(debug, debug|release) {
    DESTDIR = ./debug
} else {
    DESTDIR = ./release
}

DEFINES += CONTACTWIDGET_TEST

symbian: LIBS += -lxqservice -lthumbnailmanagerqt
CONFIG += hb mobility
MOBILITY = serviceframework contacts 

RESOURCES += ./t_contactwidgethsplugin.qrc


INCLUDEPATH += ./inc \
               ../inc \
               ../contactwidgeths/inc

DEPENDPATH += ./inc \
              ./src \
              ../inc \
              ../src \
              ../contactwidgeths/inc


symbian {
    TARGET.UID3 = 0x2002C354
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
}

include(t_contactwidgethsplugin.pri)

