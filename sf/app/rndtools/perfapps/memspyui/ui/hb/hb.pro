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
# Description: 
#
#

TEMPLATE = app
TARGET = MemSpyUI
DEPENDPATH += . 
INCLUDEPATH += /epoc32/include/platform/memspy/api
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

load(hb.prf)
symbian:CONFIG -= symbian_i18n

HEADERS += inc/enginewrapper.h \
	inc/viewmanager.h \
	inc/memspyview.h \
	inc/memspylistview.h \
	inc/memspymainview.h \
	inc/memspyprocessview.h \
	inc/memspythreadview.h \
	inc/memspythreaddetailindexview.h \
	inc/memspythreaddetailview.h \
	inc/memspykernelobjecttypeview.h \
	inc/memspykernelobjectview.h \
	inc/memspykernelobjectdetailview.h \
	inc/memspytrackingview.h \
	inc/memspyswmtview.h \
	inc/memspyheapdumpsview.h \
	inc/memspysettingsview.h \
	inc/memspyserverview.h \
	inc/memspyecomcategoryview.h \
	inc/memspyecominterfaceview.h \
	inc/memspyecomimplementationview.h \
	inc/memspyecomimplementationdetailview.h \
	inc/memspywindowgroupsview.h \
	inc/memspywindowgroupsdetailview.h

SOURCES += src/main.cpp \
	src/enginewrapper.cpp \
	src/viewmanager.cpp \
	src/memspyview.cpp \
	src/memspylistview.cpp \
	src/memspymainview.cpp \
	src/memspyprocessview.cpp \
	src/memspythreadview.cpp \
	src/memspythreaddetailindexview.cpp \
	src/memspythreaddetailview.cpp \
	src/memspykernelobjecttypeview.cpp \
	src/memspykernelobjectview.cpp \
	src/memspykernelobjectdetailview.cpp \
	src/memspytrackingview.cpp \
	src/memspyswmtview.cpp \
	src/memspyheapdumpsview.cpp \
	src/memspysettingsview.cpp \
	src/memspyserverview.cpp \
	src/memspyecomcategoryview.cpp \
	src/memspyecominterfaceview.cpp \
	src/memspyecomimplementationview.cpp \
	src/memspyecomimplementationdetailview.cpp \
	src/memspywindowgroupsview.cpp \
	src/memspywindowgroupsdetailview.cpp

RESOURCES +=

symbian: {
    LIBS += -lestor \
        -lbafl \
        -lMemSpyClient \
        -lefsrv

    TARGET.CAPABILITY = ReadUserData WriteUserData ReadDeviceData WriteDeviceData SwEvent PowerMgmt

    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x2002129F
    TARGET.SID = 0x2002129F
    TARGET.VID = 0x101FB657 // Nokia

    TARGET.EPOCHEAPSIZE = 0x10000 0x2000000  // Min 64Kb, Max 32Mb

    ICON = ../../icons/qgn_menu_memspyui.svg

    RSS_RULES += "group_name = \"RnD Tools\"";
} else {
    error("Only Symbian supported")
}
