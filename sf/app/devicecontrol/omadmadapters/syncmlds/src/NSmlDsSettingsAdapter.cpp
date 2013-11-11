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
* Description:    DS-settings adapter
*
*/




// INCLUDE FILES
#include "NSmlDSSettingsAdapter.h"
#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldmconstants.h"
#include "nsmldmimpluids.h"
#include "nsmldmiapmatcher.h"

#include <implementationproxy.h>
#include <badesca.h>
#include <utf.h>
#include <SyncMLTransportProperties.h>

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::NewL
//  Two-phased constructor.
// -----------------------------------------------------------------------------
CNSmlDSSettingsAdapter* CNSmlDSSettingsAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::NewL(): begin");

	CNSmlDSSettingsAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();

	_DBG_FILE("CNSmlDSSettingsAdapter::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::NewLC
// -----------------------------------------------------------------------------
CNSmlDSSettingsAdapter* CNSmlDSSettingsAdapter::NewLC(MSmlDmCallback* aDmCallback )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::NewLC(): begin");
	CNSmlDSSettingsAdapter* self = new ( ELeave ) CNSmlDSSettingsAdapter( aDmCallback );
	CleanupStack::PushL( self );
	self->iCallBack = aDmCallback;

	self->ConstructL();
	self->iLeafType = EDSUnset;
    
	_DBG_FILE("CNSmlDSSettingsAdapter::NewLC(): end");
	return self;
	}

//------------------------------------------------------------------------------
// CNSmlDSSettingsAdapter::CNSmlDSSettingsAdapter
//------------------------------------------------------------------------------
CNSmlDSSettingsAdapter::CNSmlDSSettingsAdapter(TAny* aEcomArguments) : CSmlDmAdapter(aEcomArguments)
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::CNSmlDSSettingsAdapter(): begin");
	_DBG_FILE("CNSmlDSSettingsAdapter::CNSmlDSSettingsAdapter(): end");
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::~CNSmlDSSettingsAdapter()
//  Class destructor.
//------------------------------------------------------------------------------
CNSmlDSSettingsAdapter::~CNSmlDSSettingsAdapter()
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::~CNSmlDSSettingsAdapter(): begin");
    delete iPrevURI;
    delete iField;
    
	// Command buffer cleaning
	if ( iBuffer )
		{
		
		for( TInt i=0; i<iBuffer->Count(); i++ )
			{
			delete iBuffer->At(i).iMappingName;
			delete iBuffer->At(i).iName;

			for( TInt j=0; j<iBuffer->At(i).iNodeBuf->Count(); j++ )
				{
				delete iBuffer->At(i).iNodeBuf->At(j).iUri;
				if( iBuffer->At(i).iNodeBuf->At(j).iData )
					{
					delete iBuffer->At(i).iNodeBuf->At(j).iData;
					}
				}
			iBuffer->At(i).iNodeBuf->Reset();
			delete iBuffer->At(i).iNodeBuf;
			}

		iBuffer->Reset();
		delete iBuffer;
		}
    
    iDSProfile.Close();
    iSyncSession.Close();
    iSyncSessionOpen = EFalse;
        
    delete iPortNbr;
    delete iPortBuf;
    delete iRdbDataStore;
    
    iBufferExecution = EFalse;
    
    iCallBack = 0;

	_DBG_FILE("CNSmlDSSettingsAdapter::~CNSmlDSSettingsAdapter(): end");
	}

//------------------------------------------------------------------------------
//		CNSmlDSSettingsAdapter::ConstructL
//		Second phase constructor.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::ConstructL()
	{
	iField = HBufC8::NewL( KNSmlMaxURLLength );
	iPrevURI = HBufC8::NewL( KNSmlMaxURLLength );
    iRdbDataStore = HBufC16::NewL( KNSmlMaxRemoteNameLength );
//
//	Command buffering used for AddNode + AddLeaf
//
	iBuffer = new ( ELeave ) CArrayFixFlat <TNSmlDSBufferElement> ( KNSmlDSGranularity );

    iSyncSessionOpen = EFalse;

    iPortNbr = 0;
    iBufferExecution = EFalse;
	}
	
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::DDFVersionL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::DDFVersionL(CBufBase& aVersion)
	{
	aVersion.InsertL( 0, KVersion );
    }

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::DDFStructureL
//  Fills the DDF structure of the adapter.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::DDFStructureL(): begin");

	TSmlDmAccessTypes *aclTypes = new ( ELeave ) TSmlDmAccessTypes();
	CleanupStack::PushL( aclTypes );

    //  Set Get as acceptable operations
	aclTypes->SetGet();

    MSmlDmDDFObject* ddfRoot = &aDDF;
    MSmlDmDDFObject& ddf = ddfRoot->AddChildObjectL( KNSmlDdfRoot );
    FillNodeInfoL( ddf, *aclTypes, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode, 
                    KNSmlDdfRootDescription, KNullDesC8, KMimeType );
    
    ddf.SetDFTitleL( KDSSettingsTitle );


	MSmlDmDDFObject& nDSAcc = ddf.AddChildObjectL( KNSmlDefDSAcc );
	nDSAcc.SetOccurenceL( MSmlDmDDFObject::EOne );
	nDSAcc.SetDefaultValueL( KNullDesC8 );
	nDSAcc.SetScopeL( MSmlDmDDFObject::EPermanent );
	nDSAcc.SetAccessTypesL( *aclTypes );
	nDSAcc.SetDFFormatL( MSmlDmDDFObject::ENode );
//
//  Set rest acceptable operations for data itself
//
    aclTypes->SetAdd();
	aclTypes->SetReplace();
	aclTypes->SetDelete();

	TSmlDmAccessTypes aclTypesLimit;
	aclTypesLimit.SetGet();
	aclTypesLimit.SetAdd();

	TSmlDmAccessTypes aclTypesGet;
	aclTypesGet.SetGet();

	TSmlDmAccessTypes aclTypesNoGet;
	aclTypesNoGet.SetReplace();
	aclTypesNoGet.SetAdd();
	aclTypesNoGet.SetDelete();

	TSmlDmAccessTypes aclTypesNoDelete;
	aclTypesNoDelete.SetGet();
	aclTypesNoDelete.SetAdd();
	aclTypesNoDelete.SetReplace();

	MSmlDmDDFObject& nProDDF = nDSAcc.AddChildObjectGroupL();
    FillNodeInfoL( nProDDF, *aclTypes, MSmlDmDDFObject::EZeroOrMore, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNullDesC8, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nAddrDDF = nProDDF.AddChildObjectL(KNSmlDdfAddr);	// Addr
	FillNodeInfoL( nAddrDDF, *aclTypes, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAddrDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nAddrTypeDDF = nProDDF.AddChildObjectL(KNSmlDdfAddrType);	// AddrType
    FillNodeInfoL( nAddrTypeDDF, *aclTypes, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAddrTypeDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nPortNbrDDF = nProDDF.AddChildObjectL(KNSmlDdfPortNbr);// PortNbr
	FillNodeInfoL( nPortNbrDDF, *aclTypes, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EInt, 
                    KNSmlDdfPortNbrDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nNameDDF = nProDDF.AddChildObjectL(KNSmlDdfName);	// Name
	FillNodeInfoL( nNameDDF, *aclTypes, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfNameDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nDBrootDDF = nProDDF.AddChildObjectL(KNSmlDdfDB);	// DB
    FillNodeInfoL( nDBrootDDF, *aclTypes, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNSmlDdfDBDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nDBDDF = nDBrootDDF.AddChildObjectGroupL(); // DB/<X>
    FillNodeInfoL( nDBDDF, *aclTypes, MSmlDmDDFObject::EZeroOrMore, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNullDesC8, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nCTTypeDDF = nDBDDF.AddChildObjectL(KNSmlDdfCTType);	// CTType
	FillNodeInfoL( nCTTypeDDF, aclTypesLimit, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfCTTypeDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nRDBURIDDF = nDBDDF.AddChildObjectL(KNSmlDdfRDBURI);	// RDBURI
	FillNodeInfoL( nRDBURIDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfRDBURIDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nLDBURIDDF = nDBDDF.AddChildObjectL(KNSmlDdfLDBURI);	// LDBURI
    FillNodeInfoL( nLDBURIDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfLDBURIDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nClientUserNameDDF = nProDDF.AddChildObjectL(KNSmlDdfClientUserName);
    FillNodeInfoL( nClientUserNameDDF, *aclTypes, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfClientUserNameDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nServerIdDDF = nProDDF.AddChildObjectL(KNSmlDdfServerId);
    FillNodeInfoL( nServerIdDDF, *aclTypes, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfServerIdDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nClientPWDDF = nProDDF.AddChildObjectL(KNSmlDdfClientPW);// ClientPW
	FillNodeInfoL( nClientPWDDF, aclTypesNoGet, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfClientPWDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nToNAPIDDDF = nProDDF.AddChildObjectL(KNSmlDdfToNAPID);// ToNAPID
    FillNodeInfoL( nToNAPIDDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfToNAPIDDescription, KNullDesC8, KMimeType );

	CleanupStack::PopAndDestroy(); //aclTypes
	_DBG_FILE("CNSmlDSSettingsAdapter::DDFStructureL(): end");
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::UpdateLeafObjectL
//  The function updates the leaf object data.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                                const TDesC8& aParentLUID, 
                                                const TDesC8& aObject, 
                                                const TDesC8& /*aType*/, 
                                                const TInt aStatusRef )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::UpdateLeafObjectL(): begin");
//
//	Check which field going to be handled
//
	// Static node feature start
		TInt profileLUID = -1;
		if(aParentLUID.Length() == 0 )
		{
			iLUID = -1;
			if( IsDSAccUriFormatMatchPredefined(aURI) )
    	{
    		iLUID = ConstructTreeL(aURI);
    		profileLUID = GetProfileIdFromURIL( aURI ); 
    	}    	
    	if ((iLUID == 0 ) && (aURI.Match(_L8("SyncML/DSAcc/DSId*/DB/CTId*"))  == KErrNone))
			{
				_DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): ENotFound end");
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
				return;
    	}
    }
		// Static node feature end
	SetField( aURI );
    TInt fieldID = GetDSField();
	TInt fieldType = GetDSFieldTypeL();
	if ( fieldType == EWrong || fieldID < 0 )
		{
		_DBG_FILE("CNSmlDSSettingsAdapter::UpdateLeafObjectL(): EError end");
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}
	if(profileLUID == -1)
    	profileLUID = GetIntObject( aParentLUID );
	
	if( profileLUID == -1 && fieldType != EParam )
		{
		_DBG_FILE("CNSmlDSSettingsAdapter::UpdateLeafObjectL( ): ENotFound end");
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}	
    
    if ( fieldType == EParam )
		{
		if( GetBufferIndex( aURI ) >= 0 && iBufferExecution == EFalse )
			{
			AddLeafBufferL( aURI, aParentLUID, aObject, aStatusRef );
            return;
			}
		profileLUID = GetProfileIdFromURIL( aURI );
        if( FindProfileIdL( profileLUID ) == EFalse )
            {
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
            }
        if( iField->Compare( KNSmlDdfCTType ) == 0 )
            {
            if( MimeTypeAllreadyExitsL( profileLUID, aObject ) )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
                return;
                }
            if( aObject.Length() == 0 )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
                return;
                }
            }
        }

	if( profileLUID != iParentLUID && fieldType != EParam )
		{											    
		if( FindProfileIdL( profileLUID ) )
			{
			iParentLUID = profileLUID;
			}
		else
			{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		}
	if ( fieldType == EStr || fieldType == EInt )
		{
//
//		Check if Object length id valid
//
		if( NotValidStrLenght( fieldID, aObject ) )
			{
			_DBG_FILE("CNSmlDSSettingsAdapter::AddLeafObjectL(): KErr length end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ETooLargeObject );
			return;
			}
		}

    TInt ret = 0;
	if ( fieldType == EStr || fieldType == EInt )
		{
		if( fieldID == EProfileURL )
			{
			TInt num=0;
            ret = ParseUri( aObject, num );
            if( ret == KErrNone )
				{
			    ret = SetProfileServerURIL( iParentLUID, aObject );
                if( ret != 0 )
					{
					_DBG_FILE("CNSmlDSSettingsAdapter::AddLeafObjectL(): URI set failed");
					iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
					return;
					}
				}
			else //if port number not included in URL
				{
                ret = SetProfileServerURIL( iParentLUID, aObject );
                if( ret != 0 )
					{
					_DBG_FILE("CNSmlDSSettingsAdapter::AddLeafObjectL(): URI set failed");
					iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
					return;
					}
				}
		    		    
			}
		else if( fieldID == EProfileMediumType )//AddrType
			{
			if ( iLeafType == EDSDelete )
			    {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
	            return;            
			    }
            iObject = GetIntObject( aObject );
            ret = SetProfileAddrTypeL( iParentLUID, iObject );
			}
        else
			{
			ret = SetProfileStrValueL( iParentLUID, aObject, fieldID );
			}
		}
	else if ( fieldType == EParam )
		{
        TInt sRet = KErrGeneral;
        if( iField->Compare( KNSmlDdfCTType ) == 0 ) //Task Mime Type
			{
//			DBG_ARGS8(_S8("NEW: Object <%S>"), &aObject);
			TInt dataProviderId(-1);
			if(aParentLUID.Length() == 0 )
			{
				dataProviderId = iLUID ;
			}
			else
			{
				dataProviderId = GetIntObject( aParentLUID );
			}
            if( FindTaskIDL( profileLUID, dataProviderId ) == EFalse )
                {
                sRet = AddNewDataProviderL( profileLUID, aObject, dataProviderId );
                }
            else
                {
                sRet = KErrAlreadyExists;
                }
            if( sRet == KErrDiskFull )
			    {
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EDiskFull );
				}
			else if( sRet == KErrNotFound )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
                }
            else if( sRet == KErrAlreadyExists )
                {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
                }
            else if( sRet != KErrNone )
			    {
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
				}
            else
                {
                TPtrC8 addDBURI = RemoveLastURISeg( aURI ); 
				TBuf8<16> dataProvider;
				dataProvider.Num( dataProviderId );
		    	iCallBack->SetMappingL( addDBURI, dataProvider );
				SetBufferLUID( aURI,dataProviderId );
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk ); 
				
				
                }                        
            return;
            }
        //
        // Only database values handled
        //
/*		if( iField->Compare( KNSmlDdfLDBURI ) == 0 )
		    {
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
		    }
*/
		if( !(iField->Compare( KNSmlDdfLDBURI ) == 0) &&
		    !(iField->Compare( KNSmlDdfRDBURI ) == 0) )	
		    {
			_DBG_FILE("CNSmlDSSettingsAdapter::AddLeafObjectL(): Param header end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
			return;
			}


        TInt dataProviderId(-1);
		if(aParentLUID.Length() == 0 )
		{
			dataProviderId = iLUID ;
		}
		else
		{
        	dataProviderId = GetIntObject( aParentLUID );
		}
        if( FindTaskIDL( profileLUID, dataProviderId ) )
            {
            ret = UpdateDataSourceL( profileLUID, dataProviderId, aObject, *iField);
            if( ret != KErrNone )
                {
                ret = KErrGeneral;
                }
            TPtrC8 dbURI = RemoveLastURISeg( aURI ); 
		    TBuf8<16> dataProvider;
		    dataProvider.Num( dataProviderId );
		    iCallBack->SetMappingL( dbURI, dataProvider );
			}
		else
		    {
		    ret = KErrNone;
		    //ret = KErrNotFound;
			}
        }
	
    if ( ret != KErrNone )
		{
		if( ret == KErrDiskFull)
			{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EDiskFull );
			}
        else if( ret == KErrNotFound )
            {
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
            }
        else if( ret == KErrArgument )
            {
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
            }
        else if( ret == KErrOverflow)
            {
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ETooLargeObject  );
            }
		else
			{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
			}
		}
	else if ( fieldID == EProfilePort )
	    {
        // in case port is updated, save status ref to be used in 
        // CompleteOutstandingCmdsL (i.e. status not set yet)
        iPortBufStatusRef = aStatusRef;
	    }
	else
	    {
    	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
	    }
	
	_DBG_FILE("CNSmlDSSettingsAdapter::AddLeafObjectL(): end");
	return;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::DeleteObjectL
//  The function Deletes leaf object data or node data content.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::DeleteObjectL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            const TInt aStatusRef )
	{
    _DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): begin");
	iLUID = IntLUID( aLUID );
	if( aLUID.Length() == 0 )
		{
		_DBG_FILE("CNSmlDSSettingsAdapter DELETE::DeleteObjectL( ): ENotFound end");
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}

//
//	Check which field going to be handled
//
	SetField( aURI );
    if ( iField->Compare( KNSmlDdfCTType ) == 0 
        || iField->Compare( KNSmlDdfRDBURI ) == 0 
        || iField->Compare( KNSmlDdfLDBURI ) == 0 
        || iField->Compare( KNSmlDdfToNAPID ) == 0 )
		{
		_DBG_FILE("CNSmlDSSettingsAdapter DELETE::DeleteObjectL( ): EError end");
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}
	
	TInt last = aURI.LocateReverse( KNSmlDMUriSeparator );
	TInt pos  = aURI.Find( KNSmlDefDSAcc );

	if(last - 5 == pos )
		{
		iLUID = IntLUID( aLUID );
		if( !FindProfileIdL( iLUID ) )
			{
			_DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): notfound end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		else
			{
			TInt sRet = DeleteProfileL( iLUID );
			if( sRet!=KErrNone )
				{
				_DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): ENotCommitted end");
				if( sRet == KErrNotFound )
				    {
                    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
				    }
				else if( sRet == KErrInUse )
				    {
                    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EObjectInUse );
				    }
				else
                    {
                    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
                    }
				return;
				}
			else
				{
				iPrevURI->Des().Format( KNullDesC8 );
				iPrevLUID = 0;
				_DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): EOk end");
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
				return;
				}
			}
		}
	else if( aURI.Match( KDBUri1 ) != KErrNotFound )
		{
		iLUID = GetProfileIdFromURIL( aURI ); // Get profile Id for DB-level
		if (iLUID < 0)
			{
			_DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): EError end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
			return;
			}

		if( !FindProfileIdL( iLUID ) )
			{
			_DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): ENotFound end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		TInt adapterId = GetIntObject( aLUID );//Get task ID
        TInt retval =  DeleteTaskByIDL( iLUID, adapterId );
        if( retval != KErrNone )
            {
            if( retval == KErrNotFound )
                {
                _DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): ENotFound end");
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
                }
            else
				{
				_DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): EError end");
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
               	}
            }
		else
            {
            _DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): EOk end");
		    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
            }
        return;
		}

	iLUID = IntLUID( aLUID );

	if ( iLUID != iParentLUID )
		{
		if( !FindProfileIdL( iLUID ) )
			{
			_DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		iParentLUID = iLUID;
		}

	HBufC8* lObject = HBufC8::NewLC( 15 );
    lObject->Des().Format( KNullDesC8 );
	
	iLeafType = EDSDelete;
	UpdateLeafObjectL( aURI, aLUID, *lObject, KMimeType, aStatusRef );
	iLeafType = EDSUnset;

	_DBG_FILE("CNSmlDSSettingsAdapter::DeleteObjectL( ): end");
	CleanupStack::PopAndDestroy(); //lObject 
	return;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::FetchLeafObjectL
//  The function fetches the leaf object data.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                                const TDesC8& aLUID, 
                                                const TDesC8& aType, 
                                                const TInt aResultsRef, 
                                                const TInt aStatusRef )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::FetchLeafObjectL(): begin");
	DBG_ARGS(_S16("DS:Fetch aURI    - %S - %S"), &aURI, &aLUID);

    CBufBase *lObject = CBufFlat::NewL( 128 );
	CleanupStack::PushL( lObject );
	lObject->Reset();
    if( FetchObjectL( aURI, aLUID, lObject, aStatusRef ) != KErrNone )
        {
        CleanupStack::PopAndDestroy(); //lObject
        _DBG_FILE("CNSmlDSSettingsAdapter::FetchLeafObjectL() : Error end");
        return;
        };
    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
    iCallBack->SetResultsL( aResultsRef, *lObject, aType );
	CleanupStack::PopAndDestroy(); //object    
    _DBG_FILE("CNSmlDSSettingsAdapter::FetchLeafObjectL() : end");
    return;
	}

//------------------------------------------------------------------------------
//  CSmlDSSettingsAdapter::ChildURIListL
//  Function returns the list of profiles or leafs of the profile 
//  acording to aURI.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::ChildURIListL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                                            const TInt aResultsRef, const TInt aStatusRef )
	{
    _DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): begin");
//	DBG_ARGS(_S16("ChildURIListL URI - <%S>"), &aURI);

	CBufBase *currentURISegmentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentURISegmentList );

	if( aURI.Match( KDSAccMatch ) != KErrNotFound &&
	   aURI.Match( KDSAccMatch2 ) == KErrNotFound )
		{
		//
		//	Check if Profile exists
		//
		TInt checkLUID = GetIntObject( aLUID ); 
		if( FindProfileIdL( checkLUID ) )
		{
			currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDSAcc );
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
		   	iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        }
        else
        {
			checkLUID = -1;
			if( IsDSAccUriFormatMatchPredefined(aURI) )
		    {
		    	checkLUID = ConstructTreeL(aURI); 
    		}    	
    		if (checkLUID == -1 ) 
		    {
		    	_DBG_FILE( "CNSmlDMSettingsAdapter::childURI(): ENotFound end" );
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
				CleanupStack::PopAndDestroy(  ); // currentURISegmentList
        		_DBG_FILE( "CNSmlDMSettingsAdapter::ChildURIListL(): end" );
			    return;
		   	}
		    else
		    {
       		    currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDSAcc );
 				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
			    iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
		    	}
    	
			// Static node feature start end

			_DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): end");
			CleanupStack::PopAndDestroy(); //currentURISegmentList 
			return;
		}
		}		
	else if( aURI.Match( KDSDBMatch ) != KErrNotFound )
		{
		//
		//	Check if ContenType exists (profileId from adapters)
		// 
		TInt profileLUID = GetProfileIdFromURIL( aURI ); // ProfileId for DB-level
        if( OpenSyncSessionAndDSProfileL( profileLUID, ESmlOpenRead ) == KErrNone )
            {
            RArray<TSmlTaskId> taskIdArray;
            CleanupClosePushL( taskIdArray );
            iDSProfile.ListTasksL( taskIdArray );
            if( taskIdArray.Count() == 0 )
                {
                currentURISegmentList->InsertL( currentURISegmentList->Size(), KNullDesC8 );
                }
            else
                {
                currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDSAcc2 );
                }
            CloseDSProfile();
            CleanupStack::PopAndDestroy();//taskIdArray
            }
        else
            { 
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		    _DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): CT-NOTFOUND end");
			CleanupStack::PopAndDestroy();//currentURISegmentList 
			return;
            }
            
        iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
		iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
		_DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): end");
		CleanupStack::PopAndDestroy(); //currentURISegmentList
		return;
		}
//
//	Handle ContentType (DB) request -> TASKS of the profile
//
	if( aURI.Match( KDSDBMatch2 ) != KErrNotFound )
		{
		TInt lProfileId = GetIntObject( aLUID ); 
		
		if( ! FindProfileIdL( lProfileId ) )
			{
			_DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): NOTFOUND CTTYPE end");
			CleanupStack::PopAndDestroy();// currentURISegmentList
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		
        TInt ret = CreateURITaskSegmentListL( aURI, lProfileId, currentURISegmentList, 
                                                aPreviousURISegmentList );
		if( ret != KErrNone && ret != KErrNotFound )
			{
			_DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): ContentType error end");
			CleanupStack::PopAndDestroy();// currentURISegmentList
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		_DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): CTTYPE-OK end");
		iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8);
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );	
		CleanupStack::PopAndDestroy();// currentURISegmentList
		return;
		}

	
    TInt ret = CreateURIProfileSegmentListL( currentURISegmentList, aPreviousURISegmentList );

    if( ret != KErrNone )
	    {
		_DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): Profile not found end");
		CleanupStack::PopAndDestroy();// currentURISegmentList
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}
   
	iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
	CleanupStack::PopAndDestroy(); // currentURISegmentList
	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
	_DBG_FILE("CNSmlDSSettingsAdapter::ChildURIListL(): end");
	return;
    }

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::AddNodeObjectL
//  The function adds the new node object.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::AddNodeObjectL( const TDesC8& aURI, 
                                                const TDesC8& aParentLUID, 
                                                const TInt aStatusRef )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::AddNodeObjectL(): begin");
//
//	Check if ContentType-add and insert to Buffer
//
    iParentLUID = GetIntObject( aParentLUID );
    
	if( aURI.Match( KDSDBAddMatch )!= KErrNotFound &&
		aURI.Match( KDSDBAddMatch2 )== KErrNotFound )
		{
		if( aParentLUID.Length() > 0 && iParentLUID >= 0 && iBufferExecution == EFalse )
			{
            AddNodeBufferL( aURI, aParentLUID, aStatusRef );
            }
		else if( iParentLUID < 0 )
			{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
			_DBG_FILE("CNSmlDSSettingsAdapter::AddNodeObjectL(): end");
			}
		else
			{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
			_DBG_FILE("CNSmlDSSettingsAdapter::AddNodeObjectL(): end");
			}
		return;
		}
    
	if( aURI.Match(KDSDBAddMatch3) != KErrNotFound )
		{
		if( FindProfileIdL( iParentLUID ) ) // Profile on database
			{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
			_DBG_FILE("CNSmlDSSettingsAdapter::AddNodeObjectL(): end");
			}
		else
			{
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
			_DBG_FILE("CNSmlDSSettingsAdapter::AddNodeObjectL(): end");
			}
		return;
		}
   
	if( FindProfileIdL( iParentLUID ) )
		{
		iCallBack->SetStatusL(aStatusRef,CSmlDmAdapter::EAlreadyExists);
		_DBG_FILE("CNSmlDSSettingsAdapter::AddNodeObjectL(): EAlreadyExists end");
		return;
		}
	else
		{
		if( aParentLUID.Length() > 0 )
		{		
			TInt ret = iCallBack->RemoveMappingL(KNSmlDSSettingsAdapterImplUid,
						GetDynamicDSNodeUri( aURI ), ETrue );
			if(ret)
				{
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
				return;
				}
		}		
        //
        //	Create new profile
        //
   	    TInt newPID = -1;
        TInt sRet = CreateNewProfileL( newPID );
        if( sRet != KErrNone || newPID < 0 )
		    {	
		    if( sRet == KErrDiskFull )
			    {
			    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EDiskFull );
			    }
		    else if( sRet == KErrInUse )
			    {
			    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EObjectInUse );
			    }
		    else
			    {
			    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
			    }
		    return;
		    }
		//
        //	Get new ProfileId from database
        //
	    iLUID = newPID;
        //
   	    TBuf8<8> newLUID;
	    newLUID.Num( iLUID );
	    iCallBack->SetMappingL( aURI, newLUID );
        newLUID.Delete( 0, newLUID.Size() );
        }
	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
	_DBG_FILE("CNSmlDSSettingsAdapter::AddNodeObjectL(): end");
    return;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::UpdateLeafObjectL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                                const TDesC8& /*aLUID*/, 
                                                RWriteStream*& /*aStream*/, 
                                                const TDesC8& /*aType*/, 
                                                const TInt /*aStatusRef*/ )
	{
	//Not supported since streaming is not supported by the adapter.
	}
	
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::FetchLeafObjectSizeL
//  The function fetches the size of the leaf object data.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                                    const TDesC8& aLUID, 
                                                    const TDesC8& aType, 
                                                    const TInt aResultsRef, 
                                                    const TInt aStatusRef )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::FetchLeafObjectSizeL() : begin");
    CBufBase *lObject = CBufFlat::NewL( 128 );
	CleanupStack::PushL( lObject );
	lObject->Reset();
    if( FetchObjectL( aURI, aLUID, lObject, aStatusRef ) != KErrNone )
        {
        CleanupStack::PopAndDestroy(); //lObject
        _DBG_FILE("CNSmlDSSettingsAdapter::FetchLeafObjectSizeL() : Error end");
        return;
        };
    TInt objSizeInBytes = lObject->Size();
	TBuf8<16> stringObjSizeInBytes;
	stringObjSizeInBytes.Num( objSizeInBytes );
	lObject->Reset();
    lObject->InsertL( 0, stringObjSizeInBytes );
	
	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
	iCallBack->SetResultsL( aResultsRef, *lObject, aType );
	CleanupStack::PopAndDestroy(); //object
    _DBG_FILE("CNSmlDSSettingsAdapter::FetchLeafObjectSizeL() : end");
    return;
    }

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::ExecuteCommandL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                                const TDesC8& /*aLUID*/, 
                                                const TDesC8& /*aArgument*/, 
                                                const TDesC8& /*aType*/, 
                                                const TInt /*aStatusRef*/ )
	{
	// Not supported	
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::ExecuteCommandL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                                const TDesC8& /*aLUID*/, 
                                                RWriteStream*& /*aStream*/, 
                                                const TDesC8& /*aType*/, 
                                                const TInt /*aStatusRef*/ )
	{
	// Not supported by the adapter.
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::CopyCommandL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                                            const TDesC8& /*aTargetLUID*/, 
                                            const TDesC8& /*aSourceURI*/, 
                                            const TDesC8& /*aSourceLUID*/, 
                                            const TDesC8& /*aType*/, 
                                            TInt /*aStatusRef*/ )
	{
	// Not supported by the adapter.
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::StartAtomicL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::StartAtomicL()
	{
	// Atomic commands not supported by the adapter.
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::CommitAtomicL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::CommitAtomicL()
	{
	// Atomic commands not supported by the adapter.
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::RollbackAtomicL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::RollbackAtomicL()
	{
	// Atomic commands not supported by the adapter.
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::StreamingSupport()
//  Indicates if streaming is supported.
//------------------------------------------------------------------------------
TBool CNSmlDSSettingsAdapter::StreamingSupport( TInt& /*aItemSize*/ )
	{
	return EFalse;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::StreamCommittedL
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::StreamCommittedL()
	{
	//  Not supported since streaming not supported by the adapter.
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::CompleteOutstandingCmdsL
//  The function completes all buffered command from the buffer.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::CompleteOutstandingCmdsL()
	{
    
//
//	Handle unhandled nodes if any
//
    TInt count = iBuffer->Count();
	for( TInt h = 0; h < count ; h++ )
		{
		TBuf8<150> commandURI;
		commandURI.Append( iBuffer->At(0).iMappingName->Des() );
		commandURI.Append( KDummyTxt );
		TRAP_IGNORE( ExecuteBufferL( commandURI ) );
		}
	iBuffer->Reset();
	CloseDSProfile();
	
	// if port was updated, set it
    if ( iPortBuf )
        {
        TInt err = SetProfileConnectionPortNrL( iPortBufLUID, *iPortBuf );
        
        // set status
        if ( err != KErrNone )
            {
            iCallBack->SetStatusL( iPortBufStatusRef, CSmlDmAdapter::EError );
            }
        else
            {
            iCallBack->SetStatusL( iPortBufStatusRef, CSmlDmAdapter::EOk );
            }

        delete iPortBuf;
        iPortBuf = NULL;
        }
	}

//------------------------------------------------------------------------------
//  TBool CNSmlDSSettingsAdapter::AcceptDSField
//  The function checks if leaf to be handled is valid for the adapter.
//------------------------------------------------------------------------------
TBool CNSmlDSSettingsAdapter::AcceptDSField()
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::AcceptDSField(): begin");

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
	else if ( iField->Compare( KNSmlDdfClientUserName ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfName ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfCTType ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfRDBURI ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfLDBURI ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfClientPW ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfToNAPID ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare( KNSmlDdfHidden ) == 0 )
		{
		return EFalse;
		}

	_DBG_FILE("CNSmlDSSettingsAdapter::AcceptDSField(): end");
	return EFalse;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetDSField
//  The function checks what leaf to handled and returns enum value for the leaf.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetDSField() const
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::GetDSField(): begin");

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
	else if ( iField->Compare( KNSmlDdfClientUserName ) == 0 )
		{
		return EProfileSyncServerUsername;
		}
	else if ( iField->Compare( KNSmlDdfServerId ) == 0 )
		{
		return EProfileServerId;
		}
	else if ( iField->Compare( KNSmlDdfClientPW ) == 0 )
		{
		return EProfileSyncServerPassword;
		}
	else if ( iField->Compare( KNSmlDdfName ) == 0 )
		{
		return EProfileName;
		}
	else if ( iField->Compare( KNSmlDdfToNAPID ) == 0 )
		{
		return EProfileIAPId;
		}
	else if ( iField->Compare( KNSmlDdfHidden ) == 0 )
		{
		return EHiddenProfile;
		}
    else if ( iField->Compare( KNSmlDdfDB ) == 0 ||
            iField->Compare( KNSmlDdfCTType ) == 0 ||
            iField->Compare( KNSmlDdfCTVerL ) == 0 ||
            iField->Compare( KNSmlDdfRDBURI ) == 0 ||
            iField->Compare( KNSmlDdfLDBURI ) == 0 )
		{
		return ESyncAccepted;
		}
	
	_DBG_FILE("CNSmlDSSettingsAdapter::GetDSField(): end");
	return KErrNotFound;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetDSFieldTypeL
//  The function return the data type for the leaf.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetDSFieldTypeL() const
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::GetDSFieldTypeL(): begin");

	if ( iField->Compare( KNSmlDdfAddrType ) == 0 ||
        iField->Compare( KNSmlDdfPortNbr ) == 0 ||
        iField->Compare( KNSmlDdfToNAPID ) == 0 ||
        iField->Compare( KNSmlDdfHidden ) == 0 )
		{
		return EInt;
		}
	else if ( iField->Compare( KNSmlDdfAddr ) == 0 ||
            iField->Compare( KNSmlDdfClientUserName ) == 0 ||
            iField->Compare( KNSmlDdfServerId ) == 0 ||
            iField->Compare( KNSmlDdfClientPW ) == 0 ||
            iField->Compare( KNSmlDdfName ) == 0 )
		{
		return EStr;
		}
	else if ( iField->Compare( KNSmlDdfDB ) == 0 ||
            iField->Compare( KNSmlDdfCTType ) == 0 ||
            iField->Compare( KNSmlDdfCTVerL ) == 0 ||
            iField->Compare( KNSmlDdfRDBURI ) == 0 ||
            iField->Compare( KNSmlDdfLDBURI ) == 0 )
		{
		return EParam;
		}

	_DBG_FILE("CNSmlDSSettingsAdapter::GetDSFieldTypeL(): end");
	return EWrong;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::IntLUID
//	Returns IntValue for aLUID.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::IntLUID( const TDesC8& aLUID )
	{

	TLex8 lLex( aLUID );

	if ( lLex.Val( iLUID ) == KErrNone )
	    {
	    return iLUID;
	    }
	else
        {
        return 0;
        }
	}

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::GetIntObject
//	Returns IntValue for a aObject
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetIntObject( const TDesC8& aObject )
	{
	TLex8 lLex( aObject );

	TInt value = 0;

	if ( lLex.Val( value ) == KErrNone )
	    {
	    return value;
	    }
	else
        {
        return KErrGeneral;
        }
	}

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::GetIntObject16()
//	Returns IntValue for a 16-bit aObject
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetIntObject16( const TDesC& aObject )
	{
	TLex lLex( aObject );

	TInt value = 0;

	if ( lLex.Val( value ) == KErrNone )
	    {
	    return value;
	    }
	else
        {
        return KErrGeneral;
        }
	}
//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::SetIntObjectLC
//	Converts integer to HBufC8 type buffer and returns reference to it.
//------------------------------------------------------------------------------
TDesC8& CNSmlDSSettingsAdapter::SetIntObjectLC( const TInt& aObject )
	{
	HBufC8* buf = HBufC8::NewLC( 8 );
	TPtr8 ptrBuf = buf->Des();

	ptrBuf.Num( aObject );

	return *buf;
	}

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::NotValidStrLenght
//	The function checks if data length for a leaf is valid.
//------------------------------------------------------------------------------

TBool CNSmlDSSettingsAdapter::NotValidStrLenght( const TInt& aProfileItem, 
                                                const TDesC8& aSource )
	{
	TInt lLen = aSource.Length();
	TBool lBool = EFalse;
	
	switch ( aProfileItem )
		{
		case ( EProfileName ) :
			{
			if( lLen > KNSmlMaxProfileNameLength )
				lBool = ETrue;	
			}
		break;
		
		case ( EProfileSyncServerUsername ) :
			{
			if( lLen > KNSmlMaxUsernameLength )
				lBool = ETrue;	
			}
		break;

		case ( EProfileSyncServerPassword ) :
			{
			if( lLen > KNSmlMaxPasswordLength )
				lBool = ETrue;	
			}
		break;

		case ( EProfileURL ) :
			{
			if( lLen > KNSmlMaxURLLength )
				lBool = ETrue;	
			}
		break;

		case ( EProfileIAPId ) :
			{
			if( lLen > KNSmlMaxURLLength )// Value from InternetAdapter for
				lBool = ETrue;			// given URI ./AP/xxx
			}
		break;

		case ( EProfilePort ) :
		case ( EHiddenProfile ) :
		case ( EProfileMediumType ) :
			{
			if( lLen > 8 )
				lBool = ETrue;	
			}
		break;

		case ( EProfileServerId ) :
			{
			if( lLen > KNSmlMaxUsernameLength )
				lBool = ETrue;			
			}
		break;

		default:
			{
			return ETrue;
			}
		}

	return lBool;
	}

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::SetField
//	The function finds out the last element of the URI.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetField( const TDesC8& aSource )
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

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::ConvertTo8LC
//	Converts 16-bit string value to 8-bit.
//------------------------------------------------------------------------------
TDesC8& CNSmlDSSettingsAdapter::ConvertTo8LC( const TDesC& aSource )
	{
	HBufC8* buf = HBufC8::NewLC( aSource.Length() * 2 );
	TPtr8 bufPtr = buf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );
    return *buf;
	}

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::ConvertTo16LC
//	Converts 8-bit string value to 16-bit.
//------------------------------------------------------------------------------
TDesC16& CNSmlDSSettingsAdapter::ConvertTo16LC( const TDesC8& aSource )
	{
	HBufC16* buf16 = HBufC16::NewLC( aSource.Length() );
	TPtr16 bufPtr16 = buf16->Des();
	CnvUtfConverter::ConvertToUnicodeFromUtf8( bufPtr16, aSource );
    return *buf16;
	}

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::AddNewDataProviderL
//	Check if data provider exits and required MIME type is supported by the 
//  data provider. If supported new task will be created.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::AddNewDataProviderL( TInt aIntLUID, 
                                                    const TDesC8& aMIMEType, 
                                                    TInt& aDataProviderId )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::AddNewDataProviderL(): begin");
    
    TInt retVal = OpenSyncSessionAndDSProfileL( aIntLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
        CloseDSProfile();
        return KErrGeneral;	
		}
    if( iRdbDataStore->Size() <= 0 )
        {
        CloseDSProfile();
        return KErrGeneral;
        }
    RArray<TSmlDataProviderId> dataProviderArray;
    CleanupClosePushL( dataProviderArray );
    iSyncSession.ListDataProvidersL( dataProviderArray );
    TInt dataProviderCount = dataProviderArray.Count();
    if( dataProviderCount == 0 )
        {
        CloseDSProfile();
        CleanupStack::PopAndDestroy(); // dataProviderIdArray
    	_DBG_FILE("CNSmlDSSettingsAdapter::AddNewDataProviderL(): dataprovirdercount == 0 End");
        return KErrNotFound;   
        }
    HBufC8* object = aMIMEType.AllocLC();
    TPtr8 objectPtr = object->Des();
	objectPtr.LowerCase();
    RSyncMLDataProvider dataProvider;
        
    for ( TInt i= 0; i < dataProviderCount; i++ )
        {
        dataProvider.OpenL( iSyncSession, dataProviderArray[i] );
        CleanupClosePushL( dataProvider );   
        for ( TInt n = 0; n < dataProvider.MimeTypeCount(); n++ )
            {
            HBufC* mimeType = dataProvider.MimeType(n).AllocLC();
            if( objectPtr.CompareF( ConvertTo8LC( *mimeType ) ) == 0 ) //if MIME match
                {
               	_DBG_FILE("CNSmlDSSettingsAdapter::AddNewDataProviderL(): Mimetype found");

                aDataProviderId = dataProvider.Identifier();
                RSyncMLTask task;
                CleanupClosePushL( task ); 
                if( dataProvider.AllowsMultipleDataStores() )
                    {
                   	_DBG_FILE("CNSmlDSSettingsAdapter::AddNewDataProviderL(): AllowsMultipleDataStores");
                    CDesCArray* dataStoreName = new ( ELeave ) CDesCArrayFlat( 1 );
                    CleanupStack::PushL( dataStoreName );
                    dataProvider.GetDataStoreNamesL( *dataStoreName );
                    if ( dataStoreName->Count() == 0 )
                        {
                        // Get default if there are no databases
                        _DBG_FILE("CNSmlDSSettingsAdapter::AddNewDataProviderL():  no databases -> get default");
                        HBufC* localDataStoreName = dataProvider.DefaultDataStoreName().AllocLC();
                        task.CreateL( iDSProfile, aDataProviderId, *iRdbDataStore, 
                                    *localDataStoreName );
					    task.UpdateL();
						CleanupStack::PopAndDestroy();  //localDataStoreName
                        }
                    else
                        {
                        TPtrC16 ptrDataStoreName = dataStoreName->MdcaPoint(0);
                        task.CreateL( iDSProfile, aDataProviderId, *iRdbDataStore, 
		                            ptrDataStoreName );
		                task.UpdateL();
                        }
                    CleanupStack::PopAndDestroy();//dataStoreName
                    }
                else
                    {
                   	_DBG_FILE("CNSmlDSSettingsAdapter::AddNewDataProviderL():  MultipleDataStores not allowed");
                    HBufC* localDataStoreName = dataProvider.DefaultDataStoreName().AllocLC();
                    task.CreateL( iDSProfile, aDataProviderId, *iRdbDataStore, 
                                    *localDataStoreName );
                    task.UpdateL();
                    CleanupStack::PopAndDestroy();  //localDataStoreName
                    }
                iDSProfile.UpdateL();
                CleanupStack::PopAndDestroy( 6 ); //dataProviderArray, dataProvider, 
                                                //object, mimeType, ConvertTo8LC, 
                                                //task
                CloseDSProfile();
                _DBG_FILE("CNSmlDSSettingsAdapter::AddNewDataProviderL(): end KErrNone");
                return KErrNone;
                }
            CleanupStack::PopAndDestroy( 2 ); //mimeType, ConvertTo8LC    
            }
        CleanupStack::PopAndDestroy(  ); //dataProvider
        }
    
    CleanupStack::PopAndDestroy( 2 ); //dataProviderArray, object
    CloseDSProfile();
    _DBG_FILE("CNSmlDSSettingsAdapter::AddNewDataProviderL(): end KErrNotFound");
    return KErrNotFound;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetToNAPIDL
//	The function gets URI for given aObject.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetToNAPIDL( const TInt aLUID, CBufBase& aObject )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::GetToNAPIDL() : begin");
		
    TInt profIAPid = -1;
    TInt apIdResponse = GetProfileIAPIdL(aLUID, profIAPid);
    if( profIAPid < 0 || apIdResponse != KErrNone )
        {
        return KErrNotFound;
        }
    CNSmlDMIAPMatcher* apmatch = CNSmlDMIAPMatcher::NewL( iCallBack );
    CleanupStack::PushL( apmatch );
    HBufC8* uri8 = apmatch->URIFromIAPIdL( profIAPid );
    if( uri8 )
        {
        CleanupStack::PushL( uri8 );
        aObject.InsertL(aObject.Size(),uri8->Des());
        CleanupStack::PopAndDestroy(); // uri8
        }
    else
        {
        CleanupStack::PopAndDestroy(); // apMatch
        return KErrGeneral;    
        }
    CleanupStack::PopAndDestroy(); // apMatch
	_DBG_FILE("CNSmlDSSettingsAdapter::GetToNAPIDL() : end");
	return KErrNone;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetToNAPIDL
//	The function sets NAPId value according given URI.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetToNAPIDL( const TInt aLUID, const TDesC8& aObject )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::SetToNAPIDL() : begin");
    CNSmlDMIAPMatcher* apmatch = CNSmlDMIAPMatcher::NewL( iCallBack );
    CleanupStack::PushL( apmatch );
    TInt lIAPid = apmatch->IAPIdFromURIL( aObject );
    TInt apIdResponse = SetProfileIAPIdL( aLUID, lIAPid );
	CleanupStack::PopAndDestroy(); // apmatch
    _DBG_FILE("CNSmlDSSettingsAdapter::SetToNAPIDL() : end");
	return apIdResponse;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::AddNodeBufferL
//	The function sets the new node to buffer.	
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::AddNodeBufferL( const TDesC8& aURI, 
                                                const TDesC8& aParentLUID,
                                                const TInt aStatusRef )
	{
	TNSmlDSBufferElement newNode;
	newNode.iMappingName = aURI.AllocLC();
	newNode.iName = LastURISeg(aURI).AllocLC(); 
	newNode.iNodeBuf = new (ELeave) CArrayFixFlat <TNSmlDSAddElement> ( KNSmlDSGranularity );
	newNode.iExecuted = EFalse;
	newNode.iLuid = GetIntObject( aParentLUID );
	iBuffer->AppendL( newNode );
	CleanupStack::Pop( 2 ); //newNode.iMappingName,newNode.iName

	TNSmlDSAddElement newCommand;

	newCommand.iUri = aURI.AllocLC();
	newCommand.iData = 0;
	newCommand.iStatusRef = aStatusRef;
	newCommand.iLeaf = EFalse;
	newCommand.iDone = EFalse;

	TInt index = iBuffer->Count() - 1;
	iBuffer->At(index).iNodeBuf->AppendL( newCommand );
	CleanupStack::Pop(); //newCommand.iUri
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::AddLeafBufferL
//  The function sets the new leaf to buffer.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::AddLeafBufferL( const TDesC8& aURI, 
                                                const TDesC8& aParentLUID, 
                                                const TDesC8& aObject, 
                                                const TInt aStatusRef )
	{
    TInt index = GetBufferIndex( aURI );
    if(index<0)
		{
		DBG_ARGS(_S16("DS-adapter:AddLeafBufferL URI - <%S> <%S> NOTFOUND"), &aURI, &aParentLUID );
		iCallBack->SetStatusL(aStatusRef,  CSmlDmAdapter::ENotFound);
		return;
        }

	TNSmlDSAddElement newCommand;
	newCommand.iUri = aURI.AllocLC();
	newCommand.iData = aObject.AllocLC();
	newCommand.iStatusRef = aStatusRef;
	newCommand.iLeaf = ETrue;
	newCommand.iDone = EFalse;
    iBuffer->At(index).iNodeBuf->AppendL( newCommand );
	CleanupStack::Pop( 2 ); //newCommand.iUri, newCommand.iData
    }

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::LastURISeg
//  The function returns only the last uri segment.
//------------------------------------------------------------------------------
TPtrC8 CNSmlDSSettingsAdapter::LastURISeg(const TDesC8& aURI)
	{
	TInt i;
	for( i=aURI.Length() - 1; i >= 0; i-- )
		{
		if( aURI[i] == KNSmlDMUriSeparator )
			{
			break;
			}
		}
	if( i == 0 )
		{
		return aURI;
		}
	else
		{
		return aURI.Mid( i + 1 );
		}
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::RemoveLastURISeg
//  The function removes the last URI segment.
//------------------------------------------------------------------------------
TPtrC8 CNSmlDSSettingsAdapter::RemoveLastURISeg( const TDesC8& aURI )
	{
	TInt i;
	for( i = aURI.Length() - 1; i >= 0; i-- )
		{
		if( aURI[i] == KNSmlDMUriSeparator )
			{
			break;
			}
		}
	if( i == 0 )
		{
		return aURI;
		}
	else
		{
		return aURI.Left( i );
		}
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::ExecuteBufferL
//  Executes commands which have not been executed.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::ExecuteBufferL( const TDesC8& aURI )
	{
	TInt index = GetBufferIndex( aURI );
	if( index < 0 )
		{
		return;
		}
	//
	// Execute all commands for Profiles new ContentType
	//
    TBool rdburinotfound = ETrue;
    TInt newcmd = 1;
    
    TNSmlDSBufferElement& bufElem = iBuffer->At(index); 
    
	for( TInt cmd = bufElem.iNodeBuf->Count() - 1; cmd >= 0; cmd-- )
		{
		TBuf8<16> addLUID;
		
		TNSmlDSAddElement& addElem = bufElem.iNodeBuf->At(cmd); 
			
		if( cmd == 0 )
			{
			if( bufElem.iLuid > 0 )
			    {
			    addLUID.Format( KFormat, -1 ); // Set status for addnode command
			    }
			else
                {
                addLUID.Format( KFormat, 0 );
                }
            iBufferExecution = ETrue;
            AddNodeObjectL( addElem.iUri->Des(), 
                            addLUID, addElem.iStatusRef );

			bufElem.iExecuted = ETrue;
            iBufferExecution = EFalse;
            }
		else 
			{
            //First RDBURI leaf must be stored
            while( rdburinotfound && newcmd < iBuffer->At(index).iNodeBuf->Count() )
    			{
	    		if( bufElem.iNodeBuf->At(newcmd).iUri->Des().Find( KNSmlDdfRDBURI ) >= 0 )
				    {
                    SetRDBDataStore( bufElem.iNodeBuf->At(newcmd).iData->Des() );
				    rdburinotfound = EFalse;
				    break;
				    }
			    newcmd++;
			    }//while
			addLUID.Format( KFormat, bufElem.iLuid );
			iBufferExecution = ETrue;
            UpdateLeafObjectL( addElem.iUri->Des(),
				addLUID,
				addElem.iData->Des(),
				KNullDesC8,
				addElem.iStatusRef );
            iBufferExecution = EFalse;
			}

		addElem.iDone = ETrue;

		delete addElem.iUri;
		addElem.iUri=NULL;
		if( addElem.iData )
			{
			delete addElem.iData;
			addElem.iData=NULL;
			}
		bufElem.iNodeBuf->Delete(cmd);
		}
//
//	Remove data from handled ContentType-commands
//
	bufElem.iNodeBuf->Reset();
	delete bufElem.iNodeBuf;

	if( bufElem.iMappingName )
		{
		delete bufElem.iMappingName;
		bufElem.iMappingName=NULL;
		}

	if( bufElem.iName )
		{
		delete bufElem.iName;
		bufElem.iName=NULL;
		}

	iBuffer->Delete( index ); 
	iBuffer->Compress();
	}


//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetProfileIdFromURIL
//	The function gets the Profile Id for given aURI.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetProfileIdFromURIL( const TDesC8& aURI )
	{
	HBufC8* apURI = HBufC8::NewLC( aURI.Size() );
//
//  Second slash after SyncML/DSAcc/XXXX/
//
	TInt i = 0;
	
	for( i = aURI.Find( KDSAcc1 ) + 13; i < aURI.Length(); i++ )
		{
		if( aURI[i] == KNSmlDMUriSeparator )
			{
			break;
			}
		}
	if( i > 0 )
		{
		apURI->Des().Format( aURI.Left(i) );
		}
//
//	Same profile as previous
//
	if( iPrevURI->Match( *apURI ) != KErrNotFound )
		{
		CleanupStack::PopAndDestroy(); // apURI
		return iPrevLUID;
		}
//
//	Fetch Profile-id for URI
//

    HBufC8* apLUID = iCallBack->GetLuidAllocL( *apURI );
	CleanupStack::PushL( apLUID );
    if( apLUID->Length() == 0 )
        {
        CleanupStack::PopAndDestroy( 2 );//apURI, apLUID
        return KErrGeneral;
        }
	TInt resLUID = GetIntObject( *apLUID );
	iPrevURI->Des().Format( *apURI );
	iPrevLUID = resLUID;
	CleanupStack::PopAndDestroy(2); //apURI,apLUID

	return resLUID;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetContentTypeL
//  Get CTType (MimeType) for given adapter.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetContentTypeL( const TInt aDataProviderId, 
                                              const TInt aProfileLUID, 
                                              CBufBase*& aObject )
	{
    _DBG_FILE("CNSmlDSSettingsAdapter::GetContentTypeL(): begin");
    TInt retVal = OpenSyncSessionAndDSProfileL( aProfileLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
        CloseDSProfile();
		return retVal;
		}

    RArray<TSmlDataProviderId> dataProviderIdArray;
    CleanupClosePushL( dataProviderIdArray );
    iSyncSession.ListDataProvidersL( dataProviderIdArray );
    
    TInt dpIndex = dataProviderIdArray.Find( aDataProviderId );
    if( dpIndex == KErrNotFound )
        {
        CloseDSProfile();
        CleanupStack::PopAndDestroy( 1 ); // dataProviderIdArray
        return KErrNotFound;
        }
    RSyncMLDataProvider dataProvider;
    CleanupClosePushL( dataProvider );
    dataProvider.OpenL( iSyncSession, dataProviderIdArray[dpIndex] );
    for( TInt i = 0; i < dataProvider.MimeTypeCount(); i++ )
        {
        //first mime type will be returned now
        HBufC* mimeType = dataProvider.MimeType(i).AllocLC();
        aObject->InsertL( aObject->Size(), ConvertTo8LC( *mimeType ) );  
        CleanupStack::PopAndDestroy( 2 ); //mimeType, ConvertTo8LC
        break;
        }
    CleanupStack::PopAndDestroy( 2 ); // dataProviderIdArray, dataProvider
    CloseDSProfile();
    _DBG_FILE("CNSmlDSSettingsAdapter::GetContentTypeL(): end");
    return KErrNone;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetBufferIndex
//  The function resolves buffer index for given URI.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetBufferIndex( const TDesC8& aURI )
	{
	TInt index = -1;
	for( TInt i = 0; i<iBuffer->Count(); i++ )
		{
		TPtrC8 parentUri = RemoveLastURISeg( aURI );
		if( iBuffer->At(i).iMappingName->Compare( parentUri ) == 0 )
			{
			index = i;
			break;
			}
		}
	return index;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetBufferLUID
//  The function sets the LUID to buffer for given URI.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetBufferLUID( const TDesC8& aURI, const TInt aLUID )
	{
	TInt index = GetBufferIndex( aURI );
	if( index < 0 )
		{
		return 0;
		}
	iBuffer->At(index).iLuid = aLUID;
	return 1;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::ParseUri
//  Resolves portnbr (separated with ':') as integer from URI.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::ParseUri( const TDesC8& aRealURI, TInt& aPort )
	{
    TInt offset=0;
    //http:
	if( aRealURI.Find( KNSmlDSSettingsHTTP )==0 )//http:
		{
		offset = KNSmlDSSettingsHTTP().Length();
		}
	else if( aRealURI.Find( KNSmlDSSettingsHTTPS )==0 )//https:
		{
		offset = KNSmlDSSettingsHTTPS().Length();
		}

	TBool portFound=EFalse;
	TBool portExtracted = EFalse;
    ////123.234.345.456:80/syncml
	for( TInt i = offset; i < aRealURI.Length() && !portExtracted; i++ )
		{
		if( aRealURI[i] == KNSmlDMColon )// ':'
			{
			portFound = ETrue;
			offset=i;
			}
		if( portFound && aRealURI[i] == KNSmlDMUriSeparator )// '/'
			{
			aPort = GetIntObject( aRealURI.Mid( (offset + 1), (i - offset-1) ) );
			portExtracted = ETrue;
			}
		else if ( portFound && i == aRealURI.Length() - 1 ) // last char
		    {
		    // port is from offset to the end
		    aPort = GetIntObject( aRealURI.Right( aRealURI.Length() - offset - 1 ) );
			portExtracted = ETrue;
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

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::FindProfileIdL
//  The function checkd if profile can be found.
//------------------------------------------------------------------------------
TBool CNSmlDSSettingsAdapter::FindProfileIdL( const TInt aIntLUID )
	{
	if( iSyncSessionOpen == EFalse )
	    {
        iSyncSession.OpenL();
        iSyncSessionOpen = ETrue;
	    }
    RArray <TSmlProfileId> profileList;
    CleanupClosePushL( profileList );
	TSmlUsageType usageType = ESmlDataSync;		
	iSyncSession.ListProfilesL( profileList, usageType );
	if( profileList.Count() == 0 )
	    {
        CleanupStack::PopAndDestroy(); //profileList
        return EFalse;
	    }
    TInt item = profileList.Find( aIntLUID );
    CleanupStack::PopAndDestroy(); //profileList
    if( item != KErrNotFound )
		{
		return ETrue;
		}
	return EFalse;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetProfileIdentifierL
//  The function gets profile Id from Client API.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetProfileIdentifierL( const TInt aIntLUID, 
                                                    TInt& aProfileID )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aIntLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
        CloseDSProfile();
		return retVal;	
		}
	aProfileID = iDSProfile.Identifier();
	CloseDSProfile();
    return retVal;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetDSFieldDataL
//  The function checks what leaf to be handled and fetches the data for a leaf.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetDSFieldDataL( const TInt aIntLUID, 
                                                CBufBase*& aObject )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::GetDSFieldData(): begin");
    
	if ( iField->Compare( KNSmlDdfAddrType ) == 0 )
		{
        TSmlTransportId connType = -1;
		connType = GetProfileConnectiontypeL( aIntLUID );
        if ( connType <= 0 )
			{
            return KErrNotFound;
			}
		aObject->InsertL( aObject->Size(), SetIntObjectLC( connType ) );
		CleanupStack::PopAndDestroy();//SetIntObjectLC
        return KErrNone;
		}
	else if ( iField->Compare( KNSmlDdfAddr ) == 0 ) //Server URL
		{
		TInt retValue1 = GetProfileServerURLL( aIntLUID, aObject );
		TInt port(0);
		if(ParseUri(aObject->Ptr(0),port)==KErrNone)
		    {
		    HBufC8* addr = aObject->Ptr(0).AllocLC();
		    aObject->Reset();
		    TInt portStart = addr->LocateReverse(KNSmlDMColon);		    
		    aObject->InsertL(aObject->Size(),addr->Left(portStart));
		    
		    TInt portLen=addr->Mid(portStart).Locate(KNSmlDMUriSeparator);
		    if(portLen!=KErrNotFound)
		        {
		        aObject->InsertL(aObject->Size(),addr->Mid(portStart+portLen));
		        }
		    	    			    
		    CleanupStack::PopAndDestroy(); //addr
		    }

        if( retValue1 != KErrNone )
		    {
		    return KErrNotFound;
		    }
		return KErrNone;
		}
	else if ( iField->Compare( KNSmlDdfPortNbr ) == 0 )
		{
		TInt retValue2 = GetProfileServerURLL( aIntLUID, aObject );
        if( retValue2 != KErrNone )
		    {
		    return KErrNotFound;
            }
        if ( aObject->Size() == 0 )
            {
            return KErrNone;
            }
        TInt bSize = aObject->Size();
        HBufC8* buf = HBufC8::NewLC( bSize );
		TPtr8 bufPtr = buf->Des();
		aObject->Read( 0, bufPtr, aObject->Size() );
		aObject->Reset();
		
        TInt pnum = 0;
		//find out the port number
        ParseUri( *buf, pnum );
        if(pnum)
            {
    		aObject->InsertL( aObject->Size(), SetIntObjectLC( pnum ) );
            CleanupStack::PopAndDestroy(  ); //SetIntObjectLC( pnum )
            }
        CleanupStack::PopAndDestroy(  ); //buf
        return KErrNone;
		}
	else if ( iField->Compare( KNSmlDdfClientUserName ) == 0 )
		{
		if( GetProfileUserNameL( aIntLUID, aObject ) != KErrNone )
            {
            return KErrGeneral;
            }
		return KErrNone;
		}
	else if ( iField->Compare( KNSmlDdfServerId ) == 0 )
		{
		if( GetProfileServerIdL( aIntLUID, aObject ) != KErrNone )
            {
            return KErrGeneral;
            }
		return KErrNone;
		}
	else if ( iField->Compare( KNSmlDdfClientPW ) == 0 ) //Not allowed
		{
		return KErrGeneral;
        }
	else if ( iField->Compare( KNSmlDdfName ) == 0 )
		{
		TInt retValue = GetProfileDisplayNameL( aIntLUID, aObject );
		if( retValue != KErrNone )
		    {
            return KErrGeneral;
		    }
		return KErrNone;
		}
	else if ( iField->Compare( KNSmlDdfToNAPID ) == 0 )
		{
		TInt retToNaPID = GetToNAPIDL( aIntLUID, *aObject );
		if ( retToNaPID == KErrNone || retToNaPID == KErrNotFound )
			{
			return retToNaPID;
			}
		return KErrGeneral;			
		}
	else if ( iField->Compare( KNSmlDdfHidden ) == 0 )
		{
		//Hidden profile not supported
		return KErrGeneral;
		}
    _DBG_FILE("CNSmlDSSettingsAdapter::GetDSFieldData(): end");
    return KErrGeneral;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetProfileConnectiontypeL()
//  The function what medium type is supported and returns it.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetProfileConnectiontypeL( const TInt aIntLUID )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aIntLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
        CloseDSProfile();
		return retVal;	
		}
	RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
	iDSProfile.ListConnectionsL( transportIdArray );
	TInt transportItems = transportIdArray.Count();
    if( transportItems == 0 )
		{
		//No matching items
        CleanupStack::PopAndDestroy(); //transportIdArray	
		CloseDSProfile();
		return KErrGeneral;
		}
	else if( transportItems > 1 )
		{
		//Error case : against specification
        CleanupStack::PopAndDestroy(); //transportIdArray	
		CloseDSProfile();
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
    
    CleanupStack::PopAndDestroy(); //transportIdArray
	CloseDSProfile();
	return mediumType;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetProfileServerURLL
//  The function gets the profile Addr leaf value.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetProfileServerURLL( const TInt aIntLUID, 
                                                    CBufBase*& aURL )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aIntLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
        CloseDSProfile();
		return retVal;	
		}
	RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
	iDSProfile.ListConnectionsL( transportIdArray );
    if( transportIdArray.Count() > 0 )
        {
        RSyncMLConnection profConn;
        CleanupClosePushL( profConn );
	    profConn.OpenL( iDSProfile, transportIdArray[0] );

	    HBufC8* sURI = profConn.ServerURI().AllocL();
        CleanupStack::PushL( sURI );
        TPtr8 uriPtr = sURI->Des();
        if( uriPtr.Length() > 0 )
            {
            aURL->InsertL( aURL->Size(),uriPtr );
            }
        else
            {
            aURL->InsertL( aURL->Size(), KNullDesC8 );
            }
        CleanupStack::PopAndDestroy( 2 );//profConn, sURI
        retVal = KErrNone;
        }
    else
        {
        retVal = KErrNotFound;
        }
    CleanupStack::PopAndDestroy(); //transportIdArray
    CloseDSProfile();
    return retVal;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetProfileUserNameL
//  The function gets the ClientUserName leaf value.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetProfileUserNameL( const TInt aIntLUID, 
                                                    CBufBase*& aUserName )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aIntLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
        CloseDSProfile();
		return retVal;	
		}
	HBufC8* userName = iDSProfile.UserName().AllocL();
    CleanupStack::PushL( userName );
    TPtr8 ptrUserName = userName->Des();
    CloseDSProfile();
    aUserName->InsertL( aUserName->Size(), ptrUserName );
    CleanupStack::PopAndDestroy(); //userName
    return KErrNone;
	}
	
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetProfileServerIdL
//  The function gets the ClientUserName leaf value.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetProfileServerIdL( const TInt aIntLUID, 
                                                    CBufBase*& aUserName )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aIntLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
        CloseDSProfile();
		return retVal;	
		}
    aUserName->InsertL( aUserName->Size(), iDSProfile.ServerId() );
    CloseDSProfile();
    return KErrNone;
	}
	
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetProfileIAPIdL
//  The function gets the IAP id value from Client API.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetProfileIAPIdL( const TInt aIntLUID, TInt& aIAPid )
	{
    aIAPid = -1;
    TInt retVal = OpenSyncSessionAndDSProfileL( aIntLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
    RArray<TSmlTransportId> connIdArray;
    CleanupClosePushL( connIdArray );
    iDSProfile.ListConnectionsL( connIdArray );
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
	        conn.OpenL( iDSProfile, connIdArray[0] );
            HBufC8* iapId = conn.GetPropertyL( KNSmlIAPId ).AllocLC();
            aIAPid = GetIntObject( *iapId );
            CleanupStack::PopAndDestroy( 2 ); //conn, iapId
            retVal = KErrNone;
            }
        else
            {
            retVal = KErrNotFound;
            }
        CleanupStack::PopAndDestroy(); //transport
        CloseDSProfile();
        }
    else
        {
        retVal = KErrNotFound;
        }
    CleanupStack::PopAndDestroy(); //connIdArray
    return retVal;
    }

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetProfileDisplayNameL
//  The function gets the Name leaf value for a profile.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetProfileDisplayNameL( const TInt aIntLUID, 
                                                        CBufBase*& aDisplayName )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aIntLUID, ESmlOpenRead );
    if(retVal != KErrNone)
		{
		CloseDSProfile();
        return retVal;	
		}
	HBufC* dataBuf = iDSProfile.DisplayName().AllocL();
    CleanupStack::PushL( dataBuf );
    TPtr16 ptrDisplayName = dataBuf->Des();
    aDisplayName->InsertL( aDisplayName->Size(), ConvertTo8LC( ptrDisplayName ) );
    CleanupStack::PopAndDestroy( 2 );//dataBuf, ConvertTo8LC
    CloseDSProfile();
    return KErrNone;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::CreateNewProfileL
//  The function creates new profile through Client API.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::CreateNewProfileL( TInt& aPID )
	{
    if( iSyncSessionOpen == EFalse )
	    {
        iSyncSession.OpenL();
        iSyncSessionOpen = ETrue;
	    }
    TRAPD( trapValue1, iDSProfile.CreateL( iSyncSession ) );
    if( trapValue1 != KErrNone )
        {
        return trapValue1;
        }
    //iDSProfile.SetProtocolVersionL(ESmlVersion1_1_2);
    TRAPD( trapValue2, iDSProfile.UpdateL() );
    if( trapValue2 != KErrNone )
        {
        return trapValue2;
        }
    aPID = iDSProfile.Identifier();
   	

    CloseDSProfile();
    return KErrNone;
    }

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetProfileServerURIL
//  The function sets the Addr leaf value.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfileServerURIL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aPID, ESmlOpenReadWrite );
    if(retVal != KErrNone)
		{
		CloseDSProfile();
        return retVal;	
		}
	RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
	iDSProfile.ListConnectionsL( transportIdArray );
	if( transportIdArray.Count() == 0 )
	    {
        CleanupStack::PopAndDestroy();//transportIdArray
        CloseDSProfile();
        return KErrGeneral;
	    }
	    
	TInt newPort(0);
	
	RSyncMLConnection conn;
    CleanupClosePushL( conn );
    conn.OpenL( iDSProfile, transportIdArray[0] );
    
    if( transportIdArray[0] == KUidNSmlMediumTypeInternet.iUid )
        {
        if(ParseUri(aObj,newPort)==KErrNotFound)
            {
            TInt port(0);
            ParseUri(conn.ServerURI(),port);
            if(port==0)
            	{
            	// set default port
            	if( aObj.Find( KNSmlDSSettingsHTTPS ) == 0 ) //https://
            		{
            		port = KNSmlDSSettingsHTTPSPort;
            		}
            	else //http://
        	        {
        	        port = KNSmlDSSettingsHTTPPort;
        	        }
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
        }
    else // medium type not internet, no port allowed
        {
        if(ParseUri(aObj,newPort)==KErrNotFound)
            {
            conn.SetServerURIL( aObj );
    	    conn.UpdateL();
            }
        else
            {
            // remove port from uri
            HBufC8* newServerURI = HBufC8::NewL( aObj.Length() );
            CleanupStack::PushL( newServerURI );
            TBuf8<5> buf;
        	buf.Num( 0 );
            // change iLeafType to EDSDelete -> deleting port
            TNSmlDSLeafType	tmpType(iLeafType); // store current value
            iLeafType = EDSDelete;
            SetPortNrToAddrStr( aObj, newServerURI, buf );
            iLeafType = tmpType; // restore original value
            
            conn.SetServerURIL( *newServerURI );
            conn.UpdateL();
            CleanupStack::PopAndDestroy( newServerURI );
            }
        }

     
    iDSProfile.UpdateL();
    CleanupStack::PopAndDestroy( 2 ); //transportIdArray, conn
	CloseDSProfile();
	return retVal;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetProfileDisplayNameL
//  The function sets the Name leaf value for a profile.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfileDisplayNameL( const TInt aPID, 
                                                        const TDesC8& aObj )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;
		}
	iDSProfile.SetDisplayNameL( ConvertTo16LC( aObj ) );
	iDSProfile.UpdateL();
    CleanupStack::PopAndDestroy();//ConvertTo16LC
	CloseDSProfile();
	return retVal;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetProfileUserNameL
//  The function sets the ClientUserName leaf value for a profile.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfileUserNameL( const TInt aPID,  
                                                    const TDesC8& aObj )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
	iDSProfile.SetUserNameL( aObj );
	iDSProfile.UpdateL();
	CloseDSProfile();
	return retVal;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetProfileServerIdL
//  The function sets the ClientUserName leaf value for a profile.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfileServerIdL( const TInt aPID,  
                                                    const TDesC8& aObj )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
	iDSProfile.SetServerIdL( aObj );
	if(aObj.Length())
	    {
    	iDSProfile.SetProtocolVersionL(ESmlVersion1_2);
	    }
	else
	    {
    	iDSProfile.SetProtocolVersionL(ESmlVersion1_1_2);	    
	    }
	iDSProfile.UpdateL();
	CloseDSProfile();
	return retVal;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetProfilePasswordL
//  The function sets the ClientPW leaf value for a profile.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfilePasswordL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
	iDSProfile.SetPasswordL( aObj );
	iDSProfile.UpdateL();
	CloseDSProfile();
	return retVal;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetProfileStrValueL
//  The function checks what leaf to be handles and calls the proper function to
//  set leaf data.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfileStrValueL( const TInt aPID, 
                                                    const TDesC8& aObj, 
                                                    const TInt aFieldID )
	{
	TInt ret = 0;
	switch ( aFieldID )
		{
		case ( EProfileName ) :
			{
			ret = SetProfileDisplayNameL( aPID, aObj );
			}
		break;
		
		case ( EProfileSyncServerUsername ) :
			{
			ret = SetProfileUserNameL( aPID, aObj );
			}
		break;

		case ( EProfileServerId ) :
			{
			ret = SetProfileServerIdL( aPID, aObj );
			}
		break;


		case ( EProfileSyncServerPassword ) :
			{
			ret = SetProfilePasswordL( aPID, aObj );
			}
		break;
        case ( EProfileIAPId ) :
			{
			ret = SetToNAPIDL( aPID, aObj );
			}
		break;
		case ( EProfilePort ) :
            {
			delete iPortBuf;
			iPortBuf = NULL;
			
            // check that port is a valid integer
            TLex8 lex( aObj );
            TInt port = KErrNotFound;
            TInt err = lex.Val( port );
            
            if ( err != KErrNone || !lex.Eos() )
                {
                ret = KErrArgument;
                }
            else
                {
    			TInt portval = GetIntObject(aObj);
			if(portval<0)
			{
				ret=KErrArgument;
			}
			else if(portval>65535)
			{
			    	ret=KErrOverflow;	
			}
			
			else
			{
			// save information of the port
    			iPortBuf = aObj.AllocL();
    			iPortBufLUID = aPID;
    			SetProfileConnectionPortNrL( iPortBufLUID, *iPortBuf );
			}
                }
            }
        break;
        case ( EProfileURL ) :
		case ( EHiddenProfile ) :
		case ( EProfileMediumType ) :
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	return ret;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::DeleteProfileL
//  The function deletes profile data content.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::DeleteProfileL( const TInt aPID )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return KErrGeneral;	
		}
    RArray<TSmlTaskId> taskIdArray;
    CleanupClosePushL( taskIdArray );
    iDSProfile.ListTasksL( taskIdArray );
    if( taskIdArray.Count() > 0 )
        {
        for ( TInt i = 0; i < taskIdArray.Count(); i++ )
            {
            iDSProfile.DeleteTaskL( taskIdArray[i] );
            iDSProfile.UpdateL();
            }
        }
    CloseDSProfile();
    RArray<TSmlProfileId> profileIdArray;
    CleanupClosePushL( profileIdArray );
    TSmlUsageType usageType = ESmlDataSync;
    iSyncSession.ListProfilesL( profileIdArray, usageType );
    TInt index = profileIdArray.Find( aPID );
    iSyncSession.DeleteProfileL( profileIdArray[index] );
    CleanupStack::PopAndDestroy( 2 ); //profileIdArray, taskIdArray
    iSyncSession.Close();
    iSyncSessionOpen = EFalse;
    return KErrNone;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::CreateURITaskSegmentListL
//  The function creates list of the content types that profile supports.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::CreateURITaskSegmentListL( const TDesC8& aURI, 
                                                        const TInt aPID, 
                                                        CBufBase*& aCurrentURISegmentList, 
                                                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList )
	{
	TInt retVal = OpenSyncSessionAndDSProfileL( aPID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return KErrGeneral;	
		}
    RArray<TSmlTaskId> taskIdArray;
    CleanupClosePushL( taskIdArray );
    iDSProfile.ListTasksL( taskIdArray );
	if( taskIdArray.Count() == 0 )
		{
		CloseDSProfile();
		CleanupStack::PopAndDestroy();//taskIdArray
    	return KErrNotFound;
		}
	
	RSyncMLTask task;
    CleanupClosePushL( task );
            
    for ( TInt i = 0; i < taskIdArray.Count(); i++ )
		{			
		//	
		//	Check if ContentType -> Task ID on list
		//
		TBool notInList( ETrue );
		TInt newline( 0 );
		TInt ddId( 0 );
		task.OpenL( iDSProfile, taskIdArray[i] );
        ddId = task.DataProvider();
        task.Close();
		while( notInList && newline <aPreviousURISegmentList.Count() )
			{
			TInt ppId = GetIntObject( aPreviousURISegmentList.At(newline).iURISegLUID );
			if( ppId == ddId )
				{
				notInList = EFalse;
				break;
				}
			newline++;
			}//while

		if( notInList )
			{
			TBuf8<9> addNAME( Kprev );
			//Get Task ID
			addNAME.AppendNumFixedWidth( taskIdArray[i], EDecimal,3 ); 
			aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), addNAME );
			aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), KNSmlDSSlash );
	
			TBuf8<80> addURI;
			
			addURI.Append( aURI );
			addURI.Append( Kprev2 );
			addURI.AppendNumFixedWidth( taskIdArray[i], EDecimal, 3 ); 

			TBuf8<16> addLUID;
			addLUID.Num( ddId );
				
			iCallBack->SetMappingL( addURI, addLUID );
			}
		else //not a new object
			{
			aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
                                            aPreviousURISegmentList.At(newline).iURISeg );

			aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), KNSmlDSSlash );

//			DBG_ARGS8(_S8("InList: Id = %d Name %S"), newline, 
//                            aPreviousURISegmentList.At(newline).iURISeg);
			}
		} //For
	CloseDSProfile();
	CleanupStack::PopAndDestroy(2);// task, taskIdArray
	return KErrNone;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::CreateURIProfileSegmentListL
//  The function creates list of the profiles.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::CreateURIProfileSegmentListL( CBufBase*& aCurrentURISegmentList, 
                                                            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList )
	{
	if( iSyncSessionOpen == EFalse )
	    {
        iSyncSession.OpenL();
        iSyncSessionOpen = ETrue;
	    }
    RArray<TSmlProfileId> profileIdArray;
    CleanupClosePushL( profileIdArray );
	TSmlUsageType usageType = ESmlDataSync;
	iSyncSession.ListProfilesL( profileIdArray, usageType );
		
    if(profileIdArray.Count() == 0)
		{
        CleanupStack::PopAndDestroy();//profileIdArray
		return KErrNotFound;
		}
		
	for(TInt p = 0; p < profileIdArray.Count(); p++ )
		{
		TBool notInList = ETrue;
		TInt newline = 0;
		while( notInList && newline < aPreviousURISegmentList.Count() )
			{
				TInt ddId = profileIdArray[p];
				TInt ppId = GetIntObject( aPreviousURISegmentList.At(newline).iURISegLUID );
				if( ppId == ddId ) 
					{
					notInList = EFalse;
					break;
					}
				newline++;
			}

		if( notInList )
			{ // Those which not have mapping data
			
			TBuf8<9> addNAME( KDSprev );
			addNAME.AppendNumFixedWidth( profileIdArray[p], EDecimal, 3 ); 

			aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), addNAME );
			aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), KNSmlDSSlash );
            
			// KNSmlDSStart includes start text for URISeg
			TBuf8<20> addURI; // SyncML/DSAcc/DSIdnnn , nnn = profileid
			addURI.Append( KNSmlDSStart );
			addURI.AppendNumFixedWidth( profileIdArray[p], EDecimal, 3 ); 
			
			TBuf8<16> addLUID;
			addLUID.Num( profileIdArray[p] );
			
			iCallBack->SetMappingL( addURI,addLUID );
            }
		else
			{ // Add those in PreviousList which are on database
			aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), 
                                            aPreviousURISegmentList.At(newline).iURISeg );

			aCurrentURISegmentList->InsertL( aCurrentURISegmentList->Size(), KNSmlDSSlash );
//			DBG_ARGS8(_S8("InList: Id = %d Name %S"), newline, 
//                           aPreviousURISegmentList.At(newline).iURISeg);
			}
		}
    CleanupStack::PopAndDestroy();//profileIdArray
	return KErrNone;
	}

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::OpenSyncSessionAndDSProfileL
//  The function opens the server session and DS profile.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::OpenSyncSessionAndDSProfileL( const TInt aIntLUID, 
                                                            TSmlOpenMode aMode )
    {
    if( iSyncSessionOpen == EFalse )
        {
        iSyncSession.OpenL();
        iSyncSessionOpen = ETrue;
        }
    RArray<TSmlProfileId> profileIdArray;
    CleanupClosePushL( profileIdArray );
    TSmlUsageType usageType = ESmlDataSync;
    iSyncSession.ListProfilesL( profileIdArray, usageType );
    TInt index = profileIdArray.Find( aIntLUID );
    CleanupStack::PopAndDestroy(); //profileIdArray
    if( index == KErrNotFound )
        {
        return KErrNotFound;
        }
    TRAPD( rValue, iDSProfile.OpenL( iSyncSession, aIntLUID, aMode ) );
    if( rValue != KErrNone )
        {
        iDSProfile.Close();
		iSyncSession.Close();
        iSyncSessionOpen = EFalse;
        }
    return rValue;
    }
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::CloseDSProfile
//  The function closes the DS profile connection.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::CloseDSProfile()
    {
    iDSProfile.Close();
    }

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::FindTaskIDL
//  The function searches if task can be found from the profile task list.
//------------------------------------------------------------------------------
TBool CNSmlDSSettingsAdapter::FindTaskIDL( const TInt aProfLUID, const TInt aDataProviderId )
    {
    TInt retVal = OpenSyncSessionAndDSProfileL( aProfLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return EFalse;	
		}
    RArray<TSmlTaskId> taskIdArray;
    CleanupClosePushL( taskIdArray );//taskIdArray
    iDSProfile.ListTasksL( taskIdArray );
    
    RSyncMLTask task;
    CleanupClosePushL( task );
    // find task
    for (TInt i=0; i<taskIdArray.Count(); i++)
        {
        task.OpenL( iDSProfile, taskIdArray[i] );
        if (task.DataProvider() == aDataProviderId)
            {
            CleanupStack::PopAndDestroy(2); //taskIdArray, task
            CloseDSProfile();
            return ETrue;
            }
        task.Close();
        }
    // task not found
    CleanupStack::PopAndDestroy(2); //taskIdArray, task
    CloseDSProfile();
    return EFalse;
    }
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::DeleteTaskByIDL
//  The function deletes the task by given id.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::DeleteTaskByIDL( const TInt aLUID, const TInt aDataProviderId )
    {
    TInt retVal = OpenSyncSessionAndDSProfileL( aLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
    RArray<TSmlTaskId> taskIdArray;
    CleanupClosePushL( taskIdArray );
    iDSProfile.ListTasksL( taskIdArray );
    
    RSyncMLTask task;
    CleanupClosePushL( task );
    // find task
    for (TInt i=0; i<taskIdArray.Count(); i++)
        {
        task.OpenL( iDSProfile, taskIdArray[i] );
        if (task.DataProvider() == aDataProviderId)
            {
            task.Close();
            CleanupStack::PopAndDestroy(); // task
            iDSProfile.DeleteTaskL( taskIdArray[i] );
            iDSProfile.UpdateL();
            CleanupStack::PopAndDestroy(); //taskIdArray
            CloseDSProfile();
            return KErrNone;
            }
        task.Close();
        }
    // task not found
    CleanupStack::PopAndDestroy(2); //taskIdArray, task
    CloseDSProfile();
    return KErrNotFound;
    }
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetClientDataSourceL
//  The function gets the local data store (LDBURI).
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetClientDataSourceL( const TInt aDataProviderId, 
                                                    const TInt aProfileLUID, 
                                                    CBufBase*& aObject )
    {
    TInt retVal = OpenSyncSessionAndDSProfileL( aProfileLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
    RArray<TSmlTaskId> taskArray;
    CleanupClosePushL( taskArray );
    iDSProfile.ListTasksL( taskArray );
    
    RSyncMLTask task;
    CleanupClosePushL( task );
    // find task
    for (TInt i=0; i<taskArray.Count(); i++)
        {
        task.OpenL( iDSProfile, taskArray[i] );
        if (task.DataProvider() == aDataProviderId)
            {
            HBufC* localDataStore = task.ClientDataSource().AllocLC();
            aObject->InsertL( aObject->Size(), ConvertTo8LC( *localDataStore ) );
            CloseDSProfile();
            CleanupStack::PopAndDestroy(4); //taskIdArray, task, localDataStore, ConvertTo8LC
            
            return KErrNone;
            }
        task.Close();
        }
    // task not found
    CleanupStack::PopAndDestroy(2); //taskArray, task
    CloseDSProfile();
    return KErrNotFound;
    }
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::GetServerDataSourceL
//  The function gets the remote data store (RDBURI).
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::GetServerDataSourceL( const TInt aDataProviderId, 
                                                    const TInt aProfileLUID, 
                                                    CBufBase*& aObject )
    {
    TInt retVal = OpenSyncSessionAndDSProfileL( aProfileLUID, ESmlOpenRead );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
    RArray<TSmlTaskId> taskArray;
    CleanupClosePushL( taskArray );
    iDSProfile.ListTasksL( taskArray );
    
    RSyncMLTask task;
    CleanupClosePushL( task );
    // find task
    for (TInt i=0; i<taskArray.Count(); i++)
        {
        task.OpenL( iDSProfile, taskArray[i] );
        if (task.DataProvider() == aDataProviderId)
            {
            HBufC* serverDataStore = task.ServerDataSource().AllocLC();
            aObject->InsertL( aObject->Size(), ConvertTo8LC( *serverDataStore ) );
            CloseDSProfile();
            CleanupStack::PopAndDestroy(4); //taskIdArray, task, serverDataStore, ConvertTo8LC
            
            return KErrNone;
            }
        task.Close();
        }
    // task not found
    CleanupStack::PopAndDestroy(2); //taskIdArray, task
    CloseDSProfile();
    return KErrNotFound;
    }
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::UpdateDataSourceL
//  The function updates the LDBURI or RDBURI leaf value.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::UpdateDataSourceL( const TInt aProfileLUID, 
                                                const TInt aDataProviderId, 
                                                const TDesC8& aObject, 
                                                const HBufC8& aField)
    {
    // check that aField is RemoteDB or LocalDB
    if( aField.Compare( KNSmlDdfRDBURI ) != 0 && aField.Compare( KNSmlDdfLDBURI ) != 0)
        {
        return KErrGeneral;
        }
    
    TInt retVal = OpenSyncSessionAndDSProfileL( aProfileLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
    RArray<TSmlTaskId> taskArray;
    CleanupClosePushL( taskArray );
    iDSProfile.ListTasksL( taskArray );
    
    RSyncMLTask task;
    CleanupClosePushL( task );
    // find correct task
    for (TInt i=0; i<taskArray.Count(); i++)
        {
        task.OpenL( iDSProfile, taskArray[i] );
        if (task.DataProvider() == aDataProviderId)
            {
            // task found, get current localDataStore and remoteDataStore
            HBufC* localDataStore = task.ClientDataSource().AllocLC();
            HBufC* remoteDataStore = task.ServerDataSource().AllocLC();
            task.Close();
            
            HBufC16* objectBuffer = HBufC16::NewLC( aObject.Size() );
            TPtr16 ptrDataObject = objectBuffer->Des();
            CnvUtfConverter::ConvertToUnicodeFromUtf8( ptrDataObject, aObject );
            iDSProfile.DeleteTaskL( taskArray[i] );
            iDSProfile.UpdateL();
            
            // create new task with new values
            RSyncMLTask newTask;
            CleanupClosePushL( newTask );
            if( aField.Compare( KNSmlDdfRDBURI ) == 0 ) //Remote DB, Mandatory
                {
                newTask.CreateL( iDSProfile, aDataProviderId, ptrDataObject, *localDataStore );
                }
            else if( aField.Compare( KNSmlDdfLDBURI ) == 0 )
                {
                newTask.CreateL( iDSProfile, aDataProviderId, *remoteDataStore, ptrDataObject );
                }
            newTask.UpdateL();
            
            CloseDSProfile();
            CleanupStack::PopAndDestroy( 6 ); //taskArray, task, localDataStore, 
                                            //remoteDataStore, objectBuffer, newTask
            return retVal;
            }
        task.Close();
        }
    // task not found
    CleanupStack::PopAndDestroy(2);//taskArray, task
    CloseDSProfile();
    return KErrNotFound;
    }
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::FetchObjectL
//  The function fetches the requested leaf object data.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::FetchObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                                            CBufBase*& aObj, const TInt aStatusRef )
	{
	_DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): begin");
//
	DBG_ARGS(_S16("DS:Fetch aURI    - %S - %S"), &aURI, &aLUID);

    TInt retVal = KErrNone;
    TInt profileLUID = -1;

	iLUID = IntLUID( aLUID );

    if ( aLUID.Length() > 0 && 
	   ( aURI.Match( KCTTypeMatch ) != KErrNotFound ||
	    aURI.Match( KRDBUriMatch ) != KErrNotFound ||
	    aURI.Match( KLDBUriMatch ) != KErrNotFound ) )
	   {
		profileLUID = GetProfileIdFromURIL( aURI ); // ProfileId for DB-level
        if( FindTaskIDL( profileLUID, iLUID ) == EFalse )
            {
            _DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): ENotFound end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return KErrNotFound;
			}
        }
	else if ( aLUID.Length() > 0 ) //Profile
		{
		TInt profileID = -1; 
        TInt retValue = GetProfileIdentifierL( iLUID, profileID );
		if( retValue != KErrNone || profileID < 0 )
			{
			_DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): ENotFound end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return KErrNotFound;
			}
		iLUID = profileID;
		}
	else
		{
		// Static node feature start
		iLUID = -1;
		if( IsDSAccUriFormatMatchPredefined(aURI) )
    	{
    		iLUID = ConstructTreeL(aURI);
    		profileLUID = GetProfileIdFromURIL( aURI ); 
    	}
    	
    	if ((iLUID == 0 ) && (aURI.Match(_L8("SyncML/DSAcc/DSId*/DB/CTId*"))  == KErrNone))
		{
		_DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): ENotFound end");
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return KErrNotFound;
    	}
		// Static node feature end
		}

	SetField( aURI );
//
//	Check if valid field
//
/*	if( !AcceptDSField() )
		{
		_DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): EInvalidObject end");
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
		return KErrGeneral;
		}
*/
//
//	Check which field going to be handled
//
    TInt fType = GetDSFieldTypeL();
    if ( fType == EInt || fType == EStr )
		{
		retVal = GetDSFieldDataL( iLUID, aObj );
        }
	else if ( fType == EParam )
		{ // DB and CTType (dataprovider info / MIME type)
		  // RDBURI + LDBURI (dataprovider data paths)
	    if( iField->Compare( KNSmlDdfCTType ) == 0 )
		    {
		    retVal = GetContentTypeL( iLUID, profileLUID, aObj );
		    }
        else if( iField->Compare( KNSmlDdfLDBURI ) == 0 ) // Local database (optional)
			{
			//ClientDataSource
            retVal = GetClientDataSourceL( iLUID, profileLUID, aObj );
            }
		else if( iField->Compare( KNSmlDdfRDBURI ) == 0 ) // Remote database 
			{
            //ServerDataSource
		    retVal = GetServerDataSourceL( iLUID, profileLUID, aObj );
            }
		}
	else 
		{
		_DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): Unknown type end");
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return KErrNotFound;
		}
    if( retVal != KErrNone )
        {
        if ( retVal == KErrNotFound )
            {
			_DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): ENotFound end");
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
            }
        else
            {
			_DBG_FILE("CNSmlDSSettingsAdapter::FetchObjectL(): retVal != KErrNone");
            iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );        
            }            
        }
    return retVal;
    
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::FillNodeInfoL
//  The function fills the node or leaf information.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode, 
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
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetProfileAddrTypeL
//  The function sets the medium type for a profile.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfileAddrTypeL( const TInt aLUID, 
                                                    const TInt aIntObj )
    {
    _DBG_FILE("CNSmlDSSettingsAdapter::SetProfileAddrTypeL(): begin");
    TInt retVal = OpenSyncSessionAndDSProfileL( aLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        _DBG_FILE("CNSmlDSSettingsAdapter::SetProfileAddrTypeL(): end");
        return retVal;	
		}
    TSmlTransportId mediumType = ENone;
    GetMediumTypeL( aIntObj, mediumType );
    if( mediumType == ENone )
        {
        return KErrGeneral;
        }
    RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
    iDSProfile.ListConnectionsL( transportIdArray );

    RSyncMLConnection conn;
    conn.OpenL( iDSProfile, transportIdArray[0] );
    CleanupClosePushL( conn );

    HBufC8* sURI = conn.ServerURI().AllocLC();
    if(mediumType!=transportIdArray[0])
        {
        conn.CreateL( iDSProfile, mediumType ); //changes medium type and clears the buffer
        }
        
    if(mediumType!=KUidNSmlMediumTypeInternet.iUid && sURI->Length() > 0)
        {
        // remove port
        HBufC8* newServerURI = HBufC8::NewL( sURI->Size() );
        CleanupStack::PushL( newServerURI );
        TBuf8<5> buf;
    	buf.Num( 0 );
        // change iLeafType to EDSDelete -> deleting port
        TNSmlDSLeafType	tmpType(iLeafType); // store old value
        iLeafType = EDSDelete;
        SetPortNrToAddrStr( *sURI, newServerURI, buf );
        iLeafType = tmpType; // restore original value
        
        conn.SetServerURIL( *newServerURI );
        conn.UpdateL();
        
        CleanupStack::PopAndDestroy( newServerURI );
        }
    else
        {
        conn.SetServerURIL( *sURI );
        conn.UpdateL();
        }
                      
    CleanupStack::PopAndDestroy( 3 );//transportIdArray, conn, sURI
    CloseDSProfile();
    _DBG_FILE("CNSmlDSSettingsAdapter::SetProfileAddrTypeL(): end");
    return KErrNone;
    }
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetProfileConnectionPortNrL
//  The function sets the port number (PortNbr) for a profile.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfileConnectionPortNrL( const TInt aLUID, 
                                                            const TDesC8& aPort )
    {
    _DBG_FILE("CNSmlDSSettingsAdapter::SetProfileConnectionPortNrL(): begin");
    TInt retVal = KErrNone;
    
    if( iPortNbr )
        {
        delete iPortNbr;
        iPortNbr = NULL;
        }
    iPortNbr = HBufC8::NewL( aPort.Length() );
    TPtr8 newPortNbr = iPortNbr->Des();
    newPortNbr.Append( aPort );
        
    retVal = OpenSyncSessionAndDSProfileL( aLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        _DBG_FILE("CNSmlDSSettingsAdapter::SetProfileConnectionPortNrL(): end");
        return retVal;	
		}
    
    RArray<TSmlTransportId> transportIdArray;
    CleanupClosePushL( transportIdArray );
    iDSProfile.ListConnectionsL( transportIdArray );
    
    RSyncMLConnection conn;
    CleanupClosePushL( conn );
    conn.OpenL( iDSProfile, transportIdArray[0] );
    
    // if medium type is not internet -> do not add port
    if( transportIdArray[0] != KUidNSmlMediumTypeInternet.iUid )
        {
        CloseDSProfile();
        CleanupStack::PopAndDestroy( 2 ); //transportIdArray, conn
        _DBG_FILE("CNSmlDSSettingsAdapter::SetProfileConnectionPortNrL(): end");
        return retVal;
        }
        
    HBufC8* serverURI = conn.ServerURI().AllocLC();
    if( serverURI->Length() == 0 )
        {
        CloseDSProfile();
        CleanupStack::PopAndDestroy( 3 ); //transportIdArray, conn, serverURI
        _DBG_FILE("CNSmlDSSettingsAdapter::SetProfileConnectionPortNrL(): end");
        return retVal;
	    }
    TInt totalSize = serverURI->Size() + newPortNbr.Size() + 1;
    HBufC8* newServerURI = HBufC8::NewL( totalSize );
    CleanupStack::PushL( newServerURI );
    SetPortNrToAddrStr( *serverURI, newServerURI, newPortNbr );
    
    conn.SetServerURIL( *newServerURI );
    conn.UpdateL();
    
    CleanupStack::PopAndDestroy( 4 ); //transportIdArray, conn, serverURI, newServerURI
    CloseDSProfile();
    _DBG_FILE("CNSmlDSSettingsAdapter::SetProfileConnectionPortNrL(): end");
    return KErrNone;	
    }
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetPortNrToAddrStr
//  The function sets or replaces a portnbr (separated with ':') to Addr Uri.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::SetPortNrToAddrStr( const TDesC8& aSrvURI, 
                                                    HBufC8* aNewURI, 
                                                    const TDesC8& aPort )
	{
	TInt offset=0;
    if( aSrvURI.Find( KNSmlDSSettingsHTTP ) == 0 )//http://
		{
		offset = KNSmlDSSettingsHTTP().Length();
		}
	else if( aSrvURI.Find( KNSmlDSSettingsHTTPS ) == 0 )//https://
		{
		offset = KNSmlDSSettingsHTTPS().Length();
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
    if( portStart < 0 && separatorPos > 0 && iLeafType != EDSDelete )
        {
        ptrNewSrvURI.Append( aSrvURI.Left( separatorPos ) );//http://123.234.345.456
        ptrNewSrvURI.Append( KNSmlDMColon );
        ptrNewSrvURI.Append( aPort );
        ptrNewSrvURI.Append( aSrvURI.Right( aSrvURI.Length() - separatorPos ) );//123.234.345.456:xx(x)/syncml    
        }
    else if ( portStart > 0 && separatorPos > 0 && iLeafType != EDSDelete )
        {
        ptrNewSrvURI.Append( aSrvURI.Left( portStart + 1 ) );//123.234.345.456:
        ptrNewSrvURI.Append( aPort );//123.234.345.456:xx(x)
        ptrNewSrvURI.Append( aSrvURI.Right( aSrvURI.Length() - separatorPos ) );//123.234.345.456:xx(x)/syncml
        }
    else if ( portStart > 0 &&  iLeafType == EDSDelete ) //delete port number from uri
        {
        ptrNewSrvURI.Append( aSrvURI.Left( portStart ) );//123.234.345.456
        ptrNewSrvURI.Append( aSrvURI.Right( aSrvURI.Length() - separatorPos ) );//123.234.345.456:xx(x)/syncml
        }
    else
        {
        ptrNewSrvURI.Append( aSrvURI );
        }
    return;
	}
//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::MimeTypeAllreadyExitsL
//  The function checks if mime type allready exits in this profile.
//------------------------------------------------------------------------------
TBool CNSmlDSSettingsAdapter::MimeTypeAllreadyExitsL( const TInt aLuid, 
                                                        const TDesC8& aMimeType )
    {
    TInt retVal = OpenSyncSessionAndDSProfileL( aLuid, ESmlOpenRead );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return EFalse;	
		}
    RArray<TSmlDataProviderId> dataProviderArray;
    CleanupClosePushL( dataProviderArray );
    iSyncSession.ListDataProvidersL( dataProviderArray );
    TInt dataProviderCount = dataProviderArray.Count();
    if( dataProviderCount == 0 )
        {//error case, data provider not found
        CloseDSProfile();
        CleanupStack::PopAndDestroy();//dataProviderIdArray
        return EFalse;   
        }

    RSyncMLTask task;
    RSyncMLDataProvider dataProvider;
    RArray<TSmlTaskId> taskIdArray;
    iDSProfile.ListTasksL( taskIdArray );
    CleanupClosePushL(taskIdArray);
    
    HBufC8* object = aMimeType.AllocLC();
    TPtr8 objectPtr = object->Des();
	objectPtr.LowerCase();
    
    for( TInt i = 0; i<taskIdArray.Count(); i++ )
        {
        task.OpenL( iDSProfile, taskIdArray[i] );
        CleanupClosePushL(task);
        TInt dataProviderUID = task.DataProvider();
        TInt dataProviderIndex = dataProviderArray.Find( dataProviderUID );
        if( dataProviderIndex != KErrNotFound )
            {
            dataProvider.OpenL( iSyncSession, dataProviderArray[dataProviderIndex] );
            CleanupClosePushL(dataProvider);
            for ( TInt n = 0; n < dataProvider.MimeTypeCount(); n++ )
                {
                HBufC* mimeType = dataProvider.MimeType(n).AllocLC();
                if( objectPtr.Compare( ConvertTo8LC( *mimeType ) ) == 0 ) //if MIME match
                    {
                    CleanupStack::PopAndDestroy( 7 ); //dataProviderArray, task, dataProvider,
                                                    //taskIdArray, object, mimeType, ConvertTo8LC    
                    CloseDSProfile();            
                    return ETrue;
                    }
                CleanupStack::PopAndDestroy( 2 ); //mimeType, ConvertTo8LC
                }
            CleanupStack::PopAndDestroy( ); //dataProvider
            }
        CleanupStack::PopAndDestroy( ); //task
        }
    CleanupStack::PopAndDestroy( 3 ); //dataProviderArray,taskIdArray, object
    CloseDSProfile();
    return EFalse;
    }

//------------------------------------------------------------------------------
//  CNSmlDSSettingsAdapter::SetRDBDataStore
//  The function sets the RDMURI to memeber variable.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::SetRDBDataStore( const TDesC8& aStore )
    {
	TPtr16 bufPtr16 = iRdbDataStore->Des();
    bufPtr16.Delete(0, bufPtr16.Size());
	CnvUtfConverter::ConvertToUnicodeFromUtf8( bufPtr16, aStore );
    }

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::SetProfileIAPIdL
//	The function sets the IAPId value to profile connection properties.
//------------------------------------------------------------------------------
TInt CNSmlDSSettingsAdapter::SetProfileIAPIdL( const TInt aLUID, const TInt aIAPid )
    {
    TInt retVal = OpenSyncSessionAndDSProfileL( aLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
		CloseDSProfile();
        return retVal;	
		}
    RArray<TSmlTransportId> connIdArray;
    CleanupClosePushL( connIdArray );
    iDSProfile.ListConnectionsL( connIdArray );
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
	        conn.OpenL( iDSProfile, connIdArray[0] );
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
        CleanupStack::PopAndDestroy(); //transport
        }
    else
        {
        retVal = KErrNotFound;
        }
    CloseDSProfile();
    CleanupStack::PopAndDestroy(); //connIdArray
    return retVal;
    }

//------------------------------------------------------------------------------
//	CNSmlDSSettingsAdapter::GetMediumTypeL
//	The function checks the requested medium type and gets it UID.
//------------------------------------------------------------------------------
void CNSmlDSSettingsAdapter::GetMediumTypeL( const TInt aIntObj, 
                                            TSmlTransportId& aMediumType )
    {
    if( aIntObj == EHttp|| aIntObj == ENone || aIntObj == EWsp)
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
    return;    
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//------------------------------------------------------------------------------
// TImplementationProxy ImplementationTable[]
//------------------------------------------------------------------------------

#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)	{{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY( KNSmlDSSettingsAdapterImplUid, CNSmlDSSettingsAdapter::NewL )
    };

//------------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
//------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	_DBG_FILE("ImplementationGroupProxy() for CNSmlDSSettingsAdapter: begin");

    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	_DBG_FILE("ImplementationGroupProxy() for CNSmlDSSettingsAdapter: end");
    return ImplementationTable;
	}


//--------------------------------------------------------------------
//TInt CNSmlDMSettingsAdapter::IsDSAccUriFormatMatchPredefined(const TDesC8 & aURI)
//
//-------------------------------------------------------------------

TBool CNSmlDSSettingsAdapter::IsDSAccUriFormatMatchPredefined(const TDesC8 & aURI)
{
	
	// include strings for  tree for Bearer , DNS related Dynamic nodes . 
	
	if(aURI.Match(_L8("SyncML/DSAcc/DSId*"))  != KErrNotFound ||
         aURI.Match(_L8("SyncML/DSAcc/DSId*/DB/CTId*"))  != KErrNotFound)
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
	
	
}

//--------------------------------------------------------------------
//TInt CNSmlDMSettingsAdapter::ConstructTreeL(const TDesC8& aURI)
//
//-------------------------------------------------------------------

TInt CNSmlDSSettingsAdapter::ConstructTreeL(const TDesC8& aURI)
{
		TInt profileID = KErrNotFound;
RDebug::Print( _L("jshong FOTA : ConstructTreeL 1 ")) ;
		TBool notDBSet(EFalse);
        if( iSyncSessionOpen == EFalse )
            {
            iSyncSession.OpenL();
            iSyncSessionOpen = ETrue;
            }
        RArray<TSmlProfileId> profileIdArray;
        CleanupClosePushL( profileIdArray );
        TSmlUsageType usageType = ESmlDataSync;
        iSyncSession.ListProfilesL( profileIdArray, usageType );
RDebug::Print( _L("jshong FOTA : ConstructTreeL 1-2 ") );
	    
        for( TInt p = 0; p < profileIdArray.Count(); p++ )
		    {
            TInt ddId = profileIdArray[p] ; // update remove KMAxDatasyncID - KMaxDataSyncID;
RDebug::Print( _L("jshong FOTA : ConstructTreeL 1-3 ") );
            
		    _LIT8( Kprev, "DSId" );//  update changed from DMId to DSId
		    TBuf8<7> addNAME(Kprev); // DMIdnnn , nnn = profileid
            TInt fixedProfId = profileIdArray[p] ; // update remove KMAxDatasyncID // - KMaxDataSyncID; //fit to 3 decimal
		    addNAME.AppendNumFixedWidth( fixedProfId, EDecimal, 3 ); 
   		   	DBG_ARGS8( _S8("notInList: Id = %d Name %S"), p, &addNAME );
   		   	RDebug::Print( _L("notInList: Id = %d Name %S"), p, &addNAME );
		    // KNSmlDMStart includes start text for URISeg
			    TBuf8<20> addURI; // SyncML/DMAcc/DMIdnnn , nnn = profileid
			    addURI.Append( KNSmlDSStart );
			    addURI.AppendNumFixedWidth( fixedProfId, EDecimal, 3 ); 
				TBuf8<3> addLUID;
	    		addLUID.Num( fixedProfId );

			HBufC8* mapInfo = iCallBack->GetLuidAllocL(addURI);
RDebug::Print( _L("jshong FOTA : ConstructTreeL 1-4 addURI = %S"), &addURI );
					
						 if (mapInfo->Length() == 0)
						 {
						 	 iCallBack->SetMappingL(addURI,addLUID);
							 
RDebug::Print( _L("jshong FOTA : ConstructTreeL 1-5 ") );
						 	 	 
						 	 if(( aURI.Find(addURI) >= 0 ) && !notDBSet)
                    			{ 
                    				profileID = fixedProfId;
                    			}
                    			
						 	 // update , Update ../DB/CTId also 
						 	 	TInt retVal = OpenSyncSessionAndDSProfileL( fixedProfId, ESmlOpenRead );
						    if( retVal == KErrNone )
								{
							
RDebug::Print( _L("jshong FOTA : ConstructTreeL 2 ") );
						        
						 	 						 	 
						    RArray<TSmlTaskId> taskIdArray;
						    CleanupClosePushL( taskIdArray );
						    iDSProfile.ListTasksL( taskIdArray );
							if( taskIdArray.Count() == 0 )
								{
RDebug::Print( _L("jshong FOTA : ConstructTreeL 3 ")) ;
								CloseDSProfile();
								CleanupStack::PopAndDestroy();//taskIdArray
						    //	return KErrNotFound;
								}
							else
							{
								RSyncMLTask task;
RDebug::Print( _L("jshong FOTA : ConstructTreeL 4 ")) ;

						    CleanupClosePushL( task );
						            
						    for ( TInt i = 0; i < taskIdArray.Count(); i++ )
								{			
								//	
								//	Check if ContentType -> Task ID on list
								//
								TInt ddId( 0 );
RDebug::Print( _L("jshong FOTA : ConstructTreeL 5 ")) ;
								
								task.OpenL( iDSProfile, taskIdArray[i] );
						        ddId = task.DataProvider();
						        task.Close();
								
									//Get Task ID
								//	addNAME.AppendNumFixedWidth( taskIdArray[i], EDecimal,3 ); 
RDebug::Print( _L("jshong FOTA : ConstructTreeL 6 ") );
							
									TBuf8<80> addDBURI;
									
									addDBURI.Append( addURI );
									_LIT8( KDB, "/DB" );// addDb also 
									addDBURI.Append( KDB );
									addDBURI.Append( Kprev2 );
									addDBURI.AppendNumFixedWidth( taskIdArray[i], EDecimal, 3 ); 
RDebug::Print( _L("jshong FOTA : ConstructTreeL 7 ")) ;

									TBuf8<16> addDBLUID;
									addDBLUID.Num( ddId );
									HBufC8* mapInfo = iCallBack->GetLuidAllocL(addDBURI);
								 	if (mapInfo->Length() == 0)
						 			{
						 	 			iCallBack->SetMappingL(addDBURI,addDBLUID);
					 	 	 
						 	 			if( aURI.Find(addDBURI) >= 0 )
                    					{ 
                    						profileID = ddId;
                    						notDBSet = ETrue;
                    					}
						 			}							
								
								} //For
							CloseDSProfile();
							CleanupStack::PopAndDestroy(2);// task, taskIdArray
							}
							
						 //return KErrGeneral;	
								}	 
							else
							{
									CloseDSProfile();
						 }
						 
						 }
						 else
						 {
RDebug::Print( _L("jshong FOTA : ConstructTreeL 1-5 ") );
						 
						 	delete mapInfo;
						 }
						 
	    			
    			}
	    	
         CleanupStack::PopAndDestroy( 1 );//profileIdArray
        return profileID;
}

//------------------------------------------------------------------------------
// TPtrC8 CUtils::GetDynamicDSNodeUri( const TDesC8& aURI )
// returns Syncml/DSAcc/xxx URI
//------------------------------------------------------------------------------
TPtrC8 CNSmlDSSettingsAdapter::GetDynamicDSNodeUri(const TDesC8& aURI)
    {    
    TInt i= 0;
    TBuf8<50> DsAccRoot(KDSAcc1);
    for ( i = aURI.Find( KDSAcc1 ) + DsAccRoot.Length() + 1 ; i < aURI.Length(); i++ )
        {
        if( aURI[i] == '/'  )            
			{            break;            
			}
        }    
		
    return aURI.Left( i );
    }
	
// End of File

