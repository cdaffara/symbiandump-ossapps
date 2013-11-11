/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Utils for application library state plugin.
 *
 */

#ifndef HSAPPLIBSTATEUTILS_H
#define HSAPPLIBSTATEUTILS_H

#include <QObject>

#include "hsmenustates_global.h"

class CaEntry;

class HsAppLibStateUtils
{
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
    
public:
    
    static bool isCWRTWidgetOnHomeScreen(const CaEntry *entry);
    
};

#endif //HSAPPLIBSTATEUTILS_H

