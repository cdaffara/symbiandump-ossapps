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
 * Description:  Implementation of applicationmanagement components
 *
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <utf.h>
#include "nsmldmiapmatcher.h"
#include <smldmadapter.h>
#include <featmgr.h>

// ------------------------------------------------------------------------------------------------
// CNSmlDMIAPMatcher* CNSmlDMIAPMatcher::NewL( MSmlDmCallback* aDmCallback )
// ------------------------------------------------------------------------------------------------
CNSmlDMIAPMatcher* CNSmlDMIAPMatcher::NewL( 
	MSmlDmCallback* aDmCallback )
    {
    CNSmlDMIAPMatcher* self = CNSmlDMIAPMatcher::NewLC( aDmCallback );	
    CleanupStack::Pop();
    return self;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMIAPMatcher* CNSmlDMIAPMatcher::NewLC( MSmlDmCallback* aDmCallback )
// ------------------------------------------------------------------------------------------------
CNSmlDMIAPMatcher* CNSmlDMIAPMatcher::NewLC(
	MSmlDmCallback* aDmCallback )
    {
    CNSmlDMIAPMatcher* self = new( ELeave ) CNSmlDMIAPMatcher;
    CleanupStack::PushL( self );
    self->ConstructL( aDmCallback );
    return self;
    }

// ------------------------------------------------------------------------------------------------
// void CNSmlDMIAPMatcher::ConstructL( MSmlDmCallback* aDmCallback )
// ------------------------------------------------------------------------------------------------
void CNSmlDMIAPMatcher::ConstructL( MSmlDmCallback* aDmCallback )
    {
    iCallback = aDmCallback;
    FeatureManager::InitializeLibL();
    iFeatMgrInitialized = ETrue;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMIAPMatcher::~CNSmlDMIAPMatcher()
// ------------------------------------------------------------------------------------------------
CNSmlDMIAPMatcher::~CNSmlDMIAPMatcher()
    {
    	if(iFeatMgrInitialized)
    		{
    			FeatureManager::UnInitializeLib();
    		}
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlDMIAPMatcher::IAPIdFromURIL( const TDesC8& aURI )
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMIAPMatcher::IAPIdFromURIL( const TDesC8& aURI )
    {
    TInt iapId(KErrNotFound);

	MSmlDmAdapter::TError status;
	CBufBase* allIAPs = CBufFlat::NewL(16);
	CleanupStack::PushL(allIAPs);
	iCallback->FetchLinkL( GetConRef(), *allIAPs, status );
	TPtrC8 uri = RemoveDotSlash(aURI);

    if( status == MSmlDmAdapter::EOk &&
        uri.Find(GetConRef()) == 0 &&
        allIAPs->Ptr(0).Find(LastURISeg(uri))>=0)
        {
        HBufC8* luid = iCallback->GetLuidAllocL( uri ); 

        if ( luid->Length() )
            {
            iapId = DesToInt(*luid);
            } 
        delete luid;
        }

    CleanupStack::PopAndDestroy(allIAPs);
    
    return iapId;
    }

// ------------------------------------------------------------------------------------------------
// HBufC8* CNSmlDMIAPMatcher::URIFromIAPIdL( TInt aIAPId )
// ------------------------------------------------------------------------------------------------
HBufC8* CNSmlDMIAPMatcher::URIFromIAPIdL( TInt aIAPId )
    {
    CBufBase *allIAPs = CBufFlat::NewL(16);
	CleanupStack::PushL(allIAPs);
    MSmlDmAdapter::TError status;

    iCallback->FetchLinkL( GetConRef(), *allIAPs, status );

    if ( status == MSmlDmAdapter::EOk )
        {   
        TInt index(0);
        TInt segStart(0);

        while ( index != KErrNotFound)
            {                
            TPtrC8 allIAPsptr = allIAPs->Ptr(segStart).Mid(0);
            index = allIAPsptr.Locate('/');
            HBufC8* uriSeg = 0;

            if ( index == KErrNotFound )
                {
                TPtrC8 uriSeg8Ptr = allIAPs->Ptr(segStart);
                uriSeg = uriSeg8Ptr.AllocLC();
                }
            else
                {
                TPtrC8 uriSeg8Ptr = allIAPs->Ptr(segStart).Mid( 0, index );
                uriSeg = uriSeg8Ptr.AllocLC();
                }

            HBufC8* uri = HBufC8::NewLC( GetConRef().Length() + 1 +
            	uriSeg->Length() );
            	
			TPtr8 uriPtr = uri->Des();
			uriPtr.Format( GetConRef());
			uriPtr.Append( KNSmlDMSeparator8 );
			uriPtr.Append( *uriSeg );
			HBufC8* luid = iCallback->GetLuidAllocL( uriPtr );
			CleanupStack::PushL(luid);

			if ( luid->Length() > 0 )
				{				
				TInt iapId = DesToInt(luid->Des());
				if ( iapId == aIAPId )
					{
		            CleanupStack::PopAndDestroy();  //luid
		            CleanupStack::Pop();  //uri
		            CleanupStack::PopAndDestroy(2);  //uriSeg, allIAPs
		            return uri;
					}
				}
            CleanupStack::PopAndDestroy(3); // luid, uri, uriSeg

            segStart += index + 1;
            }        
        }

    CleanupStack::PopAndDestroy(); // allIAPs

    return 0;
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlDMIAPMatcher::DesToInt(const TDesC& aLuid)
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMIAPMatcher::DesToInt(const TDesC8& aLuid) const
	{
	TLex8 lex(aLuid);
	TInt value = 0;
	lex.Val(value);
	return value;
	}

// ------------------------------------------------------------------------------------------------
// TPtrC8 CNSmlDMIAPMatcher::RemoveDotSlash(const TDesC8& aURI) const
// ------------------------------------------------------------------------------------------------
TPtrC8 CNSmlDMIAPMatcher::RemoveDotSlash(const TDesC8& aURI) const
	{
	if(aURI.Find(KNSmlDMIAPUriDotSlash)==0)
		{
		return aURI.Right( aURI.Length()-KNSmlDMIAPUriDotSlash().Length() );
		}
	else
		{
		return aURI;
		}
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMIAPMatcher::TPtrC8 LastURISeg(const TDesC8& aURI)
// Returns only the last uri segemnt
// ------------------------------------------------------------------------------------------------
TPtrC8 CNSmlDMIAPMatcher::LastURISeg(const TDesC8& aURI) const
	{
	TInt i;
	for(i=aURI.Length()-1;i>=0;i--)
		{
		if(aURI[i]==KNSmlDMSlash)
			{
			break;
			}
		}
	if(i==0)
		{
		return aURI;
		}
	else
		{
		return aURI.Mid(i+1);
		}
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDMIAPMatcher::const TDesC8& GetConRef()
// Returns the connection reference
// ------------------------------------------------------------------------------------------------
const TDesC8& CNSmlDMIAPMatcher::GetConRef()
	{
	if(FeatureManager::FeatureSupported(KFeatureIdFfDmConnmoAdapter))
		{
			return KNSmlDMConnMOIAPUri();
		}
	else
		{
			return KNSmlDMIAPUri();
		}	
  }
  


