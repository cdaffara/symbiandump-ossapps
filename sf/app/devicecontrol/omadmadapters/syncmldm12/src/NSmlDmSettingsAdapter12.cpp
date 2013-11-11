/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "NSmlDMSettingsAdapter12.h"
#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldmconstants.h"
#include "nsmldmiapmatcher.h"
#include "nsmldmauthinfo.h"
#include <featmgr.h>
#include <implementationproxy.h>
#include <SyncMLClientDM.h>
#include <SyncMLTransportProperties.h>
#include <e32base.h>
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

const TUint KNSmlDMSettingsAdapterImplUid = 0x10282CE8;

const TInt KBufSize32   = 32;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::NewL
//  Two-phased constructor.
// -----------------------------------------------------------------------------
CNSmlDMSettingsAdapter12* CNSmlDMSettingsAdapter12::NewL( MSmlDmCallback* aDmCallback )
	{
	_DBG_FILE("CNSmlDMSettingsAdapter12::NewL(): begin");

	CNSmlDMSettingsAdapter12* self = NewLC( aDmCallback );
	CleanupStack::Pop();

	_DBG_FILE("CNSmlDMSettingsAdapter12::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::NewLC( )
//  Loads the CNSmlDMSettingsAdapter12 class and pushes it to clenupstack.
// -----------------------------------------------------------------------------
CNSmlDMSettingsAdapter12* CNSmlDMSettingsAdapter12::NewLC( MSmlDmCallback* aDmCallback )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::NewLC(): begin" );
	CNSmlDMSettingsAdapter12* self = new ( ELeave ) CNSmlDMSettingsAdapter12( aDmCallback );
	CleanupStack::PushL( self );
    self->iCallBack = aDmCallback;
	self->ConstructL();
	self->iLeafType = EDMUnset;

	_DBG_FILE( "CNSmlDMSettingsAdapter12::NewLC(): end" );
	return self;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::CNSmlDMSettingsAdapter()
//  Default constructor.
// -----------------------------------------------------------------------------
CNSmlDMSettingsAdapter12::CNSmlDMSettingsAdapter12( TAny* aEcomArguments) : 
                                                CSmlDmAdapter(aEcomArguments )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::CNSmlDMSettingsAdapter(): begin" );
	_DBG_FILE( "CNSmlDMSettingsAdapter12::CNSmlDMSettingsAdapter(): end" );
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::~CNSmlDMSettingsAdapter()
//  Destructor.
// -----------------------------------------------------------------------------
CNSmlDMSettingsAdapter12::~CNSmlDMSettingsAdapter12()
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::~CNSmlDMSettingsAdapter(): begin" );

    delete iField;
	delete iURIField;
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
    iProfileLockBuffer.Reset();
    iProfileLockBuffer.Close();
        
    iPrivateApi.Close();
    
    iCallBack = 0; 
    FeatureManager::UnInitializeLib();            
	_DBG_FILE( "CNSmlDMSettingsAdapter12::~CNSmlDMSettingsAdapter(): end" );
	}

// -----------------------------------------------------------------------------
//		CNSmlDMSettingsAdapter12::ConstructL()
//		Second phase constructor.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::ConstructL()
	{
	iField = HBufC8::NewL( KNSmlMaxURLLength );
	iURIField = NULL;	
    iSyncSessionOpen = EFalse;
	FeatureManager::InitializeLibL();
	// variables used for Buffering
	iBufOn = EFalse;
	iComplete = EFalse;
	iExecutionIndex = -1;
	iNewProfile = EFalse;
	// initialising iBuffer
	iBuffer = new (ELeave) CArrayFixFlat <TNSmlDMBufferElement> (KNSmlDMGranularity);
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::DDFVersionL()
//  Returns the DDF version number.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::DDFVersionL( CBufBase& aVersion )
	{
	_DBG_FILE(
            "CNSmlDMSettingsAdapter12::DDFVersionL(CBufBase& aDDFVersion): begin" );
	aVersion.InsertL( 0, KVersion );
	_DBG_FILE(
            "CNSmlDMSettingsAdapter12::DDFVersionL(CBufBase& aDDFVersion): end" );
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::DDFStructureL()
//  Fills the DDF structure of the adapter.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::DDFStructureL(): begin" );
      
    //Making DMAcc as root 
    
	MSmlDmDDFObject* ddfRoot = &aDDF;
	MSmlDmDDFObject& nDmDDF = ddfRoot->AddChildObjectL( KNSmlDefDMAcc );

	TSmlDmAccessTypes *aclTypes = new ( ELeave ) TSmlDmAccessTypes();
	CleanupStack::PushL( aclTypes );
    
    // Set Get as acceptable operations
    aclTypes->SetGet();
	
	nDmDDF.SetDFTitleL( KNSmlDMadapterTitle );
	
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

    TSmlDmAccessTypes aclTypesG;
    aclTypesG.SetGet();
    
    TSmlDmAccessTypes aclTypesAG;
    aclTypesAG.SetGet();
    aclTypesAG.SetAdd();
    
    TSmlDmAccessTypes aclTypesAGD;
    aclTypesAGD.SetGet();
    aclTypesAGD.SetAdd();
    aclTypesAGD.SetDelete();
    
        
	MSmlDmDDFObject& nProDDF = nDmDDF.AddChildObjectGroupL();
    FillNodeInfoL( nProDDF, *aclTypes, MSmlDmDDFObject::EZeroOrMore, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNullDesC8, KNullDesC8, KMimeType );

    MSmlDmDDFObject& nAppIdDDF = nProDDF.AddChildObjectL( KNSmlDdfAppId );			// Addr
    FillNodeInfoL( nAppIdDDF, aclTypesG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAppIdDescription, KNullDesC8, KMimeType );

      //PrefConRef 
    MSmlDmDDFObject& nPrefConRefDDF = nProDDF.AddChildObjectL( KNSmlDdfPrefConRef );	// PrefConRef
    FillNodeInfoL( nPrefConRefDDF, aclTypesARG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfConRefDescription, KNullDesC8, KMimeType );

           
                    
   // App Address nodes 
    MSmlDmDDFObject& nAppAddrDDF = nProDDF.AddChildObjectL( KNSmlDdfAppAddr );	// AppAddr
    FillNodeInfoL( nAppAddrDDF, aclTypesG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNSmlDdfAddrDescription, KNullDesC8, KMimeType );
   
    MSmlDmDDFObject& rtToAppAddrRef = nAppAddrDDF.AddChildObjectGroupL();  //AppAddr/<X>
    FillNodeInfoL(rtToAppAddrRef,aclTypesG,MSmlDmDDFObject::EOneOrMore,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::ENode,
                  KNSmlDdfAddrDescription,KNullDesC8, KMimeType );


    MSmlDmDDFObject& nAddrForAppAddrDDF = rtToAppAddrRef.AddChildObjectL( KNSmlDdfAddr );			// Addr
    FillNodeInfoL( nAddrForAppAddrDDF, aclTypesARG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAddrDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nAddrTypeForAppAddrDDF = rtToAppAddrRef.AddChildObjectL( KNSmlDdfAddrType );	// AddrType
    FillNodeInfoL( nAddrTypeForAppAddrDDF, aclTypesG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAddrTypeDescription, KNullDesC8, KMimeType );

	MSmlDmDDFObject& nPortForAppAddrDDF = rtToAppAddrRef.AddChildObjectL( KNSmlDdfPort );	// PortNbr
    FillNodeInfoL( nPortForAppAddrDDF, aclTypesG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNSmlDdfPortNbrDescription, KNullDesC8, KMimeType );

    //Runtime port address
    MSmlDmDDFObject& nrtToPortDDF = nPortForAppAddrDDF.AddChildObjectGroupL();	// PortNbr
    FillNodeInfoL( nrtToPortDDF, aclTypesG, MSmlDmDDFObject::EOneOrMore, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNSmlDdfPortNbrDescription, KNullDesC8, KMimeType );
    
    MSmlDmDDFObject& nPortNbrToPortDDF = nrtToPortDDF.AddChildObjectL( KNSmlDdfPortNbr );	// PortNbr
    FillNodeInfoL( nPortNbrToPortDDF, aclTypesARG, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EInt, 
                    KNSmlDdfPortNbrDescription, KNullDesC8, KMimeType );
                    

    MSmlDmDDFObject& nAuthSchemeDDF = nProDDF.AddChildObjectL( KNSmlDdfAuthScheme );	// AuthPref
    FillNodeInfoL( nAuthSchemeDDF, aclTypesARG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAuthSchemeDescription, KNullDesC8, KMimeType );                     
   //APPAUthNodes 
   
   MSmlDmDDFObject& nAppAuthDDF = nProDDF.AddChildObjectL( KNSmlDdfAppAuth );	// AppAuth
    FillNodeInfoL( nAppAuthDDF, aclTypesG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNSmlDdfAuthSchemeDescription, KNullDesC8, KMimeType ); 
                    
   
   MSmlDmDDFObject& nrtToAppAuthDDF = nAppAuthDDF.AddChildObjectGroupL();	// appauth/<x>
    FillNodeInfoL( nrtToAppAuthDDF, aclTypesG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNSmlDdfAuthSchemeDescription, KNullDesC8, KMimeType ); 


   MSmlDmDDFObject& nAauthLevelDDF = nrtToAppAuthDDF.AddChildObjectL( KNSmlDdfAAuthLevel );	// AAuthLevel
    FillNodeInfoL( nAauthLevelDDF, aclTypesARG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAuthLevelDescription, KNullDesC8, KMimeType );
                    
    
   MSmlDmDDFObject& nAauthtypeDDF = nrtToAppAuthDDF.AddChildObjectL( KNSmlDdfAAuthType );	// AAuthType
    FillNodeInfoL( nAauthtypeDDF, aclTypesG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAddrTypeDescription, KNullDesC8, KMimeType );
                    

   MSmlDmDDFObject& nAauthNameDDF = nrtToAppAuthDDF.AddChildObjectL( KNSmlDdfAAuthName );	// AddrType
    FillNodeInfoL( nAauthNameDDF, aclTypesARG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAuthNameDescription, KNullDesC8, KMimeType );
                    

   MSmlDmDDFObject& nAauthSecretDDF = nrtToAppAuthDDF.AddChildObjectL( KNSmlDdfAAuthSecret );	// AddrType
    FillNodeInfoL( nAauthSecretDDF, aclTypesAR, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAuthSecretDescription, KNullDesC8, KMimeType );                     

    MSmlDmDDFObject& nAauthDataDDF = nrtToAppAuthDDF.AddChildObjectL( KNSmlDdfAAuthData );	// AddrType
    FillNodeInfoL( nAauthDataDDF, aclTypesAR, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfAuthDataDescription, KNullDesC8, KMimeType ); 
                    

	MSmlDmDDFObject& nNameDDF = nProDDF.AddChildObjectL( KNSmlDdfName );	// Name
    FillNodeInfoL( nNameDDF, aclTypesARG, MSmlDmDDFObject::EZeroOrOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfNameDescription, KNullDesC8, KMimeType );
                    
	

	MSmlDmDDFObject& nServerIdDDF = nProDDF.AddChildObjectL( KNSmlDdfServerId );	// ServerId
    FillNodeInfoL( nServerIdDDF, aclTypesAG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfServerIdDescription, KNullDesC8, KMimeType );

   	MSmlDmDDFObject& nextDDF = nProDDF.AddChildObjectL( KNSmlDdfExt );	// Ext
    FillNodeInfoL( nextDDF, aclTypesG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, 
                    KNSmlDdfServerIdDescription, KNullDesC8, KMimeType );
                    
   MSmlDmDDFObject& nlockedDDF = nextDDF.AddChildObjectL( KNSmlDdfLocked );	// locked
    FillNodeInfoL( nlockedDDF, aclTypesARG, MSmlDmDDFObject::EOne, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, 
                    KNSmlDdfServerIdDescription, KNullDesC8, KMimeType );                 

	CleanupStack::PopAndDestroy(); //aclTypes
	
	_DBG_FILE("CNSmlDMSettingsAdapter12::DDFStructureL(): end");
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::UpdateLeafObjectL
//  Updates profile leaf object data based on URI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::UpdateLeafObjectL( const TDesC8& aURI,
                                                const TDesC8& aLUID, 
                                                const TDesC8& aObject, 
                                                const TDesC8& /*aType*/, 
                                                const TInt aStatusRef )
	{
	_DBG_FILE("CNSmlDMSettingsAdapter12::UpdateLeafObjectL(): begin");
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
			TInt parentLUID = GetIntObject( aLUID ); 
			if( parentLUID < 0 )
			{
				if( IsDMAccUriFormatMatchPredefined(aURI) )
    		{
    			parentLUID = ConstructTreeL(aURI);
    		}
    		}
		if( parentLUID < 0 )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter12::UpdateLeafObjectL( ): ENotFound end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}

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
	SetURIInProcessL( aURI);
	if( !AcceptDMField() )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter12::AddLeafObjectL(): KErr field end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
		return;
		}

    //	Get Database field type 
	TNSmlDMFieldType fType = GetDMFieldType();
	if ( fType == EWrong )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter12::AddLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}
 	
    TNSmlDMProfileData fData = GetDMField();
    if( fData == ESyncAccepted )
        {
        _DBG_FILE( "CNSmlDMSettingsAdapter12::AddLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
        }
    if( NotValidStrLenght( fData, aObject ) )
	    {
		_DBG_FILE( "CNSmlDMSettingsAdapter12::AddLeafObjectL(): KErr length end" );
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
       
        _DBG_FILE( "CNSmlDMSettingsAdapter12::UpdateLeafObjectL():end" );
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
    else if ( fData == EProfileLock && iNewProfile )
    {
        if(rValue == KErrNone)
         iProfileLockBuffer[iProfileLockBuffer.Count()-1].iProfileLockStatusRef = aStatusRef; 	
        else
         iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotAllowed);
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
            else if(rValue == KErrAccessDenied) 
               {
                iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotAllowed);

               }
            else if (  rValue == KErrArgument)
             {
             	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject);
             	
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
	_DBG_FILE( "CNSmlDMSettingsAdapter12::UpdateLeafObjectL():end" );
    return;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::DeleteObjectL
//  Deletes whole profile or leaf object data acording to aURI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::DeleteObjectL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            const TInt aStatusRef )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteObjectL( ): begin" );

	if( aURI.Find( KNSmlDdfConRef ) != KErrNotFound )
		{	
		_DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteObjectL(): NOT ALLOWED end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}
	iLUID = IntLUID( aLUID );
	if(IsProfileLockedL(iLUID))
	{
	  _DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteObjectL(): NOT ALLOWED end" );
	  
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotAllowed );
		return;	
	}
		

    //	Check which field going to be handled
	SetField( aURI );
	SetURIInProcessL(aURI);

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
                    "CNSmlDMSettingsAdapter12::DeleteObjectL( ): notfound end");
				
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
					_DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteObjectL( ): EOk end" );
					iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
					return;
					}
				}
			}
		else
			{
			_DBG_FILE(
                "CNSmlDMSettingsAdapter12::DeleteObjectL( ): EInvalidObject end" );
			iCallBack->SetStatusL( aStatusRef,CSmlDmAdapter::EInvalidObject );
			return;
			}
		}//Delete whole Profile

	iLUID = IntLUID( aLUID );
	if ( iLUID != iParentLUID )
		{
		if( FindProfileIdL( iLUID ) == EFalse )
			{
			_DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteObjectL( ): end" );
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

	_DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteObjectL( ): end" );
	return;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::FetchLeafObjectL
//  Fetches profile leaf object data acordint to aURI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::FetchLeafObjectL( const TDesC8& aURI, 
                                                const TDesC8& aLUID, 
                                                const TDesC8& aType, 
                                                const TInt aResultsRef, 
                                                const TInt aStatusRef )
	{

	_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): begin" );
	DBG_ARGS(_S16( "DM:Fetch aURI    - %S - %S"), &aURI, &aLUID );
		
	iLUID = IntLUID( aLUID );
	if ( iLUID >= 0 )
		{
		if( FindProfileIdL( iLUID ) == EFalse )
			{
			_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): ENotFound end" );
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		}
	else
		{
		
		// Static node feature start
		iLUID = -1;
		if( IsDMAccUriFormatMatchPredefined(aURI) )
    	{
    		iLUID = ConstructTreeL(aURI);
    	}
    	
    	if (iLUID == -1) 
    	{
    			_DBG_FILE( "CNSmlDMSettingsAdapter::FetchLeafObjectL(): ENotFound end" );
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
				return;
    	}
		// Static node feature end
		}

    //	Check which field going to be handled
	SetField(aURI);
	SetURIInProcessL(aURI);

    //	Check if valid field
	if( !AcceptDMField() )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): EInvalidObject end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
		return;
		}
    /*if(iField->Compare(KNSmlDdfExt) == 0)
        {
        iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        CBufBase* extNode = CBufFlat::NewL(128);
        CleanupStack::PushL(extNode);
        iCallBack->SetResultsL( aResultsRef, *extNode, KNullDesC8 );
        CleanupStack::PopAndDestroy(extNode);
        return;
        }*/
         
    //	Get Database field type 
	TInt fType = GetDMFieldType();
	if ( fType == EWrong )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
		}

    TNSmlDMProfileData fieldId = GetDMField();
    if( fieldId == ESyncAccepted )
        {
        _DBG_FILE( "CNSmlDMSettingsAdapter12::AddLeafObjectL(): EError end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		return;
        }
    CBufBase *lObject = CBufFlat::NewL( 128 );
    CleanupStack::PushL( lObject );
    lObject->Reset();
    
    CSmlDmAdapter::TError status;

    if ( fType == EStr || fType == EInt || fType == EBin )
		{ 
		//  Check for APPId and return w7 
		TInt rValue = KErrNone;
       /* if( iField->Compare(KNSmlDdfAppId ) ==0 )
        {
        	status = CSmlDmAdapter::EOk;
        	lObject->InsertL(lObject->Size(),KNSmlDdfAppIdValue);
        	
        }*/
        
        rValue = GetObjectL( iLUID, fieldId, *lObject );
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
        _DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): ENotFound end" );
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
	_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): end" );
	return;
	}

// -----------------------------------------------------------------------------
//  CSmlDmSettingsAdapter::ChildURIListL
//  Function returns the list of profiles or leafs of the profile 
//  acording to aURI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::ChildURIListL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                                            const TInt aResultsRef, 
                                            const TInt aStatusRef  )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::ChildURIListL(): begin" );
	CBufBase *currentURISegmentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentURISegmentList );
 
	if(( aURI.Match( KDmAccMatch ) != KErrNotFound ) &&
	 (aURI.Match( KDmAccMatch3 ) == KErrNotFound ))
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
        		// Static node feature start
				checkLUID = -1;
				if( IsDMAccUriFormatMatchPredefined(aURI) )
		    	{
		    		checkLUID = ConstructTreeL(aURI);
		    	}
		    	
		    	if (checkLUID == -1) 
		    	{
		    			_DBG_FILE( "CNSmlDMSettingsAdapter::childURI(): ENotFound end" );
						iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
						 CleanupStack::PopAndDestroy(  ); // currentURISegmentList
        			_DBG_FILE( "CNSmlDMSettingsAdapter::ChildURIListL(): end" );
						return;
		    	}
		    	else
		    	{
		    		currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDMAcc );
				iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
			    iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
		    
		    	}
    	
    	
		//	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		// Static node feature start end
        	}

        CleanupStack::PopAndDestroy(  ); // currentURISegmentList
        _DBG_FILE( "CNSmlDMSettingsAdapter12::ChildURIListL(): end" );
		return;
		}
    else if(( aURI.Match( KDmAccMatch2 ) != KErrNotFound ) &&
              (aURI.Match( KDmAccMatch ) == KErrNotFound ))
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
        
    // AppAddr subtree
    else if( ( aURI.Match( KDmAccAppAddrMatch ) != KErrNotFound ) && (aURI.Match( KDmAccPortMatch ) == KErrNotFound ) && (aURI.Match( KDmAccPortFullMatch ) == KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KDmAccAppAddrId );
        }
    else if(( aURI.Match( KDmAccAppAddrFullMatch ) != KErrNotFound ) && (aURI.Match( KDmAccPortMatch ) == KErrNotFound ) && (aURI.Match( KDmAccPortFullMatch ) == KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDMAccAppAddr );
        }   
    else if(( aURI.Match( KDmAccPortMatch ) != KErrNotFound ) && (aURI.Match( KDmAccPortFullMatch ) == KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDMAccPortRT );
        } 
   else if( aURI.Match( KDmAccPortFullMatch ) != KErrNotFound )
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDMAccport );
        } 
          
   else if(( aURI.Match( KDmAccAppAuthMatch ) != KErrNotFound ) && (aURI.Match( KDmAccAppAuthFullMatch ) == KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KDmAccAppAuthNodes );
        } 
    else if((aURI.Match(KDmAccAppAuthSrvMatch) != KErrNotFound) || (aURI.Match(KDmAccAppAuthCliMatch) != KErrNotFound) || (aURI.Match(KDmAccAppAuthTransMatch) != KErrNotFound)) //|| (aURI.Match(KDmAccAppAuthObexMatch) != KErrNotFound) || (aURI.Match(KDmAccAppAuthHttpMatch) != KErrNotFound))
        {
           currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDMAccAppAuth );
        }
    else if(( aURI.Match( KDmAccAppAuthFullMatch ) != KErrNotFound ) && (aURI.Find( KDmAccAppAuthNodes ) != KErrNotFound))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDMAccAppAuth );
        } 
                         
    else if( aURI.Match( KDmAccExtMatch ) != KErrNotFound )
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDMAccLocked );
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
	_DBG_FILE( "CNSmlDMSettingsAdapter12::ChildURIListL(): end" );
	return;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::AddNodeObjectL
//  Adds new DM-settings profile to database
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::AddNodeObjectL( const TDesC8& aURI, 
                                            const TDesC8& aParentLUID, 
                                            const TInt aStatusRef )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::AddNodeObjectL(): begin" );

	if( aParentLUID.Length() > 0 )
		{
		iParentLUID = GetIntObject( aParentLUID );
        if( FindProfileIdL( iParentLUID ) != EFalse )// iProfile != NULL)
		    {
		    _DBG_FILE( "CNSmlDMSettingsAdapter12::AddNodeObjectL(): EAlreadyExists end" );
		    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
		    return;
		    }
		else
			{						
			TInt ret = iCallBack->RemoveMappingL(KNSmlDMSettingsAdapterImplUid,
							GetDynamicDMNodeUri( aURI ), ETrue );
			if(ret != KErrNone)
				{
			    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
		    	return;
				}			
			}		
		}
	iNewProfile = ETrue; 
	AddNodeBufferL (aURI, aStatusRef);
	_DBG_FILE( "CNSmlDMSettingsAdapter12::AddNodeObjectL(): end" );
	return;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::UpdateLeafObjectL( const TDesC8&, const TDesC8&, 
                                                RWriteStream*&, const TDesC8&, 
                                                const TInt )
	{
	//Not supported because data streaming is not supported by this adapter.
	}
	
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::FetchLeafObjectSizeL
//  This function fetches leaf object and returns its size.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                                    const TDesC8& aLUID, 
                                                    const TDesC8& aType, 
                                                    const TInt aResultsRef, 
                                                    const TInt aStatusRef )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectSizeL() : begin" );
    DBG_ARGS( _S16("DM:Fetch aURI    - %S - %S"), &aURI, &aLUID );
	
	iLUID = IntLUID( aLUID );
	if ( iLUID >= 0 )
		{
		if( FindProfileIdL( iLUID ) == EFalse )
			{
			_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): ENotFound end" );
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
			return;
			}
		}
	else
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): ENotFound end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
		return;
		}

    //	Check which field going to be handled
	SetField( aURI );
	SetURIInProcessL (aURI);
     
    //	Check if valid field
	if( !AcceptDMField() )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): EInvalidObject end" );
		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
		return;
		}
    /*if(aURI.Find( KNSmlDdfExt ) != KErrNotFound)
        {
        return;
        }*/
    
    //	Get Database field type 
	TInt fType = GetDMFieldType();
	if ( fType == EWrong )
		{
		_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): EError end" );
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
		_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectL(): ENotFound end" );
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
	_DBG_FILE( "CNSmlDMSettingsAdapter12::FetchLeafObjectSizeL(): end" );
	return;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::ExecuteCommandL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::ExecuteCommandL( const TDesC8&, const TDesC8&,
                                                const TDesC8&, const TDesC8&, 
                                                const TInt )
	{
	//Not supported. Command execution via DM settings adapter not possible.	
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::ExecuteCommandL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::ExecuteCommandL( const TDesC8&, const TDesC8&, 
                                                RWriteStream*&, const TDesC8&, 
                                                const TInt )
	{
	//Not supported. Command execution via DM settings adapter not possible.
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::CopyCommandL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::CopyCommandL( const TDesC8&, const TDesC8&, 
                                            const TDesC8&, const TDesC8&, 
                                            const TDesC8&, TInt )
	{
	//Not supported.
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::StartAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::StartAtomicL()
	{
	//Not supported. Handling Atomic command cycles not supported by the adapter.
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::CommitAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::CommitAtomicL()
	{
	//Not supported. Handling Atomic command cycles not supported by the adapter.
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::RollbackAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::RollbackAtomicL()
	{
	//Not supported. Handling Atomic command cycles not supported by the adapter.
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::StreamingSupport()
//  Indicates if streaming is supported.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter12::StreamingSupport( TInt& /*aItemSize*/ )
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::StreamCommittedL()
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::StreamCommittedL()
	{
	//Not supported. Streaming not supported by the DM settings adapter.
	}
// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::CompleteOutstandingCmdsL()
// If buffering used, commands must be executed before returning this function.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::CompleteOutstandingCmdsL()
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
 for (TInt i = 0;i<iProfileLockBuffer.Count(); i++)
	    {
	       TInt err = SetProfileLockL(iProfileLockBuffer[i].iProfileLockLUID, iProfileLockBuffer[i].iProfileLock) ;
   
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
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::AcceptDMField
//  This function checks if field to be handled is valid.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter12::AcceptDMField()
	{
	_DBG_FILE("CNSmlDMSettingsAdapter12::AcceptDMField(): begin");

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
		
	else if ( iField->Compare( KNSmlDdfId ) == 0 )
		{
		return ETrue;
		}
	else if ( iField->Compare(KNSmlDdfAppId) == 0 )
	    {
	    return ETrue;
	    }
	else if ( iField->Compare(KNSmlDdfPrefConRef) == 0 )
	    {
	    return ETrue;
	    }
	else if (iField->Compare(KNSmlDdfAAuthLevel) == 0 )
	    {
	    return ETrue;
	    }
  	    
    else if (iField->Compare(KNSmlDdfAAuthType) == 0 )
        {
        return ETrue;
        }
    else if (iField->Compare(KNSmlDdfAAuthName) == 0 )
        {
        return ETrue;
        }
        
    else if (iField->Compare(KNSmlDdfAAuthSecret) == 0 )
        {
        return ETrue;
        }
    else if (iField->Compare(KNSmlDdfAAuthData) == 0 )
        {
        return ETrue;
        }
    else if (iField->Compare(KNSmlDdfLocked) == 0 )
        {
        return ETrue;
        }    

	_DBG_FILE( "CNSmlDMSettingsAdapter12::AcceptDMField(): end" );
	return EFalse; // Not a valid Type
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::GetDMField
//	Matches URI fieldname to DM-database. 
// -----------------------------------------------------------------------------
TNSmlDMProfileData CNSmlDMSettingsAdapter12::GetDMField() 
	{

	if ( iField->Compare( KNSmlDdfAddrType ) == 0 )
		{
	
	     return EProfileAddrType;
	      
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
	
	else if ( iField->Compare( KNSmlDdfPrefConRef ) == 0 )
		{
		return EProfileIAPId;
		}
	
    else if ( iField->Compare( KNSmlDdfAppId ) == 0 )
		{
		return EProfileIAPId;
		}
    else if ( iField->Compare( KNSmlDdfLocked) == 0 )
		{
		return EProfileLock;
		}

    else if (iField->Compare(KNSmlDdfAAuthLevel) == 0 )
	    {
	     if(iURIField->Find(KDmAccAppAuthDyn1) != KErrNotFound)
	    return EPRofileServerCred;
	     else if (iURIField->Find(KDmAccAppAuthDyn2)!= KErrNotFound)
	    return EPRofileClientCred;
		else if (iURIField->Find(KDmAccAppAuthTransport)!= KErrNotFound)
	    return EProfileTransport;		     
	    }
  	    
    else if (iField->Compare(KNSmlDdfAAuthType) == 0 )
        {
        return EProfileAAuthType;
        }
    else if (iField->Compare(KNSmlDdfAAuthName) == 0 )
        {
        if(iURIField->Find(KDmAccAppAuthDyn1) != KErrNotFound)
	    return EProfileName;
	    else if (iURIField->Find(KDmAccAppAuthDyn2)!= KErrNotFound)
	    return EProfileDMServerUsername;
	    else if (iURIField->Find(KDmAccAppAuthTransport)!= KErrNotFound)
	    {
	       return ETransportAuthName;
	    }
	    
        // return ETrue;
        }
        
    else if (iField->Compare(KNSmlDdfAAuthSecret) == 0 )
        {
         if(iURIField->Find(KDmAccAppAuthDyn1) != KErrNotFound)
	    return ESharedSecret;
	        else if (iURIField->Find(KDmAccAppAuthDyn2)!= KErrNotFound)
	    return EProfileDMServerPassword;
		else if (iURIField->Find(KDmAccAppAuthTransport)!= KErrNotFound)
	    {
	       return EProfileTransportPassword;
	    }
        }
    else if (iField->Compare(KNSmlDdfAAuthData) == 0 )
        {
          if(iURIField->Find(KDmAccAppAuthDyn1) != KErrNotFound)
	    return EServerNonce;
	     else if (iURIField->Find(KDmAccAppAuthDyn2)!= KErrNotFound)
	    return EClientNonce;
	    else if (iURIField->Find(KDmAccAppAuthTransport)!= KErrNotFound)
	     return EProfileTransportAuthData;
        }
        		
	
	_DBG_FILE( "CNSmlDMSettingsAdapter12::GetDMField(): Error" );
	return ESyncAccepted;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::GetDMFieldType
//	Gets field data type. 
// -----------------------------------------------------------------------------
TNSmlDMFieldType CNSmlDMSettingsAdapter12::GetDMFieldType() const
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
		
	else if ( iField->Compare( KNSmlDdfId ) == 0 )
		{
		return EInt;
		}
    else if( iField->Compare(KNSmlDdfAppId ) ==0 ) 
        {
        	return EStr;
        }
	else if ( iField->Compare( KNSmlDdfPrefConRef ) == 0 )
		{
		return EInt;
		}
	else if (iField->Compare(KNSmlDdfAAuthLevel) == 0 )
	    {
	    return EStr;
	    }
  	    
    else if (iField->Compare(KNSmlDdfAAuthType) == 0 )
        {
        return EStr;
        }
    else if (iField->Compare(KNSmlDdfAAuthName) == 0 )
        {
        return EStr;
        }
        
    else if (iField->Compare(KNSmlDdfAAuthSecret) == 0 )
        {
        return EStr;
        }
    else if (iField->Compare(KNSmlDdfAAuthData) == 0 )
        {
        return EBin;
        }
        
	else if(iField->Compare(KNSmlDdfLocked)==0)
	    {
	    return EStr;
	    }
		        

	_DBG_FILE( "CNSmlDMSettingsAdapter12::GetDMFieldType(): Error" );
	return EWrong;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::IntLUID
//	Returns integer value for a profile LUID.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::IntLUID( const TDesC8& aLUID )
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
//  CNSmlDMSettingsAdapter12::GetIntObject
//	Returns integer value for a aObject.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::GetIntObject( const TDesC8& aObject )
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
//	CNSmlDMSettingsAdapter12::GetIntObject16()
//	Returns Integer Value for a 16 bit data object.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::GetIntObject16( const TDesC& aObject )
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
//  CNSmlDMSettingsAdapter12::SetIntObjectLC
//	Converts integer to HBufC8 type buffer and returns reference to it.
// -----------------------------------------------------------------------------
TDesC8& CNSmlDMSettingsAdapter12::SetIntObjectLC( const TInt& aObject )
	{
	HBufC8* buf = HBufC8::NewLC( 8 );
	TPtr8 ptrBuf = buf->Des();

	ptrBuf.Num( aObject );
    return *buf;
	}

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter12::NotValidStrLenght
//	Checks if data length is valid.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter12::NotValidStrLenght( const TNSmlDMProfileData& aProfileItem, 
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
        
		case ( EProfileHttpUsername ) :
		case (ETransportAuthName):
			{
				if( lLen > KNSmlDmMaxHttpAuthUsernameLength )
			    {
			    lBool = ETrue;
			    }
			}
		break;
		case ( EProfileHttpPassword ) :
		case (EProfileTransportPassword):
			{
				if( lLen > KNSmlDmMaxHttpAuthPasswordLength )
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
		case ( EProfileAAuthType ) :
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
		case ( EProfileAddrType ) :
	
			{
			if( lLen > 8 )
			    {
			    lBool = ETrue;	
			    }
			}
		break;
		
        case ( EServerNonce ) :
	    case ( EClientNonce ) :
		case (EProfileTransportAuthData):
	        {
            lBool = EFalse;
	        }
        break;
		case ( EProfileTransport ):
		{
			lBool = EFalse;
		}
		break;
		case EProfileLock :
		 {
		 	if(lLen > 6 )
		 	{
			    lBool = ETrue;	
			}
		 	
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
//	CNSmlDMSettingsAdapter12::SetField
//	Last element of the URI (after last '/') is returned.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetField( const TDesC8& aSource )
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
//	CNSmlDMSettingsAdapter12::SetField
//	Last element of the URI (after last '/') is returned.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetURIInProcessL( const TDesC8& aSource )
	{
    
    if( iURIField )
    {
    	delete iURIField;
    	iURIField = NULL;
    }
    iURIField = HBufC8::NewL( aSource.Length() );
    TPtr8 newPortNbr = iURIField->Des();
    newPortNbr.Append( aSource );          

    return KErrNone;
	}
	



// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::ConvertTo8LC
//	Converts string value to 8-bit and returns reference.
// -----------------------------------------------------------------------------
TDesC8& CNSmlDMSettingsAdapter12::ConvertTo8LC( const TDesC& aSource )
	{
	HBufC8* buf = HBufC8::NewLC( aSource.Length() * 2 );
	TPtr8 bufPtr = buf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return *buf;
	}

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter12::ConvertTo16LC
//	Converts string value to 16-bit and returns reference.
// -----------------------------------------------------------------------------
TDesC16& CNSmlDMSettingsAdapter12::ConvertTo16LC( const TDesC8& aSource)
	{
	HBufC16* buf16 = HBufC16::NewLC( aSource.Length() );
	TPtr16 bufPtr16 = buf16->Des();

	CnvUtfConverter::ConvertToUnicodeFromUtf8( bufPtr16, aSource );

    return *buf16; 
	}

// -------------------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::GetConRefL
//	Gets the URI for given aObject (IAP ID).
// -------------------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::GetConRefL( CBufBase& aObject )
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::GetConRefL() : begin" );
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
//  CNSmlDMSettingsAdapter12::SetConRefL()
//	Set the profile IAP Id value according to given URI.
// -------------------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetConRefL( const TInt aLUID, const TDesC8& aObject )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetConRefL() : begin" );
    CNSmlDMIAPMatcher* apmatch = CNSmlDMIAPMatcher::NewL( iCallBack );
    CleanupStack::PushL( apmatch );
    TInt lIAPid = apmatch->IAPIdFromURIL( aObject );
    CleanupStack::PopAndDestroy(); // iapmatch
    if(aObject.Length() >0 && lIAPid == KErrNotFound) 
    {
    	return KErrGeneral;
    }
    TInt apIdResponse = SetProfileConnectionPropertyL( aLUID, lIAPid );    
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetConRefL() : end" );
	return apIdResponse;
	}

// ------------------------------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::ParseUri
//  Resolves portnbr (separated with ':') as integer from Uri. 
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::ParseUri( const TDesC8& aRealURI, TInt& aPort )
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
//  CNSmlDMSettingsAdapter12::OpenSyncSessionAndDSProfileL
//  The function opens the server session and DM profile.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::OpenSyncSessionAndProfileL( const TInt aIntLUID, 
                                                        TSmlOpenMode aMode )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::OpenSyncSessionAndProfileL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::OpenSyncSessionAndProfileL(): end" );
    return rValue;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::FindProfileId
//  Function checks if profile can be found.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter12::FindProfileIdL( const TInt aIntLUID )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::FindProfileIdL(): begin" );
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
		_DBG_FILE( "CNSmlDMSettingsAdapter12::FindProfileIdL(): end" );
        return ETrue;
		}
    _DBG_FILE( "CNSmlDMSettingsAdapter12::FindProfileIdL(): end" );
	return EFalse;
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::IsServerIDExistL
//  Function checks if serverid exist.
// -----------------------------------------------------------------------------
TBool CNSmlDMSettingsAdapter12::IsServerIDExistL( const TDesC8& aServerid )
{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::IsServerIDExistL(): begin" );
	if( iSyncSessionOpen == EFalse )
	{
       	iSyncSession.OpenL();
       	iSyncSessionOpen = ETrue;
	}
    TBool ret(EFalse);
	RArray <TSmlProfileId> profileList;
    CleanupClosePushL( profileList );
	TSmlUsageType usageType = ESmlDevMan;		
	iSyncSession.ListProfilesL( profileList, usageType );
	TInt count = profileList.Count();
	if( count > 0 ) 
	{
		for(TInt i =0; i< count; i++)
		{
			TInt pid = i + KMaxDataSyncID;
			TInt index = profileList.Find(pid );
			if( index >= 0  )
			{	    		
	        	TInt rValue(KErrNone);
	        	RSyncMLDevManProfile profile;
	    		TRAP( rValue, profile.OpenL( iSyncSession, pid, ESmlOpenRead ) );
	    		if( rValue == KErrNone )
	        	{
			        CleanupClosePushL(profile);
	        		if(((aServerid.Compare(profile.ServerId())) == 0) &&
						((profile.ServerId()).Length() == aServerid.Length()))
	        		{
	        			ret = ETrue;
			            CleanupStack::PopAndDestroy(&profile);//profile
						break;	        			
	        		}						
			        CleanupStack::PopAndDestroy(&profile);//profile
				}				
			}	
		}        
	}		
    CleanupStack::PopAndDestroy(); //profileList
    _DBG_FILE( "CNSmlDMSettingsAdapter12::IsServerIDExistL(): end" );
	return ret;
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::CreateNewProfileL
//  The function creates new DM profile via client API.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::CreateNewProfileL( TInt& aPID )
	{
    _DBG_FILE("CNSmlDMSettingsAdapter12::CreateNewProfileL(): begin");
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::CreateNewProfileL(): end" );
    return KErrNone;
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileServerURIL()
//  The function sets the server URL for a profile.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileServerURIL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileServerURIL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileServerURIL(): end" );
	return KErrNone;
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileDisplayNameL()
//  The function sets the display name for a profile.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileDisplayNameL( const TInt aPID, 
                                                        const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileDisplayNameL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileDisplayNameL(): end" );
	return KErrNone;
	}
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::IsProfileLockedL()
//  The function to tell whether the profile is locked
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::IsProfileLockedL(const TInt aPID)
{
   _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileDisplayNameL(): begin" );
	TInt retVal = KErrNone;
    retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenRead );
     if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return retVal;	
		}
		
	retVal=iProfile.ProfileLocked(EFalse, EFalse);
	iCurrentProfile = 0;
    iProfile.Close();
    if(retVal)
      {
      /* CRepository* centrep = NULL;
       TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
       TInt IsPrfoileEditable = 0;
       if(!err)
         centrep->Get( KDevOverrideProfileLock, IsPrfoileEditable );
       delete centrep;
       if (!IsPrfoileEditable)
          retVal=  KErrAccessDenied;
       else
         retVal= KErrNone;*/
       retVal=  KErrAccessDenied;
      }
      
      
       
    return retVal;
    
}



// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileLockL()
//  The function to setor reset the profile lock.
// -----------------------------------------------------------------------------

TInt CNSmlDMSettingsAdapter12::SetProfileLockL( const TInt aPID, TInt aLockValue)
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileLockL(): begin" );
	TInt retVal = KErrNone;
    retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return retVal;	
		}
		 
		iProfile.ProfileLocked(ETrue, aLockValue );
    iProfile.UpdateL();
	  iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileLockL(): end" );
	return KErrNone;
	}







// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileHttpUserNameL
//  The function sets the profile http/network name.
// -----------------------------------------------------------------------------

TInt CNSmlDMSettingsAdapter12::SetProfileHttpUserNameL( const TInt aPID, const TDesC8& aObj )
{
	TInt retVal = KErrNone;
	 retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return KErrGeneral;	
		}
        
	  RSyncMLConnection connection;
	  TBuf8<KBufSize32> key; 
      RArray<TSmlTransportId> connIdArray;
      CleanupClosePushL( connIdArray );
	  iProfile.ListConnectionsL( connIdArray );
      if( connIdArray.Count() == 1 )
        {
        
		connection.OpenL( iProfile, KUidNSmlMediumTypeInternet.iUid );
		CleanupClosePushL( connection );
		
		connection.SetPropertyL( KNSmlHTTPAuth, KNSmlTrueVal );
		connection.SetPropertyL( KNSmlHTTPUsername, aObj );
		connection.UpdateL();
		CleanupStack::PopAndDestroy(  );
		}
	  else
		{
		connection.SetPropertyL( KNSmlHTTPAuth, KNSmlFalseVal );
		connection.UpdateL();
		}
		
		       
        CleanupStack::PopAndDestroy(   );
    
	return retVal;
	
}
    
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileUserNameL
//  The function sets the profile user name.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileUserNameL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileUserNameL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileUserNameL(): end" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileProtocolVersionL
//  The function sets the protocol version to be supported.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileProtocolVersionL( const TInt aPID, 
                                                            const TDesC8& )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileProtocolVersionL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileProtocolVersionL(): begin" );
	return iError;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileServerIdL
//  The function sets the profile ServerId value.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileServerIdL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
    _DBG_FILE("CNSmlDMSettingsAdapter12::SetProfileServerIdL(): begin");
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileServerIdL(): end" );
	return retVal;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileServerPasswordL
//  The function sets the profile server password.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileServerPasswordL( const TInt aPID, 
                                                        const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileServerPasswordL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileServerPasswordL(): end" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileHttpPasswordL
//  The function sets the profile network/http password.
// -----------------------------------------------------------------------------


  TInt CNSmlDMSettingsAdapter12::SetProfileHttpPasswordL( const TInt aPID, const TDesC8& aObj )
  {
 TInt retVal = KErrNone;
	 retVal = OpenSyncSessionAndProfileL( aPID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        return KErrGeneral;	
		}
        
	  RSyncMLConnection connection;
	  TBuf8<KBufSize32> key; 
      RArray<TSmlTransportId> connIdArray;
      CleanupClosePushL( connIdArray );
	  iProfile.ListConnectionsL( connIdArray );
      if( connIdArray.Count() == 1 )
        {
        
		connection.OpenL( iProfile, KUidNSmlMediumTypeInternet.iUid );
		CleanupClosePushL( connection );
		
		connection.SetPropertyL( KNSmlHTTPAuth, KNSmlTrueVal );
		connection.SetPropertyL( KNSmlHTTPPassword, aObj );
		connection.UpdateL();
		CleanupStack::PopAndDestroy(  );
		}
	  else
		{
	//	connection.SetPropertyL( KNSmlHTTPAuth, KNSmlFalseVal );
		connection.UpdateL();
		}
		
		       
        CleanupStack::PopAndDestroy(   );
    
	return retVal;
	
	
  	
  }
  
  
// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfilePasswordL
//  The function sets the profile password.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfilePasswordL( const TInt aPID, 
                                                    const TDesC8& aObj )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfilePasswordL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfilePasswordL(): begin" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetProfileConnectionPropertyL
//  This function sets the profile IAP id.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileConnectionPropertyL( const TInt aLUID, 
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
//  CNSmlDMSettingsAdapter12::DeleteProfileL
//  The function deleted the profile from DM database.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::DeleteProfileL( const TInt aPID )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteProfileL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteProfileL(): end" );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::GetObjectL
//  The function checks which leaf data to be fetched.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::GetObjectL( TInt aLUID, TNSmlDMProfileData aDMField, 
                                            CBufBase& aObject )
    {
    TInt retVal = KErrNone;
    if( iField->Compare(KNSmlDdfAppId ) ==0 )
        {
         	aObject.InsertL(aObject.Size(),KNSmlDdfAppIdValue);
         	return retVal;
        	
        }
        
    retVal = OpenSyncSessionAndProfileL( aLUID, ESmlOpenRead );
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
		case ( EProfileAddrType ) :
		    {
		    aObject.InsertL(aObject.Size(),KNSmlAddrtype);
		    }
		   break;
		case (EProfileHttpUsername ):
		    {
		       retVal = GetHttpUsernameL(aObject);
		    }
		break;
		case (EPRofileServerCred) :
		    {
		    aObject.InsertL(aObject.Size(),KDmAccAppAuthLevelSrv);
		    
		    }
		  break;
        case (EPRofileClientCred) :
		    {
		    aObject.InsertL(aObject.Size(),KDmAccAppAuthLevelCli);
		    }
		  break;
		  
		case (ETransportAuthName) :
		    {
    			TInt mediumType= ENone;
    			GetProfileConnectiontypeL( mediumType );
    			if( mediumType == EHttp)
    			{
    		    retVal = GetHttpUsernameL(aObject);
    		    }
    		    else if(mediumType == EObex)
    		    {
    		      aObject.InsertL(aObject.Size(),KNSmlDMNullDesc8);
    		    }
		    
		    }
		break;
		
		
		case (EProfileTransport) :
		{
			TInt mediumType= ENone;
			GetProfileConnectiontypeL( mediumType );
			if( mediumType == EHttp)
			{
		    aObject.InsertL(aObject.Size(),KDmAccAppAuthLevelHttp);
		    }
		    else if(mediumType == EObex)
		    {
		      aObject.InsertL(aObject.Size(),KDmAccAppAuthLevelObx);
		    }
            
		}

       break;
        case ( EAuthenticationRequired ) :
		case ( EProfileAAuthType ):
        case ( EServerNonce ) :
        case ( EClientNonce ) :
            {
            retVal = GetDMAuthInfoL( aLUID, aDMField, aObject );
            }
        break;
        
     case EProfileLock :
      {
        	
       TBool isProfileLocked = iProfile.ProfileLocked(EFalse, EFalse);
       if(isProfileLocked)
       {
       	aObject.InsertL(aObject.Size(),KDmProfileLocked);
       }
       else
       {
       	aObject.InsertL(aObject.Size(),KDmProfileUnLocked);
       }
       
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
//  CNSmlDMSettingsAdapter12::SetProfileConnectionPortNrL()
//  The function sets the profile port number.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileConnectionPortNrL( const TInt aLUID, 
                                                            const TDesC8& aPort )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileConnectionPortNrL(): begin" );
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
        _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileConnectionPortNrL(): end" );
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
        _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileConnectionPortNrL(): end" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileConnectionPortNrL(): end" );
    return retVal;	
    }

// ------------------------------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetPortNrToAddrStr
//  Sets a portnbr (separated with ':') to profile server URI. 
// ------------------------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::SetPortNrToAddrStr( const TDesC8& aSrvURI, HBufC8* aNewURI, 
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
//  CNSmlDMSettingsAdapter12::SetDMFieldDataL
//  Selects and Sets value to a field based on field id got from URI.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetDMFieldDataL( const TInt aLUID, const TDesC8& aObject, 
                                                const TNSmlDMProfileData aField )
    {
    TInt rValue = KErrGeneral;
    TInt lockvalue =0;
     if(aField != EProfileLock)
    {
      rValue=IsProfileLockedL(aLUID);	
      if (rValue!= KErrNone)
      {
       return rValue;
      }
         
     }
     else if (aField == EProfileLock)
     {
     	 if (! iNewProfile)
     	  {
     	  	 lockvalue =0;
     	  	  if(aObject.Length()> 1)
     	  	  {
     	  	    rValue = KErrArgument;
              	 return rValue;
     	  	  }
           lockvalue = GetIntObject(aObject);
           if(lockvalue != 0 && lockvalue != 1)
               {
               	  //lockvalue = 0; 
               	  rValue = KErrArgument;
               	 return rValue;
               }  
     	  	 rValue  = SetProfileLockL(aLUID , lockvalue);
     	  	 return rValue;
     	  }
      
     }
      
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
		case ( ETransportAuthName ):
		{
			TInt retVal = OpenSyncSessionAndProfileL( aLUID, ESmlOpenReadWrite );
    	if( retVal != KErrNone )
			{
	    	iCurrentProfile = 0;
				iProfile.Close();
        _DBG_FILE( "CNSmlDMSettingsAdapter12::SetDMFieldDataL(): end" );
        return retVal;	
			}
		    
		  TInt mediumType = ENone;
	    GetProfileConnectiontypeL( mediumType );
			if( mediumType == EHttp)
			{
		    rValue = SetProfileHttpUserNameL ( aLUID, aObject );
		  }
		  else if(mediumType == EObex)
		  {
		     rValue = KErrNone;		   
		  }		
		}
		break;
		case ( EProfileTransportPassword ):
		{
		 	TInt mediumType = ENone;
		 	TInt retVal = OpenSyncSessionAndProfileL( aLUID, ESmlOpenReadWrite );
    	if( retVal != KErrNone )
			{
	    	iCurrentProfile = 0;
				iProfile.Close();
        _DBG_FILE( "CNSmlDMSettingsAdapter12::SetDMFieldDataL(): end" );
        return retVal;	
			}		  
	    GetProfileConnectiontypeL( mediumType );
			if( mediumType == EHttp)
			{
		    rValue = SetProfileHttpPasswordL ( aLUID, aObject );
		  }
		  else if(mediumType == EObex)
		  {
		     rValue = KErrNone;		    
		  }		
		}
		break;
		case (EProfileTransportAuthData) :
			{
				rValue = KErrNone;
			}
		break;
		
		case ( EProfileHttpUsername ) :
		{
			rValue = SetProfileHttpUserNameL ( aLUID, aObject );
		}
		break;
		case ( EProfileHttpPassword ) :
		{
			rValue = SetProfileHttpPasswordL ( aLUID, aObject );
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
		
		case ( EProfileAddrType ) :
			{
			  if (aObject.Match(KNSmlAddrtype)!=KErrNotFound)
			  rValue=KErrNone;
			  
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
		case ( EProfileTransport) :
		{
			if (aObject.Match(KDmAccAppAuthLevelHttp)!=KErrNotFound)
			{
				iObject = EHttp;
			}
			else if (aObject.Match(KDmAccAppAuthLevelObx)!=KErrNotFound)
			{
				iObject = EObex;
			}
		 rValue = SetProfileAddrTypeL( aLUID, iObject );	
		}
		break;
        case ( EAuthenticationRequired ) :
        case ( EClientNonce ) :
        case ( EServerNonce ) :
		break;
     case ( EProfileLock ) :
              lockvalue =0;
              lockvalue = GetIntObject(aObject);
             if(lockvalue != 0 && lockvalue != 1)
                   lockvalue = 0; 
            
            TLockBuffer element;
	    	element.iProfileLock=lockvalue;
	    	element.iProfileLockLUID = aLUID;
	    	iProfileLockBuffer.Append(element);
	    	rValue=KErrNone;
           
             
          // rValue  = SetProfileLock(aLUID , lockvalue);
     break;               
        default:
            rValue = KErrNotSupported;
        }
    return rValue;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::FillNodeInfoL
//  The function fills the node or leaf information.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::FillNodeInfoL( MSmlDmDDFObject& aNode, 
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
//  CNSmlDMSettingsAdapter12::GetDMAuthInfoL
//  This function gets authentication data from private API.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::GetDMAuthInfoL( const TInt aLUID, 
                                                const TNSmlDMProfileData aField, 
                                                CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetDMAuthInfoL(): begin" );
   
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
	    else if (( aField == EAuthenticationRequired ) || (aField == EProfileAAuthType))
	        {
	        TUint authPref = authInfo->iAuthPref;
	        /*if( authPref == ENoAuth )
	            {
	            aObject.InsertL( aObject.Size(), KNSmlDMAuthPrefNoAuth ); 
	            }*/
	         if( authPref == EBasic )
	            {
	            if(aField == EAuthenticationRequired)
	            {
	            aObject.InsertL( aObject.Size(), KNSmlDMAuthPrefBasic );	
	            }
	            else
	            {
	            aObject.InsertL( aObject.Size(), KNSmlDMAuthTypeBasic );	
	            }
	            
	            }
	        else if( authPref == EMD5 )
	            {
	            if(aField == EAuthenticationRequired)
	            {
	            aObject.InsertL( aObject.Size(), KNSmlDMAuthPrefMD5 );	
	            }
	            else
	            {
				aObject.InsertL( aObject.Size(), KNSmlDMAuthTypeMD5 );
	            }
	            
	            }
	        else
	            {
	            aObject.InsertL( aObject.Size(), KNullDesC8 );
	            }
	        }
    	}
    iPrivateApi.Close();
    CleanupStack::PopAndDestroy(authInfo);
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetDMAuthInfoL(): end" );
    return ret;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::DeleteDMAuthInfoL
//  This function resets the authentication data.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::DeleteDMAuthInfoL( const TInt aLUID, 
                                                const TNSmlDMProfileData aField )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteDMAuthInfoL(): begin" );
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
	_DBG_FILE( "CNSmlDMSettingsAdapter12::DeleteDMAuthInfoL(): end" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::SetProfileAddrTypeL
// This function sets the AddrType value.    
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::SetProfileAddrTypeL( const TInt aLUID, 
                                                    const TInt aIntObj )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileAddrTypeL(): begin" );
    TInt retVal = OpenSyncSessionAndProfileL( aLUID, ESmlOpenReadWrite );
    if( retVal != KErrNone )
		{
	    iCurrentProfile = 0;
		iProfile.Close();
        _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileAddrTypeL(): end" );
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
    HBufC8* sURI = conn.ServerURI().AllocLC();
    if(mediumType!=transportIdArray[0])
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdSyncMlDmObex  ) )
            {
            CleanupStack::PopAndDestroy( 3 );//transportIdArray, conn, sURI
            return EInvalidObject;
            }
        else
            {
            conn.CreateL(iProfile,mediumType);
            }
        }
    conn.SetServerURIL( *sURI );
    conn.UpdateL(); 	                    
    CleanupStack::PopAndDestroy( 3 );//transportIdArray, conn, sURI
    iCurrentProfile = 0;
    iProfile.Close();
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetProfileAddrTypeL(): end" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::GetHttpUsernameL
// This function gets the httpuser value.    
// -----------------------------------------------------------------------------
	

TInt CNSmlDMSettingsAdapter12::GetHttpUsernameL( CBufBase& aObject )
	{
	
	  TInt retVal = KErrNone;
      TBuf8<KBufSize32> key; 
      TInt intValue;
      
      RArray<TSmlTransportId> connIdArray;
      CleanupClosePushL( connIdArray );
	  iProfile.ListConnectionsL( connIdArray );
      if( connIdArray.Count() == 1 )
        {
        RSyncMLTransport transport;
        CleanupClosePushL( transport );
	    transport.OpenL( iSyncSession, connIdArray[0] );
    	const CSyncMLTransportPropertiesArray&  arr = transport.Properties();
		const TSyncMLTransportPropertyInfo& info = arr.At( EPropertyHttpUsed );
		key = info.iName;
		RSyncMLConnection conn;
        CleanupClosePushL( conn );
	    conn.OpenL( iProfile, connIdArray[0] );
       	HBufC8* value = conn.GetPropertyL( key ).AllocLC();
        intValue = GetIntObject( *value );
        CleanupStack::PopAndDestroy(  ); //value
        if ( intValue == 1 )
        	{
                retVal = KErrNone;
                const TSyncMLTransportPropertyInfo& info2 = arr.At( EPropertyHttpUserName );
                key= info2.iName;
                HBufC8* value = conn.GetPropertyL( key ).AllocLC();
                aObject.InsertL(aObject.Size(),value->Des());
                CleanupStack::PopAndDestroy(  ); //value
                }
                else
                    {
                    retVal = KErrNotFound;
                    }
               CleanupStack::PopAndDestroy( 2 ); //  conn, transport
        }
        CleanupStack::PopAndDestroy(  ); //connIdArray
    
	return retVal;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::GetProfileConnectiontypeL
//  The function gets the profile medium type.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::GetProfileConnectiontypeL( TInt& aMediumTYpe )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfileConnectiontypeL(): begin" );
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
	aMediumTYpe = 0;
    if ( transportIdArray[0] == KUidNSmlMediumTypeInternet.iUid )
        {
        aMediumTYpe = 1; //EHttp;
        }
    else if ( transportIdArray[0] == KUidNSmlMediumTypeBluetooth.iUid ||
              transportIdArray[0] == KUidNSmlMediumTypeUSB.iUid ||   
              transportIdArray[0] == KUidNSmlMediumTypeIrDA.iUid  )
        {
        aMediumTYpe = 3; //EObex;
        }
    CleanupStack::PopAndDestroy( 1 ); //transportIdArray, SetIntObjectLC
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::GetProfileIAPIdL
//  The function gets the profile IAP Id.
// -----------------------------------------------------------------------------
TInt CNSmlDMSettingsAdapter12::GetProfileIAPIdL( TInt& aIAPid )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfileIAPIdL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfileIAPIdL(): end" );
    return retVal;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::GetProfileServerURLL
//  The function gets the profile Server URI.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::GetProfileServerURLL( CBufBase& aURL )
	{
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfileServerURLL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfileServerURLL(): end" );
    return;
	}

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter12::GetMediumTypeL
//	Checks the requested medium type.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::GetMediumTypeL( const TInt aIntObj, 
                                            TInt& aMediumType )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetMediumTypeL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetMediumTypeL(): end" );
    return;    
    }

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter12::GetServerIdL
//	Gets the server id value.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::GetServerIdL( CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetServerIdL(): begin" );
    HBufC8* serverId = iProfile.ServerId().AllocL();
    CleanupStack::PushL( serverId );
    aObject.InsertL( aObject.Size(), *serverId );
    CleanupStack::PopAndDestroy( 1 ); //serverId
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetServerIdL(): end" );
    return;
    }

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter12::GetProfileNameL
//	Gets the display name value.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::GetProfileNameL( CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfileNameL(): begin" );
    HBufC16* dataBuf = iProfile.DisplayName().AllocL();
    CleanupStack::PushL( dataBuf );
    TPtr16 ptrDisplayName = dataBuf->Des();
    HBufC8* dataBuf8 = HBufC8::NewL( ptrDisplayName.Length() * 2 );
    CleanupStack::PushL( dataBuf8 );

    TPtr8 ptrDisplayName8 = dataBuf8->Des();
    ptrDisplayName8 = ConvertTo8LC( ptrDisplayName );
    aObject.InsertL( aObject.Size(), ptrDisplayName8 );
    CleanupStack::PopAndDestroy( 3 );//dataBuf, ConvertTo8LC
    _DBG_FILE("CNSmlDMSettingsAdapter12::GetProfileNameL(): end");
    return;
    }

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter12::GetProfileUserNameL
//	Gets the profile user name value.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::GetProfileUserNameL( CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfileUserNameL(): begin" );
    HBufC8* dataBuf = iProfile.UserName().AllocL();
    CleanupStack::PushL( dataBuf );
    TPtr8 ptrUserName = dataBuf->Des();
    aObject.InsertL( aObject.Size(), ptrUserName );
    CleanupStack::PopAndDestroy();//dataBuf
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfileUserNameL(): end" );
    return;
    }

// -----------------------------------------------------------------------------
//	CNSmlDMSettingsAdapter12::GetProfilePortNumberL
//	Gets the port number value.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::GetProfilePortNumberL( CBufBase& aObject )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfilePortNumberL(): begin" );
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::GetProfilePortNumberL(): end" );
    return;
    }

// -----------------------------------------------------------------------------
//  CNSmlDMSettingsAdapter12::SetAuthInfoL
//  The function checks if authinfo already exits. If exits command is replace.
// -----------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::SetAuthInfoL( const TInt aLUID, 
                                            const TNSmlDMProfileData aField, 
                                            const TDesC8& aObject, TInt& aStatus )
    {
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetAuthInfoL(): begin" );
    
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
    _DBG_FILE( "CNSmlDMSettingsAdapter12::SetAuthInfoL(): end" );
    return;
	}

//------------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::AddNodeBufferL()
// Buffers Node Object
//------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::AddNodeBufferL( const TDesC8& aURI,
		const TInt aStatusRef)
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::AddNodeBufferL(): begin" );
	TInt index = -1;

	for (TInt i = 0; i<iBuffer->Count (); i++)
		{
		if ( iBuffer->At(i).iMappingName->Compare (aURI)== 0)
			{
			index = i;
			//CallBack for duplicate
			_DBG_FILE( "CNSmlDMSettingsAdapter12::AddNodeBufferL(): EAlreadyExists end" );
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
	_DBG_FILE( "CNSmlDMSettingsAdapter12::AddNodeBufferL(): end" );
	}

//------------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::AddLeafBufferL()
// Buffers Leaf Obj & checks for Mandatory fields
//------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::AddLeafBufferL( const TDesC8& aURI,
		const TDesC8& aObject, const TInt aStatusRef)
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::AddLeafBufferL(): begin" );
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
	SetURIInProcessL ( aURI);
	if ( iField->Compare (KNSmlDdfAddr)== 0)
		{
		iBuffer->At(iExecutionIndex).iAddr = ETrue;
		}
	else
		if ( iField->Compare (KNSmlDdfServerId)== 0)
			{
			if(!IsServerIDExistL(aObject))
				iBuffer->At(iExecutionIndex).iServerId = ETrue;
			}
		else
			if ( iField->Compare (KNSmlDdfAAuthName)== 0)
				{
				//AAuthName under AppAuthCli
				if ( iURIField->Find (KDmAccAppAuthDyn2)!= KErrNotFound)
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
	_DBG_FILE( "CNSmlDMSettingsAdapter12::AddLeafBufferL(): end" );
	return;
	}

//------------------------------------------------------------------------------
// CNSmlDMSettingsAdapter12::ExecuteBufferL()
// Adds the Node/Leaf obj to the db
//------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::ExecuteBufferL()
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::ExecuteBufferL(): begin" );
	TBuf8<8> newLUID;
	TInt rValue;
	//Buffer is being Executed
	iBufOn = ETrue;

	// Robustness check for Execution Index
	if ( iExecutionIndex<0)
		{
		return;
		}

	//Execute BufferL is called from CompleteOutstandingCmdsL only when -
	//mandatory fields are not set. Dont update Node/Leaf to the db, return a failure

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
	//Buffer Execution Complete
	iBufOn = EFalse;
    _DBG_FILE( "CNSmlDMSettingsAdapter12::ExecuteBufferL(): end" );
	}

//------------------------------------------------------------------------------
// TPtrC8 CNSmlDMSettingsAdapter12::ParentURI(const TDesC8& aURI)
// returns parent uri 
// Searches till the 2nd "/" is reached
//------------------------------------------------------------------------------
TPtrC8 CNSmlDMSettingsAdapter12::ParentURI(const TDesC8& aURI)
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::ParentURI(): begin" );
	TInt count;
	TInt flag = 0;
	for (count=0; count<=aURI.Length ()-1;count++)
		{

		if ( aURI[count]=='/')
			{
			flag++;
			if ( flag == KDynNodePosn)   // KDynNodePosn =2 --> DMAcc/DMIDxxx
				break;
			}
		}
        _DBG_FILE( "CNSmlDMSettingsAdapter12::ParentURI(): end" ); 
	return aURI.Left (count);
	}

//-----------------------------------------------------------------------------------------    
// Clears the elements of the passed in LeafElement Structure
//-----------------------------------------------------------------------------------------
void CNSmlDMSettingsAdapter12::ClearBuffer(
		CArrayFixFlat<TNSmlDMLeafElement>* aBuffer)
	{
	_DBG_FILE( "CNSmlDMSettingsAdapter12::ClearBuffer(): begin" );
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
	_DBG_FILE( "CNSmlDMSettingsAdapter12::ClearBuffer(): end" );
	}

//--------------------------------------------------------------------
//TInt CNSmlDMSettingsAdapter::IsDMAccUriFormatMatchPredefined(const TDesC8 & aURI)
//
//-------------------------------------------------------------------


TBool CNSmlDMSettingsAdapter12::IsDMAccUriFormatMatchPredefined(const TDesC8 & aURI)
{
	
	// include strings for  tree for Bearer , DNS related Dynamic nodes . 
	
	if(aURI.Match(_L8("DMAcc/DMId*"))   != KErrNotFound )
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

TInt CNSmlDMSettingsAdapter12::ConstructTreeL(const TDesC8& aURI)
{
		TInt profileID = KErrNotFound;
		_LIT8( KNSmlDMIdZero,			"DMAcc/DMId000" );
		TBuf8<20> zeroURI; // DMAcc/DMIdnnn , nnn = profileid
		zeroURI.Append( KNSmlDMIdZero );
		HBufC8* ismapInfo = iCallBack->GetLuidAllocL(zeroURI);
		if (ismapInfo->Length() != 0) // Tree is already constructrd, no need to proceed.
		{
			delete ismapInfo;
			return profileID;
		}
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
      TInt ddId = profileIdArray[p] - KMaxDataSyncID;
           
		  _LIT8( Kprev, "DMId" );
		  TBuf8<7> addNAME(Kprev); // DMIdnnn , nnn = profileid
      TInt fixedProfId = profileIdArray[p] - KMaxDataSyncID; //fit to 3 decimal
		  addNAME.AppendNumFixedWidth( fixedProfId, EDecimal, 3 ); 
   		DBG_ARGS8( _S8("notInList: Id = %d Name %S"), p, &addNAME );
		  // KNSmlDMStart includes start text for URISeg
			TBuf8<20> addURI; // DMAcc/DMIdnnn , nnn = profileid
			addURI.Append( KNSmlDMStart );
			addURI.AppendNumFixedWidth( fixedProfId, EDecimal, 3 ); 
			TBuf8<3> addLUID;
	    addLUID.Num( fixedProfId );

			HBufC8* mapInfo = iCallBack->GetLuidAllocL(addURI);
					
			if (mapInfo->Length() == 0)
			{
				iCallBack->SetMappingL(addURI,addLUID);
				if( aURI.Find(addURI) >= 0 )
        { 
         	profileID = fixedProfId;
        }
			}
			else
			{
				delete mapInfo;
			}
 		}
	    	
    CleanupStack::PopAndDestroy( 1 );//profileIdArray
    return profileID;
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)	{{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
		IMPLEMENTATION_PROXY_ENTRY( KNSmlDMSettingsAdapterImplUid, CNSmlDMSettingsAdapter12::NewL )
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

//------------------------------------------------------------------------------
// TPtrC8 CNSmlDMSettingsAdapter12::GetDynamicDMNodeUri( const TDesC8& aURI )
// returns DM/xxx URI
//------------------------------------------------------------------------------
TPtrC8 CNSmlDMSettingsAdapter12::GetDynamicDMNodeUri(const TDesC8& aURI)
    {    
    TInt i= 0;
	TBuf8<50> DmAccRoot(KNSmlDefDMAcc);
    for ( i = aURI.Find( KNSmlDefDMAcc ) + DmAccRoot.Length() + 1; i < aURI.Length(); i++ )
        {
        if( aURI[i] == '/' )
            {
            break;
            }
        }
    
    return aURI.Left( i );
    }
// End of File

