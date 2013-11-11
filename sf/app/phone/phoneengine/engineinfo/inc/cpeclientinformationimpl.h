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
* Description:  Phone client information for a single call
*
*/


#ifndef C_CPECLIENTINFORMATIONIMPL_H
#define C_CPECLIENTINFORMATIONIMPL_H

#include "cpeclientinformation.h"

/**
 * Holds phone client information.
 * 
 * This information is specific on a per-call basis.
 */
NONSHARABLE_CLASS( CPEClientInformationImpl ) 
    : public CPEClientInformation
    {

public:
    
    /**
     * Two-phased constructor.
     * 
     * @return  The newly created object. Ownership is transferred to the caller.
     */
    static CPEClientInformationImpl* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CPEClientInformationImpl();

// from base class MPEClientInformation    
    
    void Reset();
    void Set( const MPEClientInformation& aSource );
    void SetContactLink( const TDesC8& aLink );
    const TDesC8& ContactLink() const;
    void SetName( const TName& aName );
    const TName& Name() const;
    void SetNumber( const TPEPhoneNumber& aNumber );
    const TPEPhoneNumber& Number() const;
    void SetCallType( TPECallType aType );
    TPECallType CallType() const;
    void SetAllowMatch( TBool aAllow );
    TBool AllowMatch() const;    
    void SetShowNumber( TBool aShow );
    TBool ShowNumber() const;
    void SetEndOtherCalls( TBool aEndOtherCalls );
    TBool EndOtherCalls() const;
    
private:    
    
    /**
     * Constructor.
     */
    CPEClientInformationImpl();
    
    /**
     * Second-phase constructor.
     */
    void ConstructL();
    
private:
    
    /**
     * Contact link used for retrieving contact information.
     * Owned.
     */
    RBuf8 iContactLink;

    /**
     * Name corresponding the phone number.
     */
    TName iName;
    
    /**
     * Phone number to dial.
     */
    TPEPhoneNumber iNumber;
    
    /**
     * Type of the call.
     */
    TPECallType iCallType;

    /**
     * Flag to indicate if contact matching is allowed.
     */
    TBool iAllowMatch;    
    
    /**
     * Flag to indicate if phone number should be shown.
     */
    TBool iShowNumber;
    
    /**
     * Flag to indicate if other calls should be terminated before attempting this one.
     */
    TBool iEndOtherCalls;
    
    };

#endif // C_CPECLIENTINFORMATIONIMPL_H
