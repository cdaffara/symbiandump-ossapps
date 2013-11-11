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
* Description:  CalenEventListViewItem class definition
*
*/

#ifndef __CALEN_EVENT_LIST_VIEW_ITEM_H__
#define __CALEN_EVENT_LIST_VIEW_ITEM_H__

// System includes
#include <hblistviewitem.h>

class CalenEventListViewItem : public HbListViewItem
{
    Q_OBJECT
  
public:
    /**
     * @brief Constructor
     * @param CalenAgendaViewWidget
     */
    CalenEventListViewItem(QGraphicsItem *parent = 0);
    
    /**
     * @brief Destructor
     */
    ~CalenEventListViewItem();

    /**
     * @brief creates the instance  of the class
     */
    HbAbstractViewItem * createItem();
    
    /**
     * @brief crestes the item of the list
     */
    void updateChildItems();
    
};

#endif      // __CALEN_EVENT_LIST_VIEW_ITEM_H__
