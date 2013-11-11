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

#include "cnteditviewheadingitem.h"

#include <qtcontacts.h>

#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbtoucharea.h>
#include <hbinstantfeedback.h>
#include <hbmainwindow.h>
#include <hbeffect.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <cntuids.h>

#include <QGraphicsSceneMouseEvent>

CntEditViewHeadingItem::CntEditViewHeadingItem(QGraphicsItem *parent) :
    HbWidget(parent),
    mIcon(NULL),
    mLabel(NULL),
    mSecondLabel(NULL),
    mFrameItem(NULL),
    mFocusItem(NULL),
    mIconTouchArea(NULL),
    mIconFocused(false),
    mTextFocused(false)
{
    HbEffect::add("icon", "groupbox_icon_click", "iconclick");
    setProperty("state", "normal");
}

CntEditViewHeadingItem::~CntEditViewHeadingItem()
{

}

void CntEditViewHeadingItem::createPrimitives()
{
    if (!icon.isNull())
    {
        if (!mIcon && mainWindow()->orientation() != Qt::Horizontal)
        {
            mIcon = new HbIconItem(this);
            mIcon->setAlignment(Qt::AlignCenter);
            mIcon->setIcon(icon);
            style()->setItemName(mIcon, "icon");
        }
    }
    else
    {
        if (mIcon)
        {
            delete mIcon;
        }
        mIcon = NULL;
    }

    if (!text.isNull())
    {
        if (!mLabel)
        {
            mLabel = new HbTextItem(this);
            mLabel->setText(text);
            mLabel->setMaximumLines(2);
            mLabel->setTextWrapping(Hb::TextWordWrap);
            mLabel->setElideMode(Qt::ElideNone);
            style()->setItemName(mLabel, "text");
        }
    }
    else
    {
        if (mLabel)
        {
            delete mLabel;
        }
        mLabel = NULL;
    }

    if (!second_text.isNull())
    {
        if (!mSecondLabel)
        {
            mSecondLabel = new HbTextItem(this);
            mSecondLabel->setText(second_text);
            style()->setItemName(mSecondLabel, "second_text");
        }
    }
    else
    {
        if (mSecondLabel)
        {
            delete mSecondLabel;
        }
        mSecondLabel = NULL;
    }

    if (!mFrameItem)
    {
        mFrameItem = new HbFrameItem(this);
        mFrameItem->frameDrawer().setFrameGraphicsName("qtg_fr_groupbox_normal");
        mFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFrameItem->setZValue(-2);
        style()->setItemName(mFrameItem, "background");
    }

    if (!mFocusItem)
    {
       mFocusItem = new HbFrameItem(this);
       mFocusItem->frameDrawer().setFrameGraphicsName("qtg_fr_groupbox_pressed");
       mFocusItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
       mFocusItem->setZValue(-1);
       mFocusItem->setVisible(false);
       style()->setItemName(mFocusItem, "highlight");
    }
    
    if (!mIconTouchArea)
    {
        mIconTouchArea = new HbTouchArea(this);
        style()->setItemName(mIconTouchArea, "iconArea");
    }
}

void CntEditViewHeadingItem::recreatePrimitives()
{
    HbWidget::recreatePrimitives();

    delete mIcon;
    mIcon = NULL;

    delete mLabel;
    mLabel = NULL;

    delete mSecondLabel;
    mSecondLabel = NULL;

    delete mFrameItem;
    mFrameItem = NULL;

    delete mIconTouchArea;
    mIconTouchArea = NULL;

    createPrimitives();
}


void CntEditViewHeadingItem::updatePrimitives()
{
    HbWidget::updatePrimitives();
    if (mTextFocused)
    {
       if (mFocusItem)
           mFocusItem->setVisible(true);
       
       setProperty("state", "pressed");
    }
    else if (mIconFocused)
    {
       HbEffect::start(mIcon, QString("icon"), QString("iconclick"));
    }
    else
    {
       if (mFocusItem)
           mFocusItem->setVisible(false);
       
       setProperty("state", "normal");
    }
}

void CntEditViewHeadingItem::setDetails(const QContact* contact, bool myCard)
{
    text.clear();
    second_text.clear();
    icon.clear();

    // icon label shows default icon first
    icon = HbIcon("qtg_large_add_contact_picture");

    QContactName name = contact->detail<QContactName>();

    XQSettingsManager settingsMng;
    XQSettingsKey nameOrderKey(XQSettingsKey::TargetCentralRepository,
                             KCRCntSettings.iUid,
                             KCntNameOrdering);
    int setting = settingsMng.readItemValue(nameOrderKey, XQSettingsManager::TypeInt).toInt();
    QStringList nameList = QStringList();
    QString last_first;
    
    switch( setting ) {
        case CntOrderLastFirst:
            nameList << name.prefix() << name.lastName() << name.firstName() << name.middleName() << name.suffix();
            break;
        case CntOrderLastCommaFirst:
            if (!name.firstName().isEmpty() && !name.lastName().isEmpty())
                last_first = name.lastName() + ", " + name.firstName();
            else
                last_first = !name.firstName().isEmpty() ? name.firstName() : name.lastName();
            nameList << name.prefix() << last_first << name.middleName() << name.suffix();
            break;
        default:    // Default to first name last name
            nameList << name.prefix() << name.firstName() << name.middleName() << name.lastName() << name.suffix();
            break;
    }
    
    nameList.removeAll("");
    text = nameList.join(" ").trimmed();
    
    if (!contact->detail<QContactNickname>().nickname().isEmpty())
    {
        second_text = contact->detail<QContactNickname>().nickname();
    }

    if (text.isEmpty() && second_text.isEmpty())
    {
        if (myCard)
        {
            text = hbTrId("txt_phob_list_enter_your_name");
        }
        else
        {
            text = hbTrId("txt_phob_list_enter_name");
        }
    }

    recreatePrimitives();
}

void CntEditViewHeadingItem::setIcon(const HbIcon newIcon)
{
    if (newIcon != icon )
    {
        icon = newIcon;
        if (mIcon != NULL)
        {
            delete mIcon;
            mIcon = NULL;
        }
        createPrimitives();
        repolish();
    }
}

void CntEditViewHeadingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HbInstantFeedback::play(HbFeedback::Basic);
    if (mIconTouchArea->rect().contains(event->pos()))
    {
        mIconFocused = true;
        updatePrimitives();
    }
    else if (rect().contains(event->pos()))
    {
        mTextFocused = true;
        updatePrimitives();
    }

    event->accept();
}

void CntEditViewHeadingItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!mIconTouchArea->rect().contains(event->pos()) && mIconFocused)
    {
        mIconFocused = false;
    }
    else if ((!rect().contains(event->pos()) || mIconTouchArea->rect().contains(event->pos())) && mTextFocused)
    {
        mTextFocused = false;
        updatePrimitives();
    }
    event->accept();
}


void CntEditViewHeadingItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (mIconTouchArea->rect().contains(event->pos()) && mIconFocused)
    {
        mIconFocused = false;
        updatePrimitives();
        emit iconClicked();
    }
    else if (rect().contains(event->pos()) && !mIconTouchArea->rect().contains(event->pos()) && mTextFocused)
    {
        mTextFocused = false;
        updatePrimitives();
        emit textClicked();
    }
    event->accept();
}

QVariant CntEditViewHeadingItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSceneHasChanged)
    {
        HbMainWindow *window = mainWindow();
        if (window)
        {
            connect(window, SIGNAL(orientationChanged(Qt::Orientation)), 
                this, SLOT(orientationChanged(Qt::Orientation)));
        }
        else
        {
            QObject::disconnect(this, SLOT(orientationChanged(Qt::Orientation)));
        }
    }
    return HbWidget::itemChange(change, value);
}

void CntEditViewHeadingItem::orientationChanged(Qt::Orientation)
{
    recreatePrimitives();
    repolish();
}
