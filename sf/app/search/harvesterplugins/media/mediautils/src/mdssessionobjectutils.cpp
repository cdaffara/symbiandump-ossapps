/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MDE session utility class
 *
*/


#include "mdssessionobjectutils.h"
#include "harvesterserverlogger.h"
// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewL()
// -----------------------------------------------------------------------------
//
CMdsSessionObjectUtils* CMdsSessionObjectUtils::NewL()
    {
    CMdsSessionObjectUtils* self = CMdsSessionObjectUtils::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewL()
// -----------------------------------------------------------------------------
//
CMdsSessionObjectUtils* CMdsSessionObjectUtils::NewLC()
    {
    CMdsSessionObjectUtils* self  = new (ELeave) CMdsSessionObjectUtils();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::~CMdsSessionObjectUtils()
// -----------------------------------------------------------------------------
//
CMdsSessionObjectUtils::~CMdsSessionObjectUtils()
    {
    if(iWait->IsStarted())
      iWait->AsyncStop();
    delete iWait;
    if (iQuery)
        {
        delete iQuery;
        iQuery = NULL;
        }
    delete iMdeSession;    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::CMdsSessionObjectUtils()
// -----------------------------------------------------------------------------
//
CMdsSessionObjectUtils::CMdsSessionObjectUtils()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::ConstructL()
// -----------------------------------------------------------------------------
//
void CMdsSessionObjectUtils::ConstructL()
    {
    iWait = new (ELeave) CActiveSchedulerWait;
    //iMdeSession = NULL;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::InitializeL()
// -----------------------------------------------------------------------------
//
void CMdsSessionObjectUtils::InitializeL()
    {
    if(!iMdeSession)
       {
       iMdeSession = CMdESession::NewL(*this);
       }
    iWait->Start();  //Complete in Handle
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::GetSession()
// -----------------------------------------------------------------------------
//
CMdESession* CMdsSessionObjectUtils::GetSession()
    {
    return iMdeSession;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::GetObjectL()
// -----------------------------------------------------------------------------
//
CMdEObject& CMdsSessionObjectUtils::GetObjectL(const TItemId& aItemId,
                                               const TDesC& aObjectDefName)
    {
    CPIXLOGSTRING("START CMdsSessionObjectUtils::GetObjectL()");
    //delete iQuery;//remove old instance if any
	iQuery->Delete(iQuery);
	iQuery = NULL;
    CMdENamespaceDef& defaultNamespaceDef = iMdeSession->GetDefaultNamespaceDefL();
    //Made reusable so that other objectdefs can also use this as it is
    CMdEObjectDef& audioObjDef = defaultNamespaceDef.GetObjectDefL(aObjectDefName );    
    // query objects with object definition e.g "Audio" "video" "images" etc..
    iQuery = iMdeSession->NewObjectQueryL( defaultNamespaceDef, audioObjDef, this );
    //Add object condition to match object id as aItemId
    CMdELogicCondition& rootCondition = iQuery->Conditions();
    (void)rootCondition.AddObjectConditionL(aItemId);
    iQuery->FindL();
    iWait->Start();  //Complete in Handle
    CPIXLOGSTRING("CMdsSessionObjectUtils::GetObjectL() FindL and Started");
    TInt count = iQuery->Count();
    if(count!=1)
        {
        CPIXLOGSTRING("CMdsSessionObjectUtils::GetObjectL() count!=1 Leaving");
        User::Leave(KErrNotFound);
        }
    CMdEObject& mediaObject = static_cast<CMdEObject&>( iQuery->ResultItem( 0));
    CPIXLOGSTRING("END CMdsSessionObjectUtils::GetObjectL()");
    return mediaObject;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::HandleSessionOpened()
// -----------------------------------------------------------------------------
//
void CMdsSessionObjectUtils::HandleSessionOpened(CMdESession& aSession, TInt aError)
    {
    if ( aError != KErrNone )
        {
        delete iMdeSession;
        iMdeSession = NULL;
        }
    if(iWait->IsStarted())
        {
        iWait->AsyncStop();//Signal it
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::HandleQueryNewResults()
// -----------------------------------------------------------------------------
//
void CMdsSessionObjectUtils::HandleQueryNewResults(CMdEQuery& aQuery,
                                                   TInt aFirstNewItemIndex,
                                                   TInt aNewItemCount)
    {
    TInt count = aQuery.Count();
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::HandleQueryCompleted()
// -----------------------------------------------------------------------------
//
void CMdsSessionObjectUtils::HandleQueryCompleted(CMdEQuery& aQuery, TInt aError)
    {
    if(aError !=KErrNone)
        {
        TInt count = aQuery.Count();
        }
    if(iWait->IsStarted()) //Complete in Handle)
        iWait->AsyncStop();
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::HandleSessionError()
// -----------------------------------------------------------------------------
//
void CMdsSessionObjectUtils::HandleSessionError(CMdESession& aSession, TInt aError)
    {
    if(aError != KErrNone)
        {
        delete iMdeSession;
        iMdeSession = NULL;
        }
    }
