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
#
# Release
#

r01.path = $$PWD/../../bin/release
r01.files = ./release/*.*

INSTALLS += r01

#
# Debug
#

d01.path = $$PWD/../../bin/debug
d01.files = ./debug/*.*

INSTALLS += d01
