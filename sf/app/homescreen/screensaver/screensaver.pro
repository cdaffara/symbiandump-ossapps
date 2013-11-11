#
# Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: screensaver.pro
#

TEMPLATE = subdirs

SUBDIRS = screensavermodel \
          snsrutils \
          snsrplugins \
          devicedialogplugins

CONFIG += ordered

symbian: {
    include(screensaver_exports_to_rom.pri)
    SUBDIRS += snsrdisplaycontrol_s60/group
}
