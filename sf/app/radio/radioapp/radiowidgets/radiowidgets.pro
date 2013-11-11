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

TMP_DIR_NAME = widgets
include(../buildflags.pri)

TEMPLATE    = lib
TARGET      = fmradiowidgets
CONFIG      += dll hb
DEFINES     += BUILD_WIDGETS_DLL

symbian:TARGET.UID3 = 0x2002FF4C

INCLUDEPATH += inc
INCLUDEPATH += ../radiouiengine/inc
INCLUDEPATH += ../../common

LIBS        += -lfmradiouiengine
symbian:LIBS += -lafservice

LOGGING_ENABLED:LIBS += -lfmradioenginewrapper

DEPENDPATH  += $$INCLUDEPATH src

# Input
HEADERS     += radiowidgetsexport.h
HEADERS     += radiowindow.h
HEADERS     += radioutil.h
HEADERS     += radioviewbase.h
HEADERS     += radiouiloader.h
HEADERS     += radiomainview.h
HEADERS     += radiostripbase.h
HEADERS     += radiofrequencystrip.h
HEADERS     += radiofrequencystripbase.h
HEADERS     += radiofrequencystripdef.h
HEADERS     += radiofrequencyitem.h
HEADERS     += radiostationsview.h
HEADERS     += radiofrequencyscanner.h
HEADERS     += radiofadinglabel.h
HEADERS     += radiostationcarousel.h
HEADERS     += radiocarouselitem.h
HEADERS     += radiocarouselitemobserver.h
HEADERS     += radiocarouselanimator.h
HEADERS     += radiohistoryview.h

SOURCES     += radiowindow.cpp
SOURCES     += radioutil.cpp
SOURCES     += radioviewbase.cpp
SOURCES     += radiouiloader.cpp
SOURCES     += radiomainview.cpp
SOURCES     += radiostripbase.cpp
SOURCES     += radiofrequencystrip.cpp
SOURCES     += radiofrequencystripbase.cpp
SOURCES     += radiofrequencyitem.cpp
SOURCES     += radiostationsview.cpp
SOURCES     += radiofrequencyscanner.cpp
SOURCES     += radiofadinglabel.cpp
SOURCES     += radiostationcarousel.cpp
SOURCES     += radiocarouselitem.cpp
SOURCES     += radiocarouselanimator.cpp
SOURCES     += radiohistoryview.cpp

DOCML       += res/layout/stationsview.docml res/layout/mainview.docml res/layout/historyview.docml
RESOURCES   += res/fmradioui.qrc
