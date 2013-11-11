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
SUBDIRS = unit/unit_backuprestoresettings \
          unit/ut_fminternaldrivermodel \
          unit/ut_fminternalfmutils \
          unit/ut_fmfiletypeprovider \
          unit/ut_fmfiletyperecognizer \
          unit/ut_fmoprationserviceapi

CONFIG += ordered
#CONFIG += symbian_test
test.depends = first
test.CONFIG += recursive
autotest.depends = first
autotest.CONFIG += recursive
QMAKE_EXTRA_TARGETS += test autotest
