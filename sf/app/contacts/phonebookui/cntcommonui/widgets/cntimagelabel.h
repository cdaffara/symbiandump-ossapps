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

#ifndef CNTIMAGELABEL_H
#define CNTIMAGELABEL_H

#include <QObject>
#include <hbwidget.h>

class QGesture;
class QPixmap;
class HbIconItem;
class HbIcon;

class CntImageLabel : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(HbIcon icon READ avatarIcon WRITE setAvatarIcon)

public:
    CntImageLabel(QGraphicsItem *parent = 0);
    ~CntImageLabel();

    void setAvatarIcon(const HbIcon &icon);
    HbIcon avatarIcon() const;
    
    void setIcon(const QPixmap &pixmap);
    void clear();

protected:
    void gestureEvent(QGestureEvent* event);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    
signals:
    void iconClicked();
    void iconLongPressed(const QPointF&);
    
private:
    HbIconItem*  mDefaultAvatar;
    HbIcon       mIcon;
    QPixmap      mPixmap;
    
};

#endif // CNTEDITVIEWHEADINGITEM_H
