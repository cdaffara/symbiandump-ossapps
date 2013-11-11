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
* Description:  stub hbcheckbox for testing video list selection dialog
* 
*/

#ifndef HBCHECKBOX_H
#define HBCHECKBOX_H

#include "hbwidget.h"
#include <QGraphicsLayout>
#include <QGraphicsWidget>
#include <QString>

class HbCheckBox : public HbWidget
{
    Q_OBJECT
    
public:
    
    /**
     * Contructor.
     */ 
    HbCheckBox(QGraphicsItem *parent = 0) : HbWidget(parent){initializeCount++;}
      
    /**
     * Destructor.
     */   
    virtual ~HbCheckBox(){initializeCount--;}
    
    /**
     * sets mChecked
     */
    void setChecked(bool check){mChecked = check;}
    
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
    /**
     * dummy value setted in setchecked
     */
    bool mChecked;
    
signals:
    /**
     * state changed -signal
     */
    void stateChanged(int);
 
};

#endif
