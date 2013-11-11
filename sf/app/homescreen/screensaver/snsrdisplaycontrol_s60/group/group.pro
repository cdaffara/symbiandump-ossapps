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
# Description: Project file of Symbian client-server pair to contorl display power save mode.
#              These are pure Symbian components and should be excluded from build on any
#              other platform.
#

TEMPLATE = subdirs

BLD_INF_RULES.prj_exports += \
     "../inc/snsrdisplaycontrolclient.h           |../../inc/snsrdisplaycontrolclient.h" \
     "../rom/snsrdisplaycontrol_s60.iby           CORE_APP_LAYER_IBY_EXPORT_PATH(snsrdisplaycontrol_s60.iby)"

BLD_INF_RULES.prj_mmpfiles += "snsrdisplaycontrolclient.mmp"
BLD_INF_RULES.prj_mmpfiles += "snsrdisplaycontrolserver.mmp"
