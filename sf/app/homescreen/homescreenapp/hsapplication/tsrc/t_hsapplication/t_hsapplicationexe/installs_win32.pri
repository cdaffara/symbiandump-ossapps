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

testexe.path = ../../../../../../bin/hstests/
testexe.files = ./debug/t_hsapplicationexe.exe

mockdll.path = ../../../../../../bin/hstests/hsresources/plugins/stateplugins
mockdll.files += ./debug/hsresources/plugins/stateplugins/*.dll \
                            ./debug/hsresources/plugins/stateplugins/*.xml
                            
compos.path = ../../../../../../bin/hstests/
compos.files = ../../../../../../bin/debug/hsdomainmodel.dll \
               ../../../../../../bin/debug/hsutils.dll \
               ../../../../../../bin/debug/caclient.dll

compos2.path = ./debug
compos2.files = ../../../../../../bin/debug/hsdomainmodel.dll \                
                ../../../../../../bin/debug/hsutils.dll \
                ../../../../../../bin/debug/caclient.dll                            

INSTALLS += testexe mockdll compos compos2
