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

SERVICEAPP=app
TARGET=serviceapp2

CONFIG += hb
CONFIG += service

#XQSERVICE_ROOT=../../../qthighway
#include(../../../qthighway/xqservicebase.pri)
include(src/serviceapp.pri)

LIBS+=-lxqservice -lxqserviceutil

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xE0011E73
    TARGET.CAPABILITY = CAP_APPLICATION
}

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable

libFiles.sources = serviceapp2.exe 
libFiles.path = "!:\sys\bin"
DEPLOYMENT += libFiles



symbian:MMP_RULES += SMPSAFE
