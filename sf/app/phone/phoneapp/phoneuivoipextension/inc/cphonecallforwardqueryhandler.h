/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handles queries related to call forwarding.
*
*/


#ifndef C_CPHONECALLFORWARDQUERYHANDLER_H
#define C_CPHONECALLFORWARDQUERYHANDLER_H

#include <e32base.h>
#include <pevirtualengine.h>
#include "mphonequerycommandobserver.h"

class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class CPhoneStateUtilsVoip;

/**
 *  Handles queries related to call forwarding.
 *
 *  @lib PhoneUIVoIPExtension.dll
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CPhoneCallForwardQueryHandler ) : 
        public CBase,
        public MPhoneQueryCommandObserver
    {

public:

    /**
     * Enum for query types.
     */
    enum TQueryType
        {
        EPhoneQueryNotSet,
        EPhoneQueryAddToContacts,
        EPhoneMovedPermanently,
        EPhoneMultipleChoices
        };
    
    /**
     * Two-phased constructor.
     * @param    aStateMachine       Handle to the state machine.
     * @param    aViewCommandHandle  Handle to the PhoneUIView.
     */
    static CPhoneCallForwardQueryHandler* NewL( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );

    /**
     * Two-phased constructor.
     * @param    aStateMachine       Handle to the state machine.
     * @param    aViewCommandHandle  Handle to the PhoneUIView.
     */
    static CPhoneCallForwardQueryHandler* NewLC( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );
    
    /**
     * Destructor.
     */
    virtual ~CPhoneCallForwardQueryHandler();

    /**
     * Shows moved permanently query. 
     * @param aCallId Id of the call
     */
    void ShowMovedPermanentlyQueryL( TInt aCallId );
 
    /**
     * Shows multiple choices query. 
     * @param aCallId Id of the call
     */
    void ShowMultipleChoicesQueryL( TInt aCallId );

protected:
    
// From MPhoneQueryCommandObserver
    /**
     * Processes query commands
     * @param aCommandId A command id.
     */
    void ProcessCommandL( TInt aCommandId  );
    
private:

    CPhoneCallForwardQueryHandler(         
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );

    void ConstructL();
     
    /**
     * Adds string to contacts.
     * @param aCreateNew ETrue creates new contact,
     *                   EFalse updates exiting contact. 
     */
    void AddToContacsL( TBool aCreateNew );
          
    /**
     * Saves address to contacts.
     */
    void SaveAddressToContactsL();

    /**
     * Shows call is being forwarded information note.
     */
    void ShowCallForwardedNoteL();
 
    /**
     * Resets multiple choices item text array.
     * @param aAddressArray New items for array.
     */
    void ResetMultipleChoicesArrayL(
            const MDesC8Array& aAddressArray );

    /**
     * Shows info note "Call is being forwarded" and starts
     * call forward to selected address.
     * @param aIndex Array index for address where call is being
     *               forwarded.
     */
    void ForwardCallToAddressL( TInt aIndex );
    
private: // data

    /**
     * Interface to the state machine.
     * Not own.
     */
    MPhoneStateMachine& iStateMachine;
    
    /**
     * Interface to the phone ui view.
     * Not own.
     */
    MPhoneViewCommandHandle& iViewCommandHandle;
    
    /**
     * Address array.
     * Not own.
     */
    const CDesC8Array* iAddressArray;

    /**
     * Address array.
     * Own.
     */
    CDesC16ArrayFlat* iMultipleChoicesArray;
    
    /**
     * Call id
     */
    TInt iCallId;
    
    /**
     * Query type
     */
    TInt iQueryType;

    /**
     * Flag to check is command already processed.
     */
    TBool iCommandProcessed;
    };


#endif // C_CPHONECALLFORWARDQUERYHANDLER_H
