/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class is a pure virtual mixin class for MMS bearer observers
*
*/



#ifndef MMMSTRANSPORTOBSERVER_H
#define MMMSTRANSPORTOBSERVER_H

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  This class is a pure virtual mixin class for wap operations
*/
class MMmsTransportObserver 
    {  
    public: // New functions              
        /**
        * Callback for timer cancellation
        */               
        virtual void TimerExpired() = 0;

        /**
        *  Callback for gprs suspend notification
        */               
        virtual void GprsSuspended() = 0;

        /**
        * Callback for gprs resume notification
        */               
        virtual void GprsResumed() = 0;

        /**
        * Callback for transfer cancel notification
        */               
        virtual void TransferCancelled() = 0;

    };

#endif // MMMSTRANSPORTOBSERVER_H
            
// End of File
