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
* Description:  Listener class for mediator commands
*
*/


#ifndef C_VTENGMDTRMESSAGELISTENER_H
#define C_VTENGMDTRMESSAGELISTENER_H

//  INCLUDES

//#include <e32cmn.h>
#include <MediatorCommandResponder.h>
#include <MediatorEventConsumer.h>
#include "MVtEngEventObserver.h"
#include  "CVtEngStateManager.h"
#include  "MVtEngSessionInfo.h"

// FORWARD DECLARATIONS
class CMediatorCommandResponder;
class CVtEngUtility;

// CLASS DECLARATION

/**
*  Mediator command and event listener 
*
*  @lib videoteleng
*  @since 5.0
*/
NONSHARABLE_CLASS( CVtEngMdtrMessageListener ) : 
    public CBase, 
    private MMediatorCommandObserver,
    private MMediatorEventObserver,
    private MVtEngEventObserver
    
    {
public: // constructor and destructors

    /**
    * two-phase constructor
    */
    static CVtEngMdtrMessageListener* NewL( CVtEngUtility& aUtils );
    
    // Destructor
    ~CVtEngMdtrMessageListener();

public: // from MMediatorCommandObserver

    virtual void MediatorCommandL( TUid aDomain,
         TUid aCategory, 
         TInt aCommandId,
         TVersion aVersion, 
         const TDesC8& aData );

    virtual void CancelMediatorCommand( TUid aDomain,
         TUid aCategory, 
         TInt aCommandId );

public: // from MMediatorEventObserver

    virtual void MediatorEventL( TUid aDomain,
        TUid aCategory, 
        TInt aEventId, 
        const TDesC8& aData );       
            
public: // from MVtEngEventObserver

    // handles deferred CLI event dispatching
    virtual void HandleVtEventL( TInt aEvent );
                                  
private: // constructors

    CVtEngMdtrMessageListener( CVtEngUtility& aUtils );
    
    void ConstructL();

private: // new functions

    void HandleCLIEventL( const TDesC8& aData );
    
    TBool IsReadyForCLIEvent() const;
    
private:    
    CVtEngUtility& iUtils;
    
    // for mediator command reception
    CMediatorCommandResponder* iMediatorResponder;  
    
    CMediatorEventConsumer* iMediatorEvents;
    
    // supported mediator commands
    MediatorService::RCommandList iCommandList;
    };

#endif // C_VTENGMDTRMESSAGELISTENER_H

// End of File
