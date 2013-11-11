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
* Description:  DCMO DM Adapter implementation
*
*/

// INCLUDE
#include <implementationproxy.h>
#include <smldmadapter.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include <e32base.h>
#include <ecom.h>
#include <utf.h>
#include <SettingEnforcementInfo.h> // DCMO Enforcement
#include "nsmldebug.h"
#include "dcmodmadapter.h"
#include "dcmointerface.h"

// CONSTANTS

#define MAXBUFLEN 255

_LIT8( KTextType,"text/plain" ); // Leaf inputs
_LIT8( KDCMODMAdapterVersion, "1.0" ); // Adapter version
_LIT8( KDCMORootNode, "DCMO" ); // root URI


// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::NewL
// Symbian 1st phase contructor
// (static, may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
CDcmoDMAdapter* CDcmoDMAdapter::NewL( MSmlDmCallback* aCallback )
    {
    CDcmoDMAdapter* self = NewLC( aCallback );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::NewLC
// Symbian 1st phase contructor. Push object to cleanup-stack
// (static, may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
CDcmoDMAdapter* CDcmoDMAdapter::NewLC( MSmlDmCallback* aCallback )
    {
    	_DBG_FILE( "CDcmoDMAdapter NewLC : begin"  );
    	CDcmoDMAdapter* self = new ( ELeave ) CDcmoDMAdapter( aCallback );
    	CleanupStack::PushL( self );
    	self->ConstructL();
    	_DBG_FILE( "CDcmoDMAdapter NewLC : end"  );
    	return self;
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::CDcmoDMAdapter()
// C++ Constructor
// Status : Draft
// ----------------------------------------------------------------------------
CDcmoDMAdapter::CDcmoDMAdapter( TAny* aEcomArguments )
    : CSmlDmAdapter::CSmlDmAdapter( aEcomArguments )
    {
    	_DBG_FILE( "CDcmoDMAdapter Constructor"  );
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::ConstructL
// 2nd phase contructor
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::ConstructL()
    {
    	_DBG_FILE( "CDcmoDMAdapter::ConstructL"  );
    	idcmoClient = RDCMOClient::NewL();
    	iExtOpNode = EFalse;	
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::~CDcmoDMAdapter()
// C++ Destructor
// Status : Draft
// ----------------------------------------------------------------------------
//
CDcmoDMAdapter::~CDcmoDMAdapter()
    {
    	_DBG_FILE( "CDcmoDMAdapter Destructor"  );
    	if( idcmoClient )
    		{
    			idcmoClient->Close();
    			delete idcmoClient;
    		}
    	idcmoClient = NULL;
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::DDFVersionL
// Return DM plug-in version
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    // Insert version information
    _DBG_FILE( "CDcmoDMAdapter::DDFVersionL" );
    aDDFVersion.InsertL( 0, KDCMODMAdapterVersion );
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::DDFStructureL
// Return DM plug-in structure
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    // Declare accesses
    _DBG_FILE( "CDcmoDMAdapter::DDFStructureL begin" );
    TSmlDmAccessTypes accessTypes;
    accessTypes.SetGet();    
    
    // Create root node 
    MSmlDmDDFObject& root = aDDF.AddChildObjectL( KDCMORootNode );
    FillNodeInfoL( root,
                   accessTypes,
                   MSmlDmDDFObject::EPermanent,
                   MSmlDmDDFObject::ENode,
                   MSmlDmDDFObject::EOne,
                   KNullDesC8(),
                   KTextType );
                   
   	root.SetDFTitleL( KNSmlDCMOAdapterTitle );
                   
   	DDFDCMOAdaptersL(root);
   	_DBG_FILE( "CDcmoDMAdapter::DDFStructureL end" );

    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::StreamingSupport
// Return streaming support status, set supported item size
// Status : Draft
// ----------------------------------------------------------------------------
//
TBool CDcmoDMAdapter::StreamingSupport( TInt& /* aItemSize */ )
    {
    // Not supported
    _DBG_FILE( "CDcmoDMAdapter::StreamingSupport"  );
    return EFalse;
    }
    
// ----------------------------------------------------------------------------
// CDcmoDMAdapter::StreamCommittedL
// Commit stream buffer
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::StreamCommittedL()
    {        
    _DBG_FILE("CDcmoDMAdapter::StreamCommitted"  );
    // Intentionally left empty 
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::CompleteOutstandingCmdsL
// Complete outstanding commands
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE( "CDcmoDMAdapter::CompleteOutstandingCmdsL"  );
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::FillNodeInfoL
// Fill node info
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::FillNodeInfoL( MSmlDmDDFObject& aDDFObject, 
                                     TSmlDmAccessTypes& aAccessTypes, 
                                     MSmlDmDDFObject::TScope aScope,
                                     MSmlDmDDFObject::TDFFormat aFormat, 
                                     MSmlDmDDFObject::TOccurence aOccurence,
                                     const TDesC8& aDescription,
                                     const TDesC8& aMIMEType )
    {
    _DBG_FILE( "CDcmoDMAdapter::FillNodeInfoL - begin"  );
    aDDFObject.SetAccessTypesL( aAccessTypes );
    aDDFObject.SetScopeL( aScope );
    aDDFObject.SetOccurenceL( aOccurence );
    aDDFObject.SetDFFormatL( aFormat );
    aDDFObject.SetDescriptionL( aDescription );
    if ( aFormat != MSmlDmDDFObject::ENode )
        {
        aDDFObject.AddDFTypeMimeTypeL( aMIMEType );
        }
    _DBG_FILE( "CDcmoDMAdapter::FillNodeInfoL - end"  );    
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::CopyCommandL
// Copy object
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                                    const TDesC8& /*aTargetLUID*/,
                                    const TDesC8& /*aSourceURI*/, 
                                    const TDesC8& /*aSourceLUID*/,
                                    const TDesC8& /*aType*/, 
                                    TInt aStatusRef )
    {
    // Not supported
    _DBG_FILE( "CDcmoDMAdapter::CopyCommandL"  );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }

// ----------------------------------------------------------------------------
// DeleteObjectL
// Delete object
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::DeleteObjectL( const TDesC8& /* aURI */, 
                                     const TDesC8& /* aLUID */,
                                     TInt aStatusRef )

    {
    // Not supported
    _DBG_FILE( "CDcmoDMAdapter::DeleteObjectL"  );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }
    
// ----------------------------------------------------------------------------
// CDcmoDMAdapter::FetchLeafObjectL
// Fetch leaf
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                        const TDesC8& /* aLUID */,
								        const TDesC8& aType, 
								        TInt aResultsRef,
								        TInt aStatusRef )
    {
    _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - begin"  );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk; 
		TDCMOStatus err (EDcmoFail);
		
    TBool dcmoEnforce=EFalse;
	  TRAPD(eError, dcmoEnforce =CheckEnforcementL())
	  if(!(eError==KErrNone && dcmoEnforce))
	  {
	  	 // Respond
	  	retValue = CSmlDmAdapter::ENotAllowed;
  		Callback().SetStatusL( aStatusRef, retValue );
  		return;
	  }
    
		TBuf<MAXBUFLEN> iName;
		
		TPtrC8 category = GetCategoryFromUriL( aURI );
		iName.Copy ( category );

		CBufBase *lObject = CBufFlat::NewL( 128 );
    CleanupStack::PushL( lObject );
    lObject->Reset();
    TBool value;
    TInt group;
    	
		if(aURI.Find(KNSmlDdfProperty)!= KErrNotFound)
		{
			 _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - In Property"  );
			TBuf<KDCMOMaxStringSize> property;
			err = idcmoClient->GetDCMOStrAttributeValue(iName, EProperty,property); 
			lObject->Reset();
			TBuf8<KDCMOMaxStringSize> tmp;
			tmp.Copy(property);
			lObject->InsertL(lObject->Size(),tmp);
		}
			
		else if(aURI.Find(KNSmlDdfGroup)!= KErrNotFound)
		{
			 _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - In Group"  );
			err = idcmoClient->GetDCMOIntAttributeValue(iName, EGroup, group); 
			switch(group)
			{
				case EHardware			: 	lObject->InsertL(lObject->Size(),KNSmlHardware);
										break;
				case EIO						:	lObject->InsertL(lObject->Size(),KNSmlIO);
										break;
				case EConnectivity	:	lObject->InsertL(lObject->Size(),KNSmlConnectivity);
										break;
				case ESoftware			:	lObject->InsertL(lObject->Size(),KNSmlSoftware);
										break;
				case EService				:	lObject->InsertL(lObject->Size(),KNSmlService);
										break;	
				default : retValue = CSmlDmAdapter::ENotFound;
									
			}
		}			
		else if(aURI.Find(KNSmlDdfDescription)!= KErrNotFound)
		{
			 _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - In Description"  );
			TBuf<KDCMOMaxStringSize> description;
			err = idcmoClient->GetDCMOStrAttributeValue(iName, EDescription, description); 
			lObject->Reset();
			TBuf8<KDCMOMaxStringSize> tmp;
			tmp.Copy(description);
			lObject->InsertL(lObject->Size(),tmp);
		}
			
		else if(aURI.Find(KNSmlDdfAttached)!= KErrNotFound)
		{
			 _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - In Attached"  );
			err = idcmoClient->GetDCMOIntAttributeValue(iName, EAttached, value); 
			if(value)
				lObject->InsertL(lObject->Size(),KNSmlTrue);
			else
				lObject->InsertL(lObject->Size(),KNSmlFalse);
		}
		
		else if(aURI.Find(KNSmlDdfEnabled)!= KErrNotFound) 
		{
			 _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - In Enabled"  );
			err = idcmoClient->GetDCMOIntAttributeValue(iName, EEnabled, value); 
			if(value)
				lObject->InsertL(lObject->Size(),KNSmlTrue);
			else
				lObject->InsertL(lObject->Size(),KNSmlFalse);			
		}
			
		else if(aURI.Find(KNSmlDdfDenyUserEnable)!= KErrNotFound)
		{
			 _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - In DenyUserEnable"  );
			err = idcmoClient->GetDCMOIntAttributeValue(iName, EDenyUserEnable, value); 
			if(value)
				lObject->InsertL(lObject->Size(),KNSmlTrue);
			else
				lObject->InsertL(lObject->Size(),KNSmlFalse);
		}
			
		else if(aURI.Find(KNSmlDdfNotifyUser)!= KErrNotFound)
		{
			 _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - In NotifyUser"  );
			err = idcmoClient->GetDCMOIntAttributeValue(iName, ENotifyUser, value); 
			if(value)
				lObject->InsertL(lObject->Size(),KNSmlTrue);
			else
				lObject->InsertL(lObject->Size(),KNSmlFalse);
		}
		
		else if(aURI.Find(KNSmlDdfExt)!= KErrNotFound)
		{
			_DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - In Ext"  );
			lObject->Reset();
			lObject->InsertL(lObject->Size(), KNullDesC8);
			err = EDcmoSuccess;
		}
			
		else
			retValue = CSmlDmAdapter::ENotFound;
		
		if(retValue == CSmlDmAdapter::EOk)
    {
			switch(err)
			{
				case EDcmoSuccess 		:	Callback().SetResultsL( aResultsRef, *lObject, aType );
											break;
				case EDcmoNotSupported	:	retValue = CSmlDmAdapter::EError;
											break;
				case EDcmoAccessDenied	:	retValue = CSmlDmAdapter::ENotAllowed;
											break;
				default :
											retValue = CSmlDmAdapter::EError;						
			}
				
  	}
    	
    CleanupStack::PopAndDestroy(); //lObject	

    // Respond
  Callback().SetStatusL( aStatusRef, retValue );
   _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectL - end"  );
    }    
    
// ----------------------------------------------------------------------------
// CDcmoDMAdapter::FetchLeafObjectSizeL
// Calculate leaf object size
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::FetchLeafObjectSizeL( const TDesC8& /* aURI */, 
                                            const TDesC8& /* aLUID */,
									        const TDesC8& /* aType */, 
									        TInt /* aResultsRef */,
									        TInt aStatusRef )
    {
    _DBG_FILE( "CDcmoDMAdapter::FetchLeafObjectSizeL"  );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    // Respond
    Callback().SetStatusL( aStatusRef, retValue );
    }    

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::ChildURIListL
// Create child URI list
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::ChildURIListL( const TDesC8& aURI, 
                                     const TDesC8& /* aLUID */,
					                 const CArrayFix<TSmlDmMappingInfo>& /* aPreviousURISegmentList */,
            					     TInt aResultsRef, 
            					     TInt aStatusRef )
    {
    _DBG_FILE( "CDcmoDMAdapter::ChildURIListL - begin"  );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    CBufBase *currentURISegmentList = CBufFlat::NewL( 128 );
		CleanupStack::PushL( currentURISegmentList );	

 		if(( aURI.Match( KDcmoMatch2 ) != KErrNotFound ) &&
		 (aURI.Match( KDcmoMatch3 ) == KErrNotFound ))
		{	
            currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDcmo );
			Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
		    Callback().SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
            CleanupStack::PopAndDestroy(  ); // currentURISegmentList
       		_DBG_FILE( "CDcmoDMAdapter::ChildURIListL(): end" );
			return;
		}
    else if(( aURI.Match( KDcmoMatch ) != KErrNotFound ) &&
              (aURI.Match( KDcmoMatch2 ) == KErrNotFound ))
        {
        //    
        }
    // DCMOConfig subtree
    else if( ( aURI.Match( KDcmoConfigMatch ) != KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDcmoConfig );
        }
    // Operations    
    else if(( aURI.Match( KDcmoOperationsMatch ) != KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmDcmoOperations );
        } 
    // Ext    
    else if(( aURI.Match(KNSmlBluetooth) != KErrNotFound) && ( aURI.Match(KDcmoOperationsExtMatch)!= KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDdfOpExtSemiEnable );
        }        
    else
        {
        retValue = CSmlDmAdapter::ENotFound;
        }

    // Respond
    Callback().SetStatusL( aStatusRef, retValue );
    CleanupStack::PopAndDestroy(); // currentURISegmentList
    _DBG_FILE( "CDcmoDMAdapter::ChildURIListL - end"  );
    }    

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::AddNodeObjectL
// Add node
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::AddNodeObjectL( const TDesC8& /* aURI */,
                                     const TDesC8& /* aParentLUID */,
								     TInt aStatusRef )
    {
    // Not supported
    _DBG_FILE( "CDcmoDMAdapter::AddNodeObjectL - begin"  );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CDcmoDMAdapter::AddNodeObjectL - end"  );
    }    

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::ExecuteCommandL
// Execute command
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::ExecuteCommandL( const TDesC8&  aURI , 
                                      const TDesC8& /* aLUID */,
							          const TDesC8& /* aArgument */, 
							          const TDesC8& /* aType */,
								      TInt aStatusRef )
    {
    _DBG_FILE( "CDcmoDMAdapter::ExecuteCommandL - begin"  );
   	MSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    TInt err (EDcmoFail);
    
    TBool dcmoEnforce=EFalse;
	  TRAPD(eError, dcmoEnforce =CheckEnforcementL())
	  if(!(eError==KErrNone && dcmoEnforce))
	  {
	  	 // Respond
	  	status = CSmlDmAdapter::ENotAllowed;
  		Callback().SetStatusL( aStatusRef, status );
  		return;
	  }
	  
		TBuf<MAXBUFLEN> iName;
		TPtrC8 category = GetCategoryFromUriL( aURI );
		iName.Copy ( category );		
		
		if((category.Compare( KNSmlBluetooth ) == KErrNone ) && (aURI.Find( KNSmlDdfOpExtSemiEnable ) != KErrNotFound ))
			err = idcmoClient->SetDCMOIntAttributeValue(iName, EEnable, 2); 
		else if(aURI.Find( KNSmlDdfEnable ) != KErrNotFound ) 
			err = idcmoClient->SetDCMOIntAttributeValue(iName, EEnable, 1); 
		else if(aURI.Find( KNSmlDdfDisable ) != KErrNotFound ) 
			err = idcmoClient->SetDCMOIntAttributeValue(iName, EEnable, 0); 		
		else	
			err = EDcmoNotFound;				
			// map error	
		status = MapErrorToStatus( err );
								
    Callback().SetStatusL( aStatusRef, status );
    _DBG_FILE( "CDcmoDMAdapter::ExecuteCommandL - end"  );
    }    

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::ExecuteCommandL
// Execute command, streaming enabled
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::ExecuteCommandL( const TDesC8& /* aURI */, 
                                      const TDesC8& /* aLUID */,
								      RWriteStream*& /* aStream */,
								      const TDesC8& /* aType */,
								      TInt aStatusRef )
    {
    // Not supported
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }    
    
// ----------------------------------------------------------------------------
// CDcmoDMAdapter::UpdateLeafObjectL
// Update leaf object
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::UpdateLeafObjectL( const TDesC8& /* aURI */, 
                                        const TDesC8& /* aLUID */,
                                        const TDesC8& /* aObject */,
                                        const TDesC8& /* aType */,
                                        TInt aStatusRef )
    {
      _DBG_FILE( "CDcmoDMAdapter::UpdateLeafObjectL - begin"  );
 		// Not supported
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
   _DBG_FILE( "CDcmoDMAdapter::UpdateLeafObjectL - end"  );
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::UpdateLeafObjectL
// Update leaf object, streaming enabled
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::UpdateLeafObjectL( const TDesC8& /* aURI */, 
                                         const TDesC8& /* aLUID */,
									     RWriteStream*& /* aStream */, 
									     const TDesC8& /* aType */,
		 							     TInt aStatusRef )
    {
    // Not supported
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }
    
// ----------------------------------------------------------------------------
// CDcmoDMAdapter::StartAtomicL
// Start atomic
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::StartAtomicL()
    {
    }
    
// ----------------------------------------------------------------------------
// CDcmoDMAdapter::CommitAtomicL
// Commit atomic commands
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::CommitAtomicL()
    {
    }

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::RollbackAtomicL
// Lose all modifications after 'StartAtomicL' command
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::RollbackAtomicL()
    {
    }    

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::MapStatusToError()
// Returns a status code corresponding to the system error code given 
// as parameter.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CDcmoDMAdapter::MapErrorToStatus( TInt aError ) const
 {
    CSmlDmAdapter::TError err = CSmlDmAdapter::EError;
    
    switch( aError )
        {
        case EDcmoSuccess:
            err = CSmlDmAdapter::EExecSuccess;
            break;
        case EDcmoFail:
            err = CSmlDmAdapter::EExecClientError;
            break;    
        case EDcmoNotFound:
            err = CSmlDmAdapter::EExecDownloadFailed;
            break;
        case EDcmoNotSupported:
            err = CSmlDmAdapter::EExecClientError;
            break;
        case EDcmoAccessDenied:
            err = CSmlDmAdapter::EExecClientError;
            break;            
        default :
        		err = CSmlDmAdapter::EExecClientError;        
        }
    
    return err;
 }
      
// ========================= OTHER EXPORTED FUNCTIONS =========================

void CleanupEComArray(TAny* aArray)
	{
	(static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
	(static_cast<RImplInfoPtrArray*> (aArray))->Close();
	}

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::DDFDCMOAdaptersL
// This function loads all the DCMO plug-in adapters
// ----------------------------------------------------------------------------
//
void CDcmoDMAdapter::DDFDCMOAdaptersL(MSmlDmDDFObject& rootdcmo)
{
		_DBG_FILE("CDcmoDMAdapter::DDFDCMOAdaptersL(): begin");	
			TBuf8<MAXBUFLEN> buf;
      TSmlDmAccessTypes accessTypes;
    	accessTypes.SetGet();
    	
			CRepository* centrep = NULL;
			TInt err(KErrNone);
			TRAP(err, centrep = CRepository::NewL( KCRUidDCMOServer ));
			if(err == KErrNone)
			{
				TUint32 centrepKey = 0;
				TInt count(0);
				TInt reterr = centrep->Get( centrepKey, count );	
				if(reterr == KErrNone)
				{		
					TBuf<MAXBUFLEN> buffer;
					for(TInt i=0; i<count; i++)
					{   
        		centrepKey = KDCMOKeyMaxNumber*i + KDCMOPropertyNumber ;
        		reterr = centrep->Get( centrepKey, buffer );
	    			if( reterr== KErrNone )
        		{        			
        			buf.Copy ( buffer );
        			MSmlDmDDFObject& entityNode = rootdcmo.AddChildObjectL( buf );
    					FillNodeInfoL( entityNode,
                   accessTypes,
                   MSmlDmDDFObject::EDynamic,
                   MSmlDmDDFObject::ENode,
                   MSmlDmDDFObject::EZeroOrOne,                   
                   KDCMOCapabilityDescription,
                   KMimeType ); 
                                       
							FillDCMODDFL(entityNode);
							buf.Zero();
        		}
					}
				}	
				delete centrep;
				centrep = NULL;
			}
			
			// Read info about all implementations into infoArray
			RImplInfoPtrArray infoArray;
			// Note that a special cleanup function is required to reset and destroy
			// all items in the array, and then close it.
			TCleanupItem cleanup(CleanupEComArray, &infoArray);
			CleanupStack::PushL(cleanup);
			REComSession::ListImplementationsL(KDCMOInterfaceUid, infoArray);

			// Loop through each info for each implementation
			// and create and use each in turn
			CDCMOInterface* ex;			

			for (TInt i=0; i< infoArray.Count(); i++)
				{

				TUid impluid = infoArray[i]->ImplementationUid();
				// Create object of type and call its function
				
				buf = infoArray[i]->OpaqueData();
				
				CDCMOInterface::TDCMOInterfaceInitParams initParms;
				initParms.uid = impluid;
				TBuf<MAXBUFLEN> categoryName;
				categoryName.Copy(buf);
				initParms.descriptor = categoryName.AllocL(); 
				ex = CDCMOInterface::NewL(initParms);
				CleanupStack::PushL(ex);
				if( buf.Compare(KNSmlBluetooth)== KErrNone )
				{
					iExtOpNode = ETrue;
				}	
    			MSmlDmDDFObject& entityNode = rootdcmo.AddChildObjectL( buf );
    			FillNodeInfoL( entityNode,
                   accessTypes,
                   MSmlDmDDFObject::EDynamic,
                   MSmlDmDDFObject::ENode,
                   MSmlDmDDFObject::EZeroOrOne,                   
                   KDCMOCapabilityDescription,
                   KMimeType ); 
                                       
				FillDCMODDFL(entityNode);
				ex = NULL;
				buf.Zero();
				CleanupStack::PopAndDestroy(); //ex
				}

			// Clean up
			CleanupStack::PopAndDestroy(); //infoArray, results in a call to CleanupEComArray
	_DBG_FILE("CDcmoDMAdapter::DDFDCMOAdaptersL(): end");	
}

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::FillDCMODDFL
// This function constructs the DDFnode with the details
// ----------------------------------------------------------------------------
//	
void CDcmoDMAdapter::FillDCMODDFL(MSmlDmDDFObject& aDDF)
{
	_DBG_FILE("CDcmoDMAdapter::FillDCMPDDFL(): begin");
		
	  TSmlDmAccessTypes accessTypesG;
    accessTypesG.SetGet();
    
    MSmlDmDDFObject& nPropertyDDF = aDDF.AddChildObjectL( KNSmlDdfProperty );			// Property
    FillNodeInfoL( nPropertyDDF, accessTypesG,  
                    MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne,
                    KNSmlDdfPropertyDescription, KMimeType );

    //Group 
    MSmlDmDDFObject& nGroupDDF = aDDF.AddChildObjectL( KNSmlDdfGroup );					// Group
    FillNodeInfoL( nGroupDDF, accessTypesG,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne,
                    KNSmlDdfGroupDescription, KMimeType );           
                    
    // Description node 
    MSmlDmDDFObject& nDescriptionDDF = aDDF.AddChildObjectL( KNSmlDdfDescription );		// Description
    FillNodeInfoL( nDescriptionDDF, accessTypesG, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne, 
                    KNSmlDdfDescDescription, KMimeType );

    MSmlDmDDFObject& nAttachedDDF = aDDF.AddChildObjectL( KNSmlDdfAttached );			// Attached
    FillNodeInfoL( nAttachedDDF, accessTypesG, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne,
                    KNSmlDdfAttachedDescription, KMimeType );

		MSmlDmDDFObject& nEnabledDDF = aDDF.AddChildObjectL( KNSmlDdfEnabled );			// Enabled
    FillNodeInfoL( nEnabledDDF, accessTypesG,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne,
                    KNSmlDdfEnabledDescription, KMimeType );

		MSmlDmDDFObject& nDCMOConfigDDF = aDDF.AddChildObjectL( KNSmlDdfDCMOConfig );	// DCMOConfig
    FillNodeInfoL( nDCMOConfigDDF, accessTypesG,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, MSmlDmDDFObject::EOne,
                    KNSmlDdfDCMOConfigDescription, KMimeType );
    
    MSmlDmDDFObject& nDenyUserEnableDDF = nDCMOConfigDDF.AddChildObjectL( KNSmlDdfDenyUserEnable );	// DenyUserEnable
    FillNodeInfoL( nDenyUserEnableDDF, accessTypesG,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EZeroOrOne,
                    KNSmlDdfDenyUserEnableDescription, KMimeType );
                    

    MSmlDmDDFObject& nNotifyUserDDF = nDCMOConfigDDF.AddChildObjectL( KNSmlDdfNotifyUser );	// NotifyUser
    FillNodeInfoL( nNotifyUserDDF, accessTypesG,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne,
                    KNSmlDdfNotifyUserDescription, KMimeType );                     
    //Operations 
   
  	MSmlDmDDFObject& nOperationsDDF = aDDF.AddChildObjectL( KNSmlDdfOperations );		// Operations
    FillNodeInfoL( nOperationsDDF, accessTypesG,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, MSmlDmDDFObject::EOne,
                    KNSmlDdfOperationsDescription, KMimeType ); 
                    
   	
   	TSmlDmAccessTypes accessTypesE;   
    accessTypesE.SetExec();
    
   	MSmlDmDDFObject& nEnableDDF = nOperationsDDF.AddChildObjectL( KNSmlDdfEnable );		// Enable
    FillNodeInfoL( nEnableDDF, accessTypesE,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne,
                    KNSmlDdfEnableDescription, KMimeType );
                    
    
   	MSmlDmDDFObject& nDisableDDF = nOperationsDDF.AddChildObjectL( KNSmlDdfDisable );	// Disable
    FillNodeInfoL( nDisableDDF, accessTypesE,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne,
                    KNSmlDdfDisableDescription, KMimeType );
                    

   	MSmlDmDDFObject& nOpExtDDF = nOperationsDDF.AddChildObjectL( KNSmlDdfOpExt );		// Operations/Ext
    FillNodeInfoL( nOpExtDDF, accessTypesG,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, MSmlDmDDFObject::EOne,
                    KNSmlDdfOpExtDescription, KMimeType );
    
    if( iExtOpNode )
    {	
    	MSmlDmDDFObject& nOpExtSemiEnableDDF = nOpExtDDF.AddChildObjectL( KNSmlDdfOpExtSemiEnable );		// Operations/Ext/SemiEnable
    	FillNodeInfoL( nOpExtSemiEnableDDF, accessTypesE,  
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne,
                    KNSmlDdfOpExtSemiEnableDescription, KMimeType );                    
    	iExtOpNode = EFalse;
  	}               

   	MSmlDmDDFObject& nExtDDF = aDDF.AddChildObjectL( KNSmlDdfExt );						// Ext
    FillNodeInfoL( nExtDDF, accessTypesG, 
                    MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne, 
                    KNSmlDdfExtDescription, KMimeType ); 


	_DBG_FILE("CDcmoDMAdapter::FillDCMPDDFL(): end");
}    

// ----------------------------------------------------------------------------
// CDcmoDMAdapter::GetCategoryFromUriL
// This function returns the implementation UID if the plug-in
// ----------------------------------------------------------------------------
//	
TPtrC8 CDcmoDMAdapter::GetCategoryFromUriL(const TDesC8& aURI )
{
	_DBG_FILE("CDcmoDMAdapter::GetDCMOAdapterUidL(): begin");
	
	TInt length = aURI.Length ();
	TInt count = 0;
	TInt flag = 0;
	for (count=0; count<length; count++)
		{
		if ( aURI[count]=='/')
			{
				if ( flag )
					break;
				flag = count;			
			}
		}
		
	_DBG_FILE("CDcmoDMAdapter::GetDCMOAdapterUidL(): end");	
	return ( aURI.Mid ( flag+1 , count-(flag+1) ));	


}

//----------------------------------------------------------------------
// TBool CDcmoDMAdapter::CheckEnforcementL()
// If DCMO policy is enforced true is returned else false
//----------------------------------------------------------------------
TBool CDcmoDMAdapter::CheckEnforcementL()
{
    CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
    CleanupStack::PushL(info);
    TBool enforceActive ( EFalse );
    User::LeaveIfError(info->EnforcementActive(EDCMOEnforcement, enforceActive));
    CleanupStack::PopAndDestroy(info);
    return enforceActive;
}
    
// End of File  
