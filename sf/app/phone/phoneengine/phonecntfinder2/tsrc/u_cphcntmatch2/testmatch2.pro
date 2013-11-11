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
INCLUDEPATH += .
INCLUDEPATH += ..\..\..\..\inc
INCLUDEPATH += \epoc32\include\platform\mw \
               \epoc32\include\platform \
               \epoc32\include \
               \epoc32\include\platform\app \
               \epoc32\include\app 
               
# Input
#HEADERS += ..\mocks\qcontact.h
#HEADERS += ..\mocks\qcontactdetail.h
HEADERS += ..\..\inc\cphcntmatch2.h
HEADERS += ..\..\inc\cphcntcontactidimpl2.h

#SOURCES += ..\mocks\qcontact_stub.cpp
#SOURCES += ..\mocks\qcontactdetail_stub.cpp
SOURCES += ..\..\src\cphcntmatch2.cpp
SOURCES += ..\..\src\cphcntcontactidimpl2.cpp
SOURCES += u_cphcntmatch2.cpp


TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0xe84eae66







symbian:MMP_RULES += SMPSAFE
