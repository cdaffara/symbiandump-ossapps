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
* Description: Customized list view item to display autofill items on list.
*
*/

#ifndef NMAUTOFILLLISTVIEWITEM_H_
#define NMAUTOFILLLISTVIEWITEM_H_

#include <hblistviewitem.h>

class HbLabel;
class QGraphicsLinearLayout;
class NmContactHistoryModelSubItem;
class HbRichTextItem;

class NmAutoFillListViewItem : public HbListViewItem
{
    Q_OBJECT
public:
    NmAutoFillListViewItem(QGraphicsItem * parent = NULL);
    virtual ~NmAutoFillListViewItem();

public: // from HbListViewItem
    HbListViewItem *createItem();
    void updateChildItems();
    bool canSetModelIndex(const QModelIndex &index);

private:
    QString setHtmlUnderLine(const NmContactHistoryModelSubItem &subItem);

private:
    HbRichTextItem *mDisplayName; //not owned
    HbRichTextItem *mAddress; //not owned
};

#endif /* NMAUTOFILLLISTVIEWITEM_H_ */
