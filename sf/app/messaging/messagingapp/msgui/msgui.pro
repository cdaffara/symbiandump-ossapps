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

# DLLs
SUBDIRS += msgaudiofetcher/msgaudiofetcher.pro
SUBDIRS += msguiutils/msguiutils.pro
SUBDIRS += appengine/appengine.pro
SUBDIRS += conversationview/conversationview.pro
SUBDIRS += unifiededitor/unifiededitor.pro
SUBDIRS += unifiedviewer/unifiedviewer.pro
# EXEs
SUBDIRS += msgapp/msgapp.pro

