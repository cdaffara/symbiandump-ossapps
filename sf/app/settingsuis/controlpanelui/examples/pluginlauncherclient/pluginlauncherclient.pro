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
# Description: cpframework source files
#

TEMPLATE = app
TARGET = CpPluginLauncherClient
DEPENDPATH += .
INCLUDEPATH += .

include (pluginlauncherclient.pri)

CONFIG += hb

MOC_DIR = moc
OBJECT_DIR = obj
RCC_DIR = rcc

symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_PLATFORM_EXPORT_PATH(cplogger)
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}

LIBS += -lxqservice -lcpframework
symbian::TARGET::UID3 = 0X2002873A


symbian:MMP_RULES += SMPSAFE
