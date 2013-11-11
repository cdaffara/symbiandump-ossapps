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
* Description: Interface class for handling connection notifications.
*
*/

#ifndef __PROGRESSNOTIFYHANDLER_H__
#define __PROGRESSNOTIFYHANDLER_H__


/**
* Class that specifies the functions for handling progress
* notify messages of the active interface.
*/
class MProgressNotifyHandler
{
public:
    
    /**
    * Actions when interface is up
    */
    virtual void ProgressNotifyReceivedL(TInt aStage, TInt aError) = 0;   
        
    /**
    * Handle error
    */
    virtual void ProgressNotifyError(TInt aStatus) = 0;
};

#endif