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
* Description: Wrapper for mpx harvester framework utilities - private implementation.
*
*/

#ifndef MPMPXHARVESTERFRAMEWORKWRAPPER_P_H
#define MPMPXHARVESTERFRAMEWORKWRAPPER_P_H

#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelperobserver.h>

class MMPXCollectionUtility;
class MMPXHarvesterUtility;
class MpMpxHarvesterFrameworkWrapper;
class QStringList;


class MpMpxHarvesterFrameworkWrapperPrivate : public MMPXCollectionObserver
{
public:

    explicit MpMpxHarvesterFrameworkWrapperPrivate( MpMpxHarvesterFrameworkWrapper *wrapper );
    virtual ~MpMpxHarvesterFrameworkWrapperPrivate();

    void init( quint32 clientSecureId );
    void scan();
    void cancelScan();
    void checkForSystemEvents();

private:

    void HandleOpenL( const CMPXMedia& aEntries,
                     TInt aIndex,TBool aComplete,TInt aError );
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,TInt aError );
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );

    void DoInitL();
    void DoScanL();
    void DoCancelScanL();
    void DoHandleCollectionMessageL( const CMPXMessage& aMsg );
    TBool BroadcastEventFilter( TInt aMsg );
    void DoCheckForSystemEventsL();

private:

    MpMpxHarvesterFrameworkWrapper  *q_ptr;

    MMPXCollectionUtility           *iCollectionUtility;
    MMPXHarvesterUtility            *iHarvesterUtility;
    TInt                            iNumItemsAdded;
    TBool                           iScanning;
    TInt                            iPreviousBroadCastMsg;
    TUid                            iHostUid;

};

#endif // MPMPXHARVESTERFRAMEWORKWRAPPER_P_H

