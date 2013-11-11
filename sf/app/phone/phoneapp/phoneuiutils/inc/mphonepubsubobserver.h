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
* Description:  Abstract class to observe property value changes
*
*/


#ifndef __MPHONEPUBSUBOBSERVER_H
#define __MPHONEPUBSUBOBSERVER_H

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Abstract class to observe the property value changes
*/
class MPhonePubSubObserver
    {
    public: 
        
        /**
        * This function is called when there is property value change.
        * @param aCategory Category of the property
        * @param aKey Property key that is changed
        * @param aValue New property value
        */
        virtual void HandlePropertyChangedL( 
            const TUid& aCategory,
            const TUint aKey,
            const TInt aValue ) = 0;
    };

#endif      // __MPhonePubSubObserver_H
            
// End of File
