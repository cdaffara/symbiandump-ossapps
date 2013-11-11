/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FBFILELISTVIEWITEM_H
#define FBFILELISTVIEWITEM_H

#include <HbListViewItem>

#include <QGraphicsWidget>
#include <QtGlobal>

class HbAbstractViewItem;
class HbLabel;
class HbCheckBox;
class HbWidget;

class QGraphicsLinearLayout;
class QModelIndex;
class QGraphicsItem;

class FbFileListViewItem : public HbListViewItem
{
    Q_OBJECT

public:
    FbFileListViewItem( QGraphicsItem *parent = 0 );
    ~FbFileListViewItem();

    virtual bool canSetModelIndex(const QModelIndex &) const;
    virtual HbAbstractViewItem *createItem();
    virtual void updateChildItems();

private slots:
    void setCheckedState(int state);

private:
    virtual void polish(HbStyleParameters &);

private:
    void init();

    QGraphicsLinearLayout *hLayout;

    HbLabel *mDiskNameLabel;
    HbLabel *mSizeLabel;
    HbLabel *mFreeLabel;
    //HbCheckBox *mCheckBox;
    HbLabel *mIconLabel;
};

#endif // FBFILELISTVIEWITEM_H
