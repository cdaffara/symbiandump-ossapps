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
# Description: mpplaybackwidget unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpplaybackwidget
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxviewframeworkqt.dll

HEADERS += inc/unittest_mpplaybackwidget.h \
		   stub/inc/mpplaybackdata.h \
		   stub/inc/mpalbumcoverwidget.h \
		   stub/inc/mpplaybackdocumentloader.h \
		   stub/inc/mplightmonitor.h \
           ../../inc/mpplaybackwidget.h
           

SOURCES += src/unittest_mpplaybackwidget.cpp \
  		   stub/src/mpplaybackdata.cpp \
  		   stub/src/mpalbumcoverwidget.cpp \
  		   stub/src/mpplaybackdocumentloader.cpp \
		   stub/src/mplightmonitor.cpp \
		   ../../src/mpplaybackwidget.cpp

RESOURCES += ../../resources/mpplaybackviewpluginresources.qrc