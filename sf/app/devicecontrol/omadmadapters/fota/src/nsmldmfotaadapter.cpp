/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    DM Fota Adapter
*
*/




// INCLUDE FILES
#include <implementationproxy.h> // For TImplementationProxy definition
//#ifdef RD_OMADMAPPUI_UI_EVOLUTION
#include <DevManInternalCRKeys.h>
//#endif
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include <e32property.h>
#include <centralrepository.h>
#include "fotaadapterCRKeys.h"
#include "NSmlPrivateAPI.h"
#include "nsmldmimpluids.h"
#include "nsmldmfotaadapter.h"
#include "nsmldmconst.h"
#include "nsmldebug.h"
#include "FotaSrvDebug.h"
#include "nsmldmtreedbclient.h"
#include <nsmldmconst.h>

// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter* CNSmlDmFotaAdapter::NewL()
// Creates new instance of CNSmlDmFotaEngine and returns a pointer to it.
// ---------------------------------------------------------------------------
//
CNSmlDmFotaAdapter* CNSmlDmFotaAdapter::NewL( MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::NewL(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::NewL >>"));
    CNSmlDmFotaAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop( self );
    _DBG_FILE("CNSmlDmFotaAdapter::NewL(): end");
    FLOG(_L("CNSmlDmFotaAdapter::NewL <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter* CNSmlDmFotaAdapter::NewLC()
// Creates new instance of CNSmlDmFotaEngine and returns a pointer to it. 
// Leaves the pointer onto the CleanupStack.
// ---------------------------------------------------------------------------
//
CNSmlDmFotaAdapter* CNSmlDmFotaAdapter::NewLC( MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::NewLC(): begin");
    CNSmlDmFotaAdapter* self = new (ELeave) CNSmlDmFotaAdapter( aDmCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    _DBG_FILE("CNSmlDmFotaAdapter::NewLC(): end");
    return self;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::CNSmlDmFotaAdapter()
// Constructor.
// ---------------------------------------------------------------------------
//
CNSmlDmFotaAdapter::CNSmlDmFotaAdapter( TAny* aEcomArguments )
    : CSmlDmAdapter( aEcomArguments ), iPkgId( KNSmlDMFotaNullPkgId )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::CNSmlDmFotaAdapter(): begin");
    _DBG_FILE("CNSmlDmFotaAdapter::CNSmlDmFotaAdapter(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::ConstructL()
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::ConstructL()
    {
    // Check if Fota feature is defined
    TInt value( 0 );
    CRepository * rep = CRepository::NewLC( KCRUidFotaAdapter );
	TInt err = rep->Get( KCRFotaAdapterEnabled, value );
	CleanupStack::PopAndDestroy( rep );
	
	if ( err != KErrNone || value == 0 )
    	{
    	User::Leave( KErrNotSupported );
    	}    
    	
    iFotaDb = CNSmlDmFotaAdapterDb::NewL();
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::~CNSmlDmFotaAdapter()
// Destructor.
// ---------------------------------------------------------------------------
//
CNSmlDmFotaAdapter::~CNSmlDmFotaAdapter()
    {
    _DBG_FILE("CNSmlDmFotaAdapter::~CNSmlDmFotaAdapter(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::~CNSmlDmFotaAdapter >>"));
    
    if (iFotaEngine.Handle())
        iFotaEngine.Close();

    delete iFotaDb;
    FLOG(_L("CNSmlDmFotaAdapter::~CNSmlDmFotaAdapter <<"));
    _DBG_FILE("CNSmlDmFotaAdapter::~CNSmlDmFotaAdapter(): end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapter::DDFVersionL()
//  Returns the DDF version of the adapter.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::DDFVersionL(TDes& aDDFVersion): begin");

    aDDFVersion.InsertL( 0, KNSmlDMFotaDDFVersion );

    _DBG_FILE("CNSmlDmFotaAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapter::DDFStructureL()
//  Builds the DDF structure of adapter.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::DDFStructureL(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::DDFStructureL >>"));
    TSmlDmAccessTypes accessTypes;
    
    // fota root
    MSmlDmDDFObject& FUMO = aDDF.AddChildObjectL( KNSmlDMFotaNode );
    accessTypes.SetAdd();
    accessTypes.SetGet();
    FillNodeInfoL(  FUMO, 
                    accessTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::ENode,
                    KNSmlDMFotaNodeDescription );

    accessTypes.Reset();

    // run time node <X>
    MSmlDmDDFObject& rtPkg = FUMO.AddChildObjectGroupL();
    rtPkg.AddDFTypeMimeTypeL( KNSmlDMFotaRunTimeMimeType );
    accessTypes.SetAdd();
    accessTypes.SetDelete();
    accessTypes.SetGet();
    //accessTypes.SetReplace();
    FillNodeInfoL(  rtPkg, 
                    accessTypes,
                    MSmlDmDDFObject::EZeroOrMore, 
                    MSmlDmDDFObject::EDynamic, 
                    MSmlDmDDFObject::ENode, 
                    KNSmlDMFotaRunTimeNodeDescription );

    accessTypes.Reset();

    // <X>/PkgName
    MSmlDmDDFObject& name = rtPkg.AddChildObjectL( KNSmlDMFotaNodeName );
    accessTypes.SetAdd();
    accessTypes.SetGet();
    accessTypes.SetReplace();
    FillNodeInfoL(  name,
                    accessTypes,
                    MSmlDmDDFObject::EZeroOrOne,
                    MSmlDmDDFObject::EDynamic,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMFotaNodeNameDescription );

    // <X>/PkgVersion
    MSmlDmDDFObject& version = rtPkg.AddChildObjectL( KNSmlDMFotaNodeVersion );
    FillNodeInfoL(  version,
                    accessTypes,
                    MSmlDmDDFObject::EZeroOrOne,
                    MSmlDmDDFObject::EDynamic,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMFotaNodeVersionDescription );

    accessTypes.Reset();

    // <X>/Download
    MSmlDmDDFObject& download = 
    rtPkg.AddChildObjectL( KNSmlDMFotaNodeDownload );

    accessTypes.SetGet();
    accessTypes.SetExec();
    FillNodeInfoL(  download,
                    accessTypes,
                    MSmlDmDDFObject::EZeroOrOne,
                    MSmlDmDDFObject::EDynamic,
                    MSmlDmDDFObject::ENode,
                    KNSmlDMFotaNodeDownloadDescription );

    accessTypes.Reset();

    // <X>/Download/PkgURL
    MSmlDmDDFObject& dlUrl = 
    download.AddChildObjectL( KNSmlDMFotaNodeDownloadUrl );

    accessTypes.SetGet();
    accessTypes.SetReplace();
    FillNodeInfoL(  dlUrl,
                    accessTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EDynamic,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMFotaNodeDownloadUrlDescription );

    accessTypes.Reset();


    // <X>/DownloadAndUpdate
    MSmlDmDDFObject& dlAndUpdate = 
    rtPkg.AddChildObjectL( KNSmlDMFotaNodeDownloadAndUpdate );

    accessTypes.SetGet();
    accessTypes.SetExec();
    FillNodeInfoL(  dlAndUpdate,
                    accessTypes,
                    MSmlDmDDFObject::EZeroOrOne,
                    MSmlDmDDFObject::EDynamic,
                    MSmlDmDDFObject::ENode,
                    KNSmlDMFotaNodeDownloadAndUpdateDescription );

    accessTypes.Reset();

    // <X>/DownloadAndUpdate/PkgURL
    MSmlDmDDFObject& dlAndUpdateUrl = 
    dlAndUpdate.AddChildObjectL( KNSmlDMFotaNodeDownloadAndUpdateUrl );

    accessTypes.SetGet();
    accessTypes.SetReplace();
    FillNodeInfoL(  dlAndUpdateUrl,
                    accessTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EDynamic,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMFotaNodeDownloadAndUpdateUrlDescription );

    accessTypes.Reset();

    // <X>/State
    MSmlDmDDFObject& state = rtPkg.AddChildObjectL( KNSmlDMFotaNodeState );
    accessTypes.SetGet();
    FillNodeInfoL(  state,
                    accessTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EDynamic,
                    MSmlDmDDFObject::EInt,
                    KNSmlDMFotaNodeStateDescription );

    
    
    //Check cenrep for predefined node and add if its not in DB
    CheckAndAddPredefinedNodeL();
    
    // check if there is need to make generic alert(s), 
    // and generate them if there are
    MakeGenericAlertsL();


    // Set ACL for fota root, if not yet set
    RNSmlDMCallbackSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL(session);

    CBufBase* acl=CBufFlat::NewL(10);
    CleanupStack::PushL( acl );
    TInt err = session.GetAclL( KNSmlDMFotaNode,*acl,EFalse );
		if ( err == KErrNone)
		{
    	if( acl->Size() == 0 )
        {
        _LIT8(KACLForAll, "Add=*&Get=*&Delete=*&Exec=*&Replace=*");
        session.UpdateAclL( KNSmlDMFotaNode, KACLForAll);
        }
    }
    CleanupStack::PopAndDestroy( acl );
    CleanupStack::PopAndDestroy( &session );

    _DBG_FILE("CNSmlDmFotaAdapter::DDFStructureL(): end");
    FLOG(_L("CNSmlDmFotaAdapter::DDFStructureL <<"));
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::UpdateLeafObjectL()
// Updates value of leaf object.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            const TDesC8& aObject, 
                                            const TDesC8& /*aType*/, 
                                            const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::UpdateLeafObjectL(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::UpdateLeafObjectL >>"));
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    
    if ( aLUID.Length() > 0 )
        {
        TInt res = KErrNotFound;
        TNSmlDmFwObjectId id = DesToInt( aLUID );
        
        TPtrC8 lastSeg = LastURISeg( aURI );
        if ( lastSeg == KNSmlDMFotaNodeName )
            {
            res = iFotaDb->SetPkgNameL( aObject, id );
            }
        else if ( lastSeg == KNSmlDMFotaNodeVersion )
            {
            res = iFotaDb->SetPkgVersionL( aObject, id );
            }
        else if ( lastSeg == KNSmlDMFotaNodeDownloadUrl || 
                  lastSeg == KNSmlDMFotaNodeDownloadAndUpdateUrl )
            {
            TInt configFlags( KErrNone );        
            CRepository* centrep = NULL;
            TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );    	
            if ( err == KErrNone && centrep ) 
                {
                configFlags = 2; //means fota operation
                centrep->Set( KDevManSessionType, configFlags );
                delete centrep;
                centrep = NULL;
                }
            else
                {
                User::Leave( err );
                }
            res = iFotaDb->SetPkgUrlL( aObject, id );
            }
        // map error
        status = MapErrorToStatus( res );
        }
    else
        {
        // cannot update, object unknown
        status = CSmlDmAdapter::ENotFound;
        }
#ifdef __WINS__
TInt pos=-1;
pos = aURI.Find(KNSmlDMFotaNodeDownload);

if (status == CSmlDmAdapter::EOk && pos != KErrNotFound)
	{
	if ( aURI.Find(KNSmlDMFotaNodeDownloadAndUpdate) !=KErrNotFound)
		ExecuteCommandL(_L8("./FUMO/FWUpdate/DownloadAndUpdate"),aLUID,aObject,_L8(""),status);
	else
		ExecuteCommandL(_L8("./FUMO/FWUpdate/Download"),aLUID,aObject,_L8(""),status);
	}
#endif    
    
    Callback().SetStatusL( aStatusRef, status );
    
    _DBG_FILE("CNSmlDmFotaAdapter::UpdateLeafObjectL(): end");
    FLOG(_L("CNSmlDmFotaAdapter::UpdateLeafObjectL <<"));
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::DeleteObjectL()
// Deletes a firmware object from Fota database.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::DeleteObjectL( const TDesC8& aURI, 
                                        const TDesC8& aLUID, 
                                        const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::DeleteLeafObjectL( ): begin");
    FLOG(_L("CNSmlDmFotaAdapter::DeleteObjectL >>"));
    
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    
    if ( ( aURI.Match( KNSmlDMFotaRuntimeMatch ) != KErrNotFound ) && 
         ( aLUID.Length() > 0 ) )
        {
		TBuf8<KMaxFullName> temp;
		GetPredefinedNodeL(temp);
		TBuf8<KMaxName> node[4];
    	TInt location = temp.LocateReverse(',');
    	node[0].Copy(temp.Mid(location+1));
    	TInt newLocation = (temp.Left(location-1)).LocateReverse(',');
    	node[1].Copy(temp.Mid(newLocation+1,location-(newLocation+1)));
    	location = newLocation;
    	newLocation = (temp.Left(location-1)).LocateReverse(',');
    	node[2].Copy(temp.Mid(newLocation+1,location-(newLocation+1)));
    	node[3].Copy(temp.Mid(0,newLocation ));
    	for(TInt i =0 ; i<4; i++)
    	{    		
    		if (node[i].Length() && aURI.Find( node[i] ) != KErrNotFound)
            {
            status = CSmlDmAdapter::ENotAllowed;
            Callback().SetStatusL( aStatusRef, status );
            return;
            }
    	}
        // delete object identified by aLUID
        TNSmlDmFwObjectId id = DesToInt( aLUID );
        TInt res = iFotaDb->DeleteFwObjectL( id );

        if ( res == KErrNone )
            {
            // deletion was successful, delete also from Fota Engine
            FotaEngineL().DeleteUpdatePackage( id );
            }
        else
            {
            status = MapErrorToStatus( res );
            }
        }
    else
        {
        // object unknown
        status = CSmlDmAdapter::ENotFound;
        }
    
    Callback().SetStatusL( aStatusRef, status );
    
    _DBG_FILE("CNSmlDmFotaAdapter::DeleteLeafObjectL( ): end");
    FLOG(_L("CNSmlDmFotaAdapter::DeleteObjectL <<"));
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapter::FetchLeafObjectL()
//  Fetches the values of leaf objects.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                           const TDesC8& aLUID,
                                           const TDesC8& aType,
                                           const TInt aResultsRef,
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::FetchLeafObjectL(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::FetchLeafObjectL >>"));
    // buffer for result
    CBufBase* result = CBufFlat::NewL( KNSmlDMFotaResultBufferExpandSize );
    CleanupStack::PushL( result );
    
    // fetch data
    CSmlDmAdapter::TError status = DoFetchObjectL( aURI, aLUID, *result );
    if ( status == CSmlDmAdapter::EOk )
        {
        Callback().SetResultsL( aResultsRef, *result, aType );
        }
    
    CleanupStack::PopAndDestroy( result );
    
    Callback().SetStatusL( aStatusRef, status );
    
    _DBG_FILE("CNSmlDmFotaAdapter::FetchLeafObjectL(): end");
    FLOG(_L("CNSmlDmFotaAdapter::FetchLeafObjectL <<"));
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapter::ChildURIListL()
//  Returns the list of children of a node.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::ChildURIListL( const TDesC8& aURI,
                                        const TDesC8& aLUID, 
                                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                                        const TInt aResultsRef, 
                                        const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::ChildURIListL(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::ChildURIListL >>"));
    
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    TPtrC8 mimeType( KNullDesC8 );
    
    // buffer for result
    CBufBase* result = CBufFlat::NewL( KNSmlDMFotaResultBufferExpandSize );
    CleanupStack::PushL( result );
    
    TPtrC8 lastSeg = LastURISeg( aURI );
    if ( lastSeg == KNSmlDMFotaNodeDownload && aLUID.Length() > 0 )
        {
        // target is ./FUMO/<x>/Download
        result->InsertL( 0, KNSmlDMFotaNodeDownloadUrl );
        }
    else if ( lastSeg == KNSmlDMFotaNodeDownloadAndUpdate && aLUID.Length() > 0 )
        {
        // target is ./FUMO/<x>/DownloadAndUpdate
        result->InsertL( 0, KNSmlDMFotaNodeDownloadAndUpdateUrl );
        }
    else if ( ( aURI.Match( KNSmlDMFotaRuntimeMatch ) != KErrNotFound ) 
        && ( TableExistsL( aLUID ) ) )
        {
        // target is ./FUMO/<x>
        result->InsertL( 0, KNSmlDMFotaRunTimeChildren );
        mimeType.Set( KNSmlDMFotaRunTimeMimeType );
        }
    else if ( aURI.Match( KNSmlDMFotaRootMatch ) != KErrNotFound )
        {
        // target is ./FUMO
        
        // return the children listed in aPreviousURISegmentList
        TInt childCount = aPreviousURISegmentList.Count();
        for ( TInt i = 0; i < childCount - 1; ++i )
            {
            result->InsertL( result->Size(), aPreviousURISegmentList[i].iURISeg );
            result->InsertL( result->Size(), KNSmlDMFotaSeparatorDes );
            }
        
        if ( childCount )
            {
            // insert last child without separator
            result->InsertL( result->Size(), 
                aPreviousURISegmentList[ childCount - 1 ].iURISeg );
            }
        
        }
    else
        {
        // invalid target
        status = CSmlDmAdapter::ENotFound;
        }
    
    // set result, if it was found
    if ( status == CSmlDmAdapter::EOk )
        {
        Callback().SetResultsL( aResultsRef, *result, mimeType );
        }
    
    Callback().SetStatusL( aStatusRef, status );
    
    CleanupStack::PopAndDestroy( result );
    
    _DBG_FILE("CNSmlDmFotaAdapter::ChildURIListL(): end");
    FLOG(_L("CNSmlDmFotaAdapter::ChildURIListL <<"));
    }
    


// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::TableExistsL(const TDesC8& aLUID) const
// Checks if table exists
// ---------------------------------------------------------------------------
//
TBool CNSmlDmFotaAdapter::TableExistsL(const TDesC8& aLUID) const
    {
    TBool ret = EFalse;
    if(aLUID.Length()>0)
        {
        TNSmlDmFwObjectId luid = DesToInt( aLUID );
        TSmlProfileId profileId = iFotaDb->ProfileIdL(luid);
        if (profileId!=KErrNotFound)
            {
            ret = ETrue;
            }
        }
    return ret;
    }

//
// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::AddNodeObjectL()
// Adds new row to firmware object table in Fota database.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::AddNodeObjectL( const TDesC8& aURI, 
                                         const TDesC8& aParentLUID, 
                                         const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::AddNodeObjectL(): begin");
    
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    
     // Luid specified and it's a valid luid => already exists
    if ( TableExistsL( aParentLUID ) )
        {
        // the object has already been created and mapping set to DM Framework
        status = CSmlDmAdapter::EAlreadyExists;
        }
    else if ( aURI.Match( KNSmlDMFotaRuntimeMatch ) != KErrNotFound )
        {
        TInt configFlags( KErrNone );        
        CRepository* centrep = NULL;
        TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );     
        if ( err == KErrNone && centrep ) 
            {
            configFlags = 2; //means fota operation
            centrep->Set( KDevManSessionType, configFlags );
            delete centrep;
            centrep = NULL;
            }
        else
            {
            User::Leave( err );
            }
            
    
        // add new FW object to db
        TNSmlDmFwObjectId newId = KErrNotFound;
        TRAP( err, newId = iFotaDb->AddFwObjectL() );
        
        if ( err == KErrNone )
            {
            // added ok
            TBuf8<KNSmlFwMgmtObjectIntegerLength> newLUID;
            newLUID.AppendNum( newId );
            
            Callback().SetMappingL( aURI, newLUID );
            }
        else
            {
            status = MapErrorToStatus( err );
            }

        }
    else
        {
        // error, invalid uri
        status = CSmlDmAdapter::ENotFound;
        }
    
    Callback().SetStatusL( aStatusRef, status );
//    if(status==CSmlDmAdapter::EOk)
//        {
//        TSmlProfileId profId = KErrNotFound;
//        HBufC8* serverId = NULL;
//        
//        GetServerInfoL( profId, serverId );
//        
//        if(serverId)
//            {
//            CleanupStack::PushL( serverId );
//            
//            _LIT( KFotaAclAdd, "Add=");
//            _LIT( KFotaAclGet, "Get=");
//            _LIT( KFotaAclReplace, "Replace=");
//            _LIT( KFotaAclDelete, "Delete=");
//            _LIT( KFotaAclExec, "Exec=");
//            _LIT( KFotaAclSeparator, "&");
//            _LIT( KFotaAclForAll, "*");
//            
//            const TUint length = KFotaAclAdd().Length() +
//                                 KFotaAclGet().Length() +
//                                 KFotaAclReplace().Length() +
//                                 KFotaAclDelete().Length() +
//                                 KFotaAclExec().Length() +
//                                 serverId->Length()*4 +
//                                 5; /* 4*'&'+ ''*' */
//                                 
//            HBufC8* acl = HBufC8::NewLC(length);
//            TPtr8 acltPtr = acl->Des();
//            acltPtr.Append(KFotaAclAdd);
//            acltPtr.Append(*serverId);
//            acltPtr.Append(KFotaAclSeparator);
//
//            acltPtr.Append(KFotaAclGet);
//            acltPtr.Append(*serverId);
//            acltPtr.Append(KFotaAclSeparator);
//
//            acltPtr.Append(KFotaAclReplace);
//            acltPtr.Append(*serverId);
//            acltPtr.Append(KFotaAclSeparator);
//
//            acltPtr.Append(KFotaAclDelete);
//            acltPtr.Append(KFotaAclForAll);
//            acltPtr.Append(KFotaAclSeparator);
//
//            acltPtr.Append(KFotaAclExec);
//            acltPtr.Append(*serverId);
//            
//            RNSmlDMCallbackSession session;
//            User::LeaveIfError( session.Connect() );
//            CleanupClosePushL(session);
//            session.UpdateAclL(aURI,*acl);
//            CleanupStack::PopAndDestroy( &session );
//            CleanupStack::PopAndDestroy( acl );
//            CleanupStack::PopAndDestroy( serverId );
//            }
//        }
    
    _DBG_FILE("CNSmlDmFotaAdapter::AddNodeObjectL(): end");
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::UpdateLeafObjectL()
// Updates a leaf object using streaming. In Fota adapter this method is used 
// only to update PkgData of a firmware object.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            RWriteStream*& aStream, 
                                            const TDesC8& /*aType*/, 
                                            const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::UpdateLeafObjectL(stream): begin");
    FLOG(_L("CNSmlDmFotaAdapter::UpdateLeafObjectL >>"));
    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotFound;
    
            
    Callback().SetStatusL( aStatusRef, status );
    
    _DBG_FILE("CNSmlDmFotaAdapter::UpdateLeafObjectL(stream): end");
    FLOG(_L("CNSmlDmFotaAdapter::UpdateLeafObjectL <<"));
    }

// ---------------------------------------------------------------------------
// void CNSmlDmFotaAdapter::FetchLeafObjectSizeL()
// Fetches data object and returns its size.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                               const TDesC8& aLUID, 
                                               const TDesC8& aType, 
                                               const TInt aResultsRef, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::FetchLeafObjectSizeL(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::FetchLeafObjectSizeL >>"));
    // buffer for result
    CBufBase* result = CBufFlat::NewL( KNSmlDMFotaResultBufferExpandSize );
    CleanupStack::PushL( result );
    
    // fetch data
    CSmlDmAdapter::TError status = DoFetchObjectL( aURI, aLUID, *result );
    if ( status == CSmlDmAdapter::EOk )
        {
        // get size and form a string representation
        TInt size = result->Size();
        TBuf8<KNSmlFwMgmtObjectIntegerLength> sizeStr;
        sizeStr.AppendNum( size );
        
        // insert size to result
        result->Reset();
        result->InsertL( 0, sizeStr );
        
        Callback().SetResultsL( aResultsRef, *result, aType );
        }
    
    CleanupStack::PopAndDestroy( result );
    
    Callback().SetStatusL( aStatusRef, status );
    
    _DBG_FILE("CNSmlDmFotaAdapter::FetchLeafObjectSizeL(): end");
    FLOG(_L("CNSmlDmFotaAdapter::FetchLeafObjectSizeL <<"));
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::ExecuteCommandL()
// Executes Update, Download or DownloadAndUpdate command depending on aURI. 
// If aURI does not match to any of these, ENotFound is set as status.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::ExecuteCommandL( const TDesC8& aURI, 
                                          const TDesC8& aLUID, 
                                          const TDesC8& aArgument, 
                                          const TDesC8& /*aType*/, 
                                          const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::ExecuteCommandL(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::ExecuteCommandL >>"));
    //#ifdef RD_OMADMAPPUI_UI_EVOLUTION
    TInt configFlags( 0 );        
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
    if ( centrep )
        {
        centrep->Get( KDevManSessionType, configFlags );            
        }
    if ( err != KErrNone ) 
        {
        User::Leave( err );
        }
    else
        {
        configFlags = 1;
        centrep->Set( KDevManSessionType, configFlags );
        }
    if( centrep )    
       {
        delete centrep;
       }        
    //#endif   
    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotFound;
    
    if ( aLUID.Length() > 0 )
        {
        TNSmlDmFwObjectId id = DesToInt( aLUID );
        
        // uri should target Update, Download or DownloadAndUpdate
        TPtrC8 lastSeg = LastURISeg( aURI );
        if ( lastSeg == KNSmlDMFotaNodeDownload ||
             lastSeg == KNSmlDMFotaNodeDownloadAndUpdate )
            {
            
            // save information about the execution and get profile id of 
            // currently running DM session
            TSmlProfileId profId = 0;
            TRAPD( err, profId = SaveExecInfoL( aURI, id, aArgument ) );
            
            if ( err == KErrNone )
                {
                if ( lastSeg == KNSmlDMFotaNodeDownload )
                    {
                    // download
                    status = DoExecDownloadL( id, profId, EFalse );
                    }
                else if ( lastSeg == KNSmlDMFotaNodeDownloadAndUpdate )
                    {
                    // download and update
                    status = DoExecDownloadL( id, profId, ETrue );
                    }
                
                }
            else
                {
                status = MapErrorToStatus( err );
                }
            }
        
        }
#ifndef __WINS__ 
    Callback().SetStatusL( aStatusRef, status );
#endif    
    _DBG_FILE("CNSmlDmFotaAdapter::ExecuteCommandL(): end");
    FLOG(_L("CNSmlDmFotaAdapter::ExecuteCommandL <<"));
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::ExecuteCommandL()
// Not supported.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                          const TDesC8& /*aLUID*/, 
                                          RWriteStream*& /*aStream*/, 
                                          const TDesC8& /*aType*/, 
                                          const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::ExecuteCommandL(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::ExecuteCommandL >>"));
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    FLOG(_L("CNSmlDmFotaAdapter::ExecuteCommandL <<"));
    _DBG_FILE("CNSmlDmFotaAdapter::ExecuteCommandL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::CopyCommandL()
// Not supported.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                                       const TDesC8& /*aTargetLUID*/, 
                                       const TDesC8& /*aSourceURI*/,
                                       const TDesC8& /*aSourceLUID*/, 
                                       const TDesC8& /*aType*/, 
                                       TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::CopyCommandL(): begin");
    
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    
    _DBG_FILE("CNSmlDmFotaAdapter::CopyCommandL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::StartAtomicL()
// Not supported.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::StartAtomicL()
    {
    _DBG_FILE("CNSmlDmFotaAdapter::StartAtomicL(): begin");
    _DBG_FILE("CNSmlDmFotaAdapter::StartAtomicL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::CommitAtomicL()
// Not supported.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::CommitAtomicL()
    {
    _DBG_FILE("CNSmlDmFotaAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CNSmlDmFotaAdapter::CommitAtomicL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::RollbackAtomicL()
// Not supported.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::RollbackAtomicL()
    {
    _DBG_FILE("CNSmlDmFotaAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CNSmlDmFotaAdapter::RollbackAtomicL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::StreamingSupport()
// Returns whether or not streaming is supported in this adapter.
// In Fota this method is also used to enquire whether or not a large object
// whose size is given in aItemSize fits to disk (OOD check using Fota Engine),
// and to acknowledge that Generic Alerts have been sent succesfully to remote 
// server. If aItemSize is larger that 0, OOD check feature is executed. 
// If aItemSize equals to KNSmlDMResetGenAlerts, Generic Alerts are marked 
// sent. Otherwise aItemSize is set to a treshold value, which is used by 
// DM Host Server to determineif streaming should be used (large object) 
// or not.
// ---------------------------------------------------------------------------
//
TBool CNSmlDmFotaAdapter::StreamingSupport( TInt& aItemSize )
    {
    _DBG_FILE("CNSmlDmFotaAdapter::StreamingSupport(): begin");
    FLOG(_L("CNSmlDmFotaAdapter::StreamingSupport, ITEM = %d >>"), aItemSize);
    TBool ret (EFalse);
    if ( aItemSize == KNSmlDMResetGenAlerts )
        {
        // generic alerts have been sent successfully to this server
        TRAPD( err, MarkGenericAlertsSentL() );
        if(err)
            {
        	_DBG_FILE("CNSmlDmFotaAdapter::StreamingSupport(): error in MarkGenricAlertSentL()");
            }
        ret = ETrue;
        }
        
    _DBG_FILE("CNSmlDmFotaAdapter::StreamingSupport(): end");
    FLOG(_L("CNSmlDmFotaAdapter::StreamingSupport <<"));
    return ret;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::StreamCommittedL()
// Notification method used by DM Host Server. All data has been written to 
// stream successfully and stream has been committed and closed.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::StreamCommittedL()
    {
    _DBG_FILE("CNSmlDmFotaAdapter::StreamCommittedL(): begin");
   
    _DBG_FILE("CNSmlDmFotaAdapter::StreamCommittedL(): end");
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::CompleteOutstandingCmdsL()
// Not supported.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE("CNSmlDmFotaAdapter::CompleteOutstandingCmdsL(): begin");
    _DBG_FILE("CNSmlDmFotaAdapter::CompleteOutstandingCmdsL(): end");
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::DoFetchObjectL()
// Fetches the data identified by aURI in FW object aLUID. Writes the data 
// to aObject and returns status code according to the success of the fetch.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmFotaAdapter::DoFetchObjectL( const TDesC8& aURI,
                                                          const TDesC8& aLUID,
                                                          CBufBase& aObject )
    {
    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotFound;
    
    if ( aLUID.Length() > 0 )
        {
        HBufC8* data = NULL;
        TNSmlDmFwObjectId id = DesToInt( aLUID );
        
        // get correct data according to the last segment of the uri
        TPtrC8 lastSeg = LastURISeg( aURI );
        if ( lastSeg == KNSmlDMFotaNodeName )
            {
            data = iFotaDb->PkgNameL( id );
            }
        else if ( lastSeg == KNSmlDMFotaNodeVersion )
            {
            data = iFotaDb->PkgVersionL( id );
            }
        else if ( lastSeg == KNSmlDMFotaNodeDownloadUrl || 
                  lastSeg == KNSmlDMFotaNodeDownloadAndUpdateUrl )
            {
            data = iFotaDb->PkgUrlL( id );
            }
        else if ( lastSeg == KNSmlDMFotaNodeState )
            {
            // state is fetched from fota engine
            RFotaEngineSession::TState state = FotaEngineL().GetState( id );
            data = HBufC8::NewL( KNSmlFwMgmtObjectIntegerLength );
            data->Des().AppendNum( state );
            }
        
        // if data is fetched ok, set results
        if ( data )
            {
            CleanupStack::PushL( data );
            
            aObject.InsertL( 0, *data );
            status = CSmlDmAdapter::EOk;
            
            CleanupStack::PopAndDestroy( data );
            }
        }
    
    return status;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::DoExecUpdateL()
// Gets needed data for Update from Fota DB and notifies Fota Engine that 
// Update should be executed. Returns status according to the success of 
// the method.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmFotaAdapter::DoExecUpdateL( 
                                                const TNSmlDmFwObjectId aLUID, 
                                                const TSmlProfileId aProfile )
    {
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    
    // get needed data from db for fota engine
    HBufC8* pkgName = iFotaDb->PkgNameL( aLUID );
    CleanupStack::PushL( pkgName );
    
    HBufC8* pkgVersion = iFotaDb->PkgVersionL( aLUID );
    CleanupStack::PushL( pkgVersion );
    
    TInt error = KErrNone;

    if ( pkgName && pkgVersion )
        {
        // notify fota engine to start update
        error = FotaEngineL().Update( aLUID, aProfile, *pkgName, *pkgVersion );
        
        // if immediate error occured, set final result to corresponding 
        // value so that generic alert is not sent
        if ( error == KErrNotFound )
            {
            // binary update package has not been downloaded yet -> not 
            // possible to update
            status = CSmlDmAdapter::ENotFound;
            iFotaDb->SetFinalResultL( KNSmlDMFotaUndefinedError, aLUID  );
            }
        else if ( error != KErrNone )
            {
            // other error
            status = CSmlDmAdapter::EError;
            iFotaDb->SetFinalResultL( KNSmlDMFotaUndefinedError, aLUID );
            }
        }
    else
        {
        // fw object was not found
        status = CSmlDmAdapter::ENotFound;
        }
    
    CleanupStack::PopAndDestroy( pkgVersion );
    CleanupStack::PopAndDestroy( pkgName );

    return status;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::DoExecDownloadL()
// Gets needed data for Download or DownloadAndUpdate from Fota DB and 
// notifies Fota Engine that Download or DownloadAndUpdate should be executed.
// Returns status according to the success of the method.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmFotaAdapter::DoExecDownloadL( 
                                                const TNSmlDmFwObjectId aLUID,
                                                const TSmlProfileId aProfile,
                                                TBool aUpdate )
    {
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    
    // get needed data from db for fota engine
    HBufC8* url = iFotaDb->PkgUrlL( aLUID );
    CleanupStack::PushL( url );
    
    HBufC8* pkgName = iFotaDb->PkgNameL( aLUID );
    CleanupStack::PushL( pkgName );
    
    HBufC8* pkgVersion = iFotaDb->PkgVersionL( aLUID );
    CleanupStack::PushL( pkgVersion );
    
    TInt error = KErrNone;
    
    if ( url && pkgName && pkgVersion )
        {
        TBool silent (EFalse);
        TInt value (ENonSilent);
        RProperty::Get(KPSUidNSmlSOSServerKey, KNSmlDMSilentJob, value);
        
        if (value == ESilent)
            {
            silent = ETrue;
            FLOG(_L("Session type is silent"));
            }
        if ( aUpdate )
            {
            // notify fota engine to start download and update
            error = FotaEngineL().DownloadAndUpdate( aLUID, *url, aProfile, *pkgName, *pkgVersion,silent );
            }
        else
            {
            // notify fota engine to start download
            error = FotaEngineL().Download( aLUID, *url, aProfile, *pkgName, *pkgVersion, silent );
            }
        
        // if immediate error occured, set final result to corresponding value so that 
        // generic alert is not sent
        if ( error == KErrArgument )
            {
            // url incorrectly formed
            status = CSmlDmAdapter::EInvalidObject;
            iFotaDb->SetFinalResultL( KNSmlDMFotaBadUrl, aLUID );
            }
        else if ( error != KErrNone )
            {
            // other error
            status = CSmlDmAdapter::EError;
            iFotaDb->SetFinalResultL( KNSmlDMFotaUndefinedError, aLUID );
            }
        }
    else
        {
        // fw object was not found
        status = CSmlDmAdapter::ENotFound;
        }
    
    CleanupStack::PopAndDestroy( pkgVersion );
    CleanupStack::PopAndDestroy( pkgName );
    CleanupStack::PopAndDestroy( url );

    return status;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::SaveExecInfoL()
// Saves the values needed when starting an exec command. Returns the saved 
// profile id.
// ---------------------------------------------------------------------------
//
TSmlProfileId CNSmlDmFotaAdapter::SaveExecInfoL( const TDesC8& aURI, 
                                                 const TNSmlDmFwObjectId aLUID,
                                                 const TDesC8& aCorrelator )
    {
    // set final result (null), uri and correlator to fota db
    User::LeaveIfError( iFotaDb->SetFinalResultL( KNSmlFotaNullResult, aLUID ) );
    User::LeaveIfError( iFotaDb->SetMgmtUriL( aURI, aLUID ) );
    User::LeaveIfError( iFotaDb->SetCorrelatorL( aCorrelator, aLUID ) );
    
    // fetch profile id and server id of the currently running dm session
    TSmlProfileId profId = KErrNotFound;
    HBufC8* serverId = NULL;
    
    GetServerInfoL( profId, serverId );
    CleanupStack::PushL( serverId );
    
    // save profile id and server id to fota db
    User::LeaveIfError( iFotaDb->SetServerInfoL( profId, *serverId, aLUID ) );
    
    CleanupStack::PopAndDestroy( serverId );
    
    return profId;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::GetServerInfoL()
// Fetches profile id and server id of the currently running dm session. 
// Data is fetched using Client API and set to the references given as 
// parameters. Reallocates server id to aServerId.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::GetServerInfoL( TSmlProfileId& aProfId, 
                                         HBufC8*& aServerId ) const
    {
    // Open session
    RSyncMLSession session;
    session.OpenL();
    CleanupClosePushL( session );
    
    // get current job's id (and usage type)
    TSmlJobId jobId = KErrNotFound;
    TSmlUsageType type = ESmlDevMan;
    session.CurrentJobL( jobId, type );
    
    // open currently running job
    RSyncMLDevManJob dmJob;
    dmJob.OpenL( session, jobId );
    CleanupClosePushL( dmJob );
    
    // get job's profile id
    aProfId = dmJob.Profile();
    
    // open profile using id in read-only mode
    RSyncMLDevManProfile dmProf;
    dmProf.OpenL( session, aProfId, ESmlOpenRead );
    CleanupClosePushL( dmProf );
    
    // get profile's server id
    aServerId = dmProf.ServerId().AllocL();
    
    // close handles
    CleanupStack::PopAndDestroy( &dmProf );
    CleanupStack::PopAndDestroy( &dmJob );
    CleanupStack::PopAndDestroy( &session );
    }
// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::CheckAndAddPredefinedNodeL()
// Checks if predefined node for FUMO is in tree if not add to the tree 
// ---------------------------------------------------------------------------
//    
void CNSmlDmFotaAdapter::CheckAndAddPredefinedNodeL()
    {
    
	TBuf8<KMaxFullName> temp;
	GetPredefinedNodeL(temp);
	if (temp.Length())
		{
    	RNSmlDMCallbackSession session;
    	User::LeaveIfError( session.Connect() );
    	CleanupClosePushL(session);
	TBuf8<KMaxName> node[4];
    TInt location = temp.LocateReverse(',');
    node[0].Copy(temp.Mid(location+1));
    TInt newLocation = (temp.Left(location-1)).LocateReverse(',');
    node[1].Copy(temp.Mid(newLocation+1,location-(newLocation+1)));
    location = newLocation;
    newLocation = (temp.Left(location-1)).LocateReverse(',');
    node[2].Copy(temp.Mid(newLocation+1,location-(newLocation+1)));
    node[3].Copy(temp.Mid(0,newLocation ));
	for(TInt i=0; i<4; i++)
	{	
		TBuf8<KMaxFullName> temp1;
		temp1.Zero();
		temp1.Copy(KNSmlDMFotaNode);
		temp1.Append(KNSmlDMFotaSeparatorDes);
		temp1.Append(node[i]);
   		HBufC8* luid = session.GetLuidAllocL( KNSmlDMFotaAdapterImplUid,temp1);
		CleanupStack::PushL(luid);
    	if ( luid->Length() > 0 )
			{
			CleanupStack::PopAndDestroy( luid );
    		CleanupStack::PopAndDestroy( &session );
			return;			
			}
   		else
    		{
      		// Add the mapping 
			TNSmlDmFwObjectId newId = KErrNotFound;
        	TRAPD( err, newId = iFotaDb->AddFwObjectL() );

	        if ( err == KErrNone )
				{
            	// added ok
            	TBuf8<KNSmlFwMgmtObjectIntegerLength> newLUID;
            	newLUID.AppendNum( newId );
      			session.AddMappingInfoL(KNSmlDMFotaAdapterImplUid, temp1, newLUID );
            	}
    		}
		CleanupStack::PopAndDestroy( luid );
	}
		CleanupStack::PopAndDestroy( &session );
    	}
    }
// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::GetPredefinedNodeL()
// Gets  predefined node for FUMO from cenrep 
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::GetPredefinedNodeL(TDes8& aNode)
	{
    CRepository* centrep( NULL);
	aNode.Zero();
   	centrep = CRepository::NewLC( TUid::Uid(0x101F9A0A) );

    if ( centrep )
	    {
    	TFullName temp;
		
    	if (centrep->Get( KDevManFUMOPredefinedNodes, temp )==KErrNone && temp.Length() )
			{
			temp.Trim();
			aNode.Copy(temp);
			}
		CleanupStack::PopAndDestroy(centrep);
	    }
	}
// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::MakeGenericAlertsL()
// Checks whether or not there are fw objects whose final result should be 
// sent to the remote server which is involved with the current DM session.
// If there are such objects, notifies private api using the data needed for
// generic alert generation.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::MakeGenericAlertsL()
    {
    FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL >>"));
    // search for fw objects that have empty final result
    RNSmlDmFwObjectArray ids;
    CleanupClosePushL( ids );
    
    iFotaDb->GetEmptyFinalResultsL( ids );
    if ( ids.Count() == 0 )
        {
        // none found, no need for generic alerts
        CleanupStack::PopAndDestroy( &ids );
        FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL 1"));
        return;
        }
    
    // get information of the remote DM server
    TSmlProfileId profId = 0;
    HBufC8* serverId = NULL;
    GetServerInfoL( profId, serverId );
    CleanupStack::PushL( serverId );
    
    // open handle to private api
    RNSmlPrivateAPI privateAPI;
    FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL 2"));
    privateAPI.OpenL();
    CleanupClosePushL( privateAPI );
    
    // go through all objects in ids
    for ( TInt i = 0; i < ids.Count(); ++i )
        {
        // check that server ids match. if not, move to next entry
    FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL 3"));
        HBufC8* fwServerId = iFotaDb->ServerIdL( ids[i] );
        CleanupStack::PushL( fwServerId );
        FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL 4"));
        if ( *serverId == *fwServerId )
            {
            // check that mgmt uri is not empty string. if it is, 
            // move to next entry
            HBufC8* fwMgmtUri = iFotaDb->MgmtUriL( ids[i] );
            FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL 5"));
            CleanupStack::PushL( fwMgmtUri );
            
            if ( *fwMgmtUri != KNullDesC8 )
                {
                // check that update has reached its final result
            FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL 6"));
                TInt finalResult = FotaEngineL().GetResult( ids[i] );
                
                RFotaEngineSession::TState finalState = FotaEngineL().GetState( ids[i] );
                
                if ( finalResult != KErrNotFound && finalState != RFotaEngineSession::EStartingUpdate)
                    {
                    // get fw object's correlator
                    HBufC8* fwCorrelator = iFotaDb->CorrelatorL( ids[i] );
                    CleanupStack::PushL( fwCorrelator );
                    
                    // add generic alert to dm message
                    TPtrC8 metaType = GetMetaType( *fwMgmtUri );
                    FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL 7"));
                    privateAPI.AddDMGenericAlertRequestL( *fwMgmtUri, metaType, KNSmlDMFotaMetaFormat, finalResult, *fwCorrelator );
                    FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL 8"));
                    CleanupStack::PopAndDestroy( fwCorrelator );
                    }
                }
            
            CleanupStack::PopAndDestroy( fwMgmtUri );
            }
        
        CleanupStack::PopAndDestroy( fwServerId );
        }
    
    CleanupStack::PopAndDestroy( &privateAPI );
    CleanupStack::PopAndDestroy( serverId );
    CleanupStack::PopAndDestroy( &ids );
    FLOG(_L("CNSmlDmFotaAdapter::MakeGenericAlertsL <<"));
    
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::MarkGenericAlertsSentL()
// Puts a final result value to those fw objects, that are 
// associated with current DM session's remote server, have 
// been a target to an exec command and the exec command has 
// finished. I.e. Generic Alert has been sent to remote server
// reporting the results of these exec commands --> not needed to 
// send Generic Alert anymore in next DM session.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::MarkGenericAlertsSentL()
    {
    
    FLOG(_L("CNSmlDmFotaAdapter::MarkGenericAlertsSentL >>"));
    
    // search for fw objects that have empty final result
    RNSmlDmFwObjectArray ids;
    CleanupClosePushL( ids );
    
    // get information of the remote DM server
    TSmlProfileId profId = 0;
    HBufC8* serverId = NULL;
    GetServerInfoL( profId, serverId );
    CleanupStack::PushL( serverId );
    
    FLOG(_L("CNSmlDmFotaAdapter::MarkGenericAlertsSentL 1"));
    iFotaDb->GetEmptyFinalResultsL( ids, *serverId );
    
    // go through all objects in ids
    for ( TInt i = 0; i < ids.Count(); ++i )
        {
        // check that update has reached its final result
    FLOG(_L("CNSmlDmFotaAdapter::MarkGenericAlertsSentL 2"));
        TInt finalResult = FotaEngineL().GetResult( ids[i] );
        if ( finalResult != KErrNotFound )
            {
            // set final result thus setting generic alert sent
        FLOG(_L("CNSmlDmFotaAdapter::MarkGenericAlertsSentL 3"));
            iFotaDb->SetFinalResultL( finalResult, ids[i] );
            
            // also notify fota engine, that generic alert has been
            // successfully sent, thus enabling possible cleanup
            FLOG(_L("CNSmlDmFotaAdapter::MarkGenericAlertsSentL, %d"), ids[i]);
            FotaEngineL().GenericAlertSentL( ids[i] );
            }
        }
    
    CleanupStack::PopAndDestroy( serverId );
    CleanupStack::PopAndDestroy( &ids );
    FLOG(_L("CNSmlDmFotaAdapter::MarkGenericAlertsSentL <<"));
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::GetMetaType()
// Determines by the given uri which meta/type should be used in generic 
// alert.Returns the correct type, or KNullDesC8 if uri does not match to 
// exec targets.
// ---------------------------------------------------------------------------
//
TPtrC8 CNSmlDmFotaAdapter::GetMetaType( const TDesC8& aURI ) const
    {
    TPtrC8 lastSeg = LastURISeg( aURI );
    
    if ( lastSeg == KNSmlDMFotaNodeDownload )
        {
        return KNSmlDMFotaDownloadMetaType();
        }
    else if ( lastSeg == KNSmlDMFotaNodeDownloadAndUpdate )
        {
        return KNSmlDMFotaDownloadAndUpdateMetaType();
        }

    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::MapStatusToError()
// Returns a status code corresponding to the system error code given 
// as parameter.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CNSmlDmFotaAdapter::MapErrorToStatus( TInt aError ) const
    {
    CSmlDmAdapter::TError err = CSmlDmAdapter::EError;
    
    switch( aError )
        {
        case KErrNone:
            err = CSmlDmAdapter::EOk;
            break;
        case KErrNotFound:
            err = CSmlDmAdapter::ENotFound;
            break;
        case KErrDiskFull:
            err = CSmlDmAdapter::EDiskFull;
            break;
        case KErrOverflow:
            err = CSmlDmAdapter::ETooLargeObject;
            break;
        default:
            break;
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::DesToInt()
// Converts a 8 bit descriptor to an integer.
// ---------------------------------------------------------------------------
//
TInt CNSmlDmFotaAdapter::DesToInt( const TDesC8& aLUID ) const
    {
    TLex8 lex( aLUID );
    TInt value = KErrNotFound;
    lex.Val( value );
    
    return value;
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::LastURISeg()
// Returns the last segment of the given uri.
// ---------------------------------------------------------------------------
//
TPtrC8 CNSmlDmFotaAdapter::LastURISeg( const TDesC8& aURI ) const
    {
    TInt pos = aURI.LocateReverse( KNSmlDMFotaSeparatorDes()[0] );
    if( pos == KErrNotFound )
        {
        return aURI;
        }
    else
        {
        return aURI.Mid( pos + 1 );
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::FillNodeInfoL()
// Fills the DDF node info.
// ---------------------------------------------------------------------------
//
void CNSmlDmFotaAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode, 
                                        const TSmlDmAccessTypes& aAccTypes, 
                                        MSmlDmDDFObject::TOccurence aOccurrence, 
                                        MSmlDmDDFObject::TScope aScope, 
                                        MSmlDmDDFObject::TDFFormat aFormat, 
                                        const TDesC8& aDescription ) const
    {
    aNode.SetAccessTypesL( aAccTypes );
    aNode.SetOccurenceL( aOccurrence );
    aNode.SetScopeL( aScope );
    aNode.SetDFFormatL( aFormat );
    
    if( aFormat != MSmlDmDDFObject::ENode )
        {
        aNode.AddDFTypeMimeTypeL( KNSmlDMFotaTextPlain );
        }
    
    aNode.SetDescriptionL( aDescription );
    }

RFotaEngineSession& CNSmlDmFotaAdapter::FotaEngineL()
    {
    FLOG(_L("CNSmlDmFotaAdapter::FotaEngineL >>"));
    if (!iFotaEngine.Handle())
        {
        FLOG(_L("Opening fota engine..."));
        iFotaEngine.OpenL();
        }
    
    FLOG(_L("CNSmlDmFotaAdapter::FotaEngineL <<"));
    return iFotaEngine;
    }
// ---------------------------------------------------------------------------
// TImplementationProxy ImplementationTable[]
// Needed because of ECOM architecture.
// ---------------------------------------------------------------------------
//
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KNSmlDMFotaAdapterImplUid, CNSmlDmFotaAdapter::NewL)
    };


// ========================== OTHER EXPORTED FUNCTIONS =========================

// ---------------------------------------------------------------------------
// ImplementationGroupProxy()
// Needed because of ECOM architecture.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmFotaAdapter: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmFotaAdapter: end");
    return ImplementationTable;
    }


// End of file

