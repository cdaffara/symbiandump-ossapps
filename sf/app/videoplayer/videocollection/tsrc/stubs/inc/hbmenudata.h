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
* Description:  HbMessageBox class declaration.
*
*/


#ifndef HBMENUDATA_H
#define HBMENUDATA_H

#include <hbaction.h>

class HbMenuData
{
public: // methods
    static void reset()
    {
        mMenuAction = 0;
        mEnabledSetted = false;
        mReturnEmptyActionList = false;
        mExecPoint = QPointF();
    }
    
public: // data
    static HbAction *mMenuAction;
    static bool mEnabledSetted;
    static bool mReturnEmptyActionList;
    static QPointF mExecPoint;
};

#endif // HBMENUDATA_H
