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
# Description: mpplaybackdocumentloader unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpplaybackdocumentloader
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

HEADERS += stub/inc/mpalbumcoverwidget.h \
           inc/unittest_mpplaybackdocumentloader.h \
           ../../inc/mpplaybackdocumentloader.h

SOURCES += stub/src/mpalbumcoverwidget.cpp \
           src/unittest_mpplaybackdocumentloader.cpp \
           ../../src/mpplaybackdocumentloader.cpp

