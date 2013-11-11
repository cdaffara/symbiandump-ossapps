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
#

include(..\common.pri)

TEMPLATE = app
TARGET = 
DEPENDPATH += .
DEPENDPATH += ..\mocks
INCLUDEPATH += .
INCLUDEPATH += ..\mocks
INCLUDEPATH += ..\..\inc
INCLUDEPATH += \epoc32\include\platform\mw \
                   \epoc32\include\platform \
                   \epoc32\include\platform\app \
                   \epoc32\include\app

               
# Input
HEADERS += ..\mocks\qcontactmanager.h
HEADERS += ..\mocks\qcontactmanager_moc.h
HEADERS += ..\mocks\qcontactdetail.h
HEADERS += ..\mocks\qcontact.h
HEADERS += ..\..\inc\cphcntmatch2.h
HEADERS += ..\..\inc\cphcntcontactidimpl2.h
HEADERS += ..\..\inc\cphcntmatcherimpl2.h

SOURCES += ..\..\src\cphcntmatch2.cpp
SOURCES += ..\..\src\cphcntcontactidimpl2.cpp
SOURCES += ..\..\src\cphcntmatcherimpl2.cpp
SOURCES += ..\mocks\qcontactmanager_moc.cpp
SOURCES += ..\mocks\qcontactdetail_stub.cpp
SOURCES += ..\mocks\qcontact_stub.cpp
SOURCES += u_cphcntmatcherimpl2.cpp



TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0xe84eae56







symbian:MMP_RULES += SMPSAFE
