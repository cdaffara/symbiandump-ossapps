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

#ifndef LOGSPAGEINDICATORITEM_H
#define LOGSPAGEINDICATORITEM_H

#include <hbiconitem.h>


class LogsPageIndicatorItem : public HbIconItem
{
    Q_OBJECT
    friend class UT_LogsPageIndicatorItem;
    
public:
    
    explicit LogsPageIndicatorItem(bool active = false, QGraphicsItem *parent = 0);
    ~LogsPageIndicatorItem();
    
public:

    void setActive(bool active = true);
    bool isActive() const;
   
protected:
    
    void changeEvent(QEvent * event);

private:

    void updateColor();
    void startAnimation();

private slots:
    
    void animationFinished();
    
private:
    
    bool mIsActive;    
};

#endif // LOGSPAGEINDICATORITEM_H
