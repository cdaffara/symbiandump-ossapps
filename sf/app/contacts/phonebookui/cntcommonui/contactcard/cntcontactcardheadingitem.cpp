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

#include "cntcontactcardheadingitem.h"
#include "cntdebug.h"

#include <qtcontacts.h>
#include <QGraphicsSceneMouseEvent>

#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbmarqueeitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbstyleloader.h>
#include <hbtoucharea.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbtapgesture.h>
#include <hbeffect.h>
#include <hbinstantfeedback.h>
#include <cntuids.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>

CntContactCardHeadingItem::CntContactCardHeadingItem(QGraphicsItem *parent) :
    HbWidget(parent),
    mIcon(NULL),
    mSecondaryIcon(NULL),
    mFirstLineText(NULL),
    mPrimaryText(NULL),
    mSecondaryText(NULL),
    mMarqueeItem(NULL),
    mFrameItem(NULL),
    mPictureArea(NULL),
    mIsFavorite(false),
    mIsOnline(false)
{
    HbEffect::add("icon", "groupbox_icon_click", "iconclick");
}

CntContactCardHeadingItem::~CntContactCardHeadingItem()
{

}

void CntContactCardHeadingItem::createPrimitives()
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
        mIcon = 0;
    }
    
    if (!secondaryIcon.isNull())
    {
        if (!mSecondaryIcon)
        {
            mSecondaryIcon = new HbIconItem(this);
            mSecondaryIcon->setIcon(secondaryIcon);
            style()->setItemName(mSecondaryIcon, "secondary_icon");
        }
    }
    else
    {
        if (mSecondaryIcon)
        {
            delete mSecondaryIcon;
        }
        mSecondaryIcon = 0;
    }

    if (!firstLineText.isNull())
    {
        if (!mFirstLineText)
        {
            mFirstLineText = new HbTextItem(this);
            mFirstLineText->setText(firstLineText);
            mFirstLineText->setMaximumLines(1);
            mFirstLineText->setElideMode(Qt::ElideNone);
            style()->setItemName(mFirstLineText, "first_line_text");
        }
    }
    else
    {
        if (mFirstLineText)
        {
            delete mFirstLineText;
        }
        mFirstLineText = 0;
    }

    if (!primaryText.isNull())
    {
        if (!mPrimaryText)
        {
            mPrimaryText = new HbTextItem(this);
            mPrimaryText->setText(primaryText);
            mPrimaryText->setMaximumLines(2);
            mPrimaryText->setTextWrapping(Hb::TextWordWrap);
            mPrimaryText->setElideMode(Qt::ElideNone);
            style()->setItemName(mPrimaryText, "primary_text");
        }
    }
    else
    {
        if (mPrimaryText)
        {
            delete mPrimaryText;
        }
        mPrimaryText = 0;
    }

    if (!secondaryText.isNull())
    {
        if (!mSecondaryText)
        {
            mSecondaryText = new HbTextItem(this);
            mSecondaryText->setText(secondaryText);
            mSecondaryText->setMaximumLines(1);
            style()->setItemName(mSecondaryText, "secondary_text");
        }
    }
    else
    {
        if (mSecondaryText)
        {
            delete mSecondaryText;
        }
        mSecondaryText = 0;
    }

    if (!tinyMarqueeText.isNull())
    {
        if (!mMarqueeItem)
        {
            mMarqueeItem = new HbMarqueeItem(this);
            mMarqueeItem->setLoopCount(1);
            mMarqueeItem->setText(tinyMarqueeText);
            mMarqueeItem->startAnimation();
            mMarqueeItem->setFontSpec(HbFontSpec(HbFontSpec::PrimarySmall));
            style()->setItemName(mMarqueeItem, "marquee_item");
        }
    }
    else
    {
        if (mMarqueeItem)
        {
            delete mMarqueeItem;
        }
        mMarqueeItem = 0;
    }

    if (!mFrameItem)
    {
        mFrameItem = new HbFrameItem(this);
        mFrameItem->frameDrawer().setFrameGraphicsName("qtg_fr_groupbox_normal");
        mFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFrameItem->setZValue(-2);
        style()->setItemName(mFrameItem, "background");
    }
    
    if (!mPictureArea) 
    {
        mPictureArea = new HbTouchArea(this);
        style()->setItemName(mPictureArea, "pictureArea");
    }

    if ( mainWindow()->orientation() != Qt::Horizontal)
    {
        initGesture();
    }
    else
    {
        ungrabGesture(Qt::TapGesture);
    }
}

void CntContactCardHeadingItem::setIcon(const HbIcon newIcon)
{
    if (newIcon != icon)
    {
        icon = newIcon;
    }
    if (mainWindow()->orientation() != Qt::Horizontal)
    {
        createPrimitives();
        mIcon->setIcon(icon);
        repolish();
    }
}

void CntContactCardHeadingItem::setFavoriteStatus(bool favoriteContact)
{
    if (favoriteContact != mIsFavorite)
    {
        mIsFavorite = favoriteContact;
        
        if (!mIsOnline)
        {
            setSecondaryIcon();
        }
    }
}

void CntContactCardHeadingItem::recreatePrimitives()
{
    HbWidget::recreatePrimitives();

    delete mIcon;
    mIcon = 0;
    
    delete mSecondaryIcon;
    mSecondaryIcon = 0;

    delete mFirstLineText;
    mFirstLineText = 0;

    delete mPrimaryText;
    mPrimaryText = 0;

    delete mSecondaryText;
    mSecondaryText = 0;

    delete mMarqueeItem;
    mMarqueeItem = 0;

    delete mFrameItem;
    mFrameItem = 0;
    
    delete mPictureArea;
    mPictureArea = 0;

    createPrimitives();
}


void CntContactCardHeadingItem::updatePrimitives()
{
    HbWidget::updatePrimitives();
}

bool CntContactCardHeadingItem::isNickName(const QContact* contact)
{
    return !(contact->detail<QContactNickname>().nickname().isNull());
}

bool CntContactCardHeadingItem::isCompanyName(const QContact* contact)
{
    return (!(contact->detail<QContactOrganization>().title().isNull()) 
        || !(contact->detail<QContactOrganization>().name().isNull())
        || !(contact->detail<QContactOrganization>().department().isEmpty()));
}

void CntContactCardHeadingItem::setDetails(const QContact* contact)
{
    primaryText.clear();
    firstLineText.clear();
    secondaryText.clear();
    icon.clear();
    tinyMarqueeText.clear();
    secondaryIcon.clear();

    // icon label
    icon = HbIcon("qtg_large_add_contact_picture");

    QContactName name = contact->detail<QContactName>();

    // name label
    if (isNickName(contact) || isCompanyName(contact))
    {
        firstLineText = createNameText(name);
        if (firstLineText.isEmpty())
        {
            firstLineText = hbTrId("txt_phob_list_unnamed");
        }
    }
    else
    {
        primaryText = createNameText(name);
        if (primaryText.isEmpty())
        {
            primaryText = hbTrId("txt_phob_list_unnamed");
        }
    }

    // nick label
    if (isNickName(contact))
    {
        secondaryText = contact->detail<QContactNickname>().nickname();
    }

    // company label
    if (isCompanyName(contact))
    {
        QContactOrganization org = contact->detail<QContactOrganization>();
        QStringList companyList;
        
        if (!org.title().isEmpty())
            companyList << org.title();
        if (!org.name().isEmpty())
            companyList << org.name();
        if (!org.department().isEmpty())
            companyList << org.department().join(", ").trimmed();
        tinyMarqueeText = companyList.join(", ").trimmed();
    }

    recreatePrimitives();
    repolish();
}

QString CntContactCardHeadingItem::createNameText(const QContactName name)
{
    XQSettingsManager settingsMng;
    XQSettingsKey nameOrderKey(XQSettingsKey::TargetCentralRepository,
                             KCRCntSettings.iUid,
                             KCntNameOrdering);
    int setting = settingsMng.readItemValue(nameOrderKey, XQSettingsManager::TypeInt).toInt();
    
    QStringList nameList;
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
    return nameList.join(" ").trimmed();
}

void CntContactCardHeadingItem::setGroupDetails(const QContact* contact)
{
    firstLineText.clear();
    secondaryText.clear();
    icon.clear();

    // icon label
    icon = HbIcon("qtg_large_add_group_picture");

    QContactName contactName = contact->detail( QContactName::DefinitionName );
    QString groupName = contactName.value( QContactName::FieldCustomLabel );

    primaryText = groupName;
    
    if (primaryText.isEmpty())
    {
        primaryText = hbTrId("txt_phob_list_unnamed");
    }
       
    recreatePrimitives();
}

void CntContactCardHeadingItem::processLongPress(const QPointF &point)
{
    emit passLongPressed(point);
}

void CntContactCardHeadingItem::processShortPress(const QPointF &point)
{
    emit passShortPressed(point);
}

void CntContactCardHeadingItem::setOnlineStatus(bool online)
{
    if (online != mIsOnline)
    {
        mIsOnline = online;
        
        setSecondaryIcon();
    }
}

void CntContactCardHeadingItem::gestureEvent(QGestureEvent* event)
{
    
    if (HbTapGesture *tap = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) 
    {    
        switch (tap->state()) 
        {
            case Qt::GestureFinished:
                if (tap->tapStyleHint() == HbTapGesture::Tap && mIcon->rect().contains(mapFromScene(tap->position())))
                {
                    processShortPress(tap->position());
                }
                break;
            case Qt::GestureUpdated:
                if (tap->tapStyleHint() == HbTapGesture::TapAndHold && mIcon->rect().contains(mapFromScene(tap->position()))) 
                {
                    HbEffect::cancel(mIcon, QString("iconclick"));
                    processLongPress(tap->position());
                }
                break;
            case Qt::GestureStarted:
                if (tap->tapStyleHint() == HbTapGesture::Tap && mIcon->rect().contains(mapFromScene(tap->position())))
                {
                    HbInstantFeedback::play(HbFeedback::Basic);
                    HbEffect::start(mIcon, QString("icon"), QString("iconclick"));
                }
                
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

QVariant CntContactCardHeadingItem::itemChange(GraphicsItemChange change, const QVariant &value)
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

void CntContactCardHeadingItem::orientationChanged(Qt::Orientation)
{
    recreatePrimitives();
    repolish();
}

void CntContactCardHeadingItem::initGesture()
{
    grabGesture(Qt::TapGesture);
}

void CntContactCardHeadingItem::setSecondaryIcon()
{
    if (mIsOnline)
    {
        secondaryIcon = HbIcon("qtg_small_online");
        recreatePrimitives();
        mSecondaryIcon->setIcon(secondaryIcon);
    }
    else if (!mIsOnline && mIsFavorite)
    {
        secondaryIcon = HbIcon("qtg_small_favorite");
        recreatePrimitives();
        mSecondaryIcon->setIcon(secondaryIcon);
    }
    else
    {
        secondaryIcon.clear();
        recreatePrimitives();
    }

    repolish();
}

// EOF
