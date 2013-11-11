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
TARGET=serviceapp

CONFIG += service

symbian:TARGET.UID3 = 0xE0022E73

XQSERVICE_ROOT=../../../qthighway
include(../../../qthighway/xqservicebase.pri)
include(src/serviceapp.pri)

LIBS+=-lxqservice -lxqserviceutil

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable

libFiles.sources = serviceapp.exe 
libFiles.path = "!:\sys\bin"
DEPLOYMENT += libFiles

symbian:MMP_RULES += SMPSAFE
