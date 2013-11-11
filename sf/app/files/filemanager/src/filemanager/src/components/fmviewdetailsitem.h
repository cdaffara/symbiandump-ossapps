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
 * 
 * Description:
 *     The header file of the view details list item of file manager
 */

#ifndef FMVIEWDETAILSITEM_H
#define FMVIEWDETAILSITEM_H

#include <hblistviewitem.h>

class HbLabel;
class QGraphicsLinearLayout;

class FmViewDetailsItem : public HbListViewItem
{
    Q_OBJECT
    
public:
    FmViewDetailsItem( QGraphicsItem *parent = 0  );
    virtual ~FmViewDetailsItem();
    
public:
    virtual bool canSetModelIndex( const QModelIndex &index ) const;
    virtual HbAbstractViewItem *createItem();
    virtual void updateChildItems();
    
private:
    virtual void polish(HbStyleParameters& params);

private:
    void init();
    
private:
    HbLabel *mDetailsContentLabel;
    HbLabel *mDetailsLabel;   

    QGraphicsLinearLayout *mLayout;
};

#endif /* FMVIEWDETAILSITEM_H */
