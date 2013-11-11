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
SUBDIRS += unit/unittest_cpthemecontrol
SUBDIRS += unit/unittest_cpthemelistview
SUBDIRS += unit/unittest_cpthemeplugin
SUBDIRS += unit/unittest_cpthemepluginentryitemdata
SUBDIRS += unit/unittest_cpthemelistmodel
SUBDIRS += unit/unittest_cpthemeutil

CONFIG += ordered
test.depends = first
test.CONFIG += recursive
autotest.depends = first
autotest.CONFIG += recursive
QMAKE_EXTRA_TARGETS += test autotest
