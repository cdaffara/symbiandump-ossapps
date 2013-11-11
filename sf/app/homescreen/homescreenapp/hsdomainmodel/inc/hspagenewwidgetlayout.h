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
* Description:
*
*/

#ifndef HSPAGENEWWIDGETLAYOUT_H
#define HSPAGENEWWIDGETLAYOUT_H

#include <QGraphicsLayout>
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)
class HsWidgetHost;
class HsPageNewWidgetLayout : public QGraphicsLayout
{
public: 
    HsPageNewWidgetLayout(const QPointF &touchPoint = QPointF(),
                          QGraphicsLayoutItem *parent = 0);
    ~HsPageNewWidgetLayout();
        
    int count() const;
    QGraphicsLayoutItem *itemAt(int i) const;
    void removeAt(int index);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void setGeometry(const QRectF &rect);
    void addItem(HsWidgetHost *item);

private:
    QSizeF mSize;
    QList<HsWidgetHost*> mNewWidgets;
    QPointF mTouchPoint;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif
