/**
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
* Description: stub collection for testing CVideoCollectionListener
* 
*/

#include "stub/inc/stubcollection.h"

// -----------------------------------------------------------------------------
// StubCollection
// -----------------------------------------------------------------------------
//
StubCollection::StubCollection(MMPXCollectionObserver &observer) :
mObserver(observer)
{
    
}

// -----------------------------------------------------------------------------
// ~StubCollection
// -----------------------------------------------------------------------------
//
StubCollection::~StubCollection()
{
    
}
  
// -----------------------------------------------------------------------------
// callHandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void StubCollection::callHandleCollectionMediaLFunc(const CMPXMedia& aMedia, TInt aError)
{
    mObserver.HandleCollectionMediaL(aMedia, aError);
}

// -----------------------------------------------------------------------------
// callHandleOpenLFunc
// -----------------------------------------------------------------------------
//
void StubCollection::callHandleOpenLFunc( const CMPXMedia& aEntries, TInt aIndex, TBool aComplete, TInt aError )
{
    mObserver.HandleOpenL(aEntries, aIndex, aComplete, aError);
}

// -----------------------------------------------------------------------------
// callHandleOpenLFunc
// -----------------------------------------------------------------------------
//
void StubCollection::callHandleOpenLFunc( const CMPXCollectionPlaylist& aPlaylist, TInt aError)
{
    mObserver.HandleOpenL(aPlaylist, aError);
}

// -----------------------------------------------------------------------------
// callHandleCommandComplete
// -----------------------------------------------------------------------------
//
void StubCollection::callHandleCommandComplete(CMPXCommand* aCommandResult, TInt aError)
{
    mObserver.HandleCommandComplete(aCommandResult, aError);    
}

// -----------------------------------------------------------------------------
// callHandleCollectionMessage
// -----------------------------------------------------------------------------
//
void StubCollection::callHandleCollectionMessage(CMPXMessage* aMessage, TInt aError)
{
    mObserver.HandleCollectionMessage(aMessage, aError); 
}

