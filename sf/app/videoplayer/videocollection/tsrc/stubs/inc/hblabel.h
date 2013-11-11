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
* Description:  stub hblabel for testing video collection ui uitils
* 
*/

#ifndef HBLABEL_H
#define HBLABEL_H

#include <QString>
#include "hbwidget.h"

class HbLabel : public HbWidget
{
    Q_OBJECT
public:

    /**
     * Contructor.
     */       
    HbLabel() {initializeCount++;}
    
    /**
     * Contructor.
     */ 
    HbLabel(const QString&,  QGraphicsItem *parent = 0) : HbWidget(parent){initializeCount++;}
      
    /**
     * Destructor.
     */   
    virtual ~HbLabel(){initializeCount--;}
    
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
    /**
     * set text
     */
    void setPlainText(QString txt);
    
    /**
     * sets alignment flag
     */
    void setAlignment(Qt::AlignmentFlag alignment);

    /**
     * clears text
     */
    void clear();
	
	int count() const { return 0;}
    
    QGraphicsLayoutItem *itemAt(int index) const 
    {
        Q_UNUSED(index);
        return 0;
    }
    void removeAt(int index) { Q_UNUSED(index); }
    
    void removeItem(QGraphicsLayoutItem* item){Q_UNUSED(item);}
    
    int indexOf(const QGraphicsLayoutItem* item) const
    {
        Q_UNUSED(item);
        return 0;
    }
    
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const
       {
           Q_UNUSED(which);
           Q_UNUSED(constraint);
           return QSizeF();
       }
    
    /**
     * text setted 
     */
	 QString mPlainTxt;
    
    /**
     * alinment setted
     */
    Qt::AlignmentFlag mAlignment;
    
};

#endif
