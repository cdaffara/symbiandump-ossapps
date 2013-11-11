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
* Description:  Registers mediator command to get the initial 
*               remote party information.
*
*/

#ifndef CPEREMOTEPARTYINFOMEDIATOR_H_
#define CPEREMOTEPARTYINFOMEDIATOR_H_

#include <e32base.h>

#include "mpemediator.h"

class CMediatorCommandResponder;
class CMediatorEventProvider;
class MPEPhoneModelInternal;
class CPERemotePartyInfoProxy;
class TPERemotePartyInfoMediatorUpdater;


/**
 *  Registers mediator command to get the initial remote party information.
 * 
 *  @lib phonemodel.lib
 *  @since S60 5.2
 */
NONSHARABLE_CLASS( CPERemotePartyInfoMediator ) : 
    public CBase,
    public MPEMediator
    {
public:

    static CPERemotePartyInfoMediator* NewL( MPEPhoneModelInternal& aModel );
    static CPERemotePartyInfoMediator* NewLC( MPEPhoneModelInternal& aModel );
    
    /**
    * Destructor.
    */
    virtual ~CPERemotePartyInfoMediator();
    
    /**
     * Updates remote party info information to mediator clients.
     * 
     * @since S60 5.2
     */
    void UpdateRemotePartyInfo();
    
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

    CPERemotePartyInfoMediator( );

    void ConstructL( MPEPhoneModelInternal& aModel );

private: // data
    
    /**
     * To register and respond commands.
     * Own.
     */
    CMediatorCommandResponder* iResponder;
    
    /**
     * To register and raise events.
     * Own.
     */
    CMediatorEventProvider* iEventProvider;
    
    /**
     * To filter events that contain unchanged data.
     * Own.
     */
    CPERemotePartyInfoProxy* iProxy; 
    
    /**
     * To update the call info to mediator clients.
     * Own.
     */
    TPERemotePartyInfoMediatorUpdater* iUpdater;

    };


#endif /* CPEREMOTEPARTYINFOMEDIATOR_H_ */
