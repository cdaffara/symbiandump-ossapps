/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for MCSPUUSMessageObserver class
*
*/



#ifndef MCSPUUSMESSAGEOBSERVER_H
#define MCSPUUSMESSAGEOBSERVER_H

//INCLUDES
  
// CLASS DECLARATION

/**
*  Observer interface for indicating User to User Signalling
*
*  @since S60 3.2
*/
class MCSPUUSMessageObserver
    {
    public:

        /**
        * Informs UUS message
        * @since S60 3.2
        * @param aMessage
        */    
        virtual void UUSMessageReceived( const TDesC& aMessage ) = 0;
    };

#endif // MCSPUUSMESSAGEOBSERVER_H

//End of file
