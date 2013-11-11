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

#include <QGraphicsLayout>
#include "cntdebug.h"

#include "cntcontactcarddetailitem.h"
#include "cntcontactcarddataitem.h"
#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbrichtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbeffect.h>
#include <hbstyleloader.h>
#include <QGraphicsSceneMouseEvent>
#include <hbinstantfeedback.h>
#include <hbcolorscheme.h>
#include <QGestureEvent>
#include <hbtapgesture.h>

const int CNT_CONTACTCARD_Z_FRAME = -1;
const int CNT_CONTACTCARD_Z_FOCUS = -2;

CntContactCardDetailItem::CntContactCardDetailItem(int index, QGraphicsItem *parent, bool isFocusable) :
    HbWidget(parent),
    mIcon(NULL),
    mSecondaryIcon(NULL),
    mFirstLineText(NULL),
    mSecondLineText(NULL),
    mFrameItem(NULL),
    mFocusItem(NULL),
    mHasFocus(false),
    mIndex(index),
    mIsFocusable(isFocusable),
    mValueTextElideMode(Qt::ElideNone),
    mIsUnderline(false)
{  
    if (mIsFocusable)
    {
        HbEffect::add("frameitem", "listviewitem_press", "pressed");
        HbEffect::add("frameitem", "listviewitem_release", "released");
        initGesture();
    }
    setProperty("state", "normal");
}

CntContactCardDetailItem::~CntContactCardDetailItem()
{

}

void CntContactCardDetailItem::createPrimitives()
{
    if (!icon.isNull())
    {
        if (!mIcon)
        {
            mIcon = new HbIconItem(this);
            style()->setItemName(mIcon, "icon");
        }
        mIcon->setIcon(icon);
    }
    else
    {
        if (mIcon)
        {
            delete mIcon;
        }
        mIcon = 0;
    }

    if (!secondaryIcon.isNull())
    {
        if (!mSecondaryIcon)
        {
            mSecondaryIcon = new HbIconItem(this);
            mSecondaryIcon->setFlags(HbIcon::Colorized);
            style()->setItemName(mSecondaryIcon, "secondaryIcon");
        }
        mSecondaryIcon->setIcon(secondaryIcon);
        mSecondaryIcon->setColor(HbColorScheme::color("qtc_view_normal"));
    }
    else
    {
        if (mSecondaryIcon)
        {
            delete mSecondaryIcon;
        }
        mSecondaryIcon = 0;
    }
    
    if (!mFirstLineText)
    {
        mFirstLineText = new HbTextItem(this);
        mFirstLineText->setTextWrapping(Hb::TextNoWrap);
        style()->setItemName(mFirstLineText, "text");    
    }
    mFirstLineText->setText(text);        

    
    if (!mSecondLineText)
    {
        mSecondLineText = new HbTextItem(this);
        mSecondLineText->setElideMode(mValueTextElideMode);
        style()->setItemName(mSecondLineText, "valueText");
    }
    
    mSecondLineText->setText(valueText);


    if (!mFrameItem)
    {
        mFrameItem = new HbFrameItem(this);
        mFrameItem->frameDrawer().setFrameGraphicsName("qtg_fr_list_normal");
        mFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFrameItem->setZValue(CNT_CONTACTCARD_Z_FOCUS);
        style()->setItemName(mFrameItem, "background");
    }

    
    if (!mFocusItem && mIsFocusable)
    {
        mFocusItem = new HbFrameItem(this);
        mFocusItem->frameDrawer().setFrameGraphicsName("qtg_fr_list_pressed");
        mFocusItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFocusItem->setZValue(CNT_CONTACTCARD_Z_FRAME);
        mFocusItem->setVisible(false);
        style()->setItemName(mFocusItem, "highlight");
    }
   
    HbWidget::updatePrimitives();
    updateGeometry();
    repolish();
}

void CntContactCardDetailItem::recreatePrimitives()
{
    HbWidget::recreatePrimitives();
   
    createPrimitives();
}

void CntContactCardDetailItem::initGesture()
{
    grabGesture(Qt::TapGesture);
}

void CntContactCardDetailItem::gestureEvent(QGestureEvent* event)
{
    CNT_ENTRY
    
    if (HbTapGesture *tap = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) 
    {
        switch(tap->state()) 
        {
            case Qt::GestureStarted:  //
                if (mIsFocusable)
                {
                    HbInstantFeedback::play(HbFeedback::Basic);
                }
                mHasFocus = true;
                updatePrimitives();
                break;
            case Qt::GestureCanceled: // Reset state
                mHasFocus = false;
                updatePrimitives();
                break;
            case Qt::GestureUpdated:
                if(tap->tapStyleHint() == HbTapGesture::TapAndHold) 
                {
                    emit onLongPress(event->mapToGraphicsScene(tap->position()));
                }
                break;
            case Qt::GestureFinished: // emit clicked
                mHasFocus = false;
                updatePrimitives();
                if (tap->tapStyleHint() == HbTapGesture::Tap) 
                {
                    emit clicked();                  
                }
                break;
            default:
                break;
        }  
        event->accept();
    } 
    else 
    {
        event->ignore();
    }
    
    CNT_EXIT
}

void CntContactCardDetailItem::onLongPress(const QPointF &point)
{
    mHasFocus = false;
    updatePrimitives();
    emit longPressed(point);
}

void CntContactCardDetailItem::updatePrimitives()
{
    if (mHasFocus && mIsFocusable)
    {
        mFocusItem->setVisible(true);
        setProperty("state", "pressed");
        HbEffect::start(mFocusItem, QString("frameitem"), QString("pressed"));      
    }
    else
    {
        HbEffect::start(mFocusItem, QString("frameitem"), QString("released"));
        mFocusItem->setVisible(false);
        setProperty("state", "normal");
    }
    HbWidget::updatePrimitives();
}

void CntContactCardDetailItem::setDetails(CntContactCardDataItem* aDataItem)
{
    mValueTextElideMode = aDataItem->elideMode();

    if (aDataItem->icon() != icon)
    {
        icon.clear();
        icon = aDataItem->icon();
    }
    if (aDataItem->secondaryIcon() != secondaryIcon)
    {
        secondaryIcon.clear();
        secondaryIcon = aDataItem->secondaryIcon();
    }
   
    text.clear();
    text = aDataItem->titleText();

    valueText.clear();
    valueText = aDataItem->valueText();

    recreatePrimitives();
}

//To update the secondary icon item
void CntContactCardDetailItem::setSecondaryIconItem(HbIcon aIcon)
{
    secondaryIcon.clear();
    secondaryIcon = aIcon;
    if (!mSecondaryIcon)
    {
        mSecondaryIcon = new HbIconItem(this);
        mSecondaryIcon->setFlags(HbIcon::Colorized);
        style()->setItemName(mSecondaryIcon, "secondaryIcon");
    }
    mSecondaryIcon->setIcon(secondaryIcon);
    mSecondaryIcon->setColor(HbColorScheme::color("qtc_view_normal"));
}

int CntContactCardDetailItem::index()
{
    return mIndex;
}
