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

TEMPLATE = subdirs

SYMBIAN_PLATFORMS = WINSCW ARMV5
SUBDIRS += msgnotificationdialogplugin/msgnotificationdialogplugin.pro
SUBDIRS += msgindicatorplugin/msgindicatorplugin.pro
SUBDIRS += msgerrorwatcher/msgerrorwatcher.pro
SUBDIRS += msgnotifier/msgnotifier.pro
SUBDIRS += msgerrornotifier/msgerrornotifier.pro
SUBDIRS += flashmsgnotifier/flashmsgnotifier.pro

