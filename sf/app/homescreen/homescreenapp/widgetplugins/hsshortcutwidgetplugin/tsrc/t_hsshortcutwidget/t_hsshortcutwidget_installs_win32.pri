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
testfiles.files = ./debug/t_hsshortcutwidget.exe

compos.path = ../../../../../../bin/hstests/
compos.files =  ../../../../../../bin/debug/caclient.dll \
		            ../../../../../../bin/debug/hsutils.dll \
                ../../../../../../homescreensrv/contentstorage/castorage/data/castorage.db \
                
compos3.path = ./debug
compos3.files =  ../../../../../../bin/debug/caclient.dll \
								 ../../../../../../bin/debug/hsutils.dll \
                 ../../../../../../homescreensrv/contentstorage/castorage/data/castorage.db \


compos4.path = ./
compos4.files =  ../../../../../../homescreensrv/contentstorage/castorage/data/castorage.db \
                 
INSTALLS += testfiles compos compos3 compos4

