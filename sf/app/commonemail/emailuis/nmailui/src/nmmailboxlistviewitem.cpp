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

#include "nmuiheaders.h"

const QString NmMailboxListViewItemIcon = "MailboxListViewMailboxIcon";
const QString NmMailboxListViewItemName = "MailboxListViewMailboxName";

/*!
    \class NmMailboxListViewItem
    \brief Mailbox list view item inherited from HbListViewItem
*/

/*!
    Constructor
*/
NmMailboxListViewItem::NmMailboxListViewItem(QGraphicsItem *parent)
: HbListViewItem(parent),
mLayout(NULL)
{
    NM_FUNCTION;
}

/*!
    Destructor
*/
NmMailboxListViewItem::~NmMailboxListViewItem()
{
    NM_FUNCTION;
}

/*!
    Item creation
*/
HbListViewItem* NmMailboxListViewItem::createItem()
{
    NM_FUNCTION;
    
    return new NmMailboxListViewItem(*this);
}

/*!
    update child items
*/
void NmMailboxListViewItem::updateChildItems()
{
    NM_FUNCTION;
    
    // To create primitives
    HbListViewItem::updateChildItems();

    EmailMailboxInfo mailboxInfo;
    
    NmMailboxMetaData *mailbox =
            modelIndex().data(Qt::DisplayRole).value<NmMailboxMetaData*>();
    
    HbLabel *textLabel = findChild<HbLabel *>(NmMailboxListViewItemName);
    HbLabel *icon = findChild<HbLabel *>(NmMailboxListViewItemIcon);
    
    if (mailbox && !textLabel && !icon){
        mLayout = new QGraphicsLinearLayout(Qt::Horizontal, 0);
        mLayout->setContentsMargins(0,0,0,0);

        HbLabel *mbIcon = new HbLabel();
        mbIcon->setParent(this);
        mbIcon->setObjectName(NmMailboxListViewItemIcon);
        
        QString domainName = mailbox->address();
        QString iconName = mailboxInfo.mailboxIcon(domainName);
        mbIcon->setIcon(HbIcon(iconName));
        mbIcon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

        HbLabel *mbName = new HbLabel();
        mbName->setParent(this);
        mbName->setObjectName(NmMailboxListViewItemName);
        mbName->setPlainText(mailbox->name());
        mbName->setAlignment(Qt::AlignVCenter);
        mbName->setFontSpec(HbFontSpec(HbFontSpec::Primary));
        mbName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        mLayout->addItem(mbIcon);
        mLayout->addItem(mbName);
        mLayout->setItemSpacing(0,0);

        setLayout(mLayout); // Ownership is transferred
    } else if (mailbox) {
        
        if (textLabel) {
            textLabel->setPlainText(mailbox->name());
        }
        
        QString iconName = mailboxInfo.mailboxIcon(mailbox->address());
        if (icon && !iconName.isEmpty()) {
            icon->setIcon(HbIcon(iconName));
        }
        
    }
}

/*!
    boolean value indicating model index availability
*/
bool NmMailboxListViewItem::canSetModelIndex(const QModelIndex &index) const
{
    NM_FUNCTION;
    
    Q_UNUSED(index);
    return true;
}

/*!
    polishEvent. Call to base class polish event.
*/
void NmMailboxListViewItem::polishEvent()
{
    NM_FUNCTION;
    
    QGraphicsWidget::polishEvent();
}

