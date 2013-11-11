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

TEMPLATE = lib

CONFIG += hb mobility
MOBILITY = serviceframework

include (../common.pri)

QT += sql 
HB += hbfeedback

LIBS += -lcaclient \
        -lhsutils 
        

DEFINES += HSDOMAINMODEL_LIB

DEPENDPATH += . \
              ./inc \ 
              ./src 
              

INCLUDEPATH += . \
               ./inc \
               ../hsutils/inc \
               ../hsapplication/inc




symbian: {
    DOCML+=resource/controllayer.docml
    RESOURCES += hsdomainmodel.qrc
    TARGET.UID3 = 0x20022F38
    LIBS += -lxqsettingsmanager -labclient
    
    MMP_RULES += SMPSAFE
}
!symbian:{
    RESOURCES += hsdomainmodel_win.qrc
    QT += network 
}

include(hsdomainmodel.pri)
