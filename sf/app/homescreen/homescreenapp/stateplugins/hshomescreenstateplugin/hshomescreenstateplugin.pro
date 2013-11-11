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

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \
                ../../hsutils/inc \
                ../../hsdomainmodel/inc \
                ../../hsapplication/inc \
                ../../serviceproviders/hsmenuserviceprovider/inc

QT += xml sql
HB += hbfeedback

CONFIG += plugin hb mobility
MOBILITY = serviceframework systeminfo

PLUGIN_SUBDIR = /resource/qt/plugins/homescreen

include(../../common.pri)

LIBS += -lhsutils \
        -lhsdomainmodel \
        -lhsmenuserviceprovider


symbian: {
    TARGET.UID3 = 0x20022F41
    LIBS += -lxqsettingsmanager
    MMP_RULES += SMPSAFE
}

include(hshomescreenstateplugin.pri)
