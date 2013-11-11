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

HEADERS += ./inc/hshomescreen.h \
           ./inc/hsstatemachine.h
SOURCES += ./src/hshomescreen.cpp \
           ./src/hsstatemachine.cpp \        
           ./src/main.cpp
symbian:{
    HEADERS += ./inc/hshomescreenclientserviceprovider.h \
               ./inc/hsrecoverymanager.h
    SOURCES += ./src/hshomescreenclientserviceprovider.cpp \
               ./src/hsrecoverymanager.cpp
}
