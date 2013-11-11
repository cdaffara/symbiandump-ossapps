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
# Description: Project file for customisation module.
#

symbian : {
	BLD_INF_RULES.prj_exports += \
		"./inc/calenview.h                  |../inc/calenview.h" \
		"./inc/calencommandhandler.h        |../inc/calencommandhandler.h" \
		"./inc/calenservices.h              |../inc/calenservices.h" \
		"./inc/calencustomisation.h	      |../inc/calencustomisation.h"
}

# End of file	--Don't remove this.