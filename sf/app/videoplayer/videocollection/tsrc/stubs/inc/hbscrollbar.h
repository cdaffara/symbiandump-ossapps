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
* Description:  stub hbscrollbar 
* 
*/

#ifndef HBSCROLLBAR_H
#define HBSCROLLBAR_H

#include <QObject>

#include "hbwidget.h"


class  HbScrollBar : public HbWidget
{
    Q_OBJECT
    
public:
    /**
     * Contructor.
     */       
    HbScrollBar( QGraphicsItem *parent = 0 ) : HbWidget(parent) 
    {
        initializeCount++;
    };
    
    
    /**
     * destructor
     */
   virtual ~HbScrollBar(){initializeCount--;}
    
   /**
    * sets mInteractive
    */
   void setInteractive(bool interactive)
   {
       mInteractive = interactive;
   }
   
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
    /**
     * setted in setInteractive
     */
    static bool mInteractive;
    

        
};

#endif
