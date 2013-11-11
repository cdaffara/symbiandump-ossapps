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
# Description: Project file for building Phone components
#
#

include(..\common.pri)

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ..\..\inc
INCLUDEPATH += ..\mocks
INCLUDEPATH += \epoc32\include\platform\mw \
                   \epoc32\include\platform \
                   \epoc32\include \
                   \epoc32\include\platform\app \
                   \epoc32\include\app
               
# Input
HEADERS += ..\..\inc\cphcntcontactidimpl2.h
HEADERS += ..\..\inc\cphcntfactoryimpl2.h
HEADERS += ..\mocks\qcontactdetail.h
HEADERS += ..\mocks\qcontact.h


SOURCES += ..\..\src\cphcntcontactidimpl2.cpp
SOURCES += u_cphcntcontactidimpl2.cpp
SOURCES += ..\mocks\qcontactdetail_stub.cpp
SOURCES += ..\mocks\qcontact_stub.cpp




TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0xe39991fe






symbian:MMP_RULES += SMPSAFE
