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

#ifndef CNTCOMMLAUNCHERDETAILITEM_H
#define CNTCOMMLAUNCHERDETAILITEM_H

#include <QObject>
#include <hbwidget.h>

class HbIconItem;
class HbTextItem;
class QGraphicsGridLayout;
class HbFrameItem;
class CntContactCardDataItem;

class CntContactCardDetailItem : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY( QString text READ getText )
    Q_PROPERTY( QString valueText READ getValueText )
    Q_PROPERTY( HbIcon icon READ getIcon )
    Q_PROPERTY( HbIcon secondaryIcon READ getSecondaryIcon )

public:
    CntContactCardDetailItem(int index, QGraphicsItem *parent = 0, bool isFocusable = true);
    ~CntContactCardDetailItem();

protected:
    void gestureEvent(QGestureEvent* event);
    
public:
    void initGesture();
    void createPrimitives();
    void recreatePrimitives();
    void updatePrimitives();
    void setDetails(CntContactCardDataItem* aDataItem);
    //To update the secondary icon item. Used for
    //maptile progress icon updation.
    void setSecondaryIconItem( HbIcon aIcon );
    int index();

public slots:
    void onLongPress(const QPointF &point);

signals:
    void clicked();
    void longPressed(const QPointF &point);
    
private:
    QString getText() const { return text; }
    QString getValueText() const { return valueText; }
    HbIcon getIcon() const { return icon; }
    HbIcon getSecondaryIcon() const { return secondaryIcon; }

private:
    HbIconItem              *mIcon;
    HbIconItem              *mSecondaryIcon;
    HbTextItem              *mFirstLineText;
    HbTextItem              *mSecondLineText;
    HbFrameItem             *mFrameItem;
    HbFrameItem             *mFocusItem;
    bool                    mHasFocus;
    int                     mIndex;
    bool                    mIsFocusable;
    Qt::TextElideMode       mValueTextElideMode;
    bool                    mIsUnderline;

    QString                 text;
    QString                 valueText;
    HbIcon                  icon;
    HbIcon                  secondaryIcon;
    
friend class TestCntContactCardDetailItem;
};

#endif // CNTCOMMLAUNCHERDETAILITEM_H
