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

#include "cntimagelabel.h"

#include <QPainter>
#include <QGraphicsDropShadowEffect>

#include <hbinstantfeedback.h>
#include <hbtapgesture.h>
#include <hbeffect.h>
#include <hbdeviceprofile.h>
#include <hbiconitem.h>
#include <hbicon.h>
#include <hbstyle.h>

static const qreal ImageRounding = 1.5; // units 

/*
 * Imagel label for landscape views.
 */
CntImageLabel::CntImageLabel(QGraphicsItem *parent) :
    HbWidget(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    
    mDefaultAvatar = new HbIconItem(this);
    style()->setItemName(mDefaultAvatar, "icon");
    mDefaultAvatar->setAlignment(Qt::AlignCenter);
   
    grabGesture(Qt::TapGesture);
    HbEffect::add(this, "groupbox_icon_click", "iconclick");
    
    //shadow effect
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setColor(QColor(0, 0, 0, 150));
    effect->setBlurRadius(5);
    effect->setOffset(3);
    setGraphicsEffect(effect);
}

CntImageLabel::~CntImageLabel()
{

}

void CntImageLabel::setIcon(const QPixmap &pixmap)
{
    mPixmap = pixmap;
    
    if (!mPixmap.isNull()) 
    {
       mDefaultAvatar->setVisible(false);
       mDefaultAvatar->setIcon(HbIcon());
    } 
    else 
    {
       mDefaultAvatar->setIconName(QLatin1String("qtg_large_add_contact_picture"));
       mDefaultAvatar->setVisible(true);
    }
    
    update();
}

void CntImageLabel::setAvatarIcon(const HbIcon &icon)
{
    mIcon = icon;
    
    if (mIcon.isNull()) 
    {
        mDefaultAvatar->setVisible(false);
        mDefaultAvatar->setIcon(HbIcon());
    } 
    else 
    {
        mDefaultAvatar->setIcon(icon);
        mDefaultAvatar->setVisible(true);
    }
}

HbIcon CntImageLabel::avatarIcon() const
{
    return mIcon;
}

void CntImageLabel::clear()
{
    mIcon.clear();
    mPixmap = NULL;
    mDefaultAvatar->setIconName(QLatin1String("qtg_large_add_contact_picture"));
    mDefaultAvatar->setVisible(true);
}

void CntImageLabel::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    if (mPixmap.isNull())
    {
        return;
    }
    else
    {
        painter->save();
        QPainterPath clip;
        qreal rounding = ImageRounding * HbDeviceProfile::profile(this).unitValue();
        clip.addRoundRect(rect(), rounding, rounding);
        painter->setClipPath(clip);
        painter->drawPixmap(rect().toRect(), mPixmap);
        painter->restore();
    } 
}

void CntImageLabel::gestureEvent(QGestureEvent* event)
{ 
    if (HbTapGesture *tap = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) 
    {    
        switch (tap->state()) 
        {
            case Qt::GestureStarted:
                HbEffect::start(this, QString("iconclick"));
                break;
            case Qt::GestureFinished:
                if (tap->tapStyleHint() == HbTapGesture::Tap)
                {
                    HbInstantFeedback::play(HbFeedback::Basic);
                    emit iconClicked();
                }
                break;
            case Qt::GestureUpdated:
                if (tap->tapStyleHint() == HbTapGesture::TapAndHold) 
                {
                    HbEffect::cancel(this, QString("iconclick"));
                    emit iconLongPressed(tap->scenePosition());
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
}

