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


testfiles.files = ./release/t_hsmenuserviceprovider





statemachinedllrel.path = ./release/


statemachinedllrel.files = ../../../../../hsstatemachine/release/*.so*





statemachinedlldebug.path = ./debug/


statemachinedlldebug.files = ../../../../../hsstatemachine/debug/*.so





INSTALLS += testfiles statemachinedllrel statemachinedlldebug


