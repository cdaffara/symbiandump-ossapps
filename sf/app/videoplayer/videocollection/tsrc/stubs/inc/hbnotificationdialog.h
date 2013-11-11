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
* Description:  stub hbnotificationdialog
* 
*/

#ifndef HBNOTIFICATIONDIALOG_H
#define HBNOTIFICATIONDIALOG_H


#include "hbdialog.h"
#include "hbnamespace.h"

class HbNotificationDialog :  public HbDialog
{
   Q_OBJECT
   
   
public:
 
    
    /**
     * contructor
     */
    HbNotificationDialog(QGraphicsItem *parent=0);
    
    /**
     * destructor
     */
    ~HbNotificationDialog();
    
    /**
     * sets mAttribute
     */
    void setAttribute(Qt::WidgetAttribute attr);
    
    /**
     * sets mTitle
     */
    void setTitle(QString txt);
    
    /**
     * sets mTitleTextWRapping
     */
    void setTitleTextWrapping(Hb::TextWrapping value);
    
    /**
     * method just calls deleteLater to remove object as soon as 
     * event loop gets called
     */
    void show();
            
    static QString mNotifDialogTitle;
    
    static Hb::TextWrapping mTitleTextWRapping;
    
    static Qt::WidgetAttribute mAttribute;

};

#endif
