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

dlocaldatafiles1.files =  ./hsdomainmodeltest.db
dlocaldatafiles1.path = ./debug

dlocaldatafiles2.files =  ./resource/*.png
dlocaldatafiles2.path = ./debug/wallpapers/scene/

dlocaldatafiles3.files =  ./resource/*.png
dlocaldatafiles3.path = ./debug/wallpapers/page/

globaldatafiles1.files =  ./hsdomainmodeltest.db
globaldatafiles1.path = ./../../../../../bin/hstests

globaldatafiles2.files =  ./resource/*.png
globaldatafiles2.path = ./../../../../../bin/hstests/wallpapers/scene/

globaldatafiles3.files =  ./resource/*.png
globaldatafiles3.path = ./../../../../../bin/hstests/wallpapers/page/

globaltest.files = ./../../../../../bin/debug/caclient.dll \
                 ./../../../../../bin/debug/hsutils.dll \
                  ./debug/t_hsdomainmodel.exe
globaltest.path = ./../../../../../bin/hstests

ddepend1.files = ./../../../../../bin/debug/caclient.dll \
                 ./../../../../../bin/debug/hsutils.dll
ddepend1.path = ./debug

testfiles.path = ./../../../../../bin/hstests
testfiles.files = ./debug/t_hsdomainmodel.exe

testresources.path = ./../../../../../bin/hstests/hsresources/
testresources.files = ../../resource/controllayer.docml

docmltestresources_local.path = ./hsresources/
docmltestresources_local.files = ../../resource/controllayer.docml

dtestresources_local.path = ./debug/hsresources/
dtestresources_local.files = ../../resource/controllayer.docml

INSTALLS += dlocaldatafiles1 dlocaldatafiles2 dlocaldatafiles3 globaldatafiles1 globaldatafiles2 globaldatafiles3 globaltest ddepend1 testfiles testresources docmltestresources_local dtestresources_local
