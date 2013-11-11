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
* Description:  Updates remote party information via mediator.
*
*/

#ifndef TPEREMOTEPARTYINFOMEDIATORUPDATER_H
#define TPEREMOTEPARTYINFOMEDIATORUPDATER_H

#include <e32std.h>
#include <MediatorCommandResponder.h>
#include <callremotepartyinformation.h>

class MPEMediator;
class MPEDataStore;

/**
 *  Updates remote party information to mediator
 *
 *  @lib phonemodel.lib
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( TPERemotePartyInfoMediatorUpdater ) : public MMediatorCommandObserver
    {
public:

    TPERemotePartyInfoMediatorUpdater( 
        MPEMediator& aMediator,
        MPEDataStore& iDataStore );
    
    /**
     * Updated remote party information to mediator 
     */
    void Update();
    
    /**
    * From base class MMediatorCommandObserver
    */
    virtual void MediatorCommandL( TUid aDomain,
                                   TUid aCategory, 
                                   TInt aCommandId,
                                   TVersion aVersion, 
                                   const TDesC8& aData );

    /**
     * From base class MMediatorCommandObserver
     */
    virtual void CancelMediatorCommand( TUid aDomain,
                                        TUid aCategory, 
                                        TInt aCommandId );
    
private:
    
    /**
     * Takes all remote party infos and packages infos to descriptor.
     * 
     * @return Descriptor containing remote party informations.
     */
    HBufC8* TakeSnapshotOfRemotePartyInfosL();    
    
    /**
     * Resolves remote party identity.
     * 
     * @return remote party identity.     
     */
    MCallRemotePartyInfo::TRemoteIdentityStatus ResolveRemoteIdentity( const TInt aCallId );
    
    /**
     * Converts remote party identity.
     * 
     * @return remote party identity.
     */
    MCallRemotePartyInfo::TRemoteIdentityStatus ConvertRemoteIdentity( const TInt aCallId );

private: // data
    
    // Access to mediator service
    MPEMediator& iMediator;
    
    // Engine info to gather data
    MPEDataStore& iDataStore;

    };

#endif // TPEREMOTEPARTYINFOMEDIATORUPDATER_H
