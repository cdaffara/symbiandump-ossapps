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

#ifndef NMMAILBOXLISTVIEWITEM_H_
#define NMMAILBOXLISTVIEWITEM_H_

#include <hblistviewitem.h>

class HbLabel;
class QGraphicsLinearLayout;

class NmMailboxListViewItem : public HbListViewItem
{
public:
    NmMailboxListViewItem(QGraphicsItem * parent = 0);
    virtual ~NmMailboxListViewItem();

public: // from HbListViewItem
    HbListViewItem *createItem();
    void updateChildItems();
    bool canSetModelIndex(const QModelIndex &index) const;

protected:
    void polishEvent();

private:
    QGraphicsLinearLayout *mLayout;     // Not owned
};

#endif /* NMMAILBOXLISTVIEWITEM_H_ */
