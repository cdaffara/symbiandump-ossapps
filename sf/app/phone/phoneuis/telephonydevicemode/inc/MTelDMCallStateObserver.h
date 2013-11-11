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
* Description:  Header file for MTelDMCallStateObserver class.
*                
*
*/


#ifndef MTELDMSTATEOBSERVER_H
#define MTELDMSTATEOBSERVER_H

// INCLUDES

//CLASS DECLARATION

/**
* Call state mode observer.
*
*  @since 3.1
*/
class MTelDMCallStateObserver 
    {
 
    public: // New functions

        /**
        * Notify changes of calls
        * @param  Call state from TelephonyInternalPSKeys.h
        * @return none
        */
        virtual void CallStateChangedL( TInt aCallState ) = 0;

     };

#endif      // MTELDMSTATEOBSERVER_H
            
// End of File
