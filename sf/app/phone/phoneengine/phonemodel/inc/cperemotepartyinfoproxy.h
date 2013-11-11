/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Proxy for remote party information sending
*               to Mediator. Proxy checks if data has changed and 
*               only in that case sends update event further.
*
*/

#ifndef CPEREMOTEPARTYINFOPROXY_H_
#define CPEREMOTEPARTYINFOPROXY_H_

#include <e32base.h>

#include "mpemediator.h"



/**
 *  Sends event about remote party information change if data has changed.
 * 
 *  @lib phonemodel.lib
 *  @since S60 5.2
 */
NONSHARABLE_CLASS( CPERemotePartyInfoProxy ) : 
    public CBase,
    public MPEMediator
    {
public:
    static CPERemotePartyInfoProxy* NewL( MPEMediator& aMediator );
    static CPERemotePartyInfoProxy* NewLC( MPEMediator& aMediator );
        
    /**
     * Destructor.
     */
    virtual ~CPERemotePartyInfoProxy();       
    
        
    /**
     * From base class MPEMediator.
     * @see MPEMediator.
     */
    virtual void SendResponse( TUid aCategory,
            TInt aCommandId, 
            const TDesC8& aData );   
        
    /**
     * From base class MPEMediator
     * @see MPEMediator.
     */
    virtual void RaiseEvent( TUid aCategory, 
            TInt aEventId,
            const TDesC8& aData );

private:

    CPERemotePartyInfoProxy( MPEMediator& aMediator );
    
    void ConstructL( );
    
private: // data        
        
    
    // Access to mediator service which this class is proxy for
    MPEMediator& iMediator;
    
    // current remote party data
    HBufC8* iRemotePartyData;

    };


#endif /* CPEREMOTEPARTYINFOPROXY_H_ */
