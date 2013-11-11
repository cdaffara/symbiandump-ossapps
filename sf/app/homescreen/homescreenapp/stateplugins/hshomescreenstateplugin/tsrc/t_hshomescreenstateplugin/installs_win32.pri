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

testfiles.path = ../../../../../../bin/hstests/
testfiles.files = ./debug/t_hshomescreenstateplugin.exe

testresources.path = ../../../../../../bin/hstests/hsresources/
testresources.files = ../../resource/controllayer.docml

docmltestresources_local.path = ./hsresources/
docmltestresources_local.files = ../../resource/controllayer.docml

dtestresources_local.path = ./debug/hsresources/
dtestresources_local.files = ../../../../hsdomainmodel/resource/controllayer.docml

image.path = ./debug
image.files = ./foundicon.png

INSTALLS += testfiles testresources docmltestresources_local dtestresources_local image
