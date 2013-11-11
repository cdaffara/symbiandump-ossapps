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
* Description: MTimeOutNotify is an interface class used to 
* notify about timeouts
*
*/

#ifndef _TIMEOUTNOTIFY_H_
#define _TIMEOUTNOTIFY_H_

/**
*  This class specifies the function to be called when a timeout occurs.
*  Used in conjunction with CTimeOutTimer class
*/
class MTimeOutNotify
{
public:
    
    /**
    * The function to be called when a timeout occurs
    */
    virtual void TimerExpired() = 0;
};

#endif