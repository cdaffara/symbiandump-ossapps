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
* Description: Handles commands related to unattended transfer.
*
*/


#ifndef C_CPHONEUNATTENDEDTRANSFERCMDHANDLER_H
#define C_CPHONEUNATTENDEDTRANSFERCMDHANDLER_H

#include <e32base.h>

class CPhoneTransferDialerController;
class MPhoneStateMachine;
class MPhoneViewCommandHandle;

/**
 *  Handles commands related to unattended transfer.
 *
 *  @lib PhoneUIVoIPExtension.dll
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CPhoneUnattendedTransferCmdHandler ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @param    aStateMachine       Handle to the state machine.
     * @param    aViewCommandHandle  Handle to the PhoneUIView.
     */
    static CPhoneUnattendedTransferCmdHandler* NewL( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );

    /**
     * Two-phased constructor.
     * @param    aStateMachine       Handle to the state machine.
     * @param    aViewCommandHandle  Handle to the PhoneUIView.
     */
    static CPhoneUnattendedTransferCmdHandler* NewLC( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );
    
    /**
     * Destructor.
     */
    virtual ~CPhoneUnattendedTransferCmdHandler();

    /**
     * Handles VoIP commands related to unattended transfer.
     * @param   aCommand    VoIP command identifier.
     */
    void HandleCommandL( TInt aCommand );
    
private:

    CPhoneUnattendedTransferCmdHandler(         
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );

    void ConstructL();

    /**
     * Shows customized dialer for adding unattended transfer target address. 
     */
    void ShowTransferDialerL();
     
    /**
     * Starts unattended transfer. 
     */
    void DoUnattendedTransferL();
     
    /**
     * Closes transfer dialer view.
     */
    void CloseTransferDialerL();
          
    /**
     * Opens single item contact fetch.
     * @param   aFetchType  Type of the fetch.
     */
    void OpenSingleItemContactFetchL( TInt aFetchType );
    
    /**
     * Shows "Waiting acceptance..."-note
     */
    void ShowWaitingAcceptanceNoteL();
    
private: // data

    /**
     * Unattended transfer dialer customization.
     */
    CPhoneTransferDialerController* iTransferDialerController;

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


#endif // C_CPHONEUNATTENDEDTRANSFERCMDHANDLER_H
