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
* Description: List item for conference participant list.
*
*/

#include <QGraphicsLinearLayout>
#include <QTimer>
#include <hbaction.h>
#include <hbpushbutton.h>
#include <hbgroupbox.h>
#include <hbstyleloader.h>
#include <hbabstractitemview.h>
#include <hblistviewitem.h>
#include <hbstringutil.h>

#include "bubbleparticipantlistitem.h"
#include "bubblemanagerif.h"
#include "bubbleutils.h"

// helper class to wrap buttons for group box
class BubbleParticipantListButtons : public HbWidget
{
    Q_OBJECT

public:
    BubbleParticipantListButtons(QGraphicsItem *parent = 0);
    ~BubbleParticipantListButtons();

    HbPushButton* mButton1;
    HbPushButton* mButton2;
};

BubbleParticipantListButtons::BubbleParticipantListButtons(
    QGraphicsItem *parent) : HbWidget(parent)
{
    HbStyleLoader::registerFilePath(
        QLatin1String(":/bubbleparticipantlistbuttons.css"));
    HbStyleLoader::registerFilePath(
        QLatin1String(":/bubbleparticipantlistbuttons.widgetml"));

    mButton1 = new HbPushButton(this);
    style()->setItemName( mButton1, QLatin1String("button-1"));
    mButton2 = new HbPushButton(this);
    style()->setItemName( mButton2, QLatin1String("button-2"));
}

BubbleParticipantListButtons::~BubbleParticipantListButtons()
{
}

BubbleParticipantListItem::BubbleParticipantListItem(
    QGraphicsItem *parent) :
    HbAbstractViewItem(parent),
    mGroupBox(0)
{
    HbStyleLoader::registerFilePath(
        QLatin1String(":/bubbleparticipantlistitem.css"));
    HbStyleLoader::registerFilePath(
        QLatin1String(":/bubbleparticipantlistitem.widgetml"));
}

BubbleParticipantListItem::~BubbleParticipantListItem()
{
}

HbAbstractViewItem* BubbleParticipantListItem::createItem()
{
    return new BubbleParticipantListItem(*this);
}

void BubbleParticipantListItem::updateChildItems()
{
    HbAbstractViewItem::updateChildItems();

    // create controls
    if (!mGroupBox) {
        mGroupBox = new HbGroupBox(this);
        style()->setItemName( mGroupBox, QLatin1String("group-box" ));

        // create buttons for actions (same for all items)
        BubbleParticipantListItem* p =
                static_cast<BubbleParticipantListItem*>(prototype());

        if (p->mActions.count()==2) {
            BubbleParticipantListButtons* content =
                    new BubbleParticipantListButtons();

            // button 1
            HbAction* action1 = p->mActions.at(0);
            content->mButton1->setIcon(action1->icon());
            connect(content->mButton1,
                    SIGNAL(clicked()),
                    action1,
                    SLOT(trigger()),
                    Qt::QueuedConnection);

            // button 2
            HbAction* action2 = p->mActions.at(1);
            content->mButton2->setIcon(action2->icon());
            connect(content->mButton2,
                    SIGNAL(clicked()),
                    action2,
                    SLOT(trigger()),
                    Qt::QueuedConnection);

            mGroupBox->setHeading(QLatin1String(" "));
            mGroupBox->setContentWidget(content);
            mGroupBox->setCollapsed(true);

            // for scrolling to selected item
            connect(mGroupBox,SIGNAL(toggled(bool)),
                    this,SLOT(handleItemStateChange(bool)));

            // for expand/collapse controls
            if (itemView()) {
                connect( itemView()->selectionModel(),
                         SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                         this,
                         SLOT(currentIndexChanged(QModelIndex,QModelIndex)) );
            }
        } else {
            mGroupBox->setCollapsable(false);
        }
    }

    // update group box title (CLI name)
    if (modelIndex().data(Qt::TextAlignmentRole).toInt()==Qt::ElideLeft) {
        QString converted = HbStringUtil::convertDigits(
            modelIndex().data(Qt::DisplayRole).toString());
        mGroupBox->setHeading(converted);
    } else {
        mGroupBox->setHeading(modelIndex().data(Qt::DisplayRole).toString());
    }
}

void BubbleParticipantListItem::polish(HbStyleParameters& params)
{
    HbAbstractViewItem::polish(params);
}

void BubbleParticipantListItem::addAction(HbAction* action)
{
    mActions.append(action);
}

void BubbleParticipantListItem::clearActions()
{
    mActions.clear();
}

void BubbleParticipantListItem::currentIndexChanged(
    const QModelIndex &current,
    const QModelIndex &previous)
{
    Q_UNUSED(previous);

    if ((modelIndex()!=current) && !mGroupBox->isCollapsed()) {
        // change state asynchronously
        QTimer::singleShot(100,this,SLOT(setCollapsed()));
    }
}

void BubbleParticipantListItem::setCollapsed()
{
    mGroupBox->setCollapsed(true);
}

void BubbleParticipantListItem::handleItemStateChange(bool collapsed)
{
    if (!collapsed) {
        itemView()->scrollTo(
            modelIndex(),
            HbAbstractItemView::EnsureVisible);

        // group handles tap gesture, not propagate to list,
        // current index must be updated here
        itemView()->setCurrentIndex(modelIndex());
    }
}

#include "bubbleparticipantlistitem.moc"

