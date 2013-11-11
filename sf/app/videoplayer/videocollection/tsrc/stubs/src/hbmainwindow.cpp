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
* Description: stub hblabel
* 
*/

#include "hbmainwindow.h"
#include "hbview.h"
#include "qlist.h"

// ---------------------------------------------------------------------------
// addView
// ---------------------------------------------------------------------------
//
HbView* HbMainWindow::addView(HbView *view)
{
    mViews.append(view);
    return view;
}

// ---------------------------------------------------------------------------
// removeView
// ---------------------------------------------------------------------------
//
void HbMainWindow::removeView(HbView *view)
{
    mViews.removeAll(view);
}

// ---------------------------------------------------------------------------
// views
// ---------------------------------------------------------------------------
//
QList<HbView *> HbMainWindow::views() const
{
    return mViews;
}

HbView* HbMainWindow::currentView() const
{
    if(mViews.count())
    {
        return mViews.at(0);
    }
    return 0;
}

void HbMainWindow::setCurrentView(HbView *view)
{
    for(int i = 0; i < mViews.count(); i++)
    {
        if(mViews[i] == view)
        {
            mViews.takeAt(i);
        }
    }
    mViews.prepend(view);
}
