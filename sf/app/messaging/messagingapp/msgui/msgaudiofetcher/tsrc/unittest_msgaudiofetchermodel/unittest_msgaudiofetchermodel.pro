#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

TARGET = unittest_msgaudiofetchermodel
TARGET.UID3 = 0x20024325
BLD_INF_RULES.prj_testexports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "./resources/nondrm.amr	c:/data/testAudioFetcher/nondrm.amr" \ 
    "./resources/drm.amr		c:/data/testAudioFetcher/drm.amr" \
    "./resources/DRM_DM.dm	c:/data/testAudioFetcher/DRM_DM.dm" \
    "./resources/DRM_AMR.dcf	c:/data/testAudioFetcher/DRM_AMR.dcf" \
    "./resources/Esp.rng	c:/data/testAudioFetcher/Esp.rng" \
    "./resources/Desk.rng	c:/data/testAudioFetcher/Desk.rng"
    
include(../tsrc.pri)

RESOURCES += unittest_msgaudiofetchermodel.qrc

# Input
HEADERS += unittest_msgaudiofetchermodel.h
SOURCES += unittest_msgaudiofetchermodel.cpp