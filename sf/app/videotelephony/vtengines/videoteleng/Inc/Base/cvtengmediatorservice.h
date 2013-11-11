/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles commands received via Mediator
*
*/


#ifndef C_VTENGMEDIATORSERVICE_H
#define C_VTENGMEDIATORSERVICE_H

//  INCLUDES
#include <e32base.h>
#include "mvtengoperationobserver.h"
#include "MVtEngEventObserver.h"
#include "vtengcommands.h"

// FORWARD DECLARATIONS
class CMediatorCommandResponder;
class CVtEngOperation;
class CVtEngInternalOperation;
class CVtEngUtility;

// CLASS DECLARATION

/**
*  Handles shutdown and audio commands from mediator. Deletes itself
*  after service is completed.
*
*  @lib videoteleng
*  @since 5.0
*/
NONSHARABLE_CLASS( CVtEngMediatorService ) : 
    public CBase, 
    private MVtEngOperationObserver,
    private MVtEngEventObserver
    {
public: // constructor and destructors

    /**
     * Starts requested service.
     * @param aResponder target for mediator response
     * @param aUtils engine internal utility
     * @param aCommandId service to start
     */
    static void StartLD( 
        CMediatorCommandResponder& aResponder,
        CVtEngUtility& aUtils,
        const TUid aCategory,
        const TInt aCommandId );
    
    ~CVtEngMediatorService();
    
public: // from MVtEngOperationObserver

    /**
    * @see MVtEngOperationObserver
    */
    virtual void SetOperation( CVtEngOperation* aActiveOp );
    
    /**
    * @see MVtEngOperationObserver
    */
    virtual void CommandCompleteL(
        const TVtEngCommandId aCommand,
        const TInt aResult );

public: // from MVtEngEventObserver

    // handles event according to ongoing operation
    virtual void HandleVtEventL( TInt aEvent );

private: // c++ constructor

    CVtEngMediatorService( 
        CMediatorCommandResponder& aResponder,
        CVtEngUtility& aUtils,
        const TUid aCategory, 
        const TInt aCommandId );
    
private:
    
    // prepares and starts requested operation
    void ConstructAndDoServiceL();
    
    // sends response to shutdown command if shutdown already ongoing
    TBool IsShuttingDown() const;
    
    void StartOperationL( const TVtEngCommandId aCommand );
        
    // Sends mediator response for shutdown or audio mute/unmute commands.
    // deletes itself if not waiting internal command response.
    void SendMediatorResponseD( const TBool aDeleteThis );
    
private:
    CMediatorCommandResponder& iResponder;

    CVtEngInternalOperation* iOperation;
    
    CVtEngUtility& iUtils;
    
    // mediator Category [KCatVideotelInternalCommands, KCatPhoneToVideotelCommands]
    TUid iCategory;
    
    // mediator command [shutdown, mute mic, unmute mic]
    TInt iCommandId;
    
    // this is used for controlling 
    TInt iServiceStatus;
    };

#endif // T_VTENGMEDIATORSERVICE_H

// End of File
