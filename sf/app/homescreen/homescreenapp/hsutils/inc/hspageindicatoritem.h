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
* Description:
*
*/

#ifndef HSPAGEINDICATORITEM_H
#define HSPAGEINDICATORITEM_H

#include <HbWidget>

class HbIconItem;

class HsPageIndicatorItem : public HbWidget
{
    Q_OBJECT

public:
    HsPageIndicatorItem(bool active = false, QGraphicsItem *parent = 0);
    ~HsPageIndicatorItem();

public:
    void setActive(bool active = true);
    bool isActive() const;

    bool isAnimationRunning() const;

private:
    Q_DISABLE_COPY(HsPageIndicatorItem)
    void startAnimation();

private slots:
    void animationFinished();

private:
    HbIconItem *mIconItem;    
    bool mIsActive;    
};

#endif // HSPAGEINDICATORITEM_H
