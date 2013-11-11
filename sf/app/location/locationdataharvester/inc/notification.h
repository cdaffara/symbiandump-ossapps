/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __NOTIFICATION_H__
#define __NOTIFICATION_H__

/**  
 * The observer class gives notification to derive class .
 * When calender db creates in specific path.
 */
class MNotifyChange
{
public:
    //Notification from Calender
    virtual void NotifyChangeL(TInt &aStatus)=0;
    
    //Subscribe the value from publisher
    virtual void GetChangeNotificationL(TInt &aId, TInt &addressType, TInt &addressCount )=0;
        
    virtual void SubscribeFromCalendarL(TInt aId) =0;
    
};


#endif  // __NOTIFICATION_H__
// End of file

