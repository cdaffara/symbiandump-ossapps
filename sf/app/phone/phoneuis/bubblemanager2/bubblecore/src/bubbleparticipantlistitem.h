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

#ifndef BUBBLEPARTICIPANTLISTITEM_H
#define BUBBLEPARTICIPANTLISTITEM_H

#include <hbabstractviewitem.h>

class QGraphicsItem;
class HbGroupBox;
class HbAction;

class BubbleParticipantListItem : public HbAbstractViewItem
{
    Q_OBJECT

public:
    BubbleParticipantListItem(QGraphicsItem *parent = 0);

    ~BubbleParticipantListItem();

    HbAbstractViewItem* createItem();

    void updateChildItems();
    void addAction(HbAction* action);
    void clearActions();

private:
    void polish(HbStyleParameters& params);

private slots:
    void currentIndexChanged(const QModelIndex &current,
                             const QModelIndex &previous);

    void setCollapsed();

    void handleItemStateChange(bool collapsed);

private:
    HbGroupBox*      mGroupBox;
    QList<HbAction*> mActions;
};

#endif // BUBBLEPARTICIPANTLISTITEM_H
