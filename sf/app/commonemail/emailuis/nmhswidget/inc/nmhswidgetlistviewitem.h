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

#ifndef NMHSWIDGETLISTVIEWITEM_H_
#define NMHSWIDGETLISTVIEWITEM_H_

#include <hblistviewitem.h>

class HbTextItem;
class HbIconItem;
class HbFrameItem;
class NmMessageEnvelope;


class NmHsWidgetListViewItem : public HbListViewItem
{
    Q_OBJECT
public:
    NmHsWidgetListViewItem(QGraphicsItem * parent);
    virtual ~NmHsWidgetListViewItem();
    virtual bool canSetModelIndex(const QModelIndex &index) const;
public: // from HbListViewItem
    virtual HbAbstractViewItem *createItem();
    virtual void updateChildItems();

protected:
    void setFonts(const QColor &colorRole, HbFontSpec &fontSpec);
    void getFontSizes();  
    QString cleanupDisplayName( const QString &displayName );
    
private:
    void createMessageItemLayout();
    void setContentsToMessageItem(const NmMessageEnvelope &envelope);
    void setFontsRead();
    void setFontsUnread();
 
private:
    HbIconItem *mSeparator;   // Owned
    HbTextItem *mSender;      // Owned
    HbTextItem *mSubject;     // Owned
    HbTextItem *mTime;        // Owned
    HbFrameItem *mNewMsgIcon; // Owned
    HbIconItem *mIcon1;        // Owned
    HbIconItem *mIcon2;        // Owned
    QList <HbIconItem*> mStatusIcons;  
    qreal mPrimarySize;
    qreal mSecondarySize;
    
public:    
    friend class TestNmHsWidgetListViewItem;     
};

#endif /* NMHSWIDGETLISTVIEWITEM_H_ */
