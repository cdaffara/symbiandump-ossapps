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
* Description: Mocked classes for testing CalenDayContainer class
*
*/


#ifndef  HBINSTANCE_H
#define  HBINSTANCE_H

#include "hbmainwindow.h"
#include <QList>

/*!
 Mocked class HbInstance (simulating hbinstance in unittests)
 */
class HbInstance
{
public:
    
    static HbInstance *instance() {
        return new HbInstance();
    }
    
    QList<HbMainWindow *> allMainWindows() const {
        return mMainWindows;
    }
    
    HbInstance() {
        HbMainWindow* window = new HbMainWindow();
        mMainWindows.append(window);
    }
    
    ~HbInstance() {
        for (int i = 0; i < mMainWindows.count(); i++) {
            delete mMainWindows[i];
        }
        mMainWindows.clear();
    }
    
    QList<HbMainWindow *> mMainWindows;
};

#endif // HBINSTANCE_H

// End of File
