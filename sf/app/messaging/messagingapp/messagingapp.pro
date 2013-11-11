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

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += ordered

SUBDIRS += conf/messaging_conf.pro
SUBDIRS += msgutils/msgutils.pro
SUBDIRS += msgappfw/msgappfw.pro
SUBDIRS += msgsettings/msgsettings.pro
SUBDIRS += msgnotifications/msgnotifications.pro
SUBDIRS += smartmessaging/smartmessaging.pro
SUBDIRS += shareui/shareui.pro
SUBDIRS += msgui/msgui.pro
SUBDIRS += msgservices/msgservices.pro
