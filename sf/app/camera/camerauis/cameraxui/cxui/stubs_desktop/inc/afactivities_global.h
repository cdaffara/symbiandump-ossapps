/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 * Stub version of epoc32\include\platform\mw\afactivities_global.h to be used in desktop build.
 *
 */

#ifndef AFACTIVITIES_GLOBAL_H
#define AFACTIVITIES_GLOBAL_H

#include <qglobal.h>

namespace Af {
    enum ActivationReason {
        ActivationReasonActivity = 0,
        ActivationReasonService,
        ActivationReasonNormal
    };

    const char KActivityScheme[] = "appto";

    const char KActivityUriNameKey[] = "activityname";
    const char KActivityUriBackgroundKey[] = "activityinbackground";
}

#endif // AFACTIVITIES_GLOBAL_H
