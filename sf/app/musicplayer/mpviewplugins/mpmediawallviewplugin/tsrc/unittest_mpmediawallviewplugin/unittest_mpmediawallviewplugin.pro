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
CONFIG += qtestlib
CONFIG += symbian_test
TARGET = unittest_mpmediawallviewplugin
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
    ../../inc \
    ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxviewframeworkqt.dll

HEADERS += inc/unittest_mpmediawallviewplugin.h \
           stub/inc/xqplugin.h \
           stub/inc/mpmediawallview.h \
           ../../inc/mpmediawallviewplugin.h

SOURCES += src/unittest_mpmediawallviewplugin.cpp \
           stub/src/mpmediawallview.cpp

