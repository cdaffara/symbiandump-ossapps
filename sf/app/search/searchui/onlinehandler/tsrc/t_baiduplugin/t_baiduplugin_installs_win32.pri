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
r01.path = $$PWD/../../../../bin/release/resource/qt/plugins/search           
r01.files = ./release/*.dll
            
r02.path = $$PWD/../../../../bin/release/searchresources/plugins/onlineproviders            
r02.files =  ./resource/*.manifest

INSTALLS += r01 r02

#
# Debug
#

d01.path = $$PWD/../../../../bin/debug/resource/qt/plugins/search           
d01.files = ./debug/*.dll

d02.path = $$PWD/../../../../bin/debug/searchresources/plugins/onlineproviders            
d02.files =  ./resource/*.manifest

INSTALLS += d01 d02