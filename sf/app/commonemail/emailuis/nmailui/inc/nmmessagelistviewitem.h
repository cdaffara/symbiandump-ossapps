/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMMESSAGELISTVIEWITEM_H_
#define NMMESSAGELISTVIEWITEM_H_

#include <hbtreeviewitem.h>

class HbLabel;
class HbTextItem;
class HbFrameItem;
class NmMessageListModel;
class NmMessageListModelItem;
class NmMessageEnvelope;
class HbIconItem;
class QColor;

class NmMessageListViewItem : public HbTreeViewItem
{
    Q_OBJECT
    
public:
    explicit NmMessageListViewItem(QGraphicsItem *parent = 0);
    virtual ~NmMessageListViewItem();
    void updateChildItems();
    HbTreeViewItem *createItem();
    bool canSetModelIndex(const QModelIndex &index) const;
    QRectF contiguousSelectionArea() const { return QRectF(); };
    
private:
    void createMessageItemLayout();
    void setContentsToMessageItem(const NmMessageEnvelope &envelope, bool dividersActive);
    void setFontsRead();
    void setFontsUnread();
    void setFonts(const QColor &colorRole, HbFontSpec &fontSpec);
    void getFontSizes();    
    QString senderFieldText(const NmMessageEnvelope &envelope);
    bool event(QEvent *event);
    
private:
    HbTextItem *mSender;      // Owned
    HbTextItem *mSubject;     // Owned
    HbTextItem *mTime;        // Owned
    HbLabel *mDividerTitle;   // Owned
    HbFrameItem *mNewMsgIcon; // Owned
    HbIconItem *mIcon1;       // Owned
    HbIconItem *mIcon2;       // Owned
    qreal mPrimarySize;
    qreal mSecondarySize;
};

#endif /* NMMESSAGELISTVIEWITEM_H_ */
