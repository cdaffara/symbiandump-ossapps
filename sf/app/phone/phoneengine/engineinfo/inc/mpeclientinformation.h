/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class to access the client information of a call
*
*/

#ifndef MPECLIENTINFORMATION_H
#define MPECLIENTINFORMATION_H

#include <pevirtualengine.h>

/**
 * Interface to access the client information of a single call.
 */
NONSHARABLE_CLASS( MPEClientInformation )
    {
    
public:
    
    /**
     * Resets the state of this client information.
     */
    virtual void Reset() = 0;
    
    /**
     * Sets information from another client information.
     * 
     * @param   aSource         Client information to set.
     */
    virtual void Set( const MPEClientInformation& aSource ) = 0;
    
    /**
     * Sets the contact link used for retrieving contact information.
     * 
     * Should be available when dialing from Phonebook.
     * 
     * @param   aLink           Contact link to set.
     */
    virtual void SetContactLink( const TDesC8& aLink ) = 0;

    /**
     * Returns the contact link used for retrieving contact information.
     * 
     * @return  Contact link used for retrieving contact information.
     */
    virtual const TDesC8& ContactLink() const = 0;
    
    /**
     * Sets the name corresponding the phone number.
     * 
     * Must be defined if showing the phone number is disabled.
     * 
     * @param   aName           Name corresponding the phone number.
     */
    virtual void SetName( const TName& aName ) = 0;
    
    /**
     * Returns the name corresponding the phone number.
     * 
     * @return  Name corresponding the phone number.
     */
    virtual const TName& Name() const = 0;
    
    /**
     * Sets the phone number to be dialed.
     * 
     * @param   aNumber         Phone number to set.
     */
    virtual void SetNumber( const TPEPhoneNumber& aNumber ) = 0;
    
    /**
     * Returns the phone number to be dialed.
     * 
     * @return  Phone number to be dialed.
     */
    virtual const TPEPhoneNumber& Number() const = 0;
    
    /**
     * Sets the type of the call.
     * 
     * @param   aType           Type of the call to set.
     */
    virtual void SetCallType( TPECallType aType ) = 0;
    
    /**
     * Returns the type of the call.
     * 
     * @return  Type of the call.
     */
    virtual TPECallType CallType() const = 0;

    /**
     * Enables or disables contact matching.
     * 
     * @param   aAllow          Flag to enable or disable contact matching.
     */
    virtual void SetAllowMatch( TBool aAllow ) = 0;
    
    /**
     * Returns if contact matching is allowed.
     * 
     * @return  Flag to indicate if contact matching is allowed.
     */
    virtual TBool AllowMatch() const = 0;    
        
    /**
     * Shows or hides the phone number that is being dialed.
     * 
     * @param   aShow           Flag to indicate if the phone number being dialed should be shown.
     */
    virtual void SetShowNumber( TBool aShow ) = 0;
    
    /**
     * Returns a flag indicating if the phone number being dialed should be shown.
     * 
     * @return  Flag indicating if the phone number being dialed should be shown.
     */
    virtual TBool ShowNumber() const = 0;
    
    /**
     * Flag to indicate if existing calls should be terminated before this one is being attempted.
     * 
     * @param   aEndOtherCalls      Flag to indicate if existing calls should be terminated 
     *                              before this one is being attempted.
     */
    virtual void SetEndOtherCalls( TBool aEndOtherCalls ) = 0;
    
    /**
     * Returns a flag indicating if existing calls should be terminated before this one is being attempted.
     * 
     * @return  Flag indicating if existing calls should be terminated before this one is being attempted.
     */
    virtual TBool EndOtherCalls() const = 0;
    
    };

#endif // MPECLIENTINFORMATION_H
