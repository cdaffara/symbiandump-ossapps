/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of Application Management Components
*
*/




#include <e32svr.h>
#include "ApplicationManagementClientServer.h"
#include "ApplicationManagementClient.h"
#include "ApplicationManagementRfs.h"
#include "debug.h"
#include <apacmdln.h>
#include <apgtask.h>
#include <e32property.h>

#include <apgcli.h>

#include <badesca.h>

using namespace NApplicationManagement;

// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------

static TInt StartServer()
	{
    RDEBUG("RApplicationManagement: Starting server...");

    const TUidType serverUid(KNullUid, KNullUid,
            KApplicationManagementServerUid);

    // EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
    // launching of two such processes should be detected when the second one
    // attempts to create the server object, failing with KErrAlreadyExists.
    RProcess server;
    //TInt r=server.Create(KHelloWorldServerImg,KNullDesC,serverUid);
    TInt r = server.Create(KApplicationManagementServerImg, KNullDesC);
    if (r != KErrNone)
        {
        RDEBUG_2("ApplicationManagementClient: server start failed %d",r);
        return r;
        }
    TRequestStatus stat;
    server.Rendezvous(stat);
    if (stat != KRequestPending)
        server.Kill(0); // abort startup
    else
        server.Resume(); // logon OK - start the server
    RDEBUG("ApplicationManagementClient: Started");
    User::WaitForRequest(stat);
    TInt err = RProperty::Define(KProperty, KInteger, RProperty::EInt);
    RProperty ServerLaunch;
    ServerLaunch.Attach(KProperty, KInteger, EOwnerThread);
    TRequestStatus status;

    ServerLaunch.Subscribe(status);
    //server.Resume();
    User::WaitForRequest(status);

    ServerLaunch.Close();
    RProperty::Delete(KProperty, KInteger);

    //User::WaitForRequest(stat);		// wait for start or death
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
    // _LIT_SECURITY_POLICY_S0(KWritePolicy,KApplicationManagementServerUid.iUid);
    //  _LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );

    server.Close();
    return r;
    }


EXPORT_C TInt RAppMgmtRfs::Connect()
	{
	TInt retry=2;
	TVersion ver( KAMVerMajor, KAMVerMinor, KAMVerBuild );
	for (;;)
		{
		
		TInt r=CreateSession(KApplicationManagementServerName,ver,1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			{
			return r;
			}
		}
	}
	
	
EXPORT_C void RAppMgmtRfs::Close()
	{
	RSessionBase::Close();  //basecall
	}

// New methods
EXPORT_C void RAppMgmtRfs::PerformRfsL() const
	{
	User::LeaveIfError( SendReceive( EPerformRfs, TIpcArgs() ) );	
	}
	


// This is the standard retry pattern for server connection
EXPORT_C TInt RApplicationManagement::Connect( )
	{
	TInt retry=2;
	TVersion ver( KAMVerMajor, KAMVerMinor, KAMVerBuild );
	for (;;)
		{
		
		TInt r=CreateSession(KApplicationManagementServerName,ver,1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			{
			return r;
			}
		}
	}

EXPORT_C TInt RApplicationManagement::Connect( const TCertInfo &aCertInfo )
	{
	TInt r( Connect() );
	if ( r == KErrNone || r == KErrAlreadyExists )
		{
		TPckg<TCertInfo> pkg( aCertInfo );
		r = SendReceive( EAddTrust, TIpcArgs( &pkg ) );
		}
	return r;
	}

	
EXPORT_C void RApplicationManagement::Close()
	{
	RSessionBase::Close();  //basecall
	}


	
EXPORT_C TUint32 RApplicationManagement::DeliverL( const TDCUserId &aUserId ) const
	{
	TDeploymentComponent aInfo( KNullDesC8(), KNullDesC8(), KNullDesC8(), EDCSNone );
	TDeploymentComponentIPC pkg( aInfo, KNullDesC8(), aUserId );
		
	TPckg<TDeploymentComponentIPC> pinfo(pkg);
	TUint32 id = 0;
	TPckg< TUint32 > pid( id );
	const TDesC8& empty = KNullDesC8();
	
	TInt err( SendReceive( EDeplCompDeliverStartOperation2, TIpcArgs( &pinfo, &empty, &pid) ) );	
	if ( err == KErrNone )
		{
		RDEBUG_2( "ApplicationManagementServer: DeliverL start SUCCESS id: %d", id );
		//TPckg<TDeploymentComponent> pinfo2(aInfo);

		err = SendReceive( EDeplCompDeliverCompleteOperation, TIpcArgs( &pid, &empty, &empty ));
		
		if ( err == KErrNone )
			{	
			RDEBUG_2( "ApplicationManagementServer: DeliverL complete SUCCESS id: %d", id );
			err = id;
			}
		else
			{
			RDEBUG_2( "ApplicationManagementServer: DeliverL failed2... %d", err);
			}
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: DeliverL failed1... %d", err);
		}
	User::LeaveIfError( err );
	return err;
	}
	
	
	
EXPORT_C TInt RApplicationManagement::DeploymentComponent( 
	TUint32 aInternalId, TDeploymentComponent &aReturnValue ) const
	{
	TPckg<TDeploymentComponent> pinfo(aReturnValue);
	TPckg< TUint32 > pid( aInternalId );
	TInt err( SendReceive( EDeplCompGetOperation, TIpcArgs( &pid, &pinfo) ) );	
	if ( err == KErrNone )
		{
		RDEBUG_2( "RApplicationManagement: DeploymentComponent start SUCCESS id: %d", aInternalId );
		}
	else
		{
		RDEBUG_2("RApplicationManagement: DeploymentComponent failed... %d", err);
		}
		
	return err;
	}
	
	
EXPORT_C void RApplicationManagement::DeploymentComponentUserIdL( 
	TUint32 aInternalId, TDCUserId &aResponse ) const
	{
	
	TPckgC< TUint32 > pid( aInternalId );
	TInt err( SendReceive( EDeplCompGetUserIdOperation, TIpcArgs( &pid, &aResponse) ) );	
	if ( err == KErrNone )
		{
		RDEBUG_2( "RApplicationManagement: DeploymentComponentUserIdL SUCCESS id: %d", aInternalId );
		}
	else
		{
		RDEBUG_2( "RApplicationManagement: DeploymentComponentUserIdL failed... %d", err);
		}
		
	User::LeaveIfError( err );
	}
	
	
/**
* @param aInternalId Id of the component to be found
* @param aBuffer a buffer to store results
* @return KErrNone Symbian error code
*/
EXPORT_C void RApplicationManagement::DeploymentComponentDataL( 
	TUint32 aInternalId, TDeplCompAttrType aDataType, CBufBase &aBuffer ) const
	{
	//aBuffer.ResizeL(
	TPckg< TUint32 > pid( aInternalId );
	TPckg< TDeplCompAttrType > pdt( aDataType );
	TInt length( 0 );
	TPckg< TInt > plen( length );
	aBuffer.Reset();
	TInt err( KErrNone) ;
	if ( aDataType == EDCInstallOptions 
		|| aDataType == EDCMetaData 
		|| aDataType ==	EDCData
		|| aDataType ==	EDCDataMime
		|| aDataType ==	EDCMetaDataMime
	 	|| aDataType == EDCDownloadURI )
		{
		err = SendReceive( EDeplCompGetDataLengthOperation, TIpcArgs( &pid, &pdt, &plen) ) ;
		if ( err == KErrNone )
			{
		
			}
		else
			{
			RDEBUG_2( "RApplicationManagement: DeploymentComponentDataL EDeplCompGetDataLengthOperation ERROR failed to get length... %d", err );
			}
		}
	else if ( aDataType == EDCStatus)
		{
		length = 0; // to make sure we don't fetch it twice
		TPckgBuf<TInt> ptr;
		err = SendReceive( EDeplCompGetDataOperation, TIpcArgs( &pid, &pdt, &ptr ) );
		if ( err == KErrNone )
			{	
			TBuf8<10> b;
			b.AppendNum( ptr() );
			aBuffer.InsertL( 0, b );
			RDEBUG( "RApplicationManagement: DeploymentComponentDataL EDCDownloadStatus SUCCESS!" );
			}
		else
			{
			RDEBUG_2( "RApplicationManagement: DeploymentComponentDataL EDCDownloadStatus ERROR failed... %d", err );
			}
		}
	else if ( aDataType == EDCConRef )
		{
		length = 0; // to make sure we don't fetch it twice
		TPckgBuf<TInt> ptr;
		err = SendReceive( EDeplCompGetDataOperation, TIpcArgs( &pid, &pdt, &ptr ) );
		if ( err == KErrNone )
			{	
			aBuffer.InsertL( 0, ptr );
			RDEBUG( "RApplicationManagement: DeploymentComponentDataL EDCConRef SUCCESS!" );
			}
		else
			{
			RDEBUG_2( "RApplicationManagement: DeploymentComponentDataL EDCConRefERROR failed... %d" , err );
			}
		}
	else
		{
		User::Leave( KErrArgument );
		}
	if ( length > 0 )
		{
		HBufC8 *buf = HBufC8::NewLC( length );
		TPtr8 ptr( buf->Des() );
		err = SendReceive( EDeplCompGetDataOperation, TIpcArgs( &pid, &pdt, &ptr ) );
		if ( err == KErrNone )
			{	
			aBuffer.InsertL( 0, ptr );
			RDEBUG( "RApplicationManagement: DeploymentComponentDataL EDeplCompGetDataOperation SUCCESS!" );
			}
		else
			{
			RDEBUG_2( "RApplicationManagement: DeploymentComponentDataL EDeplCompGetDataOperationERROR failed... %d", err );
			}
		CleanupStack::PopAndDestroy( buf );
		}
	else
		{
		RDEBUG( "RApplicationManagement: DeploymentComponentDataL:  Length is zero" );
		}
	User::LeaveIfError( err );
	}
	
	
EXPORT_C TInt RApplicationManagement::DeploymentComponentIdsL(
			RElementIdArray &aArray, const TDeploymentComponentState aState	) const
	{
	RDEBUG_2( "RApplicationManagement: DeploymentComponentIds aState=%d!", aState );
	TInt count = 0;
	TPckg<TInt> cnt(count);
	TPckg<TDeploymentComponentState> pk(aState);
	TInt err( SendReceive( EDeplCompCountOperation, TIpcArgs( &cnt, &pk ) ) );
	RDEBUG_2( "RApplicationManagement: DeploymentComponentIds : Count is %d", count );
	if ( err == KErrNone )
		{
		if ( count > 0 )
			{
			aArray.Reset();
			for ( TInt i( 0 ); i != count; ++i )
				{
				aArray.AppendL( 0 );
				}
			TPtr8 p( reinterpret_cast< TUint8* >( &aArray[0] ), 0, count * sizeof( TUint32 ) );
			err = SendReceive( EDeplCompIdsOperation, TIpcArgs( &p, &pk ) ) ;	
			if ( err == KErrNone )
				{	
				RDEBUG( "RApplicationManagement: DeploymentComponentIds SUCCESS!" );
				}
			else
				{
				RDEBUG_2( "RApplicationManagement: DeploymentComponentIds failed... %d", err );
				}
			}
		}
	else
		{
		RDEBUG_2( "RApplicationManagement: DeploymentComponentIds failed to get count... %d", err);
		}
	return err;
	}
	
/**
* InstallL Installs given deployment component.
* @param aDeploymentComponent The deployment component to be installed
* @param aTargetState Will the installed component be actibve or inactive (default active)
* @return KErrNone Symbian error code
*/
EXPORT_C void RApplicationManagement::InstallL( TUint32 aInternalId, 
	TDeploymentComponentState aTargetState  ) const
	{
	TPckg<TUint32> id(aInternalId);
	if ( aTargetState != EDCSActive && aTargetState != EDCSInactive )
		{
		// install only to either of above states
		User::Leave( KErrArgument );
		}
	TPckg<TDeploymentComponentState> pk( aTargetState );

	TInt err( SendReceive( EInstallOperation, TIpcArgs( &id, &pk ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: InstallL SUCCESS!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: InstallL failed... %d", err);
		if( err == KErrNotFound )
			{
			err = KErrArgument ; // indicates that data was not filled...
			}
		}
	User::LeaveIfError( err ) ;
	}
	
EXPORT_C void RApplicationManagement::UpdateDeploymentComponentDataL( 
		TUint32 aInternalId, TDeplCompAttrType aItem, const TDesC8 &aItemData, const TDesC8 &aDataMime  ) const
	{
	TDeplCompDataIPC ipc ;
	ipc.iDataMime = aDataMime;
	ipc.iInternalId = aInternalId ;
	ipc.iAttrType = aItem ;
	TPckg<TDeplCompDataIPC> pipc( ipc );
	
	TInt err( SendReceive( EDeplCompUpdateDataOperation, TIpcArgs( &pipc, &aItemData ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: UpdateDeploymentComponentDataL SUCCESS!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: UpdateDeploymentComponentDataL failed... %d", err);
		}
	User::LeaveIfError( err );
	}

EXPORT_C void RApplicationManagement::UpdateDeploymentComponentStreamedDataL( 
		TUint32 aInternalId, TDeplCompAttrType aItem, const TDesC8 &aDataMime  ) const
	{
	TDeplCompDataIPC ipc ;
	ipc.iDataMime = aDataMime;
	ipc.iInternalId = aInternalId ;
	ipc.iAttrType = aItem ;
	TPckg<TDeplCompDataIPC> pipc( ipc );
	
	TInt err( SendReceive( EDeplCompUpdateStreamedDataOperation, TIpcArgs( &pipc ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: UpdateDeploymentComponentStreamedDataL SUCCESS!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: UpdateDeploymentComponentStreamedDataL failed... %d", err);
		}
	User::LeaveIfError( err );
	}
		
	
EXPORT_C void RApplicationManagement::UpdateDeploymentComponentL( 
			TUint32 aInternalId, TDeplCompAttrType aItem, const TDesC8 &aItemData ) const
	{
	TPckg<TUint32> id(aInternalId);
	TPckg<TDeplCompAttrType> itemid(aItem);
	
	TInt err( SendReceive( EDeplCompAttrUpdateOperation, TIpcArgs( &id, &itemid, &aItemData ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: UpdateDeploymentComponentL SUCCESS!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: UpdateDeploymentComponentL failed... %d", err);
		}
	User::LeaveIfError( err );
	}
	

/**
* RemoveL Removes given deployment component.
* @param aInternalId Id of the component to be removed
* @return KErrNone Symbian error code
*/
EXPORT_C void RApplicationManagement::RemoveL( TUint32 aInternalId ) const
	{
	TPckg< TUint32 > id( aInternalId );
	RDEBUG_2( "RApplicationManagement: RemoveL %d!" , aInternalId );
		
	TInt err( SendReceive( EDeplCompRemoveOperation, TIpcArgs( &id ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: RemoveL SUCCESS!" );
		}
	else if ( err == KErrNotFound )
		{
		RDEBUG( "RApplicationManagement: RemoveL WARNING Component already missing!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: RemoveL failed... %d", err);
		User::LeaveIfError( err );
		}
	}
	

/**
		* StartDownloadL Starts download of deployment component.
		* @param aInternalId The id of the component to be downloaded
	    * @return 
		*/
EXPORT_C void RApplicationManagement::StartDownloadL( TUint32 aInternalId, 
		const TDownloadTarget aTarget ) const
	{
	TPckg< TUint32 > id( aInternalId );
	RDEBUG_2( "RApplicationManagement: StartDownloadL %d!", aInternalId );
	TPckg< TDownloadTarget > trg( aTarget );
	TInt err( SendReceive( EDeplCompStartDownloadOperation, TIpcArgs( &id, &trg ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: StartDownloadL SUCCESS!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: StartDownloadL failed... %d", err);
		}
	User::LeaveIfError( err );
	}
		

EXPORT_C TUint32 RApplicationManagement::DownloadL( const TDCUserId &aUserId ) const
	{
	TUint32 id( 0 ) ;
	TPckg< TUint32 > pid( id );
	
	TInt err( SendReceive( EDeplCompDownloadOperation, TIpcArgs( &pid, &aUserId ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: DownloadL SUCCESS!" );
		err = pid() ;
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: DownloadL failed... %d", err);
		}
	return err ;
	}
	
EXPORT_C void RApplicationManagement::UpdateL( TUint32 aSourceInternalId, TUint32 aTargetInternalId ) const
	{
	TPckg< TUint32 > sid( aSourceInternalId );
	TPckg< TUint32 > tid( aTargetInternalId );
	
	TInt err( SendReceive( EDeplCompUpdateOperation, TIpcArgs( &sid, &tid ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: UpdateL SUCCESS!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: UpdateL failed... %d", err);
		}
	User::LeaveIfError( err );
	}
	


EXPORT_C TInt RApplicationManagement::StateChangingDCIdsL( RElementIdArray &aArray ) const
	{
	RDEBUG( "RApplicationManagement: StateChangingDCIds " );
	TInt count( 0 ) ;
	TPckg<TInt> cnt(count);
	TInt err( SendReceive( EGetStateChangingIdsCountOperation, TIpcArgs( &cnt ) ) );	
	if ( err == KErrNone )
		{
		aArray.Reset();
		if ( count > 0 )
			{
			for ( TInt i( 0 ); i != count; ++i )
				{
				aArray.AppendL( 0 );
				}
			TPtr8 p( reinterpret_cast< TUint8* >( &aArray[0] ), 0, count * sizeof( TUint32 ) );
			err = SendReceive( EGetStateChangingIdsOperation, TIpcArgs( &p ) ) ;	
			if ( err == KErrNone )
				{	
				RDEBUG( "RApplicationManagement: StateChangingDCIds SUCCESS!" );
				}
			else
				{
				RDEBUG_2( "RApplicationManagement: StateChangingDCIds failed... %d" , err );
				}
			}
		else
			{
			RDEBUG_2("RApplicationManagement: StateChangingDCIds SUCCESS zero dcs... %d", count);	
			}
		}
	else
		{
		RDEBUG_2("RApplicationManagement: StateChangingDCIds failed... %d", err);
		}
	return err;
	}
	

EXPORT_C TInt RApplicationManagement::StateChangeComplete( TUint32 aInternalId ) const
	{
	TPckg<TUint32> id(aInternalId);
	
	TInt err( SendReceive( ECompleteStateChangeOperation, TIpcArgs( &id ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: StateChangeComplete SUCCESS!" );		
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: StateChangeComplete failed... %d", err);
		if( err == KErrNotFound )
			{
			err = KErrArgument ; // indicates that data was not filled...
			}
		}
	return ( err ) ;
	}


/**
* ActivateL Tries to activate given deployment component
*	
* @param aInternalId The id of the component to be activated
*/
EXPORT_C void RApplicationManagement::ActivateL( TUint32 aInternalId ) const
	{
	TPckg< TUint32 > id( aInternalId );
	RDEBUG_2( "RApplicationManagement: ActivateL %d!" , aInternalId );
	
	TInt err( SendReceive( EDeplCompActivateOperation, TIpcArgs( &id ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: ActivateL SUCCESS!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: ActivateL failed... %d", err);
		}
	User::LeaveIfError( err );	
	}
	
/**
* DeactivateL Tries to deactivate given deployment component
*	
* @param aInternalId The id of the component to be deactivated
*/
EXPORT_C void RApplicationManagement::DeactivateL( TUint32 aInternalId ) const
	{
	TPckg< TUint32 > id( aInternalId );
	RDEBUG_2( "RApplicationManagement: DeactivateL %d!", aInternalId );
	
	TInt err( SendReceive( EDeplCompDeactivateOperation, TIpcArgs( &id ) ) );	
	if ( err == KErrNone )
		{
		RDEBUG( "RApplicationManagement: DeactivateL SUCCESS!" );
		}
	else
		{
		RDEBUG_2("ApplicationManagementServer: DeactivateL failed... %d", err);
		}
	User::LeaveIfError( err );	
	}
		

/**
* DeploymentComponentDataSizeL Get deployment component attribute size
*	
* @param aInternalId The id of the component 
* @param aItem The type of attribute whose size is to be got
* @return The size
*/
EXPORT_C TInt RApplicationManagement::DeploymentComponentDataSizeL( TUint32 aInternalId, const TDeplCompAttrType &aDataType ) const
	{
	TPckg< TUint32 > pid( aInternalId );
	TPckg< TDeplCompAttrType > pdt( aDataType );
	TInt length( 0 );
	TPckg< TInt > plen( length );
	TInt err( KErrNone) ;
	if ( aDataType == EDCInstallOptions 
		|| aDataType == EDCMetaData 
		|| aDataType ==	EDCData
	 	|| aDataType == EDCDownloadURI )
		{
		err = SendReceive( EDeplCompGetDataLengthOperation, TIpcArgs( &pid, &pdt, &plen) ) ;
		if ( err == KErrNone )
			{
			
			}
		}
	else if ( aDataType == EDCStatus )
		{
		length = 0;
		TPckgBuf<TInt> ptr;
		err = SendReceive( EDeplCompGetDataOperation, TIpcArgs( &pid, &pdt, &ptr ) );
		if ( err == KErrNone )
			{	
			TBuf8<10> b;
			b.AppendNum( ptr() );
			length = b.Length();
			RDEBUG( "RApplicationManagement: DeploymentComponentDataSizeL elseif SUCCESS!" );
			}
		else
			{
			RDEBUG_2( "RApplicationManagement: DeploymentComponentDataSizeL ERROR failed... %d", err );
			}
		}
	else
		{
		TPckgBuf<TDeploymentComponent> pinfo;
		TPckg< TUint32 > pid( aInternalId );
		err = SendReceive( EDeplCompGetOperation, TIpcArgs( &pid, &pinfo) );
		if ( err == KErrNone )
			{
			RDEBUG_2( "RApplicationManagement: DeploymentComponentDataSizeL else SUCCESS id: %d", aInternalId );
			switch ( aDataType )
				{
				case EDCId:
					{
					length = pinfo().iId.Length();
					break;
					}
				case EDCName:
					{
					length = pinfo().iName.Length();
					break;
					}
				case EDCVersion:
					{
					length = pinfo().iVersion.Length();
					break;
					}
				default:
					{
					break;
					}
				}
			}
		else
			{
			RDEBUG_2("RApplicationManagement: DeploymentComponentDataSizeL failed... %d", err);
			}
		}
			
	RDEBUG_2("RApplicationManagement: DeploymentComponentDataSizeL returning... %d", length);
			
	return length;
	}

/**
* DeploymentComponentTempInstFileL ask file handle from server to be used
*	
* @param aInternalId The id of the component 
* @param aTempFile temporary file handle 
* @param aItem The type of attribute whose type of temporary file to get
* @return KErrNone Symbian error code
*/
EXPORT_C void RApplicationManagement::DeploymentComponentTempInstFileL( TUint32 aInternalId, RFile& aTempFile, const TDeplCompAttrType &aItem ) const
	{
	RDEBUG_3( "RApplicationManagement: DeploymentComponentTempInstFileL: Id (%d) AttrType (%d)", aInternalId, aItem );
	TPckg<TUint32> id(aInternalId);
	TPckg<TDeplCompAttrType> itemid(aItem);
	TPckgBuf<TInt> filehandle;

	TInt fshandle = SendReceive(EDeplCompGetTemporaryInstFile, TIpcArgs( &id, &itemid, &filehandle ) );
	TInt err = aTempFile.AdoptFromServer( fshandle, filehandle() ) ;
	User::LeaveIfError( err );
	RDEBUG_2( "RApplicationManagement: DeploymentComponentTempInstFileL fshandle (%d)", fshandle );
	}

/**
* GetFinalResultsForGAL ask results from AM Server. In response AM 
* server returns final result and target URI of node
*	
* @param aInternalId The id of the component 
* @param aFinalResult Final result of deployment component
* @param aTargetURI Target URI of deployment component
* @return void
*/

EXPORT_C void RApplicationManagement::GetFinalResultsForGAL(TUint32 aInternalId, TInt& aFinalResult,  TDes8& aTargetURI) const
{
	TPckg<TUint32>  internalid(aInternalId);
	TPckg<TInt>  finalresult(aFinalResult);
	TPckg<TDesC8> targeturi(aTargetURI);
	
	TInt err = SendReceive(EDeplCompGetFinalResultForGA, TIpcArgs( &internalid, &finalresult, &aTargetURI) );
}

EXPORT_C void RApplicationManagement::GenericAlertSentL(TUint32 aInternalId) const
{
	
	TPckg<TUint32>  internalid(aInternalId);
	
	
	TInt err = SendReceive(EDeplCompGenericAlertSent, TIpcArgs( &internalid) );
}

/**
* Implementation of Service UID() from base class. Returns AM service UID 
* to framework
*	
* @return uid of AM service UID
*/
EXPORT_C void RApplicationManagement::CheckStatusNodesL( ) const
    {
    User::LeaveIfError( SendReceive( ECheckStatusNodes, TIpcArgs() ) );
    }

TUid RApplicationManagement::ServiceUid() const
	{
    	

	return TUid::Uid( KAMServiceUid);
	}

//  End of File
