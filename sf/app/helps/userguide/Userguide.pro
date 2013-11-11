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

TEMPLATE    = app
QT	    += webkit xmlpatterns
CONFIG      += hb 
TRANSLATIONS = userguide.ts common.ts
#add "precompile_header" to CONFIG

TARGET = Userguide
DEPENDPATH  += .
INCLUDEPATH += ./inc

# PRECOMPILED_HEADER = pch.h

# Input
HEADERS += 	inc/HelpMainWindow.h \
			inc/BrowserWrapper.h \
			inc/HelpContentsView.h \
			inc/HelpCategoryView.h \
			inc/HelpDocumentLoader.h \
			inc/HelpCommon.h \
			inc/HelpDataProvider.h \
			inc/HelpUtils.h \
			inc/HelpProxyModel.h \
			inc/HelpStandardItem.h \
			inc/HelpKeywordView.h \
			inc/HelpBaseView.h

SOURCES += 	src/main.cpp \
			src/HelpMainWindow.cpp \
			src/BrowserWrapper.cpp \
			src/HelpContentsView.cpp \
			src/HelpCategoryView.cpp \
			src/HelpDocumentLoader.cpp \
			src/HelpDataProvider.cpp \
			src/HelpUtils.cpp \
			src/HelpProxyModel.cpp \
			src/HelpStandardItem.cpp \
			src/HelpKeywordView.cpp \
			src/HelpBaseView.cpp

symbian {

		TARGET.UID3 = 0x10005234
  	TARGET.CAPABILITY = CAP_APPLICATION
  	TARGET.EPOCHEAPSIZE = 0x020000 0x1000000
  	
  	MMP_RULES += SMPSAFE
    CONFIG += mobility
    MOBILITY = systeminfo
    INCLUDEPATH += /sf/mw/webruntime/wrt/runtimecore
		INCLUDEPATH += /sf/mw/webruntime/wrt
		DEFINES += CWRT_BUILDING_TENONE
  	LIBS += -lRuntimeCore -lezip -lxqservice -lxqserviceutil -lcone -leikcore -lmediaclientaudio -leikcoctl -leiksrv -lapparc -lavkon -lefsrv -lcharconv -lws32 -lhal -lgdi -lapgrfx
  	include(rom/userguide.pri)
  	SKINICON = qtg_large_help
}

RESOURCES += resources/Userguide.qrc

debug: DESTDIR = ./debug
release: DESTDIR = ./release
