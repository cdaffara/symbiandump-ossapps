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
* Description:  stub HbNotificationDialog
* 
*/

#include "hbnotificationdialog.h"

            
QString HbNotificationDialog::mNotifDialogTitle = "";
    
Hb::TextWrapping HbNotificationDialog::mTitleTextWRapping = Hb::TextNoWrap;
    
Qt::WidgetAttribute HbNotificationDialog::mAttribute = Qt::WA_Disabled;

HbNotificationDialog::HbNotificationDialog(QGraphicsItem *parent) :
HbDialog(parent)
{

}
    
HbNotificationDialog::~HbNotificationDialog()
{

}

void HbNotificationDialog::setAttribute(Qt::WidgetAttribute attr)
{
    mAttribute = attr;
}
    
void HbNotificationDialog::setTitle(QString txt)
{
    mNotifDialogTitle = txt;
}
    
void HbNotificationDialog::setTitleTextWrapping(Hb::TextWrapping value)
{
    mTitleTextWRapping = value;
}
    
void HbNotificationDialog::show()
{
    deleteLater();
}


