/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  stub hbview
*
*/


#ifndef HBVIEW_H
#define HBVIEW_H

#include <QGraphicsWidget>
#include "qstring.h"
#include "hbinstance.h"
#include "hbtoolbar.h"
#include "hbeffect.h"

class HbView : public QGraphicsWidget
{
    Q_OBJECT

public:

    HbView(QGraphicsItem *parent = 0) : QGraphicsWidget(parent), mToolBar(0), mNavigationAction(0)
    {
        Q_UNUSED(parent);
    }
    
    virtual ~HbView(){}
    
    HbMainWindow* mainWindow() const
    {
        return hbInstance->allMainWindows().value(0);
    }
    
    void setTitle(const QString &title)
    {
        mTitle = title;
    }
    
    HbToolBar *toolBar() const
    {
        if(!mToolBar) {
            mToolBar = new HbToolBar(qobject_cast<QGraphicsItem*>(this));
        }
        return mToolBar;
    }
    
    void setItemVisible(Hb::SceneItem item, bool visible)
    {
        Q_UNUSED(item);
        mSetItemVisibleLast = visible;
    }
    
    void setNavigationAction(HbAction *action)
    {
        mNavigationAction = action;
    }
    
    
    HbAction *mNavigationAction;
    
    mutable HbToolBar* mToolBar;
    
    QString mTitle;
    
public:
    
    static bool mSetItemVisibleLast;
};

#endif // HBVIEW_H
