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

#ifndef CNTEDITVIEWHEADINGITEM_H
#define CNTEDITVIEWHEADINGITEM_H

#include <QObject>
#include <hbwidget.h>
#include <qmobilityglobal.h>

class HbIconItem;
class HbTextItem;
class HbFrameItem;
class HbTouchArea;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntEditViewHeadingItem : public HbWidget
{
    friend class TestCntEditViewHeadingItem;
    
    Q_OBJECT
    Q_PROPERTY( QString text READ getText )
    Q_PROPERTY( QString second_text READ getSecond_text )
    Q_PROPERTY( HbIcon icon READ getIcon WRITE setIcon )

public:
    CntEditViewHeadingItem(QGraphicsItem *parent = 0);
    ~CntEditViewHeadingItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

signals:
    void iconClicked();
    void textClicked();

public:
    void createPrimitives();
    void recreatePrimitives();
    void updatePrimitives();
    void setDetails(const QContact* contact, bool myCard);

    QString getText() const { return text; }
    QString getSecond_text() const { return second_text; }
    HbIcon getIcon() const { return icon; }
    
    void setIcon(const HbIcon newIcon);

private slots:
    void orientationChanged(Qt::Orientation);

private:
    HbIconItem              *mIcon;
    HbTextItem              *mLabel;
    HbTextItem              *mSecondLabel;
    HbFrameItem             *mFrameItem;
    HbFrameItem             *mFocusItem;
    HbTouchArea             *mIconTouchArea;

    bool                     mIconFocused;
    bool                     mTextFocused;

    QString                 text;
    QString                 second_text;
    HbIcon                  icon;
};

#endif // CNTEDITVIEWHEADINGITEM_H
