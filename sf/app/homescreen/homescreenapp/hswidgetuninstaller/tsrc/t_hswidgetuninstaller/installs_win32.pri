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

testfiles.path = ../../../../../bin/hstests/
testfiles.files = ./debug/t_hswidgetuninstaller.exe \
                  *.manifest

ddepend1.files = ../../../../../bin/debug/caclient.dll \
                 ../../../../../bin/debug/hsdomainmodel.dll \
                 ../../../../../bin/debug/hsutils.dll
ddepend1.path = ./debug

rdepend1.files = ../../../../../bin/release/caclient.dll \
                 ../../../../../bin/release/hsdomainmodel.dll \
                 ../../../../../bin/release/hsutils.dll
rdepend1.path = ./release

INSTALLS += testfiles ddepend1 rdepend1
