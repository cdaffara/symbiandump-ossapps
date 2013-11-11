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
* Description:  Represents a session with the Preset Server
*
*/


#ifndef R_RPSSERVSESSION_H
#define R_RPSSERVSESSION_H

#include <e32base.h>

#include "pscommon.h"

class RPSServ;

/**
 * Preset server session class.
 *
 * Provides access to the preset server for the preset interface. Each session is bound to a single preset by its id.
 */
class RPSServSession
    {

public:

    /**
     * Constructor.
     *
     * @param   aServ       Preset server client.
     * @param   aId         Id of the preset.
     */
    RPSServSession( RPSServ& aServ, TInt aId );
    
    /**
     * Closes the session.
     */
    void Close();

    /**
     * Begins a transaction.
     */
    void BeginTransactionL();

    /**
     * Commits the transaction.
     */
    void CommitL();

    /**
     * Reverts all changes done to the preset prior to a commit.
     */
    void Rollback();

    /**
     * Sends data to the server via the client-server IPC framework.
     * 
     * @param   aServerCommand      Server opcode to use.
     * @param   aValue              Data to write.
     */
    void SetL( TPSOpCode aServerCommand, TInt aValue );

    /**
     * Gets data from the server via the client-server IPC framework.
     * 
     * @param   aServerCommand      Server opcode to use.
     * @param   aValue              Data to read.
     */
    void GetL( TPSOpCode aServerCommand, TInt& aValue );

    /**
     * Sends data to the server via the client-server IPC framework.
     * 
     * @param   aServerCommand      Server opcode to use.
     * @param   aValue              Data to write.
     */
    void SetL( TPSOpCode aServerCommand, const TDesC8& aValue );    
    
    /**
     * Gets data from the server via the client-server IPC framework.
     * 
     * @param   aServerCommand      Server opcode to use.
     * @param   aLengthCommand      PS operation code.
     * @param   aValue              Data to read.
     */
    void GetL( TPSOpCode aServerCommand, TPSOpCode aLengthCommand, HBufC8*& aValue );
    
    /**
     * Sends data to the server via the client-server IPC framework.
     * 
     * @param   aServerCommand      Server opcode to use.
     * @param   aValue              Data to write.
     */
    void SetL( TPSOpCode aServerCommand, const TDesC& aValue );    

    /**
     * Gets data from the server via the client-server IPC framework.
     * 
     * @param   aServerCommand      Server opcode to use.
     * @param   aLengthCommand      PS operation code.
     * @param   aValue              Data to read.
     */
    void GetL( TPSOpCode aServerCommand, TPSOpCode aLengthCommand, HBufC*& aValue );    

private:

    /** Preset server client. */
    RPSServ& iPresetServ;
    /** Id of the preset this session is bound to. */
    TInt iId;

    };

#endif // R_RPSSERVSESSION_H
