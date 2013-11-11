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

CONFIG += qtestlib hb
TEMPLATE = app
TARGET = 
DEPENDPATH  += . ../../infowidget/src 
INCLUDEPATH += .
INCLUDEPATH += ../common
INCLUDEPATH += ../../inc/
INCLUDEPATH += ../../infowidget/inc/
# the following is needed for a couple of hb mocks
INCLUDEPATH += /sf/mw/hb/include/hbcore/private
DEFINES += QT_NO_DEBUG_STREAM


QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
  INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
  LIBS += -lmocklib 		\
          -lsymbianmock 	\
          -lhswidgetmodel
}

# test code
HEADERS += ./inc/ut_infowidget.h
SOURCES += ./src/ut_infowidget.cpp

# code to be tested
HEADERS += ../../infowidget/inc/infowidget.h
SOURCES += ../../infowidget/src/infowidget.cpp

# mocks needed for testing
HEADERS += ../../infowidget/inc/infowidgetpreferences.h
SOURCES += ../mocks/mock_infowidgetpreferences.cpp
HEADERS += ../../infowidget/inc/infowidgetengine.h
SOURCES += ../mocks/mock_infowidgetengine.cpp
HEADERS += ../../infowidget/inc/infowidgetlayoutmanager.h
SOURCES += ../mocks/mock_infowidgetlayoutmanager.cpp
HEADERS += ../../infowidget/inc/infowidgetnetworkhandler.h
SOURCES += ../mocks/mock_infowidgetnetworkhandler.cpp
HEADERS += ../../infowidget/inc/infowidgetsathandler.h
SOURCES += ../mocks/mock_infowidgetsathandler.cpp

SOURCES += ../mocks/mock_hbcolorscheme.cpp
SOURCES += ../mocks/mock_hbmarqueeitem.cpp
SOURCES += ../mocks/mock_hbwidget.cpp
SOURCES += ../mocks/mock_qgraphicslinearlayout.cpp
SOURCES += ../mocks/mock_qgraphicslayout.cpp
SOURCES += ../mocks/mock_hbframedrawer.cpp
SOURCES += ../mocks/mock_hbframeitem.cpp
SOURCES += ../mocks/mock_rsatservice.cpp
SOURCES += ../mocks/mock_hbwidgetbase.cpp
SOURCES += ../mocks/mock_hbtapgesture.cpp
SOURCES += ../mocks/mock_qevent.cpp
SOURCES += ../mocks/mock_qgesture.cpp
SOURCES += ../mocks/mock_hbcheckbox.cpp
SOURCES += ../mocks/mock_hbabstractbutton.cpp
SOURCES += ../mocks/mock_qaction.cpp
SOURCES += ../mocks/mock_hbaction.cpp
SOURCES += ../mocks/mock_hbmessagebox.cpp

symbian:MMP_RULES += SMPSAFE
