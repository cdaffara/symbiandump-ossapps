/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MDataSendNotifyHandler is an interface to notify about
* the amount of data sent
*
*/

#ifndef __DATASENDNOTIFYHANDLER_H__
#define __DATASENDNOTIFYHANDLER_H__


/**
* Class that specifies the functions for handling data sending
* notify messages
*/
class MDataSendNotifyHandler
{
public:
    /**
    * Tells how many packets have been sent
    */
    virtual void NotifySend(TInt aAmount) = 0;
};

#endif //__DATASENDNOTIFYHANDLER_H__