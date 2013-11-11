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

#ifndef LOGSPAGEINDICATOR_H
#define LOGSPAGEINDICATOR_H

#include <hbwidget.h>


class LogsPageIndicatorItem;


class LogsPageIndicator: public HbWidget
{
    friend class UT_LogsPageIndicator;
    Q_OBJECT
    
public:
    
    explicit LogsPageIndicator(QGraphicsItem *parent = 0);
    ~LogsPageIndicator();

public:
    
    void initialize(int itemCount, int activeItemIndex);
    
    int itemCount() const;

    void setActiveItemIndex(int index);
    int activeItemIndex() const;
    

private:
    
    void layoutItems();    

private:
    
    QList<LogsPageIndicatorItem *> mItems;    
    int mActiveItemIndex;
};

#endif //LOGSPAGEINDICATOR
