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
* Description: Definition of MPhoneDialerController class.
*
*/

#ifndef MPHONEDIALERCONTROLLER_H_
#define MPHONEDIALERCONTROLLER_H_

// Forward declarations
class CAknToolbar;

/**
 *  Abstract custom dialer interface.
 *
 *
 *  @lib PhoneUiView.lib
 *  @since S60 v5.0
 */
class MPhoneDialerController
    {

public:

    /**
    * Initializes the controller. 
    * @param    aToolbar    CAknToolbar instance.
    * @since    S60 v5.1
    */
    virtual void InitializeL( CAknToolbar& aToolbar ) = 0;
    
    /**
    * Returns Cba resource id
    * @return Resource Id of the softkeys
    * @since S60 v5.1
    */
    virtual TInt CbaResourceId() const = 0;
    
    /**
    * Returns Menu resource id
    * @return Id of the menu resource
    * @since S60 v5.1
    */
    virtual TInt MenuResourceId() const = 0;
    
    /**
    * Returns number entry prompt text
    * @return Reference to text to be shown on number entry
    * @since S60 v5.1
    */
    virtual const TDesC& NumberEntryPromptTextL() = 0;
    
    /**
    * Handles the number entry empty event
    * @param aEmpty ETrue if numberentry is empty
    * @since S60 v5.1
    */
    virtual void HandleNumberEntryIsEmpty( TBool aEmpty ) = 0;

    /**
    * Sets visibility of buttons created by the implementation
    * @param aShow ETrue if numberentry is empty
    * @since S60 v5.1
    */
    virtual void ShowButtons( TBool aShow ) = 0;
    };

#endif /*MDIALERCONTROLLER_H_*/




