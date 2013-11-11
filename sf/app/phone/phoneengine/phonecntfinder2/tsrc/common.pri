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
######################################################################
# Unit tests
######################################################################

CONFIG += hb
CONFIG += qtestlib
DEFINES += dll \
            BUILD_QTCONTACTS \

            
            

LIBS += -lphoneengine \
        -lFeatMgr \
        -lrfs \
        -lsecui \
        -lprofileeng \
        -leuser \
        -lbafl \
        -lqtcore \
        -lqtgui \
        -lcharconv \
        -lqtcontacts
        
        
TARGET.EPOCALLOWDLLDATA =1
        

        
        
        
