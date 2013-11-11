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
 * Description: Mocked classes for testing CalenDayUtils class
 *
 */

#ifndef  HBMAINWINDOW_H
#define  HBMAINWINDOW_H

#include <HbView>

// Test variables
extern QRectF gTestWindowRect;
extern Qt::Orientation gTestOrientation;

/*!
 Mocked class HbMainWindow (simulating window in unittests)
 */
class HbMainWindow : public QObject
{
public:
    HbMainWindow()
    {
        mView = new HbView();
    }

    ~HbMainWindow()
    {
        if (mView) {
            delete mView;
            mView = 0;
        }
    }

    QRectF layoutRect() const
    {
        return gTestWindowRect;
    }

    Qt::Orientation orientation() const
    {
        return gTestOrientation;
    }

    HbView *HbMainWindow::currentView() const
    {
        return mView;
    }

    HbView *mView;
    
signals:
    void orientationChanged(Qt::Orientation orientation);
};

#endif // HBMAINWINDOW_H
