#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Project definition file for calendar regional plugins
#

BLD_INF_RULES.prj_exports += \
"./calenregionalutil/inc/calenextrarowformatter.h			|./inc/calenextrarowformatter.h" \
"./calenregionalutil/inc/calenlunarinfo.h					|./inc/calenlunarinfo.h" \
"./calenregionalutil/inc/calenlunarinfoprovider.h			|./inc/calenlunarinfoprovider.h" \
"./calenregionalutil/inc/calenlunarlocalizedinfo.h			|./inc/calenlunarlocalizedinfo.h" \
"./calenregionalutil/inc/calenlunarlocalizer.h				|./inc/calenlunarlocalizer.h" \
"./calenregionalutil/inc/calenlunarpanic.h					|./inc/calenlunarpanic.h" \
"./calenregionalutil/inc/CalenLunarPaths.h					|./inc/CalenLunarPaths.h" \
"./calenregionalutil/inc/CalenRegionalPluginUids.h			|./inc/CalenRegionalPluginUids.h" \
"./calenregionalutil/inc/CalenSolarTerms.h					|./inc/CalenSolarTerms.h" \
"./lunarchinese/data/SolarItems								/epoc32/data/z/private/10005901/solaritems" \
"./lunarchinese/data/SolarItems								/epoc32/release/winscw/udeb/z/private/10005901/solaritems" \
"./lunarchinese/data/SolarItems								/epoc32/release/winscw/urel/z/private/10005901/solaritems"

TEMPLATE = subdirs
SUBDIRS += calenregionalutil \
					 lunarchinese
CONFIG += ordered
