/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Abstract class for observe change of setting 
*                through CPhoneCenRepProxy
*
*/


#ifndef __MPHONECENREPOBSERVER_H
#define __MPHONECENREPOBSERVER_H

// CLASS DECLARATION

/**
* Abstract class for observe change of setting through CPhoneCenRepProxy
*/
class MPhoneCenRepObserver
    {
    public: // New functions
        
        /**
        * Handle the change of the setting from Central Repository
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        */
        virtual void HandleCenRepChangeL( 
            const TUid& aUid,
            const TUint aId ) = 0;
    };

#endif      // MPHONECENREPOBSERVER_H
            
// End of File
