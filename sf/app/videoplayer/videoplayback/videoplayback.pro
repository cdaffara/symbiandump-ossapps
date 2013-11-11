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
# Description: Project file for building Videoplayer components
#
#
# Version : %version: da1mmcf#5 %


TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS  += videoplaybackview \
            videoplaybackviewplugin
BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"videohelix/group/bld.inf\""