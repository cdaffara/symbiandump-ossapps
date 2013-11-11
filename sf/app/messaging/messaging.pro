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

SUBDIRS += msg_plat/msg_plat.pro
SUBDIRS += msg_pub/msg_pub.pro
SUBDIRS += messagingappbase/messagingappbase.pro
SUBDIRS += mmsengine/mmsengine.pro
SUBDIRS += messagingapp/messagingapp.pro
