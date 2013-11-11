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
 * Description:Message chat View decorator item prototype
 *
 */

#ifndef CNTHISTORYVIEWITEM_H
#define CNTHISTORYVIEWITEM_H

// INCLUDES
#include <hblistviewitem.h>

class HbFrameItem;

/**
 * This class represents the item decorator of 
 * the comm history view.  
 */
class CntHistoryViewItem : public HbListViewItem
{
    Q_OBJECT
    Q_PROPERTY( bool incoming READ getIncoming )
    Q_PROPERTY( bool newmessage READ isNewMessage )
    
public:
    /*
     * Constructor
     * @param parent, reference of QGraphicsItem
     * default set to 0
     */
    CntHistoryViewItem(QGraphicsItem* parent=0);

    /*
     * Factory method to the items
     */
    HbAbstractViewItem* createItem();

    /*
     * Overriden method to draw the custom item in the list view
     */
    void updateChildItems();
    
    bool getIncoming() const { return mIncoming; }
    bool isNewMessage() const { return mNewMessage; }
    
protected:
    /*
     * Overriden method to overwrite the default "pressed" effect
     */
    void pressStateChanged(bool pressed, bool animate);
    
private:
    bool            mIncoming;
    bool            mNewMessage;

    HbFrameItem*    mNewItem;
    HbFrameItem*    mFocusItem;
};

#endif // CNTHISTORYVIEWITEM_H
