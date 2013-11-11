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


symbian: {
    SYMBIAN_PLATFORMS = WINSCW ARMV5

    TEMPLATE = app

    include(../common.pri)

    DEPENDPATH += .

    INCLUDEPATH += ./inc ../hsdomainmodel/inc

    HEADERS +=  ./inc/*.h

    SOURCES +=  ./src/*.cpp

    ### this exe is not visible for the user so get rid of localization warnings
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    ###

    TARGET.UID3 = 0x20022F40
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles TrustedUI

    hidden = "hidden = KAppIsHidden;"
    launch = "launch = KAppLaunchInForeground;"
    datatypelist = "datatype_list = { DATATYPE { priority=EDataTypePriorityHigh; type=\"application/hs-widget-uninstall+xml\"; } };"
    RSS_RULES += hidden launch datatypelist
    LIBS += -lxqservice
    LIBS += -lxqserviceutil
    LIBS += -lapparc
    LIBS += -lhsdomainmodel
    
    MMP_RULES += SMPSAFE
}
