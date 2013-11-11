#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
#  programs, is protected by copyright controlled by Nokia.  All
#  rights are reserved.  Copying, including reproducing, storing,
#  adapting or translating, any or all of this material requires the
#  prior written consent of Nokia.  This material also contains
#  confidential information which may not be disclosed to others
#  without the prior written consent of Nokia.
# ============================================================================
#
# Description:
#

TEMPLATE = lib
TARGET = clockwidget
CONFIG += hb
DEFINES += CLOCKWIDGET_LIB

win32{
CONFIG(release, debug|release){
  TARGET = clockwidget
  DESTDIR = ../release # for easy plugin loading
}else{
  TARGET = clockwidgetd
  DESTDIR = ../debug # for easy plugin loading
  }
}

DEPENDPATH += \
			./inc \
            ./src

INCLUDEPATH += \
			./inc \
			./traces

SOURCES += \
		clockwidget.cpp \
		analogclockwidget.cpp \
		digitalclockwidget.cpp

HEADERS += \
        clockwidgetdefines.h \ 
		clockwidget.h \
		analogclockwidget.h \
		digitalclockwidget.h
            
symbian {
    TARGET.UID3 = 0x2002E715
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    LIBS += -lbafl
}

RESOURCES = clockwidgetimpl.qrc

# End of file	--Don't remove this
