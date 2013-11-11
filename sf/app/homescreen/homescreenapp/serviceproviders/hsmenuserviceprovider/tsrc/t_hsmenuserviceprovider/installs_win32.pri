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

testfiles.path = ../../../../../../bin/debug/

testfiles.files = ./release/t_hsmenuserviceprovider.exe \

testfiles.files = ./debug/t_hsmenuserviceprovider.exe \


statemachinedllrel.path = ./release/

statemachinedllrel.files = ../../../../../core/hsstatemachine/release/*.dll



statemachinedlldebug.path = ./debug/

statemachinedlldebug.files = ../../../../../core/hsstatemachine/debug/*.dll

INSTALLS += testfiles #statemachinedllrel statemachinedlldebug
