/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    DM-settings adapter
*
*/




// INCLUDE FILES
#include "NSmlDMSettingsAdapter.h"
#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldmconstants.h"
#include "nsmldmimpluids.h"
#include "nsmldmiapmatcher.h"
#include "nsmldmauthinfo.h"
#include <featmgr.h>
#include <implementationproxy.h>
#include <SyncMLClientDM.h>
#include <SyncMLTransportProperties.h>


#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::NewL
//  Two-phased constructor.
// -----------------------------------------------------------------------------
CNSmlDMSettingsAdapter* CNSmlDMSettingsAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	_DBG_FILE("CNSmlDMSettingsAdapter::NewL(): begin");

	CNSmlDMSettingsAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();

	_DBG_FILE("CNSmlDMSettingsAdapter::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::NewLC( )
//  Loads the CNSmlDMSettingsAdapter class and pushes it to clenupstack.
// -----------------------------------------------------------------------------
CNSmlDMSettingsAdapter* CNSmlDMSettingsAdapter::NewLC( MSmlDmCallback* aDmCallback )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::NewLC(): begin" );
	CNSmlDMSettingsAdapter* self = new ( ELeave ) CNSmlDMSettingsAdapter( aDmCallback );
	CleanupStack::PushL( self );
    self->iCallBack = aDmCallback;
	self->ConstructL();
	self->iLeafType = EDMUnset;

	_DBG_FILE( "CNSmlDMSettingsAdapter::NewLC(): end" );
	return self;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::CNSmlDMSettingsAdapter()
//  Default constructor.
// -----------------------------------------------------------------------------
CNSmlDMSettingsAdapter::CNSmlDMSettingsAdapter( TAny* aEcomArguments) : 
                                                CSmlDmAdapter(aEcomArguments )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::CNSmlDMSettingsAdapter(): begin" );
	_DBG_FILE( "CNSmlDMSettingsAdapter::CNSmlDMSettingsAdapter(): end" );
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::~CNSmlDMSettingsAdapter()
//  Destructor.
// -----------------------------------------------------------------------------
CNSmlDMSettingsAdapter::~CNSmlDMSettingsAdapter()
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::~CNSmlDMSettingsAdapter(): begin" );

    delete iField;

    // Client API Session & Profile
    iProfile.Close();
	iSyncSession.Close();
    iSyncSessionOpen = EFalse;
    
	// In Case the Buffer isnt cleared
	if ( iBuffer)
		{
		for (TInt val =0; val<iBuffer->Count ();val++)
			{
			delete iBuffer->At(val).iMappingName;

			ClearBuffer ( iBuffer->At(val).iLeafBuf);
			delete iBuffer->At(val).iLeafBuf;
			}

		iBuffer->Reset ();
		delete iBuffer;
		}
    delete iPortNbr;
    iPortBuffer.Reset();
    iPortBuffer.Close();    
    iPrivateApi.Close();
    
    iCallBack = 0; 
           
	_DBG_FILE( "CNSmlDMSettingsAdapter::~CNSmlDMSettingsAdapter(): end" );
	}

// -----------------------------------------------------------------------------
//		CNSmlDMSettingsAdapter::ConstructL()
//		Second phase constructor.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::ConstructL()
	{
	iField = HBufC8::NewL( KNSmlMaxURLLength );
    iSyncSessionOpen = EFalse;
	// variables used for Buffering
	iBufOn = EFalse;
	iComplete = EFalse;
	iExecutionIndex = -1;
	// initialising iBuffer
	iBuffer = new (ELeave) CArrayFixFlat <TNSmlDMBufferElement> (KNSmlDMGranularity);
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::DDFVersionL()
//  Returns the DDF version number.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::DDFVersionL( CBufBase& aVersion )
	{
	_DBG_FILE(
            "CNSmlDMSettingsAdapter::DDFVersionL(CBufBase& aDDFVersion): begin" );
	aVersion.InsertL( 0, KVersion );
	_DBG_FILE(
            "CNSmlDMSettingsAdapter::DDFVersionL(CBufBase& aDDFVersion): end" );
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::DDFStructureL()
//  Fills the DDF structure of the adapter.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::DDFStructureL(): begin" );
      
	MSmlDmDDFObject* ddfRoot = &aDDF;
	MSmlDmDDFObject& ddf = ddfRoot->AddChildObjectL( KSyncMLText );

	TSmlDmAccessTypes *aclTypes = new ( ELeave ) TSmlDmAccessTypes();
	CleanupStack::PushL( aclTypes );
    
    // Set Get as acceptable operations
    aclTypes->SetGet();
	
	ddf.SetDFTitleL( KNSmlDMadapterTitle );
    FillNodeInfoL( ddf, *aclTypes, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode, 
                    KNSmlDMadapterDesc, KNullDesC8, KNullDesC8 );


	MSmlDmDDFObject& nDmDDF = ddf.AddChildObjectL( KNSmlDefDMAcc );
    FillNodeInfoL( nDmDDF, *aclTypes, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode, 
                    KNullDesC8, KNullDesC8, KNullDesC8 );
    
	aclTypes->SetAdd();
	// Set rest acceptable operations for data itself
	aclTypes->SetReplace();
	aclTypes->SetDelete();

	TSmlDmAccessTypes aclTypesNoGet;
	aclTypesNoGet.SetReplace();
	aclTypesNoGet.SetAdd();
	aclTypesNoGet.SetDelete();

	TSmlDmAccessTypes aclTypesAR;
	aclTypesAR.SetReplace();
	aclTypesAR.SetAdd();

    TSmlDmAccessTypes aclTypesARG;
	aclTypesARG.SetReplace();
	aclTypesARG.SetAdd();
    aclTypesARG.SetGet();
    
    TSmlDmAccessTypes aclTypesAG;
	aclTypesAG.SetGet();
	aclTypesAG.SetAdd();

	MSmlDmDDFObject& nProDDF = nDmDDF.AddChildObjectGroupL();
    FillNodeInfoL( nProDDF, *aclTypes, MSmlDmDDFObject::EZeroOrMore, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNullDesC8, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nAddrDDF = nProDDF.AddChildObjectL( KNSmlDdfAddr );			// Addr
    FillNodeInfoL( nAddrDDF, *aclTypes, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAddrDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nAddrTypeDDF = nProDDF.AddChildObjectL( KNSmlDdfAddrType );	// AddrType
    FillNodeInfoL( nAddrTypeDDF, *aclTypes, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAddrTypeDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nPortNbrDDF = nProDDF.AddChildObjectL( KNSmlDdfPortNbr );	// PortNbr
    FillNodeInfoL( nPortNbrDDF, *aclTypes, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EInt, 
                    KNSmlDdfPortNbrDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nAuthSchemeDDF = nProDDF.AddChildObjectL( KNSmlDdfAuthScheme );	// AuthPref
    FillNodeInfoL( nAuthSchemeDDF, *aclTypes, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAuthSchemeDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nServerIdDDF = nProDDF.AddChildObjectL( KNSmlDdfServerId );	// ServerId
    FillNodeInfoL( nServerIdDDF, aclTypesAG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfServerIdDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nServerPWDDF = nProDDF.AddChildObjectL( KNSmlDdfServerPW );	// ServerPW
    FillNodeInfoL( nServerPWDDF, aclTypesNoGet, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfServerPWDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nServerNonceDDF = nProDDF.AddChildObjectL( KNSmlDdfServerNonce );// ServerNonce
    FillNodeInfoL( nServerNonceDDF, aclTypesNoGet, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfServerNonceDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nClientUserNameDDF = nProDDF.AddChildObjectL( KNSmlDdfClientUserName ); // ClientUserName
    FillNodeInfoL( nClientUserNameDDF, *aclTypes, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfClientUserNameDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nClientPWDDF = nProDDF.AddChildObjectL( KNSmlDdfClientPW );	// ClientPW
    FillNodeInfoL( nClientPWDDF, aclTypesNoGet, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfClientPWDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nClientNonceDDF = nProDDF.AddChildObjectL( KNSmlDdfClientNonce );	// ClientNonce
    FillNodeInfoL( nClientNonceDDF, aclTypesNoGet, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfClientNonceDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nNameDDF = nProDDF.AddChildObjectL( KNSmlDdfName );	// Name
    FillNodeInfoL( nNameDDF, *aclTypes, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfNameDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nConRefDDF = nProDDF.AddChildObjectL( KNSmlDdfConRef );	// ConRef
    FillNodeInfoL( nConRefDDF, aclTypesARG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfConRefDescription, KNullDesC8, KMimeType );

	CleanupStack::PopAndDestroy(); //aclTypes
	
	_DBG_FILE("CNSmlDMSettingsAdapter::DDFStructureL(): end");
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::UpdateLeafObjectL
//  Updates profile leaf object data based on URI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::UpdateLeafObjectL( const TDesC8& aURI,
                                                const TDesC8& aLUID, 
                                                const TDesC8& aObject, 
                                                const TDesC8& /*aType*/, 
                                                const TInt aStatusRef )
	{
	_DBG_FILE("CNSmlDMSettingsAdapter::UpdateLeafObjectL(): begin");
	TInt index = -1;
	if ( !iBufOn)
		{
		for (TInt val = 0; val<iBuffer->Count (); val++)
			{
			TPtrC8 firstUri = ParentURI (aURI);
			if ( iBuffer->At(val).iMappingName->Compare (firstUri)== 0)
				{
				index = iExecutionIndex = val;
				break;
				}
			}
		}
	if ( index >= 0)
		AddLeafBufferL (aURI, aObject, aStatusRef);
	else
		{
	if( aLUID.Length() == 0 )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::UpdateLeafObjectL( ): ENotFound end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}
	TInt parentLUID = GetIntObject( aLUID ); 
	if( parentLUID != iParentLUID )
		{
		if( FindProfileIdL( parentLUID ) != EFalse )
			{
			iParentLUID = parentLUID;
			}
		else
			{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		}
	SetField( aURI );
	if( !AcceptDMField() )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::AddLeafObjectL(): KErr field end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
		return;
		}

    //	Get Database field type 
	TNSmlDMFieldType fType = GetDMFieldType();
	if ( fType == EWrong )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::AddLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}
    TNSmlDMProfileData fData = GetDMField();
    if( fData == ESyncAccepted )
        {
        _DBG_FILE( "CNSmlDMSettingsAdapter::AddLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
        }
    if( NotValidStrLenght( fData, aObject ) )
	    {
		_DBG_FILE( "CNSmlDMSettingsAdapter::AddLeafObjectL(): KErr length end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ETooLargeObject );
		return;
		}
    //Authentication data leafs to be handled
    if ( fData == EAuthenticationRequired || fData == EClientNonce 
        || fData == EServerNonce )
        {
        //Check if replace command and replace data as defined
        
        TInt replaceStatus = KErrGeneral; 
        SetAuthInfoL( iParentLUID + KMaxDataSyncID, fData, aObject, replaceStatus );
        if ( replaceStatus == KErrNone )
            {
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
            }
        else if ( replaceStatus == KErrNotFound )
            {
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
            }
        else
            {
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
            }
       
        _DBG_FILE( "CNSmlDMSettingsAdapter::UpdateLeafObjectL():end" );
        return;
        }
        
    TInt rValue = SetDMFieldDataL( iParentLUID, aObject, fData );
    if ( fData == EProfilePort )
        {
        // in case port is updated, save status ref to be used in 
        // CompleteOutstandingCmdsL
        if(rValue==KErrNone)
            {
            iPortBuffer[iPortBuffer.Count()-1].iPortBufStatusRef = aStatusRef; 
            }
        else if( rValue == KErrOverflow)
        {
        	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ETooLargeObject );
        }
        else
        {
        	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
            }
        }
    else
        {
        if( rValue != KErrNone )
            {
            if( rValue == KErrNotFound )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
                }
            else if( rValue == KErrInUse )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EObjectInUse );
                }
            else if( rValue == KErrNoMemory )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENoMemory );
                }
            else
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
                }
            }
        else
            {
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
            }
        }
		}
        
	_DBG_FILE( "CNSmlDMSettingsAdapter::UpdateLeafObjectL():end" );
        return;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::DeleteObjectL
//  Deletes whole profile or leaf object data acording to aURI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::DeleteObjectL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            const TInt aStatusRef )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::DeleteObjectL( ): begin" );

	if( aURI.Find( KNSmlDdfConRef ) != KErrNotFound )
		{	
		_DBG_FILE( "CNSmlDMSettingsAdapter::DeleteObjectL(): NOT ALLOWED end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}

    //	Check which field going to be handled
	SetField( aURI );

    //	Check if valid field
	if( !AcceptDMField() )
		{

        // Delete whole profile if aURI called for node
		TInt last = aURI.LocateReverse( '/' );
		TInt pos  = aURI.Find( KNSmlDefDMAcc );
		if( last - 5 == pos )
			{

            //	If called for node means that profile will deleted from database
			iLUID = IntLUID( aLUID );
			DBG_ARGS8( _S8("DeleteObjectL: URI %S LUID = %d"), &aURI, iLUID );

			if( FindProfileIdL( iLUID ) == EFalse )
				{
				_DBG_FILE(
                    "CNSmlDMSettingsAdapter::DeleteObjectL( ): notfound end");
				
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
				return;
				}
			else
				{
				TInt sRet = DeleteProfileL( iLUID + KMaxDataSyncID );
				if ( sRet != KErrNone )
					{
                    if( sRet == KErrNotFound )
                        {
                        iCallBack->SetStatusL( aStatusRef,
                                                CSmlDmAdapter::ENotFound );
                        }
                    else if( sRet == KErrInUse )
                        {
                        iCallBack->SetStatusL( aStatusRef,
                                                CSmlDmAdapter::EObjectInUse );
                        }
                    else
                        {
                        iCallBack->SetStatusL( aStatusRef, 
                                                CSmlDmAdapter::EError );
					    }
					return;
					}
				else
					{
					_DBG_FILE( "CNSmlDMSettingsAdapter::DeleteObjectL( ): EOk end" );
					iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
					return;
					}
				}
			}
		else
			{
			_DBG_FILE(
                "CNSmlDMSettingsAdapter::DeleteObjectL( ): EInvalidObject end" );
			iCallBack->SetStatusL( aStatusRef,CSmlDmAdapter::EInvalidObject );
			return;
			}
		}//Delete whole Profile

	iLUID = IntLUID( aLUID );
	if ( iLUID != iParentLUID )
		{
		if( FindProfileIdL( iLUID ) == EFalse )
			{
			_DBG_FILE( "CNSmlDMSettingsAdapter::DeleteObjectL( ): end" );
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		iParentLUID = iLUID;
		}
	
	if ( iField->Compare( KNSmlDdfAuthScheme ) == 0 )
    	{
		DeleteDMAuthInfoL( iLUID, EAuthenticationRequired );
        iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        return;
		}
    else if( iField->Compare( KNSmlDdfClientNonce ) == 0 )
    	{
		DeleteDMAuthInfoL( iLUID, EClientNonce );
        iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        return;
		}
    else if( iField->Compare(KNSmlDdfServerNonce) == 0 )
		{
		DeleteDMAuthInfoL( iLUID, EServerNonce );
        iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        return;
		}

    // Sets default value if not null field
	HBufC8* dataObject = HBufC8::NewLC( 15 );
	dataObject->Des().Format( KNullDesC8 );

	_LIT8(aType,		"text/plain");
	iLeafType = EDMDelete;
	UpdateLeafObjectL( aURI, aLUID, *dataObject, aType, aStatusRef );
    CleanupStack::PopAndDestroy(); //dataObject
	iLeafType = EDMUnset;

	_DBG_FILE( "CNSmlDMSettingsAdapter::DeleteObjectL( ): end" );
	return;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::FetchLeafObjectL
//  Fetches profile leaf object data acordint to aURI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                                const TDesC8& aLUID, 
                                                const TDesC8& aType, 
                                                const TInt aResultsRef, 
                                                const TInt aStatusRef )
	{

	_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): begin" );
	DBG_ARGS(_S16( "DM:Fetch aURI    - %S - %S"), &aURI, &aLUID );
		
	iLUID = IntLUID( aLUID );
	if ( iLUID >= 0 )
		{
		if( FindProfileIdL( iLUID ) == EFalse )
			{
			_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): ENotFound end" );
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		}
	else
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): ENotFound end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}

    //	Check which field going to be handled
	SetField(aURI);

    //	Check if valid field
	if( !AcceptDMField() )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): EInvalidObject end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
		return;
		}

    //	Get Database field type 
	TInt fType = GetDMFieldType();
	if ( fType == EWrong )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}

    TNSmlDMProfileData fieldId = GetDMField();
    if( fieldId == ESyncAccepted )
        {
        _DBG_FILE( "CNSmlDMSettingsAdapter::AddLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
        }
    CBufBase *lObject = CBufFlat::NewL( 128 );
    CleanupStack::PushL( lObject );
    lObject->Reset();
    
    CSmlDmAdapter::TError status;

    if ( fType == EStr || fType == EInt || fType == EBin )
		{ 
        TInt rValue = GetObjectL( iLUID, fieldId, *lObject );
		if( rValue != KErrNone )
		    {
            if( rValue == KErrNotFound )
                {
                status = CSmlDmAdapter::ENotFound;
                }
            else if( rValue == KErrInUse )
                {
                status = CSmlDmAdapter::EObjectInUse;
                }
            else if( rValue == KErrNoMemory )
                {
                status = CSmlDmAdapter::ENoMemory;
                }
            else
                {
                status = CSmlDmAdapter::EError;
                }
		    }
        else
            {
            status = CSmlDmAdapter::EOk;
            }
		}
	else 
		{
		CleanupStack::PopAndDestroy(); //lObject 
        _DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): ENotFound end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		iProfile.Close();
        iSyncSession.Close();
        iSyncSessionOpen = EFalse;
		return;
		}
    iCallBack->SetStatusL( aStatusRef, status );
    
    if(status==CSmlDmAdapter::EOk)
    	{
		iCallBack->SetResultsL( aResultsRef, *lObject, aType );
    	}
  	
    CleanupStack::PopAndDestroy(); //lObject 
	iProfile.Close();
    iSyncSession.Close();
    iSyncSessionOpen = EFalse;
	_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): end" );
	return;
	}

// -----------------------------------------------------------------------------
//  CSmlDmSettingsAdapter::ChildURIListL
//  Function returns the list of profiles or leafs of the profile 
//  acording to aURI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::ChildURIListL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                                            const TInt aResultsRef, 
                                            const TInt aStatusRef  )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::ChildURIListL(): begin" );
	CBufBase *currentURISegmentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentURISegmentList );
 
	if( aURI.Match( KDmAccMatch ) != KErrNotFound )
		{
		//	Check if Profile exists
        TInt checkLUID = GetIntObject( aLUID );
        if( FindProfileIdL( checkLUID ) )
        	{
            currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDMAcc );
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
		    iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        	}
        else
        	{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        	}

        CleanupStack::PopAndDestroy(  ); // currentURISegmentList
        _DBG_FILE( "CNSmlDMSettingsAdapter::ChildURIListL(): end" );
		return;
		}
    else if( aURI.Match( KDmAccMatch2 ) != KErrNotFound )
        {
        if( iSyncSessionOpen == EFalse )
            {
            iSyncSession.OpenL();
            iSyncSessionOpen = ETrue;
            }
        RArray<TSmlProfileId> profileIdArray;
        CleanupClosePushL( profileIdArray );
        TSmlUsageType usageType = ESmlDevMan;
        iSyncSession.ListProfilesL( profileIdArray, usageType );
	    
        for( TInt p = 0; p < profileIdArray.Count(); p++ )
		    {
		    TBool notInList = ETrue;
		    TInt newline = 0;
            TInt ddId = profileIdArray[p] - KMaxDataSyncID;
            
		    while( notInList && newline < aPreviousURISegmentList.Count() )
			    {
				TInt ppId = GetIntObject( aPreviousURISegmentList.At(newline).iURISegLUID );
				if( ppId == ddId ) 
				    {
					notInList = EFalse;
					break;
					}
				newline++;
			    }
		    if( notInList )
			    {
			    _LIT8( Kprev, "DMId" );
			    TBuf8<7> addNAME(Kprev); // DMIdnnn , nnn = profileid
                TInt fixedProfId = profileIdArray[p] - KMaxDataSyncID; //fit to 3 decimal
			    addNAME.AppendNumFixedWidth( fixedProfId, EDecimal, 3 ); 

    			currentURISegmentList->InsertL( currentURISegmentList->Size(), addNAME );
	    		currentURISegmentList->InsertL( currentURISegmentList->Size(), KSmlDmSlash );
		    	DBG_ARGS8( _S8("notInList: Id = %d Name %S"), p, &addNAME );

			    // KNSmlDMStart includes start text for URISeg
			    TBuf8<20> addURI; // SyncML/DMAcc/DMIdnnn , nnn = profileid
			    addURI.Append( KNSmlDMStart );
			    addURI.AppendNumFixedWidth( fixedProfId, EDecimal, 3 ); 

    			TBuf8<3> addLUID;
	    		addLUID.Num( fixedProfId );

		    	// Also added to mapping
			    iCallBack->SetMappingL( addURI, addLUID );
    			}
	    	else
		    	{ // Add those in PreviousList which are on database
			    currentURISegmentList->InsertL( currentURISegmentList->Size(),
                                                aPreviousURISegmentList.At(newline).iURISeg );
    			currentURISegmentList->InsertL( currentURISegmentList->Size(), KSmlDmSlash );
//	    		DBG_ARGS8( _S8("InList: Id = %d Name %S"), newline, 
//                          aPreviousURISegmentList.At(newline).iURISeg );
		    	}
		    }
        CleanupStack::PopAndDestroy( 1 );//profileIdArray
        }
    else
        {
        iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
        CleanupStack::PopAndDestroy( 1 ); // currentURISegmentList
        return;
        }

	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
	iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
	CleanupStack::PopAndDestroy(); // currentURISegmentList
	_DBG_FILE( "CNSmlDMSettingsAdapter::ChildURIListL(): end" );
	return;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::AddNodeObjectL
//  Adds new DM-settings profile to database
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::AddNodeObjectL( const TDesC8& aURI, 
                                            const TDesC8& aParentLUID, 
                                            const TInt aStatusRef )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::AddNodeObjectL(): begin" );

	if( aParentLUID.Length() > 0 )
		{
		iParentLUID = GetIntObject( aParentLUID );
        if( FindProfileIdL( iParentLUID ) != EFalse )// iProfile != NULL)
		    {
		    _DBG_FILE( "CNSmlDMSettingsAdapter::AddNodeObjectL(): EAlreadyExists end" );
		    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
		    return;
		    }
		}
	AddNodeBufferL (aURI, aStatusRef);
	_DBG_FILE( "CNSmlDMSettingsAdapter::AddNodeObjectL(): end" );
	return;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::UpdateLeafObjectL( const TDesC8&, const TDesC8&, 
                                                RWriteStream*&, const TDesC8&, 
                                                const TInt )
	{
	//Not supported because data streaming is not supported by this adapter.
	}
	
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::FetchLeafObjectSizeL
//  This function fetches leaf object and returns its size.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                                    const TDesC8& aLUID, 
                                                    const TDesC8& aType, 
                                                    const TInt aResultsRef, 
                                                    const TInt aStatusRef )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectSizeL() : begin" );
    DBG_ARGS( _S16("DM:Fetch aURI    - %S - %S"), &aURI, &aLUID );
	
	iLUID = IntLUID( aLUID );
	if ( iLUID >= 0 )
		{
		if( FindProfileIdL( iLUID ) == EFalse )
			{
			_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): ENotFound end" );
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		}
	else
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): ENotFound end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}

    //	Check which field going to be handled
	SetField( aURI );

    //	Check if valid field
	if( !AcceptDMField() )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): EInvalidObject end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
		return;
		}

    //	Get Database field type 
	TInt fType = GetDMFieldType();
	if ( fType == EWrong )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}

    CBufBase *lObject = CBufFlat::NewL( 128 );
	CleanupStack::PushL( lObject );
    lObject->Reset();

	if ( fType == EStr || fType == EInt || fType == EBin )
		{ 
        TInt rValue = GetObjectL( iLUID, GetDMField(), *lObject );
		if( lObject->Size() == 0 || rValue != KErrNone )
		    {
            if( rValue == KErrNotFound )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
                }
            else if( rValue == KErrInUse )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EObjectInUse );
                }
            else if( rValue == KErrNoMemory )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENoMemory );
                }
            else
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
                }
		    }
		}
	else 
		{
		CleanupStack::PopAndDestroy();//lObject 
		_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): ENotFound end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}
    TInt objSizeInBytes = lObject->Size();
	TBuf8<16> stringObjSizeInBytes;
	stringObjSizeInBytes.Num( objSizeInBytes );

	lObject->Reset();
	lObject->InsertL( 0, stringObjSizeInBytes );
    iCallBack->SetResultsL( aResultsRef, *lObject, aType );
	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk);

	CleanupStack::PopAndDestroy( 1 ); //lObject
	_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectSizeL(): end" );
	return;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::ExecuteCommandL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::ExecuteCommandL( const TDesC8&, const TDesC8&,
                                                const TDesC8&, const TDesC8&, 
                                                const TInt )
	{
	//Not supported. Command execution via DM settings adapter not possible.	
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::ExecuteCommandL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::ExecuteCommandL( const TDesC8&, const TDesC8&, 
                                                RWriteStream*&, const TDesC8&, 
                                                const TInt )
	{
	//Not supported. Command execution via DM settings adapter not possible.
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::CopyCommandL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::CopyCommandL( const TDesC8&, const TDesC8&, 
                                            const TDesC8&, const TDesC8&, 
                                            const TDesC8&, TInt )
	{
	//Not supported.
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::StartAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::StartAtomicL()
	{
	//Not supported. Handling Atomic command cycles not supported by the adapter.
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::CommitAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::CommitAtomicL()
	{
	//Not supported. Handling Atomic command cycles not supported by the adapter.
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::RollbackAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::RollbackAtomicL()
	{
	//Not supported. Handling Atomic command cycles not supported by the adapter.
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::StreamingSupport()
//  Indicates if streaming is supported.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter::StreamingSupport( TInt& /*aItemSize*/ )
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::StreamCommittedL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::StreamCommittedL()
	{
	//Not supported. Streaming not supported by the DM settings adapter.
	}
// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::CompleteOutstandingCmdsL()
// If buffering used, commands must be executed before returning this function.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::CompleteOutstandingCmdsL()
	{
	// Check if Buffered
	for (int val=0; val<iBuffer->Count ();val++)
		{
		if ( !iBuffer->At(val).iExecuted)
			{
			TInt count = iBuffer->At(val).iLeafBuf->Count ()-1;
			iExecutionIndex = val;
			iComplete = ETrue;
			ExecuteBufferL ();
			}
		}
	// if port was updated, set it
	for (TInt i = 0;i<iPortBuffer.Count(); i++)
	    {
        TInt err = SetProfileConnectionPortNrL( iPortBuffer[i].iPortBufLUID, iPortBuffer[i].iPortBuf );
        
        // set status
        if ( err != KErrNone )
            {
            iCallBack->SetStatusL( iPortBuffer[i].iPortBufStatusRef, CSmlDmAdapter::EError );
            }
        else
            {
            iCallBack->SetStatusL( iPortBuffer[i].iPortBufStatusRef, CSmlDmAdapter::EOk );
            }
	    
	    }
    iPortBuffer.Reset();	
	iComplete = EFalse;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::AcceptDMField
//  This function checks if field to be handled is valid.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter::AcceptDMField()
	{
	_DBG_FILE("CNSmlDMSettingsAdapter::AcceptDMField(): begin");

	if ( iField->Compare( KNSmlDdfAddrType ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfAddr ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfPortNbr ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfAuthScheme ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfServerId ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfServerPW ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfServerNonce ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfClientUserName ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfClientPW ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfClientNonce ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfName ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfConRef ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfId ) == 0 )
		{
		return ETrue;
		}

	_DBG_FILE( "CNSmlDMSettingsAdapter::AcceptDMField(): end" );
	return EFalse; // Not a valid Type
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetDMField
//	Matches URI fieldname to DM-database. 
// -----------------------------------------------------------------------------
TNSmlDMProfileData CNSmlDMSettingsAdapter::GetDMField() const
	{

	if ( iField->Compare( KNSmlDdfAddrType ) == 0 )
		{
		return EProfileMediumType;
		}
	else if ( iField->Compare( KNSmlDdfAddr ) == 0 )
		{
		return EProfileURL;
		}
	else if ( iField->Compare( KNSmlDdfPortNbr ) == 0 )
		{
		return EProfilePort;
		}
	else if ( iField->Compare( KNSmlDdfAuthScheme ) == 0 )
		{
		return EAuthenticationRequired;
		}
	else if ( iField->Compare( KNSmlDdfServerId ) == 0 )
		{
		return EServerId;
		}
	else if ( iField->Compare( KNSmlDdfServerPW ) == 0 )
		{
		return ESharedSecret;
		}
	else if ( iField->Compare( KNSmlDdfServerNonce ) == 0 )
		{
		return EServerNonce;
		}
	else if ( iField->Compare( KNSmlDdfClientUserName ) == 0 )
		{
		return EProfileDMServerUsername;
		}
	else if ( iField->Compare( KNSmlDdfClientPW ) == 0 )
		{
		return EProfileDMServerPassword;
		}
	else if ( iField->Compare( KNSmlDdfClientNonce ) == 0 )
		{
		return EClientNonce;
		}
	else if ( iField->Compare( KNSmlDdfName ) == 0 )
		{
		return EProfileName;
		}
	else if ( iField->Compare( KNSmlDdfId ) == 0 )
		{
		return EProfileId;
		}
	else if ( iField->Compare( KNSmlDdfConRef ) == 0 )
		{
		return EProfileIAPId;
		}
	
	_DBG_FILE( "CNSmlDMSettingsAdapter::GetDMField(): Error" );
	return ESyncAccepted;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetDMFieldType
//	Gets field data type. 
// -----------------------------------------------------------------------------
TNSmlDMFieldType CNSmlDMSettingsAdapter::GetDMFieldType() const
	{
	if ( iField->Compare( KNSmlDdfAddrType ) == 0 )
		{
		return EInt;
		}
	else if ( iField->Compare( KNSmlDdfAddr ) == 0 )
		{
		return EStr;
		}
	else if ( iField->Compare( KNSmlDdfPortNbr ) == 0 )
		{
		return EInt;
		}
	else if ( iField->Compare( KNSmlDdfAuthScheme ) == 0 )
		{
		return EInt;
		}
	else if ( iField->Compare( KNSmlDdfServerId ) == 0 )
		{
		return EStr;
		}
	else if ( iField->Compare( KNSmlDdfServerPW ) == 0 )
		{
		return EStr;
		}
	else if ( iField->Compare( KNSmlDdfServerNonce ) == 0 )
		{
		return EBin;
		}
	else if ( iField->Compare( KNSmlDdfClientUserName ) == 0 )
		{
		return EStr;
		}
	else if ( iField->Compare( KNSmlDdfClientPW ) == 0 )
		{
		return EStr;
		}
	else if (iField->Compare(KNSmlDdfClientNonce) == 0)
		{
		return EBin;
		}
	else if ( iField->Compare( KNSmlDdfName ) == 0 )
		{
		return EStr;
		}
	else if ( iField->Compare( KNSmlDdfConRef ) == 0 )
		{
		return EInt;
		}
	else if ( iField->Compare( KNSmlDdfId ) == 0 )
		{
		return EInt;
		}

	_DBG_FILE( "CNSmlDMSettingsAdapter::GetDMFieldType(): Error" );
	return EWrong;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::IntLUID
//	Returns integer value for a profile LUID.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::IntLUID( const TDesC8& aLUID )
	{
	TLex8 lLex( aLUID );

	if( lLex.Val( iLUID ) == KErrNone )
	    {
	    return iLUID;
	    }
	else
        {
        return -1;
        }
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetIntObject
//	Returns integer value for a aObject.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::GetIntObject( const TDesC8& aObject )
	{
	TLex8 lLex( aObject );

	TInt value = 0;

	if( lLex.Val( value ) != KErrNone )
	    {
	    return KErrNotFound;
	    }
	else
        {
        return value;
        }
	}

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::GetIntObject16()
//	Returns Integer Value for a 16 bit data object.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::GetIntObject16( const TDesC& aObject )
	{
	TLex16 lLex( aObject );

	TInt value = 0;

	if( lLex.Val( value ) == KErrNone )
	    {
	    return value;
	    }
	else
        {
        return value;
        }
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetIntObjectLC
//	Converts integer to HBufC8 type buffer and returns reference to it.
// -----------------------------------------------------------------------------
TDesC8& CNSmlDMSettingsAdapter::SetIntObjectLC( const TInt& aObject )
	{
	HBufC8* buf = HBufC8::NewLC( 8 );
	TPtr8 ptrBuf = buf->Des();

	ptrBuf.Num( aObject );
    return *buf;
	}

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::NotValidStrLenght
//	Checks if data length is valid.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter::NotValidStrLenght( const TNSmlDMProfileData& aProfileItem, 
                                                const TDesC8& aSource )
	{
	TInt lLen = aSource.Length();
	TBool lBool = EFalse;

	switch ( aProfileItem )
		{
		case ( EProfileName ) :
			{
			if( lLen > KNSmlMaxProfileNameLength )
			    {
			    lBool = ETrue;	
			    }
			}
		break;
		
		case ( EProfileDMServerUsername ) :
			{
			if( lLen > KNSmlMaxUsernameLength )
			    {
			    lBool = ETrue;
			    }
			}
		break;

		case ( EProfileDMServerPassword ) :
			{
			if( lLen > KNSmlMaxPasswordLength )
			    {
			    lBool = ETrue;	
			    }
			}
		break;

		case ( ESharedSecret ) :
			{
			if( lLen > KNSmlDmSharedSecretMaxLength )
			    {
			    lBool = ETrue;	
			    }
			}
		break;

		case ( EProfileURL ) :
			{
			if( lLen > KNSmlMaxURLLength )
			    {
			    lBool = ETrue;	
			    }
			}
		break;

		case ( EServerId ) :
			{
			if( lLen > KNSmlDmServerIdMaxLength )
			    {
			    lBool = ETrue;	
			    }
			}
		break;

		case ( EProfileIAPId ) :
			{
			if( lLen > KNSmlMaxURLLength )
			    {
			    lBool = ETrue;	
			    }
			}
		break;

		case ( EAuthenticationRequired ) :
			{
			if( lLen > 20 ) 
			    {
			    lBool = ETrue;	
			    }
			}
		break;

		case ( EProfileId ) :
		case ( EProfilePort ) :
		case ( EProfileMediumType ) :
			{
			if( lLen > 8 )
			    {
			    lBool = ETrue;	
			    }
			}
		break;
		
        case ( EServerNonce ) :
	    case ( EClientNonce ) :
	        {
            lBool = EFalse;
	        }
        break;

		default:
			{
			lBool = ETrue;
			}
		}

	return lBool;
	}

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::SetField
//	Last element of the URI (after last '/') is returned.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetField( const TDesC8& aSource )
	{
	if ( aSource.LocateReverse( KNSmlDMUriSeparator ) == KErrNotFound )
	    {
	    iField->Des().Format( aSource );
	    }
	else
        {
        iField->Des().Format( aSource.Mid( aSource.LocateReverse( KNSmlDMUriSeparator ) + 1 ) );
        }

    return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::ConvertTo8LC
//	Converts string value to 8-bit and returns reference.
// -----------------------------------------------------------------------------
TDesC8& CNSmlDMSettingsAdapter::ConvertTo8LC( const TDesC& aSource )
	{
	HBufC8* buf = HBufC8::NewLC( aSource.Length() * 2 );
	TPtr8 bufPtr = buf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return *buf;
	}

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::ConvertTo16LC
//	Converts string value to 16-bit and returns reference.
// -----------------------------------------------------------------------------
TDesC16& CNSmlDMSettingsAdapter::ConvertTo16LC( const TDesC8& aSource)
	{
	HBufC16* buf16 = HBufC16::NewLC( aSource.Length() );
	TPtr16 bufPtr16 = buf16->Des();

	CnvUtfConverter::ConvertToUnicodeFromUtf8( bufPtr16, aSource );

    return *buf16; 
	}

// -------------------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetConRefL
//	Gets the URI for given aObject (IAP ID).
// -------------------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::GetConRefL( CBufBase& aObject )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::GetConRefL() : begin" );
	TInt profIAPid = -1;
    TInt apIdResponse = GetProfileIAPIdL( profIAPid );
    if( profIAPid < 0 )
    {
    	return KErrNone;
    }
    if( apIdResponse != KErrNone )
    {
    	return KErrNotFound;
    }
    CNSmlDMIAPMatcher* apmatch = CNSmlDMIAPMatcher::NewL( iCallBack );
    CleanupStack::PushL( apmatch );
    HBufC8* uri8 = apmatch->URIFromIAPIdL( profIAPid );
    if( uri8 )
        {
        CleanupStack::PushL( uri8 );
        aObject.InsertL( aObject.Size(), uri8->Des() );
        CleanupStack::PopAndDestroy(); // uri8
        }
    else
        {
        CleanupStack::PopAndDestroy(); // apMatch
        return KErrGeneral;    
        }
    CleanupStack::PopAndDestroy(); // iapMatch
	_DBG_FILE( "CNSmlDSSettingsAdapter::GetToNAPIDL() : end" );
	return KErrNone;
	}

// -------------------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetConRefL()
//	Set the profile IAP Id value according to given URI.
// -------------------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetConRefL( const TInt aLUID, const TDesC8& aObject )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetConRefL() : begin" );
    CNSmlDMIAPMatcher* apmatch = CNSmlDMIAPMatcher::NewL( iCallBack );
    CleanupStack::PushL( apmatch );
    TInt lIAPid = apmatch->IAPIdFromURIL( aObject ); 
    CleanupStack::PopAndDestroy(); // iapmatch
    if(aObject.Length() >0 && lIAPid == KErrNotFound) 
    {
    	return KErrGeneral;
    }
    TInt apIdResponse = SetProfileConnectionPropertyL( aLUID, lIAPid );    
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetConRefL() : end" );
	return apIdResponse;
	}

// ------------------------------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::ParseUri
//  Resolves portnbr (separated with ':') as integer from Uri. 
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::ParseUri( const TDesC8& aRealURI, TInt& aPort )
	{
	TInt offset=0;
    //http:
	if( aRealURI.Find( KNSmlDMSettingsHTTP )==0 )//http:
		{
		offset = KNSmlDMSettingsHTTP().Length();
		}
	else if( aRealURI.Find( KNSmlDMSettingsHTTPS )==0 )//https:
		{
		offset = KNSmlDMSettingsHTTPS().Length();
		}

	TBool portFound=EFalse;
    ////123.234.345.456:80/syncml
	for( TInt i = offset; i < aRealURI.Length(); i++ )
		{
		if( aRealURI[i] == KNSmlDMColon )// ':'
			{
			portFound = ETrue;
			offset=i;
			}
		if( portFound && aRealURI[i] == KNSmlDMUriSeparator ) // '/'
			{
			aPort = GetIntObject( aRealURI.Mid( (offset + 1), (i - offset-1) ) );
			}
		else if ( portFound && i == aRealURI.Length() - 1 ) // last char
		    {
		    // port is from offset to the end
		    aPort = GetIntObject( aRealURI.Right( aRealURI.Length() - offset - 1 ) );
		    }
		}
	
	if( aPort != 0 )
		{
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::OpenSyncSessionAndDSProfileL
//  The function opens the server session and DM profile.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::OpenSyncSessionAndProfileL( const TInt aIntLUID, 
                                                        TSmlOpenMode aMode )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::OpenSyncSessionAndProfileL(): begin" );
    if( iSyncSessionOpen == EFalse )
        {
        iSyncSession.OpenL();
        iSyncSessionOpen = ETrue;
        }
    TInt trueLuid = aIntLUID + KMaxDataSyncID;
    TInt rValue(KErrNone);
    
    if(trueLuid!=iCurrentProfile)
	    {
	    RArray<TSmlProfileId> profileIdArray;
	    CleanupClosePushL( profileIdArray );
	    TSmlUsageType usageType = ESmlDevMan;
	    iSyncSession.ListProfilesL( profileIdArray, usageType );
	    TInt index = profileIdArray.Find( trueLuid );
	    CleanupStack::PopAndDestroy(); //profileIdArray
	    if( index == KErrNotFound )
	        {
	        return KErrNotFound;
	        }
	    TRAP( rValue, iProfile.OpenL( iSyncSession, trueLuid, aMode ) );
	    iCurrentProfile = trueLuid;
	    if( rValue != KErrNone )
	        {
	        iCurrentProfile = 0;
	        iProfile.Close();
			iSyncSession.Close();
	        iSyncSessionOpen = EFalse;
	        }
    	}
    _DBG_FILE( "CNSmlDMSettingsAdapter::OpenSyncSessionAndProfileL(): end" );
    return rValue;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::FindProfileId
//  Function checks if profile can be found.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter::FindProfileIdL( const TInt aIntLUID )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::FindProfileIdL(): begin" );
	if( iSyncSessionOpen == EFalse )
	    {
        iSyncSession.OpenL();
        iSyncSessionOpen = ETrue;
	    }
    
	RArray <TSmlProfileId> profileList;
    CleanupClosePushL( profileList );
	TSmlUsageType usageType = ESmlDevMan;		
	iSyncSession.ListProfilesL( profileList, usageType );
	if( profileList.Count() == 0 ) 
	    {
        CleanupStack::PopAndDestroy(); //profileList
        return EFalse;
	    }		
    TInt item = profileList.Find( aIntLUID + KMaxDataSyncID );
    CleanupStack::PopAndDestroy(); //profileList
    if( item != KErrNotFound )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter::FindProfileIdL(): end" );
        return ETrue;
		}
    _DBG_FILE( "CNSmlDMSettingsAdapter::FindProfileIdL(): end" );
	return EFalse;
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::CreateNewProfileL
//  The function creates new DM profile via client API.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::CreateNewProfileL( TInt& aPID )
	{
    _DBG_FILE("CNSmlDMSettingsAdapter::CreateNewProfileL(): begin");
	if( iSyncSessionOpen == EFalse )
	    {
        iSyncSession.OpenL();
        iSyncSessionOpen = ETrue;
	    }
    TRAPD( value1, iProfile.CreateL( iSyncSession ) );
    if( value1 != KErrNone )
        {
   	    iCurrentProfile = 0;
        iProfile.Close();
        iSyncSession.Close();
        iSyncSessionOpen = EFalse;
        return value1;
        }
    
    TRAPD( value2, iProfile.UpdateL() );
    if( value2 != KErrNone )
        {
   	    iCurrentProfile = 0;
        iProfile.Close();
        iSyncSession.Close();
        iSyncSessionOpen = EFalse;
        return value2;
        }
    
    aPID = iProfile.Identifier();
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::CreateNewProfileL(): end" );
    return KErrNone;
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfileServerURIL()
//  The function sets the server URL for a profile.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileServerURIL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileServerURIL(): begin" );
	TInt retVal = KErrNone;
    retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
	    iProfile.Close();
        return retVal;	
		}
	RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
	iProfile.ListConnectionsL( transportIdArray );
    if( transportIdArray.Count() == 0 )
	    {
        CleanupStack::PopAndDestroy();//transportIdArray
	    iCurrentProfile = 0;
        iProfile.Close();
        return KErrGeneral;
	    }
	    
	TInt newPort(0);
    
    RSyncMLConnection conn;
    CleanupClosePushL( conn );
    conn.OpenL( iProfile, transportIdArray[0] );

    if(ParseUri(aObj,newPort)==KErrNotFound)
        {
        TInt port = 0;
        ParseUri(conn.ServerURI(),port);
        if(port==0)
        	{
        	port = 80;
        	}
           	
       	TBuf8<5> buf;
    	buf.Num( port );
        	
        TInt totalSize = aObj.Length() + buf.Length() + 1;
        HBufC8* newServerURI = HBufC8::NewLC( totalSize );
        SetPortNrToAddrStr( aObj, newServerURI, buf );
        conn.SetServerURIL( *newServerURI );
        conn.UpdateL();
    	CleanupStack::PopAndDestroy( newServerURI ); 
        }
    else
        {
        conn.SetServerURIL( aObj );
        conn.UpdateL();
        }
	

	iProfile.UpdateL();
	
	CleanupStack::PopAndDestroy( 2 ); //transportIdArray, conn
    iCurrentProfile = 0;
	iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileServerURIL(): end" );
	return KErrNone;
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfileDisplayNameL()
//  The function sets the display name for a profile.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileDisplayNameL( const TInt aPID, 
                                                        const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileDisplayNameL(): begin" );
	TInt retVal = KErrNone;
    retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return retVal;	
		}
    iProfile.SetDisplayNameL( ConvertTo16LC( aObj ) );
    iProfile.UpdateL();
	CleanupStack::PopAndDestroy(); //ConvertTo16LC
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileDisplayNameL(): end" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfileUserNameL
//  The function sets the profile user name.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileUserNameL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileUserNameL(): begin" );
	TInt retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return KErrGeneral;	
		}
    iProfile.SetUserNameL( aObj );
    iProfile.UpdateL();
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileUserNameL(): end" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfileProtocolVersionL
//  The function sets the protocol version to be supported.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileProtocolVersionL( const TInt aPID, 
                                                            const TDesC8& )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileProtocolVersionL(): begin" );
	TInt retVal = KErrNone;
    retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return retVal;	
		}
	//The syncSession to Sync server is open.
    TSmlProtocolVersion protocol = ESmlVersion1_2;
    iProfile.SetProtocolVersionL( protocol );
	TRAPD( iError, iProfile.UpdateL() );
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileProtocolVersionL(): begin" );
	return iError;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfileServerIdL
//  The function sets the profile ServerId value.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileServerIdL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
    _DBG_FILE("CNSmlDMSettingsAdapter::SetProfileServerIdL(): begin");
	TInt retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return KErrGeneral;	
		}
	iProfile.SetServerIdL( aObj );
    TRAP(retVal,iProfile.UpdateL() );
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileServerIdL(): end" );
	return retVal;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfileServerPasswordL
//  The function sets the profile server password.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileServerPasswordL( const TInt aPID, 
                                                        const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileServerPasswordL(): begin" );
	TInt retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return KErrGeneral;
		}
	iProfile.SetServerPasswordL( aObj );
    iProfile.UpdateL();
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileServerPasswordL(): end" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfilePasswordL
//  The function sets the profile password.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfilePasswordL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfilePasswordL(): begin" );
	TInt retVal = KErrNone;
    retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return retVal;	
		}
	iProfile.SetPasswordL( aObj );
    iProfile.UpdateL();
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfilePasswordL(): begin" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfileConnectionPropertyL
//  This function sets the profile IAP id.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileConnectionPropertyL( const TInt aLUID, 
                                                            const TInt aIAPid )
	{
    TInt retVal = OpenSyncSessionAndProfileL( aLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return retVal;	
		}
    RArray<TSmlTransportId> connIdArray;
    CleanupClosePushL( connIdArray );
    iProfile.ListConnectionsL( connIdArray );
    if( connIdArray.Count() == 1 )
        {
        RSyncMLTransport transport;
        CleanupClosePushL( transport );
	    transport.OpenL( iSyncSession, connIdArray[0] );
	    const CSyncMLTransportPropertiesArray& props = transport.Properties();
	    TInt index = props.Find( KNSmlIAPId );
        if( index > 0 )
            {
            RSyncMLConnection conn;
            CleanupClosePushL( conn );
	        conn.OpenL( iProfile, connIdArray[0] );
            HBufC8* apIdBuffer = HBufC8::NewLC( 4 );
	        TPtr8 ptrIapIdBuf = apIdBuffer->Des();
        	ptrIapIdBuf.Num( aIAPid );
            conn.SetPropertyL( KNSmlIAPId, ptrIapIdBuf );
            conn.UpdateL();
            CleanupStack::PopAndDestroy( 2 ); //conn, apIdBuffer
            retVal = KErrNone;
            }
        else
            {
            retVal = KErrNotFound;
            }
        CleanupStack::PopAndDestroy( 1 ); //transport
        }
    else
        {
        retVal = KErrNotFound;
        }
    iCurrentProfile = 0;
    iProfile.Close();
    CleanupStack::PopAndDestroy(); //connIdArray
    return retVal;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::DeleteProfileL
//  The function deleted the profile from DM database.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::DeleteProfileL( const TInt aPID )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::DeleteProfileL(): begin" );
	if( iSyncSessionOpen == EFalse )
	    {
        iSyncSession.OpenL();
        iSyncSessionOpen = ETrue;
	    }
    
	RArray<TSmlProfileId> profileIdArray;
    CleanupClosePushL( profileIdArray );
	TSmlUsageType usageType = ESmlDevMan;
	iSyncSession.ListProfilesL( profileIdArray, usageType );
	TInt index = profileIdArray.Find( aPID );
	if ( index == KErrNotFound )
		{
        //No match
        CleanupStack::PopAndDestroy();//profileIdArray
		return KErrNotFound;		
		}
    iSyncSession.DeleteProfileL( aPID );
    iSyncSession.Close();
    iSyncSessionOpen = EFalse;
    CleanupStack::PopAndDestroy();//profileIdArray
    _DBG_FILE( "CNSmlDMSettingsAdapter::DeleteProfileL(): end" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetObjectL
//  The function checks which leaf data to be fetched.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::GetObjectL( TInt aLUID, TNSmlDMProfileData aDMField, 
                                            CBufBase& aObject )
    {
    TInt retVal = OpenSyncSessionAndProfileL( aLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return retVal;	
		}
    switch ( aDMField )
		{
		case ( EProfileName ) :
			{
            GetProfileNameL( aObject );
            }
		break;
		
		case ( EProfileDMServerUsername ) :
			{
			GetProfileUserNameL( aObject );
            }
		break;

		case ( EProfileURL ) :
			{
			GetProfileServerURLL( aObject );
			TInt port(0);
			if(ParseUri(aObject.Ptr(0),port)==KErrNone)
			    {
			    HBufC8* addr = aObject.Ptr(0).AllocLC();
			    aObject.Reset();
			    TInt portStart = addr->LocateReverse(KNSmlDMColon);
			    aObject.InsertL(aObject.Size(),addr->Left(portStart));
			    
			    TInt portLen=addr->Mid(portStart).Locate(KNSmlDMUriSeparator);
			    if(portLen!=KErrNotFound)
			        {
			        aObject.InsertL(aObject.Size(),addr->Mid(portStart+portLen));
			        }
			    	    			    
			    CleanupStack::PopAndDestroy(); //addr
			    }
            }
		break;

		case ( EServerId ) :
			{
            GetServerIdL( aObject );
            }
		break;

		case ( EProfileIAPId ) :
			{
		    retVal = GetConRefL( aObject );
            }
		break;

		case ( EProfilePort ) :
            {
            GetProfileServerURLL( aObject );
            if ( aObject.Size() == 0 )
                {
                retVal = KErrNone;
                break;
                }
            GetProfilePortNumberL ( aObject );
            }
        break;
        
   		case ( EProfileDMServerPassword ) :
			{
			retVal = EInvalidObject;
            }
		break;

		case ( ESharedSecret ) :
            {//ServerPW
            retVal = EInvalidObject;
            }
        break;
        case ( EProfileId ) :
        break;
		case ( EProfileMediumType ) :
            {
            retVal = GetProfileConnectiontypeL( aObject );
            }
        break;

        case ( EAuthenticationRequired ) :
        case ( EServerNonce ) :
        case ( EClientNonce ) :
            {
            retVal = GetDMAuthInfoL( aLUID, aDMField, aObject );
            }
        break;
		
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
    iCurrentProfile = 0;
    iProfile.Close();
    return retVal;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetProfileConnectionPortNrL()
//  The function sets the profile port number.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileConnectionPortNrL( const TInt aLUID, 
                                                            const TDesC8& aPort )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileConnectionPortNrL(): begin" );
    TInt retVal = KErrNone;
    
    if( iPortNbr )
        {
        delete iPortNbr;
        iPortNbr = NULL;
        }
    iPortNbr = HBufC8::NewL( aPort.Length() );
    TPtr8 newPortNbr = iPortNbr->Des();
    newPortNbr.Append( aPort );
        
    retVal = OpenSyncSessionAndProfileL( aLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileConnectionPortNrL(): end" );
        return retVal;	
		}
    
    RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
    iProfile.ListConnectionsL( transportIdArray );
    
    RSyncMLConnection conn;
    CleanupClosePushL( conn );
    conn.OpenL( iProfile, transportIdArray[0] );
        
    HBufC8* serverURI = conn.ServerURI().AllocLC();
    if( serverURI->Length() == 0 )
        {
	    iCurrentProfile = 0;
        iProfile.Close();
        CleanupStack::PopAndDestroy( 3 ); //transportIdArray, conn, serverURI
        _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileConnectionPortNrL(): end" );
        return retVal;
	    }
    TInt totalSize = serverURI->Size() + newPortNbr.Size() + 1;
    HBufC8* newServerURI = HBufC8::NewL( totalSize );
    CleanupStack::PushL( newServerURI );
    SetPortNrToAddrStr( *serverURI, newServerURI, newPortNbr );
    
    conn.SetServerURIL( *newServerURI );
    conn.UpdateL();
    
    CleanupStack::PopAndDestroy( 4 ); //transportIdArray, conn, serverURI, newServerURI
    iCurrentProfile = 0;
	iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileConnectionPortNrL(): end" );
    return retVal;	
    }

// ------------------------------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetPortNrToAddrStr
//  Sets a portnbr (separated with ':') to profile server URI. 
// ------------------------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::SetPortNrToAddrStr( const TDesC8& aSrvURI, HBufC8* aNewURI, 
                                                    const TDesC8& aPort )
	{
	TInt offset=0;
    if( aSrvURI.Find( KNSmlDMSettingsHTTP ) == 0 )//http://
		{
		offset = KNSmlDMSettingsHTTP().Length();
		}
	else if( aSrvURI.Find( KNSmlDMSettingsHTTPS ) == 0 )//https://
		{
		offset = KNSmlDMSettingsHTTPS().Length();
		}
	TInt portStart = -1;
    TInt separatorPos = aSrvURI.Length();
    TInt i = 0;
    for( i = offset; i < aSrvURI.Length(); i++ )
		{
		if( aSrvURI[i] == KNSmlDMColon )// ':'
			{
			portStart = i;
			}
		if( aSrvURI[i] == KNSmlDMUriSeparator )// '/'
			{
            separatorPos = i;
			break;
			}
		}
	
    TPtr8 ptrNewSrvURI = aNewURI->Des();
    if( portStart < 0 && separatorPos > 0 && iLeafType != EDMDelete )
        {
        ptrNewSrvURI.Append( aSrvURI.Left( separatorPos ) );//http://123.234.345.456
        ptrNewSrvURI.Append( KNSmlDMColon );
        ptrNewSrvURI.Append( aPort );
        ptrNewSrvURI.Append( aSrvURI.Right( aSrvURI.Length() - separatorPos ) );
        }
    else if ( portStart > 0 && separatorPos > 0 && iLeafType != EDMDelete )
        {
        ptrNewSrvURI.Append( aSrvURI.Left( portStart + 1 ) );//123.234.345.456:
        ptrNewSrvURI.Append( aPort );//123.234.345.456:xx(x)
        ptrNewSrvURI.Append( aSrvURI.Right( aSrvURI.Length() - separatorPos ) );
        }
    else if ( portStart > 0 &&  iLeafType == EDMDelete ) //delete port number from uri
        {
        ptrNewSrvURI.Append( aSrvURI.Left( portStart ) );//123.234.345.456
        ptrNewSrvURI.Append( aSrvURI.Right( aSrvURI.Length() - separatorPos ) );
        }
    else
        {
        ptrNewSrvURI.Append( aSrvURI );
        }
    return;
	}

// ------------------------------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetDMFieldDataL
//  Selects and Sets value to a field based on field id got from URI.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetDMFieldDataL( const TInt aLUID, const TDesC8& aObject, 
                                                const TNSmlDMProfileData aField )
    {
    TInt rValue = KErrGeneral;
    switch ( aField )
		{
		case ( EProfileURL ) :
			{
		    rValue = SetProfileServerURIL( aLUID, aObject );    
			}
		break;
		case ( EServerId ) :
			{
		    rValue = SetProfileServerIdL( aLUID, aObject );
			}
		break;
        case ( ESharedSecret ) :
			{
		    rValue = SetProfileServerPasswordL( aLUID, aObject );
			}
		break;
        case ( EProfileDMServerUsername ) :
			{
		    rValue = SetProfileUserNameL( aLUID, aObject );
			}
		break;
        case ( EProfileDMServerPassword ) :
			{
		    rValue = SetProfilePasswordL( aLUID, aObject );
			}
		break;
        case ( EProfileName ) :
			{
		    rValue = SetProfileDisplayNameL( aLUID, aObject );
			}
		break;
        case ( EProfileIAPId ) :
			{
            rValue = SetConRefL( aLUID, aObject );
		    }
		break;
        case ( EProfileId ) :
			{
		    }
		break;
        case ( EProfilePort ) :
			{
            if(aObject.Size()<=5)
                {
			TInt port = GetIntObject(aObject);
	        if( port > KPortMinSize && port < KPortMaxSize )
		         {
	    			TPortBuffer element;
	    			element.iPortBuf=aObject;
	    			element.iPortBufLUID = aLUID;
	    			iPortBuffer.Append(element);
	    			rValue=KErrNone;
                 }
                else
                {
				if (port <= KPortMinSize )
				   rValue = KErrUnderflow;
                	else
                   rValue=KErrOverflow;
                }
			}
			else
			{
					rValue=KErrOverflow;
			}
			// save information of the port
		    }
		break;
        case ( EProfileMediumType ) :
			{
            if ( iLeafType == EDMDelete )
			    {
                return KErrNone;
			    }
            iObject = GetIntObject( aObject );
            if(iObject<0)
                {
                iObject = EHttp;
                }
            rValue = SetProfileAddrTypeL( aLUID, iObject );
		    }
		break;
        case ( EAuthenticationRequired ) :
        case ( EClientNonce ) :
        case ( EServerNonce ) :
		break;
        
        default:
            rValue = KErrNotSupported;
        }
    return rValue;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::FillNodeInfoL
//  The function fills the node or leaf information.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode, 
                                            TSmlDmAccessTypes aAccTypes, 
                                            MSmlDmDDFObject::TOccurence aOccurrence, 
                                            MSmlDmDDFObject::TScope aScope, 
                                            MSmlDmDDFObject::TDFFormat aFormat, 
                                            const TDesC8& aDescription, 
                                            const TDesC8& aDefaultValue, 
                                            const TDesC8& aMimeType )
    {
    aNode.AddDFTypeMimeTypeL( aMimeType );
    aNode.SetAccessTypesL( aAccTypes );
	aNode.SetDescriptionL( aDescription );
	aNode.SetOccurenceL( aOccurrence );
	aNode.SetDefaultValueL( aDefaultValue );
	aNode.SetScopeL( aScope );
	aNode.SetDFFormatL( aFormat );
    return;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetDMAuthInfoL
//  This function gets authentication data from private API.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::GetDMAuthInfoL( const TInt aLUID, 
                                                const TNSmlDMProfileData aField, 
                                                CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetDMAuthInfoL(): begin" );
   
    CNSmlDMAuthInfo* authInfo = new (ELeave) CNSmlDMAuthInfo;
    CleanupStack::PushL( authInfo );
    
    authInfo->iProfileId = aLUID + KMaxDataSyncID;
    iPrivateApi.OpenL();
    TInt ret = KErrNone;
    
    TRAPD(err,iPrivateApi.GetDMAuthInfoL( *authInfo ));

    if(err==KErrNone)
    	{
	    if ( aField == EServerNonce )
	        {
	        aObject.InsertL( aObject.Size(), *authInfo->iServerNonce );
	        }
	    else if ( aField == EClientNonce )
	        {
	        aObject.InsertL( aObject.Size(), *authInfo->iClientNonce );
	        }
	    else if ( aField == EAuthenticationRequired )
	        {
	        TUint authPref = authInfo->iAuthPref;
	        if( authPref == ENoAuth )
	            {
	            aObject.InsertL( aObject.Size(), KNSmlDMAuthPrefNoAuth ); 
	            }
	        else if( authPref == EBasic )
	            {
	            aObject.InsertL( aObject.Size(), KNSmlDMAuthPrefBasic );
	            }
	        else if( authPref == EMD5 )
	            {
	            aObject.InsertL( aObject.Size(), KNSmlDMAuthPrefMD5 );
	            }
	        else
	            {
	            aObject.InsertL( aObject.Size(), KNullDesC8 );
	            ret = KErrGeneral;
	            }
	        }
    	}
    iPrivateApi.Close();
    CleanupStack::PopAndDestroy(authInfo);
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetDMAuthInfoL(): end" );
    return ret;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::DeleteDMAuthInfoL
//  This function resets the authentication data.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::DeleteDMAuthInfoL( const TInt aLUID, 
                                                const TNSmlDMProfileData aField )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::DeleteDMAuthInfoL(): begin" );
    CNSmlDMAuthInfo* authInfo = new (ELeave) CNSmlDMAuthInfo;;
    CleanupStack::PushL( authInfo );
    authInfo->iProfileId = aLUID;
    iPrivateApi.OpenL();
    iPrivateApi.GetDMAuthInfoL( *authInfo );
    if( aField == EServerNonce )
        {
        authInfo->iServerNonce->Des().Delete( 0, authInfo->iServerNonce->Des().Size() );
        authInfo->iServerNonce->Des().Insert( 0, KNullDesC8 );
        }
    else if( aField == EClientNonce )
        {
        authInfo->iClientNonce->Des().Delete( 0, authInfo->iClientNonce->Des().Size() );
        authInfo->iClientNonce->Des().Insert( 0, KNullDesC8 );
        }
    else if( aField == EAuthenticationRequired )
        {
        authInfo->iAuthPref = 0;
        }
    iPrivateApi.SetDMAuthInfoL( *authInfo );
    iPrivateApi.Close();
    CleanupStack::PopAndDestroy(authInfo);
	_DBG_FILE( "CNSmlDMSettingsAdapter::DeleteDMAuthInfoL(): end" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::SetProfileAddrTypeL
// This function sets the AddrType value.    
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::SetProfileAddrTypeL( const TInt aLUID, 
                                                    const TInt aIntObj )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileAddrTypeL(): begin" );
    TInt retVal = OpenSyncSessionAndProfileL( aLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileAddrTypeL(): end" );
        return retVal;	
		}
    TInt mediumType = ENone;
    GetMediumTypeL( aIntObj, mediumType );
    if( mediumType == ENone )
        {
	    iCurrentProfile = 0;
        iProfile.Close();    
        return KErrGeneral;
        }
    RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
    iProfile.ListConnectionsL( transportIdArray );
    RSyncMLConnection conn;
    CleanupClosePushL( conn );
    conn.OpenL( iProfile, transportIdArray[0] );
    if(mediumType!=transportIdArray[0])
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdSyncMlDmObex  ) )
            {
            CleanupStack::PopAndDestroy( 2 );//transportIdArray, conn
            return EInvalidObject;
            }
        else
            {
            conn.CreateL(iProfile,mediumType);
            }
        }
    HBufC8* sURI = conn.ServerURI().AllocLC();
    conn.SetServerURIL( *sURI );
    conn.UpdateL(); 	                    
    CleanupStack::PopAndDestroy( 3 );//transportIdArray, conn, sURI
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetProfileAddrTypeL(): end" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetProfileConnectiontypeL
//  The function gets the profile medium type.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::GetProfileConnectiontypeL( CBufBase& aObject )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfileConnectiontypeL(): begin" );
	RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
	iProfile.ListConnectionsL( transportIdArray );
	TInt transportItems = transportIdArray.Count();
    if( transportItems == 0 )
		{
		//No matching items
        CleanupStack::PopAndDestroy(); //transportIdArray	
		return KErrGeneral;
		}
	else if( transportItems > 1 )
		{
		//Error case : against specification
        CleanupStack::PopAndDestroy(); //transportIdArray	
		return KErrGeneral;
		}
	TInt mediumType = ENone;
    if ( transportIdArray[0] == KUidNSmlMediumTypeInternet.iUid )
        {
        mediumType = EHttp;
        }
    else if ( transportIdArray[0] == KUidNSmlMediumTypeBluetooth.iUid ||
              transportIdArray[0] == KUidNSmlMediumTypeUSB.iUid ||   
              transportIdArray[0] == KUidNSmlMediumTypeIrDA.iUid  )
        {
        mediumType = EObex;
        }
    aObject.InsertL( aObject.Size(), SetIntObjectLC( mediumType ) );
    CleanupStack::PopAndDestroy( 2 ); //transportIdArray, SetIntObjectLC
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetProfileIAPIdL
//  The function gets the profile IAP Id.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter::GetProfileIAPIdL( TInt& aIAPid )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfileIAPIdL(): begin" );
    TInt retVal = KErrGeneral;
    aIAPid = -1;
    RArray<TSmlTransportId> connIdArray;
    CleanupClosePushL( connIdArray );
    iProfile.ListConnectionsL( connIdArray );
    if( connIdArray.Count() == 1 )
        {
        RSyncMLTransport transport;
        CleanupClosePushL( transport );
	    transport.OpenL( iSyncSession, connIdArray[0] );
	    const CSyncMLTransportPropertiesArray& props = transport.Properties();
	    TInt index = props.Find( KNSmlIAPId );
        if( index > 0 )
            {
            RSyncMLConnection conn;
            CleanupClosePushL( conn );
	        conn.OpenL( iProfile, connIdArray[0] );
            HBufC8* apId = conn.GetPropertyL( KNSmlIAPId ).AllocLC();
            aIAPid = GetIntObject( *apId );
            CleanupStack::PopAndDestroy( 2 ); //conn, apId
            retVal = KErrNone;
            }
        else
            {
            retVal = KErrNotFound;
            }
        CleanupStack::PopAndDestroy( 1 ); //transport
	    iCurrentProfile = 0;
        iProfile.Close();
        }
    else
        {
        retVal = KErrNotFound;
        }
    CleanupStack::PopAndDestroy(); //connIdArray
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfileIAPIdL(): end" );
    return retVal;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::GetProfileServerURLL
//  The function gets the profile Server URI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::GetProfileServerURLL( CBufBase& aURL )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfileServerURLL(): begin" );
    RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
	iProfile.ListConnectionsL( transportIdArray );
    if( transportIdArray.Count() > 0 )
        {
        RSyncMLConnection profConn;
        CleanupClosePushL( profConn );
	    profConn.OpenL( iProfile, transportIdArray[0] );
	    HBufC8* sURI = profConn.ServerURI().AllocL();
        CleanupStack::PushL( sURI );
        TPtr8 uriPtr = sURI->Des();
        if( uriPtr.Length() > 0 )
            {
            aURL.InsertL( aURL.Size(),uriPtr );
            }
        else
            {
            aURL.InsertL( aURL.Size(), KNullDesC8 );
            }
        CleanupStack::PopAndDestroy( 2 );//profConn, sURI
        }
    CleanupStack::PopAndDestroy(); //transportIdArray
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfileServerURLL(): end" );
    return;
	}

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::GetMediumTypeL
//	Checks the requested medium type.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::GetMediumTypeL( const TInt aIntObj, 
                                            TInt& aMediumType )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetMediumTypeL(): begin" );
    if( aIntObj == EHttp || aIntObj == EWsp )
        {
        aMediumType = KUidNSmlMediumTypeInternet.iUid;
        return;
        }
    RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
    iSyncSession.ListTransportsL( transportIdArray );
    
    if ( transportIdArray.Find( KUidNSmlMediumTypeBluetooth.iUid ) != KErrNotFound )
        {
        aMediumType = KUidNSmlMediumTypeBluetooth.iUid;
        }
    else if ( transportIdArray.Find( KUidNSmlMediumTypeUSB.iUid ) != KErrNotFound )
        {
        aMediumType = KUidNSmlMediumTypeUSB.iUid;
        }
    else if ( transportIdArray.Find( KUidNSmlMediumTypeIrDA.iUid ) != KErrNotFound )
        {
        aMediumType = KUidNSmlMediumTypeIrDA.iUid;
        }
    CleanupStack::PopAndDestroy(); //transportIdArray
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetMediumTypeL(): end" );
    return;    
    }

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::GetServerIdL
//	Gets the server id value.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::GetServerIdL( CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetServerIdL(): begin" );
    HBufC8* serverId = iProfile.ServerId().AllocL();
    CleanupStack::PushL( serverId );
    aObject.InsertL( aObject.Size(), *serverId );
    CleanupStack::PopAndDestroy( 1 ); //serverId
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetServerIdL(): end" );
    return;
    }

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::GetProfileNameL
//	Gets the display name value.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::GetProfileNameL( CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfileNameL(): begin" );
    HBufC16* dataBuf = iProfile.DisplayName().AllocL();
    CleanupStack::PushL( dataBuf );
    TPtr16 ptrDisplayName = dataBuf->Des();
    HBufC8* dataBuf8 = HBufC8::NewL( ptrDisplayName.Length() * 2 );
    CleanupStack::PushL( dataBuf8 );

    TPtr8 ptrDisplayName8 = dataBuf8->Des();
    ptrDisplayName8 = ConvertTo8LC( ptrDisplayName );
    aObject.InsertL( aObject.Size(), ptrDisplayName8 );
    CleanupStack::PopAndDestroy( 3 );//dataBuf, ConvertTo8LC
    _DBG_FILE("CNSmlDMSettingsAdapter::GetProfileNameL(): end");
    return;
    }

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::GetProfileUserNameL
//	Gets the profile user name value.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::GetProfileUserNameL( CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfileUserNameL(): begin" );
    HBufC8* dataBuf = iProfile.UserName().AllocL();
    CleanupStack::PushL( dataBuf );
    TPtr8 ptrUserName = dataBuf->Des();
    aObject.InsertL( aObject.Size(), ptrUserName );
    CleanupStack::PopAndDestroy();//dataBuf
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfileUserNameL(): end" );
    return;
    }

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter::GetProfilePortNumberL
//	Gets the port number value.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::GetProfilePortNumberL( CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfilePortNumberL(): begin" );
    TInt bSize = aObject.Size();
    TInt portNum = 0;
    HBufC8* bufStorage = HBufC8::NewLC( bSize );
	TPtr8 bufStoragePtr = bufStorage->Des();
	aObject.Read( 0, bufStoragePtr, aObject.Size() );
	aObject.Reset();
	
    //find out the port number
    ParseUri( *bufStorage, portNum );
	aObject.InsertL( aObject.Size(), SetIntObjectLC( portNum ) );

    CleanupStack::PopAndDestroy( 2 ); //bufStorage, SetIntObjectLC()
    _DBG_FILE( "CNSmlDMSettingsAdapter::GetProfilePortNumberL(): end" );
    return;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter::SetAuthInfoL
//  The function checks if authinfo already exits. If exits command is replace.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::SetAuthInfoL( const TInt aLUID, 
                                            const TNSmlDMProfileData aField, 
                                            const TDesC8& aObject, TInt& aStatus )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetAuthInfoL(): begin" );
    
    CNSmlDMAuthInfo* authInfo = new (ELeave) CNSmlDMAuthInfo();
    CleanupStack::PushL( authInfo );
    authInfo->iProfileId = aLUID;
    
    iPrivateApi.OpenL();
    TRAPD( err, iPrivateApi.GetDMAuthInfoL( *authInfo ) );
    
    if ( err )
        {
        CleanupStack::PopAndDestroy( authInfo );
        iPrivateApi.Close();
        aStatus = err;
        return;
        }
    
    CNSmlDMAuthInfo* newAuthInfo = new (ELeave) CNSmlDMAuthInfo();
    CleanupStack::PushL( newAuthInfo );
    newAuthInfo->iProfileId = aLUID;

    if ( aField == EServerNonce )
        {
        newAuthInfo->iServerNonce = aObject.Alloc();
        newAuthInfo->iClientNonce = authInfo->iClientNonce->Des().Alloc();
        newAuthInfo->iAuthPref = authInfo->iAuthPref;
        
        iPrivateApi.SetDMAuthInfoL( *newAuthInfo );
        }
    else if ( aField == EClientNonce )
        {
        newAuthInfo->iServerNonce = authInfo->iServerNonce->Des().Alloc();
        newAuthInfo->iClientNonce = aObject.Alloc();
        newAuthInfo->iAuthPref = authInfo->iAuthPref;
        
        iPrivateApi.SetDMAuthInfoL( *newAuthInfo );
        }
    else if ( aField == EAuthenticationRequired )
        {
        if ( aObject.Match( KNSmlDMAuthPrefNoAuth ) !=  KErrNotFound )
            {
            newAuthInfo->iAuthPref = ENoAuth;
            }
        else if ( aObject.Match( KNSmlDMAuthPrefBasic ) !=  KErrNotFound )
            {
            newAuthInfo->iAuthPref = EBasic;
            }
        else if ( aObject.Match( KNSmlDMAuthPrefMD5 ) !=  KErrNotFound )
            {
            newAuthInfo->iAuthPref = EMD5;
            }
        newAuthInfo->iServerNonce = authInfo->iServerNonce->Des().Alloc();
        newAuthInfo->iClientNonce = authInfo->iClientNonce->Des().Alloc();
        
        iPrivateApi.SetDMAuthInfoL( *newAuthInfo );
        }
    
    CleanupStack::PopAndDestroy( newAuthInfo );
    CleanupStack::PopAndDestroy( authInfo );
    
    iPrivateApi.Close();
    aStatus = KErrNone;
    _DBG_FILE( "CNSmlDMSettingsAdapter::SetAuthInfoL(): end" );
    return;
    }
//------------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::AddNodeBufferL()
// Buffers Node Object
//------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::AddNodeBufferL( const TDesC8& aURI,
		const TInt aStatusRef)
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::AddNodeBufferL(): begin" );
	TInt index = -1;

	for (TInt i = 0; i<iBuffer->Count (); i++)
		{
		if ( iBuffer->At(i).iMappingName->Compare (aURI)== 0)
			{
			index = i;
			//CallBack for duplicate
			_DBG_FILE( "CNSmlDMSettingsAdapter::AddNodeBufferL(): EAlreadyExists end" );
			iCallBack->SetStatusL ( aStatusRef, CSmlDmAdapter::EAlreadyExists);
			break;
			}
		}

	if ( index<0)
		{
		TNSmlDMBufferElement newNode;
		newNode.iMappingName = aURI.AllocLC ();
		newNode.iLeafBuf = new (ELeave) CArrayFixFlat <TNSmlDMLeafElement> (4);
		newNode.iExecuted = EFalse;
		newNode.iDMBuffStatusref = aStatusRef;
		newNode.iAddr = EFalse;
		newNode.iServerId = EFalse;
		newNode.iUName = EFalse;
		newNode.iServerName = EFalse;

		iBuffer->AppendL (newNode);
		//newNode.iMappingName
		CleanupStack::Pop ();
		}
	_DBG_FILE( "CNSmlDMSettingsAdapter::AddNodeBufferL(): end" );
	}
//------------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::AddLeafBufferL()
// Buffers Leaf Obj & checks for Mandatory fields
//------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::AddLeafBufferL( const TDesC8& aURI,
		const TDesC8& aObject, const TInt aStatusRef)
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::AddLeafBufferL(): begin" );
	TNSmlDMLeafElement newCommand;

	newCommand.iUri = aURI.AllocLC ();
	newCommand.iData = aObject.AllocLC ();
	newCommand.iLuid = HBufC8::NewLC (8); //Allocate Mem. for iLUID
	newCommand.iStatusRef = aStatusRef;
	//Add Leaf to the index updated by UpdateLeafObj
	iBuffer->At(iExecutionIndex).iLeafBuf->AppendL (newCommand);
	// iUri, iData, ILuid
	CleanupStack::Pop (3);
	SetField ( aURI);

	if ( iField->Compare (KNSmlDdfAddr)== 0)
		{
		iBuffer->At(iExecutionIndex).iAddr = ETrue;
		}
	else
		if ( iField->Compare (KNSmlDdfServerId)== 0)
			{
			iBuffer->At(iExecutionIndex).iServerId = ETrue;
			}
		else
			if ( iField->Compare (KNSmlDdfClientUserName)== 0)
				{
				iBuffer->At(iExecutionIndex).iUName = ETrue;
				}
			else
				if ( iField->Compare (KNSmlDdfName)== 0)
					{
					iBuffer->At(iExecutionIndex).iServerName = ETrue;
					}

	//  If Address,ServerId,ServerName,UserName then Add

	if ( iBuffer->At(iExecutionIndex).iAddr && iBuffer->At(iExecutionIndex).iServerId &&iBuffer->At(iExecutionIndex).iUName && iBuffer->At(iExecutionIndex).iServerName)
		{
		ExecuteBufferL ();
		iExecutionIndex = -1;
		}
	_DBG_FILE( "CNSmlDMSettingsAdapter::AddLeafBufferL(): end" );
	return;
	}

//------------------------------------------------------------------------------
// CNSmlDMSettingsAdapter::ExecuteBufferL()
// Adds the Node/Leaf obj to the db
//------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::ExecuteBufferL()
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::ExecuteBufferL(): begin" );
	TBuf8<8> newLUID;
	TInt rValue;
	//Buffer is being Executed
	iBufOn = ETrue;

	// Robustness check
	if ( iExecutionIndex<0)
		{
		return;
		}

	//Execute BufferL is called from CompleteOutstandingCmdsL only when
	//Mandatory fields are not set. Dont update Node/Leaf to the db, return a failure

	if ( !iComplete)
		rValue = CreateNewProfileL (iLUID);
	else
		rValue = CSmlDmAdapter::EError;

	iBuffer->At(iExecutionIndex).iExecuted = ETrue;

	if ( rValue != KErrNone)
		{
		if ( rValue == KErrNoMemory)
			{
			iCallBack->SetStatusL ( iBuffer->At(iExecutionIndex).iDMBuffStatusref, CSmlDmAdapter::ENoMemory);
			}
		else
			if ( rValue == KErrNotSupported)
				{
				iCallBack->SetStatusL ( iBuffer->At(iExecutionIndex).iDMBuffStatusref,
						CSmlDmAdapter::EInvalidObject);
				}
			else
				if ( rValue == KErrInUse)
					{
					iCallBack->SetStatusL ( iBuffer->At(iExecutionIndex).iDMBuffStatusref,
							CSmlDmAdapter::EObjectInUse);
					}
				else
					if ( rValue == KErrDiskFull)
						{
						iCallBack->SetStatusL ( iBuffer->At(iExecutionIndex).iDMBuffStatusref,
								CSmlDmAdapter::EDiskFull);
						}
					else
						{
						iCallBack->SetStatusL ( iBuffer->At(iExecutionIndex).iDMBuffStatusref,
								CSmlDmAdapter::EError);
						}
		// dont return, UpdateLeafObjectL will update the callback status for leaf Nodes 
		}
	else
		{
		if ( iLUID >= KMaxDataSyncID)
			{
			newLUID.Num ( iLUID - KMaxDataSyncID);
			}
		else
			{
			newLUID.Num ( iLUID);
			}

		iCallBack->SetMappingL ( *iBuffer->At(iExecutionIndex).iMappingName, newLUID);

		iCallBack->SetStatusL (iBuffer->At(iExecutionIndex).iDMBuffStatusref, CSmlDmAdapter::EOk);
		}

	for (TInt val= 0; val < iBuffer->At(iExecutionIndex).iLeafBuf->Count ();val++)
		{
		//Update the LUID data in the Leaf structure with the Profile LUID before calling UpdateLeafObj
		TPtr8 tempLUID(iBuffer->At(iExecutionIndex).iLeafBuf->At(val).iLuid->Des ());
		tempLUID.Append (newLUID);
		UpdateLeafObjectL (*iBuffer->At(iExecutionIndex).iLeafBuf->At(val).iUri, *iBuffer->At(iExecutionIndex).iLeafBuf->At(val).iLuid, *iBuffer->At(iExecutionIndex).iLeafBuf->At(val).iData, *iBuffer->At(iExecutionIndex).iMappingName, iBuffer->At(iExecutionIndex).iLeafBuf->At(val).iStatusRef);
		}

	// Delete the dynamically allocated buffers (Node/Leaf obj)

	ClearBuffer ( iBuffer->At(iExecutionIndex).iLeafBuf);

	delete iBuffer->At(iExecutionIndex).iLeafBuf;

	if ( iBuffer->At(iExecutionIndex).iMappingName)
		{
		delete iBuffer->At(iExecutionIndex).iMappingName;
		iBuffer->At(iExecutionIndex).iMappingName = NULL;
		}

	iBuffer->Delete (iExecutionIndex);
	iBuffer->Compress ();

	iBufOn = EFalse;
	_DBG_FILE( "CNSmlDMSettingsAdapter::ExecuteBufferL(): end" );
	}

//------------------------------------------------------------------------------
// TPtrC8 CNSmlDMSettingsAdapter::ParentURI(const TDesC8& aURI)
// returns parent uri, i.e. removes last uri segment
// Searches till the third "/" is reached
//------------------------------------------------------------------------------
TPtrC8 CNSmlDMSettingsAdapter::ParentURI(const TDesC8& aURI)
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::ParentURI(): begin" );
	TInt count;
	TInt flag = 0;
	for (count=0; count<=aURI.Length ()-1;count++)
		{

		if ( aURI[count]=='/')
			{
			flag++;
			if ( flag == KDynNodePosn) //KDynNodePosn =3 --> SyncML/DMAcc/DMIDxx
				break;
			}
		}

	_DBG_FILE( "CNSmlDMSettingsAdapter::ParentURI(): end" );
	return aURI.Left (count);
	}

//-----------------------------------------------------------------------------------------    
// Clears the elements of the passed in LeafElement Structure
//----------------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter::ClearBuffer(
		CArrayFixFlat<TNSmlDMLeafElement>* aBuffer)
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter::ClearBuffer(): begin" );
	for (TInt i = 0; i < aBuffer->Count (); i++)
		{
		delete aBuffer->At(i).iUri;
		aBuffer->At(i).iUri = NULL;

		delete aBuffer->At(i).iData;
		aBuffer->At(i).iData = NULL;

		delete aBuffer->At(i).iLuid;
		aBuffer->At(i).iLuid = NULL;
		}

	aBuffer->Reset ();
	_DBG_FILE( "CNSmlDMSettingsAdapter::ClearBuffer(): end" );
	}
// ========================== OTHER EXPORTED FUNCTIONS =========================

#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)	{{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
		IMPLEMENTATION_PROXY_ENTRY( KNSmlDMSettingsAdapterImplUid, CNSmlDMSettingsAdapter::NewL )
    };


// -----------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	_DBG_FILE( "ImplementationGroupProxy() for CNSmlDMSettingsAdapter: begin" );

    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	_DBG_FILE( "ImplementationGroupProxy() for CNSmlDMSettingsAdapter: end" );
    return ImplementationTable;
	}

// End of File

