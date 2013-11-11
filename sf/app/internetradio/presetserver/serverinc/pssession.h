/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines a session with the preset server
*
*/


#ifndef C_CPSSESSION_H
#define C_CPSSESSION_H

#include <e32base.h>

class CPSServer;

/**
 * Defines a session with the preset server, as per client-server architecture.
 */
NONSHARABLE_CLASS( CPSSession ) : public CSession2
    {

public:

    /**
     * Two-phased constructor.
     */
    static CPSSession* NewL( CPSServer& aServer );

    /**
     * Destructor.
     */
    ~CPSSession();

// from base class CSession2

    /**
     * @see CSession2::ServiceL( const RMessage2& aMessage )
     */
    void ServiceL( const RMessage2& aMessage );

    /**
     * @see CSession2::Disconnect( const RMessage2& aMessage )
     */
    void Disconnect( const RMessage2 &aMessage);
    
private:

    /**
     * Constructor.
     */
    CPSSession( CPSServer& aServer );

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Handles all service commands from the client. @see TPSOpCode.
     *
     * @param   aMessage    The IPC message.
     * @return  <code>ETrue</code> if the message was handled and can be completed, <code>EFalse</code> otherwise.
     */
    TBool DoServiceL( const RMessage2& aMessage );

    /**
     * Handles EPSOpCreatePreset.
     *
     * @param   aMessage    The IPC message.
     */
    void CreatePresetL( const RMessage2& aMessage );

    /**
     * Handles EPSOpDeletePreset.
     *
     * @param   aMessage    The IPC message.
     */
    void DeletePresetL( const RMessage2& aMessage );

    /**
     * Handles EPSOpStartTransaction.
     *
     * @param   aMessage    The IPC message.
     */
    void StartTransactionL( const RMessage2& aMessage );

    /**
     * Handles EPSOpCommitTransaction.
     *
     * @param   aMessage    The IPC message.
     */
    void CommitTransactionL( const RMessage2& aMessage );

    /**
     * Handles EPSOpRollbackTransaction.
     *
     * @param   aMessage    The IPC message.
     */
    void RollbackTransaction( const RMessage2& aMessage );

    /**
     * Handles EPSOpMovePreset.
     *
     * @param   aMessage    The IPC message.
     */
    void MovePresetL( const RMessage2& aMessage );

    /**
     * Handles EPSOpNotify.
     *
     * @param   aMessage    The IPC message.
     */
    void NotifyPresetChangeL( const RMessage2& aMessage );

    /**
     * Handles EPSOpCancelNotify.
     *
     * @param   aMessage    The IPC message.
     */
    void CancelNotifyPresetChange( const RMessage2& aMessage );

    /**
     * Handles EPSOpSetPresetName.
     *
     * @param   aMessage    The IPC message.
     */
    void SetPresetNameL( const RMessage2& aMessage );

    /**
     * Handles EPSOpSetPresetIndex.
     *
     * @param   aMessage    The IPC message.
     */
    void SetPresetIndexL( const RMessage2& aMessage );

    /**
     * Handles EPSOpSetPresetData.
     *
     * @param   aMessage    The IPC message.
     */
    void SetPresetDataL( const RMessage2& aMessage );

    /**
     * Handles EPSOpGetPresetName.
     *
     * @param   aMessage    The IPC message.
     */
    void GetPresetNameL( const RMessage2& aMessage );

    /**
     * Handles EPSOpGetPresetNameLength.
     *
     * @param   aMessage    The IPC message.
     */
    void GetPresetNameLengthL( const RMessage2& aMessage );

    /**
     * Handles EPSOpGetPresetIndex.
     *
     * @param   aMessage    The IPC message.
     */
    void GetPresetIndexL( const RMessage2& aMessage );

    /**
     * Handles EPSOpGetPresetDataHandler.
     *
     * @param   aMessage    The IPC message.
     */
    void GetPresetDataHandlerL( const RMessage2& aMessage );

    /**
     * Handles EPSOpGetPresetData.
     *
     * @param   aMessage    The IPC message.
     */
    void GetPresetDataL( const RMessage2& aMessage );

    /**
     * Handles EPSOpGetPresetDataLength.
     *
     * @param   aMessage    The IPC message.
     */
    void GetPresetDataLengthL( const RMessage2& aMessage );

    /**
     * Handles EPSOpGetPresets.
     *
     * @param   aMessage    The IPC message.
     */
    void GetPresetsL( const RMessage2& aMessage );

    /**
     * Handles EPSOpGetPresetsLength.
     *
     * @param   aMessage    The IPC message.
     */
    void GetPresetsLengthL( const RMessage2& aMessage );

private:

    /** The preset server. */
    CPSServer& iPresetServer;

    };

#endif // C_CPSSESSION_H
