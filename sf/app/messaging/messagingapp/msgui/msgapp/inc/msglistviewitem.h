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
 * Description:Item decorator of the message list view.
 *
 */

#ifndef MSG_LIST_VIEW_ITEM_H
#define MSG_LIST_VIEW_ITEM_H

// INCLUDES
#include <hblistviewitem.h>

//forward declarations
class HbIconItem;
class HbFrameItem;
class HbTextItem;
class HbIconItem;

/**
 * This class represents the item decorator of 
 * the message list view.  
 */
class MsgListViewItem : public HbListViewItem
    {  
    Q_OBJECT    	
    Q_PROPERTY(bool unReadMsg READ hasUnReadMsg WRITE setHasUnReadMsg)   

public:
    /**
     * Constructor
     */
    MsgListViewItem(QGraphicsItem* parent=0);
    
    /**
     * Creates the list view item
     */
    HbAbstractViewItem* createItem();
    
    /**
     * Sets up the list item layout and display
     */
    void updateChildItems();
       
    /**
     * Draws the seperator line between items in the View
     * @param painter the painter object for the current widget
     * @param option the graphics option for the widget
     * @param widget the widget instance
     */
    /*void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, 
    		QWidget *widget);*/
    		
    /**
     * Sets the mUnReadMsg
     * @param bool
     */
    void setHasUnReadMsg(bool unread = true);

    /**
     * Returns the value of mUnReadMsg
     * @return bool
     */
    bool hasUnReadMsg();    
 
 private:

    /**
     * Create permanent items.
     */
    void initItems();

    /**
     * Returns the preview text based on message type, sub type.
     * Also sets the indicator icon.
     * @return QString
     */
    QString defaultPreviewText(int msgType, int msgSubType);
 
    /**
     * Sets the preview text and timestamp.
     */
    void setTimestampAndPreviewText();
    
    /*
     * set Unread Count and frame
     */
    void setUnreadCountStatus();
    
    /*
     * set common indicator (icon/unread count)
     */
    void setCommonIndicator(const QString& string);

private:
    /**
     * Property to change the color of text
     */
    bool mUnReadMsg;

    /**
     * To display the new message indication
     */
    HbFrameItem* mNewMsgIndicatorItem;
    
    /**
     * To display address.
     * Owned
     */
    HbTextItem *mAddressLabelItem;

    /**
     * To display time stamp.
     * Owned
     */
    HbTextItem *mTimestampItem;

    /**
     * To display message preview.
     * Owned
     */
    HbTextItem *mPreviewLabelItem;

    /**
     * To display unread message count
     * Owned
     */
    HbTextItem *mUnreadCountItem;
    
    /**
     * To display the presence indication
     */
    HbIconItem* mMsgCommonIndicatorItem;

    /**
     * Unit Testing
     */
    #ifdef MSGUI_UNIT_TEST
	  friend class TestMsgListViewItem;
	  
	#endif
    
    };

#endif // MSG_LIST_VIEW_ITEM_H

// EOF
