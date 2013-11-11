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
 * Description: Menu view.
 *
 */

#ifndef HS_MAIN_WINDOW_H
#define HS_MAIN_WINDOW_H

#include <QObject>
#include <QPixmap>
#include "hsmenustates_global.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HsMenuModeWrapper;

class HsMainWindow
{
public:
    HsMainWindow(HsMenuModeWrapper* wrapper = 0);
    virtual ~HsMainWindow();

    virtual void setCurrentView(HbView *view);
private:
    HsMenuModeWrapper* mMenuMode;
};

#endif // HS_MAIN_WINDOW_H
