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
* Description: Handles commands related to new call.
*
*/


#ifndef C_CPHONENEWCALLCMDHANDLER_H
#define C_CPHONENEWCALLCMDHANDLER_H

#include <e32base.h>

class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class CPhoneStateUtilsVoip;

/**
 *  Handles commands related to new Internet call.
 *
 *  @lib PhoneUIVoIPExtension.dll
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CPhoneNewCallCmdHandler ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @param    aStateMachine       Handle to the state machine.
     * @param    aViewCommandHandle  Handle to the PhoneUIView.
     */
    static CPhoneNewCallCmdHandler* NewL( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );

    /**
     * Two-phased constructor.
     * @param    aStateMachine       Handle to the state machine.
     * @param    aViewCommandHandle  Handle to the PhoneUIView.
     */
    static CPhoneNewCallCmdHandler* NewLC( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );
    
    /**
     * Destructor.
     */
    virtual ~CPhoneNewCallCmdHandler();

    /**
     * Handles VoIP commands related to new call.
     * @param   aCommand    VoIP command identifier.
     */
    void HandleCommandL( TInt aCommand );
    
private:

    CPhoneNewCallCmdHandler(         
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );

    void ConstructL();

    /**
     * Shows new call query. 
     */
    void ShowNewCallQueryL();
          
    /**
     * Opens single item contact fetch.
     * @param   aFetchType  Type of the fetch.
     */
    void OpenSingleItemContactFetchL( TInt aFetchType );

    /**
     * Returns VoIP state utils.
     * @return VoIP state utils.
     */    
    CPhoneStateUtilsVoip& StateUtils();
    
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
    };


#endif // C_CPHONENEWCALLCMDHANDLER_H
