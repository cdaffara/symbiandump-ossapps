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
* Description:  Monitors file creations, modifications and deletions.
*
*/

#include <f32pluginutils.h>
#include "fastfindfileserverplugin.h"
#include "harvesterlog.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "fastfindfileserverpluginTraces.h"
#endif


_LIT( KFastFindFileServerPlugin, "CPixFileServerPlugin" );

//-----------------------------------------------------------------------------
// CFastFindFileServerPlugin implementation
//-----------------------------------------------------------------------------
//
CFastFindFileServerPlugin::CFastFindFileServerPlugin()
: iFormatDriveNumber( -1 )
	{
	OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_CFASTFINDFILESERVERPLUGIN, "CFastFindFileServerPlugin::CFastFindFileServerPlugin()" );
	WRITELOG( "CFastFindFileServerPlugin::CFastFindFileServerPlugin()" );
	}
	
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
CFastFindFileServerPlugin::~CFastFindFileServerPlugin()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGIN_CFASTFINDFILESERVERPLUGIN, "CFastFindFileServerPlugin::~CFastFindFileServerPlugin()" );
    WRITELOG( "CFastFindFileServerPlugin::~CFastFindFileServerPlugin()" );
    
    TRAP_IGNORE( DisableL() );
    iFsSession.Close();
    
    iCreatedFiles.ResetAndDestroy();
    iCreatedFiles.Close();
    
    iPaths.ResetAndDestroy();
    iPaths.Close();
    
    iIgnorePaths.ResetAndDestroy();
    iIgnorePaths.Close();
        
    iQueue.ResetAndDestroy();
    iQueue.Close();
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
CFastFindFileServerPlugin* CFastFindFileServerPlugin::NewL()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_NEWL, "CFastFindFileServerPlugin::NewL()" );
    WRITELOG( "CFastFindFileServerPlugin::NewL()" );
    return new (ELeave) CFastFindFileServerPlugin;
    }

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPlugin::InitialiseL()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_INITIALISEL, "CFastFindFileServerPlugin::InitializeL()" );
    WRITELOG( "CFastFindFileServerPlugin::InitializeL()" );
    User::LeaveIfError( iFsSession.Connect() );
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPlugin::EnableL()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_ENABLEL, "CFastFindFileServerPlugin::EnableL()" );
    WRITELOG( "CFastFindFileServerPlugin::EnableL()" );
    User::LeaveIfError( RegisterIntercept(EFsFileCreate, EPostIntercept) );
    User::LeaveIfError( RegisterIntercept(EFsFileRename, EPostIntercept) );
    User::LeaveIfError( RegisterIntercept(EFsRename, EPostIntercept) );
    User::LeaveIfError( RegisterIntercept(EFsDelete, EPostIntercept) );
//    User::LeaveIfError( RegisterIntercept(EFsFileReplace, EPostIntercept) );
    User::LeaveIfError( RegisterIntercept(EFsReplace, EPostIntercept) );
    User::LeaveIfError( RegisterIntercept(EFsMkDir, EPostIntercept) );
    User::LeaveIfError( RegisterIntercept(EFsRmDir, EPostIntercept) );
//    User::LeaveIfError( RegisterIntercept(EFsFileModified, EPostIntercept) );
    User::LeaveIfError( RegisterIntercept(EFsFileSetModified, EPostIntercept) );
    User::LeaveIfError( RegisterIntercept(EFsFileSubClose, EPostIntercept) );
	// format events
	User::LeaveIfError( RegisterIntercept(EFsFormatSubClose, EPostIntercept) );
	User::LeaveIfError( RegisterIntercept(EFsFormatOpen, EPostIntercept) );

#ifdef _DEBUG_EVENTS
    RegisterDebugEventsL();
#endif
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPlugin::DisableL()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_DISABLEL, "CFastFindFileServerPlugin::DisableL()" );
    WRITELOG( "CFastFindFileServerPlugin::DisableL()" );
    User::LeaveIfError( UnregisterIntercept(EFsFileCreate, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept(EFsFileRename, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept(EFsRename, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept(EFsDelete, EPrePostIntercept) );
//    User::LeaveIfError( UnregisterIntercept(EFsFileReplace, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept(EFsReplace, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept(EFsMkDir, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept(EFsRmDir, EPrePostIntercept) );
//    User::LeaveIfError( UnregisterIntercept(EFsFileModified, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept(EFsFileSetModified, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept(EFsFileSubClose, EPrePostIntercept) );
	// format events
	User::LeaveIfError( UnregisterIntercept(EFsFormatSubClose, EPostIntercept) );
	User::LeaveIfError( UnregisterIntercept(EFsFormatOpen, EPostIntercept) );

#ifdef _DEBUG_EVENTS
    UnregisterDebugEventsL();
#endif
    }
    

//-----------------------------------------------------------------------------
// AddConnection
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPlugin::AddConnection()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_ADDCONNECTION, "CFastFindFileServerPlugin::AddConnection()" );
    WRITELOG( "CFastFindFileServerPlugin::AddConnection()" );
    
    ++iConnectionCount;
    }

//-----------------------------------------------------------------------------
// RemoveConnection
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPlugin::RemoveConnection()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_REMOVECONNECTION, "CFastFindFileServerPlugin::RemoveConnection()" );
    WRITELOG( "CFastFindFileServerPlugin::RemoveConnection()" );
    
    --iConnectionCount;

    // remove notification request if this was last connection
    if ( iConnectionCount <= 0 )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGIN_REMOVECONNECTION, "CFastFindFileServerPlugin::RemoveConnection() last connection" );
        WRITELOG( "CFastFindFileServerPlugin::RemoveConnection() last connection" );

        iNotification = NULL;
        }
    }

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
TInt CFastFindFileServerPlugin::DoRequestL( TFsPluginRequest& aRequest )
	{
	OstTraceFunctionEntry0( CFASTFINDFILESERVERPLUGIN_DOREQUESTL_ENTRY );
	TInt err = KErrNone;
	TInt function = aRequest.Function();
	const TBool formatFunction = function == EFsFormatOpen || function == EFsFormatSubClose;

#ifdef _DEBUG_EVENTS
    PrintDebugEvents( function );
#endif

    if ( function == EFsFileSubClose && iCreatedFiles.Count() == 0 )
        {
        OstTraceFunctionExit0( CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
        return KErrNone;
        }

    OstTrace1( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL;Plugin function=%d", function );
    WRITELOG1( "----- CFastFindFileServerPlugin::DoRequestL() - plugin function: %d -----", function );

    TFileName fileName;
    fileName.Zero();
    TFileName newFileName;
    newFileName.Zero();

	if ( !formatFunction )
		{
		GetName( &aRequest, fileName );
		OstTraceExt1( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL;Filename=%S", fileName );
		WRITELOG1( "CFastFindFileServerPlugin::DoRequestL() - fileName: %S", &fileName );
		}
	
	if ( function == EFsMkDir || function == EFsRmDir)
	    {
        // This will return the folder path with trailing slash, remove it to match
	    // with the doc_id format
        TFileName filePath;
	    GetPath(&aRequest, filePath);
		if (filePath.Length() > 0)
			{
        	TPtrC temppath( filePath.Left( filePath.Length() - 1 ) );
        	fileName.Zero();
        	fileName.Append(temppath);
			}
		OstTraceExt1( TRACE_NORMAL, DUP2_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL;Filename=%S", fileName );
	    WRITELOG1( "CFastFindFileServerPlugin::DoRequestL() - fileName: %S", &fileName );
	    }

    // get process id
	TUid processId = { 0 };

    /*RThread clientThread;
    RProcess clientProcess;
    err = aRequest.Message().Client( clientThread, EOwnerThread );
    if ( err == KErrNone )
        {
        err = clientThread.Process( clientProcess );
        if ( err == KErrNone )
            {
            processId = clientProcess.Identity();
            }
        }
    clientProcess.Close();
    clientThread.Close();*/

	processId = aRequest.Message().Identity();

    // if rename, check destination path
    if ( function == EFsRename || function == EFsFileRename || function == EFsReplace )
        {
        GetNewName( &aRequest, newFileName );
        OstTraceExt1( TRACE_NORMAL, DUP3_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL;New Filename=%S", newFileName );
        WRITELOG1( "CFastFindFileServerPlugin::DoRequestL() - newFileName: %S", &newFileName );
        if ( !CheckPath(newFileName) )
            {
            OstTrace0( TRACE_NORMAL, DUP4_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - path not supported" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - path not supported" );
            OstTraceFunctionExit0( DUP1_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
            return KErrNone;
            }
        if ( !CheckAttribs(newFileName) )
            {
            OstTrace0( TRACE_NORMAL, DUP5_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - attribute check failed" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - attribute check failed" );
            OstTraceFunctionExit0( DUP2_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
            return KErrNone;
            }
        }
    else if ( !formatFunction )
        {
        if ( !CheckPath(fileName) )
            {
            OstTrace0( TRACE_NORMAL, DUP6_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - path not supported" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - path not supported" );
            OstTraceFunctionExit0( DUP3_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
            return KErrNone;
            }

        if ( !CheckAttribs(fileName) )
            {
            OstTrace0( TRACE_NORMAL, DUP7_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - attribute check failed" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - attribute check failed" );
            OstTraceFunctionExit0( DUP4_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
            return KErrNone;
            }
        }

    TInt fileEventType = EFastFindFileUnknown;
    TInt drvNumber = aRequest.DriveNumber();

    OstTrace1( TRACE_NORMAL, DUP8_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL;Drive Number=%d", drvNumber );
    WRITELOG1( "CFastFindFileServerPlugin::DoRequestL() - drive number: %d", drvNumber );

    switch( function )
        {
        case EFsFileCreate:
            {
            OstTrace0( TRACE_NORMAL, DUP9_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsFileCreate" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - EFsFileCreate" );

            TFileName* fn = new (ELeave) TFileName;
            fn->Zero();
            fn->Copy( fileName );
            iCreatedFiles.Append( fn );
            User::LeaveIfError( UnregisterIntercept(EFsFileSetModified, EPrePostIntercept) );
            OstTraceFunctionExit0( DUP5_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
            return KErrNone;
            }

        case EFsFileSubClose:
            {
            OstTrace0( TRACE_NORMAL, DUP10_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsFileSubClose" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - EFsFileSubClose" );

            TBool found = EFalse;
            for ( TInt i = 0; i < iCreatedFiles.Count(); i++ )
                {
                TFileName* fn = iCreatedFiles[i];
                if ( fn->CompareC(fileName,1,NULL) == 0 )
                    {
                    fileEventType = EFastFindFileCreated;
                    delete fn;
                    fn = NULL;
                    iCreatedFiles.Remove( i );
                    found = ETrue;
                    User::LeaveIfError( RegisterIntercept(EFsFileSetModified, EPostIntercept) );
                    }
                }

            if ( !found )
                {
                OstTraceFunctionExit0( DUP6_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
                return KErrNone;
                }
            }
            break;

        case EFsRename:
            {
            OstTraceExt1( TRACE_NORMAL, DUP11_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsRenamed;New Filename=%S", newFileName );
            WRITELOG1( "CFastFindFileServerPlugin::DoRequestL() - EFsRenamed, new file: %S", &newFileName );
            fileEventType = EFastFindFileRenamed;
            }
            break;

        case EFsFileRename:
            OstTraceExt1( TRACE_NORMAL, DUP12_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsFileRenamed;New Filename=%S", newFileName );
            WRITELOG1( "CFastFindFileServerPlugin::DoRequestL() - EFsFileRenamed, new file: %S", &newFileName );
            fileEventType = EFastFindFileRenamed;
            break;

        case EFsFileSetModified:
            OstTrace0( TRACE_NORMAL, DUP13_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsFileModified" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - EFsFileModified" );
            fileEventType = EFastFindFileModified;
            break;

        case EFsReplace:
            OstTraceExt1( TRACE_NORMAL, DUP14_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL()  - EFsReplace;New Filename=%S", newFileName );
            WRITELOG1( "CFastFindFileServerPlugin::DoRequestL() - EFsReplace, new file: %S", &newFileName );
            fileEventType = EFastFindFileReplaced;
            break;

        case EFsDelete:
            OstTrace0( TRACE_NORMAL, DUP15_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsDelete" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - EFsDelete" );
            fileEventType = EFastFindFileDeleted;
            break;

		case EFsFormatOpen:
			OstTrace0( TRACE_NORMAL, DUP16_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsFormatOpen" );
			WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - EFsFormatOpen" );
			// get the drive letter
			iFormatDriveNumber = drvNumber;
			fileEventType = EFastFindDriveFormatted;
			processId.iUid = 0;
			//return KErrNone;
			break;

		case EFsFormatSubClose:
			OstTrace0( TRACE_NORMAL, DUP17_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsFormatSubClose" );
			WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - EFsFormatSubClose" );
			if ( iFormatDriveNumber < 0 )
				{
				OstTraceFunctionExit0( DUP7_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
				return KErrNone;
				}
			drvNumber = iFormatDriveNumber;
			iFormatDriveNumber = -1;
			fileEventType = EFastFindDriveFormatted;
			if ( processId.iUid == 0 )
				{
				processId.iUid = 1;
				}
			break;
			
		case EFsMkDir:
		    OstTrace0( TRACE_NORMAL, DUP18_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsMkDir" );
		    WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - EFsMkDir" );		    
		    fileEventType = EFastFindDirCreated;
		    
            break;
            
		case EFsRmDir:
		    OstTrace0( TRACE_NORMAL, DUP19_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - EFsRmDir" );
		    WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - EFsRmDir" );
		    fileEventType = EFastFindDirDeleted;
		    break;
            
        default:
            OstTrace0( TRACE_NORMAL, DUP20_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - Unknown function" );
            WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - Unknown function" );
            OstTraceFunctionExit0( DUP8_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
            return KErrNone;
        }

    if ( iNotification )
        {
        OstTrace0( TRACE_NORMAL, DUP21_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - iNotification found" );
        WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - iNotification found" );
        TFastFindFSPStatusPckg clientStatusBuf;
        TFastFindFSPStatus& clientStatus = clientStatusBuf();
        clientStatus.iDriveNumber = drvNumber;
        clientStatus.iFileName.Copy( fileName );
        if ( newFileName.Length() > 0 )
            {
            clientStatus.iNewFileName.Copy( newFileName );
            }
        clientStatus.iFileEventType = fileEventType;
        clientStatus.iProcessId = processId;

        TRAP( err, iNotification->WriteParam1L(clientStatusBuf) );
        iNotification->Complete( err );
        iNotification = NULL;
        OstTrace0( TRACE_NORMAL, DUP22_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - iNotification complete" );
        WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - iNotification complete" );
        }
    else // no notification ready, put in the queue
        {
        OstTrace0( TRACE_NORMAL, DUP23_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - iNotification not found. Put in the queue" );
        WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - iNotification not found. Put in the queue" );

        TFastFindFSPStatus* clientStatus = new (ELeave) TFastFindFSPStatus;
        clientStatus->iDriveNumber = drvNumber;
        clientStatus->iFileName.Copy( fileName );
        if ( newFileName.Length() > 0 )
            {
            clientStatus->iNewFileName.Copy( newFileName );
            }
        clientStatus->iFileEventType = fileEventType;
        clientStatus->iProcessId = processId;

        iQueue.Append( clientStatus ); // owenership is transferred
        err = KErrNone;
        OstTrace0( TRACE_NORMAL, DUP24_CFASTFINDFILESERVERPLUGIN_DOREQUESTL, "CFastFindFileServerPlugin::DoRequestL() - added to queue" );
        WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - added to queue" );
        }

    WRITELOG( "CFastFindFileServerPlugin::DoRequestL() - return" );
    OstTraceFunctionExit0( DUP9_CFASTFINDFILESERVERPLUGIN_DOREQUESTL_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// CFastFindFileServerPluginConn implementation
//-----------------------------------------------------------------------------
class CFastFindFileServerPluginConn : public CFsPluginConn
    {
    public:
        static CFastFindFileServerPluginConn* NewL( CFastFindFileServerPlugin& aPlugin );
        virtual ~CFastFindFileServerPluginConn();

        virtual TInt DoControl( CFsPluginConnRequest& aRequest );
        virtual void DoRequest( CFsPluginConnRequest& aRequest );
        virtual void DoCancel( TInt aReqMask );

    private:
        CFastFindFileServerPluginConn( CFastFindFileServerPlugin& aPlugin );

        CFastFindFileServerPlugin& iPlugin;

        RMessage2* iMessage;
    };
    
/**
* Leaving New function for the plugin
* @internalComponent
*/
CFastFindFileServerPluginConn* CFastFindFileServerPluginConn::NewL( 
		CFastFindFileServerPlugin& aPlugin )
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINCONN_NEWL, "CFastFindFileServerPluginConn::NewL()" );
    WRITELOG( "CFastFindFileServerPluginConn::NewL()" );
    return new (ELeave) CFastFindFileServerPluginConn( aPlugin );
    }


/**
* Constructor for the plugin
* @internalComponent
*/
CFastFindFileServerPluginConn::CFastFindFileServerPluginConn(
               CFastFindFileServerPlugin& aPlugin )
  : iPlugin( aPlugin )
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINCONN_CFASTFINDFILESERVERPLUGINCONN, "CFastFindFileServerPluginConn::CFastFindFileServerPluginConn()" );
    WRITELOG( "CFastFindFileServerPluginConn::CFastFindFileServerPluginConn()" );

    iPlugin.AddConnection();
    }


/**
* The destructor for the test virus scanner hook.
* @internalComponent
*/
CFastFindFileServerPluginConn::~CFastFindFileServerPluginConn()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGINCONN_CFASTFINDFILESERVERPLUGINCONN, "CFastFindFileServerPluginConn::~CFastFindFileServerPluginConn()" );
    WRITELOG( "CFastFindFileServerPluginConn::~CFastFindFileServerPluginConn()" );

    iPlugin.RemoveConnection();
    }

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
TInt CFastFindFileServerPluginConn::DoControl( CFsPluginConnRequest& aRequest )
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINCONN_DOCONTROL, "CFastFindFileServerPluginConn::DoControl()" );
    WRITELOG( "CFastFindFileServerPluginConn::DoControl()" );
    TInt err = KErrNotSupported;
    
    CFastFindFileServerPlugin& myPlugin = *(CFastFindFileServerPlugin*)Plugin();
    
    switch( aRequest.Function() )
        {
        case EFastFindFSPOpEnable:
            {
            OstTrace0( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGINCONN_DOCONTROL, "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpEnable" );
            WRITELOG( "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpEnable" );
            TRAP( err, myPlugin.EnableL() );
            break;
            }
        
        case EFastFindFSPOpDisable:
            {
            OstTrace0( TRACE_NORMAL, DUP2_CFASTFINDFILESERVERPLUGINCONN_DOCONTROL, "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpDisable" );
            WRITELOG( "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpDisable" );
            TRAP( err, myPlugin.DisableL() );
            break;
            }
        
        default:
            {
            OstTrace0( TRACE_NORMAL, DUP3_CFASTFINDFILESERVERPLUGINCONN_DOCONTROL, "CFastFindFileServerPluginConn::DoControl() - Unknown Control" );
            WRITELOG( "CFastFindFileServerPluginConn::DoControl() - Unknown Control" );
            break;            
            }
        }

    return err;
    }
    

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPluginConn::DoRequest( CFsPluginConnRequest& aRequest )
    {
    OstTraceFunctionEntry0( CFASTFINDFILESERVERPLUGINCONN_DOREQUEST_ENTRY );
    WRITELOG( "CFastFindFileServerPluginConn::DoRequest()" );
    CFastFindFileServerPlugin& myPlugin = *(CFastFindFileServerPlugin*)Plugin();
    
    switch( aRequest.Function() )
        {
        case EFastFindFSPOpRegisterNotification:
            {
            OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINCONN_DOREQUEST, "CFastFindFileServerPluginConn::DoRequest() - EFastFindFSPOpRegisterNotification" );
            WRITELOG( "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpRegisterNotification" );
            TInt err = myPlugin.RegisterNotification( aRequest );
            
            if ( err != KErrNone )
                {
                aRequest.Complete( err );
                }
            break;
            }
            
        case EFastFindFSPOpAddNotificationPath:
            {
            OstTrace0( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGINCONN_DOREQUEST, "CFastFindFileServerPluginConn::DoRequest() - EFastFindFSPOpAddNotificationPath" );
            WRITELOG( "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpAddNotificationPath" );
            TInt err = myPlugin.AddNotificationPath( aRequest );
            aRequest.Complete( err );
            break;
            }
            
        case EFastFindFSPOpRemoveNotificationPath:
            {
            OstTrace0( TRACE_NORMAL, DUP2_CFASTFINDFILESERVERPLUGINCONN_DOREQUEST, "CFastFindFileServerPluginConn::DoRequest() - EFastFindFSPOpRemoveNotificationPath" );
            WRITELOG( "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpRemoveNotificationPath" );
            TInt err = myPlugin.RemoveNotificationPath( aRequest );
            aRequest.Complete( err );
            break;
            }
            
        case EFastFindFSPOpAddIgnorePath:
            {
            OstTrace0( TRACE_NORMAL, DUP3_CFASTFINDFILESERVERPLUGINCONN_DOREQUEST, "CFastFindFileServerPluginConn::DoRequest() - EFastFindFSPOpAddIgnorePath" );
            WRITELOG( "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpAddIgnorePath" );
            TInt err = myPlugin.AddIgnorePath( aRequest );
            aRequest.Complete( err );
            break;
            }
            
        case EFastFindFSPOpRemoveIgnorePath:
            {
            OstTrace0( TRACE_NORMAL, DUP4_CFASTFINDFILESERVERPLUGINCONN_DOREQUEST, "CFastFindFileServerPluginConn::DoRequest() - EFastFindFSPOpRemoveIgnorePath" );
            WRITELOG( "CFastFindFileServerPluginConn::DoControl() - EFastFindFSPOpRemoveIgnorePath" );
            TInt err = myPlugin.RemoveIgnorePath( aRequest );
            aRequest.Complete( err );
            break;
            }
        }
    OstTraceFunctionExit0( CFASTFINDFILESERVERPLUGINCONN_DOREQUEST_EXIT );
    }

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPluginConn::DoCancel( TInt /*aReqMask*/ )
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINCONN_DOCANCEL, "CFastFindFileServerPluginConn::DoCancel()" );
    WRITELOG( "CFastFindFileServerPluginConn::DoCancel()" );
    iRequestQue.DoCancelAll( KErrCancel );
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
CFsPluginConn* CFastFindFileServerPlugin::NewPluginConnL()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_NEWPLUGINCONNL, "CFastFindFileServerPluginConn::NewPluginConnL()" );
    WRITELOG( "CFastFindFileServerPluginConn::NewPluginConnL()" );
    return CFastFindFileServerPluginConn::NewL( *this );
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
TInt CFastFindFileServerPlugin::RegisterNotification( CFsPluginConnRequest& aRequest )
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_REGISTERNOTIFICATION, "CFastFindFileServerPlugin::RegisterNotification()" );
    WRITELOG( "CFastFindFileServerPlugin::RegisterNotification()" );
    
    if ( iNotification )
        {
        return KErrInUse;
        }
    
    iNotification = &aRequest;
    
    if ( iQueue.Count() > 0 )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGIN_REGISTERNOTIFICATION, "CFastFindFileServerPlugin::RegisterNotification() - item in queue" );
        WRITELOG( "CFastFindFileServerPlugin::RegisterNotification() - item in queue" );
        
        TFastFindFSPStatus* queueStatus = iQueue[0];
        
        TFastFindFSPStatusPckg pckg;
        TFastFindFSPStatus& status = pckg();
        
        status.iDriveNumber = queueStatus->iDriveNumber;
        status.iFileEventType = queueStatus->iFileEventType;
        status.iFileName.Copy( queueStatus->iFileName );
        status.iNewFileName.Copy( queueStatus->iNewFileName );
        status.iProcessId = queueStatus->iProcessId;
        
        TRAPD( err, iNotification->WriteParam1L(pckg) );
        iNotification->Complete( err );
        iNotification = NULL;
        
        delete queueStatus;
        queueStatus = NULL;
        iQueue.Remove( 0 );
        }
    
    return KErrNone;
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
TInt CFastFindFileServerPlugin::AddNotificationPath( CFsPluginConnRequest& aRequest )
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_ADDNOTIFICATIONPATH, "CFastFindFileServerPlugin::AddNotificationPath()" );
    WRITELOG( "CFastFindFileServerPlugin::AddNotificationPath()" );
    TInt err = KErrNone;
    
    TFastFindFSPStatusPckg pckg;
    TRAP( err, aRequest.ReadParam1L(pckg) );
    
    if ( err != KErrNone )
        {
        return err;
        }
    
    TFastFindFSPStatus& status = pckg();
    
    if ( status.iFileName.Length() > 0 )
        {
        // check if already exists
        for ( TInt i = 0; i < iPaths.Count(); i++ )
            {
            TFileName* tf = iPaths[i];
            if ( tf->Compare(status.iFileName) == 0 )
                {
                return KErrNone;
                }
            }

        OstTraceExt1( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGIN_ADDNOTIFICATIONPATH, "CFastFindFileServerPlugin::AddNotificationPath;add path=%S", status.iFileName );
        WRITELOG1( "CFastFindFileServerPlugin::AddNotificationPath() - add path: %S", &status.iFileName );
        TFileName* fn = new  TFileName;
        
        if ( fn )
            {
            fn->Copy( status.iFileName );
            iPaths.Append( fn );
            }
        else
        	{
        	err = KErrNoMemory;
        	}
        }
    else
        {
        err = KErrNotFound;
        }
        
    return err;
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
TInt CFastFindFileServerPlugin::RemoveNotificationPath( CFsPluginConnRequest& aRequest )
    {
    OstTraceFunctionEntry0( CFASTFINDFILESERVERPLUGIN_REMOVENOTIFICATIONPATH_ENTRY );
    WRITELOG( "CFastFindFileServerPlugin::RemoveNotificationPath()" );
    TInt err = KErrNone;
    
    TFastFindFSPStatusPckg pckg;
    TRAP( err, aRequest.ReadParam1L(pckg) );
    
    if ( err != KErrNone )
        {
        OstTraceFunctionExit0( CFASTFINDFILESERVERPLUGIN_REMOVENOTIFICATIONPATH_EXIT );
        return err;
        }
    
    TFastFindFSPStatus& status = pckg();
    
    if ( status.iFileName.Length() > 0 )
        {
        // check if already exist
        if ( iPaths.Count() > 0 )
            {
            for ( TInt i = 0; i < iPaths.Count(); i++ )
                {
                TFileName* tf = iPaths[i];
                if ( tf->Compare(status.iFileName) == 0 )
                    {
                    OstTraceExt1( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_REMOVENOTIFICATIONPATH, "CFastFindFileServerPlugin::RemoveNotificationPath;remove path=%S", status.iFileName );
                    WRITELOG1( "CFastFindFileServerPlugin::RemoveNotificationPath() - remove path: %S", &status.iFileName );
                    delete tf;
                    tf = NULL;
                    iPaths.Remove( i );
                    }
                }
            }
        }
    else
        {
        err = KErrNotFound;
        }
        
    OstTraceFunctionExit0( DUP1_CFASTFINDFILESERVERPLUGIN_REMOVENOTIFICATIONPATH_EXIT );
    return err;
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
TInt CFastFindFileServerPlugin::AddIgnorePath( CFsPluginConnRequest& aRequest )
    {
    OstTraceFunctionEntry0( CFASTFINDFILESERVERPLUGIN_ADDIGNOREPATH_ENTRY );
    WRITELOG( "CFastFindFileServerPlugin::AddIgnorePath()" );
    TInt err = KErrNone;
    
    TFastFindFSPStatusPckg pckg;
    TRAP( err, aRequest.ReadParam1L(pckg) );
    
    if ( err != KErrNone )
        {
        OstTraceFunctionExit0( CFASTFINDFILESERVERPLUGIN_ADDIGNOREPATH_EXIT );
        return err;
        }
    
    TFastFindFSPStatus& status = pckg();
    
    if ( status.iFileName.Length() > 0 )
        {
        // check if already exist
        if ( iIgnorePaths.Count() > 0 )
            {
            for ( TInt i = 0; i < iIgnorePaths.Count(); i++ )
                {
                TFileName* tf = iIgnorePaths[i];
                if( tf->Compare(status.iFileName) == 0 )
                    {
                    OstTraceFunctionExit0( DUP1_CFASTFINDFILESERVERPLUGIN_ADDIGNOREPATH_EXIT );
                    return KErrNone;
                    }
                }
            }
            
        OstTraceExt1( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_ADDIGNOREPATH, "CFastFindFileServerPlugin::AddIgnorePath;add path=%S", status.iFileName );
        WRITELOG1( "CFastFindFileServerPlugin::AddIgnorePath() - add path: %S", &status.iFileName );
        TFileName* fn = new TFileName;
        
        if ( fn )
            {
            fn->Copy( status.iFileName );
            iIgnorePaths.Append( fn ); // ownership is transferred
            }
        else
        	{
        	err = KErrNoMemory;
        	}
        }
    else
        {
        err = KErrNotFound;
        }
        
    OstTraceFunctionExit0( DUP2_CFASTFINDFILESERVERPLUGIN_ADDIGNOREPATH_EXIT );
    return err;
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
TInt CFastFindFileServerPlugin::RemoveIgnorePath( CFsPluginConnRequest& aRequest )
    {
    OstTraceFunctionEntry0( CFASTFINDFILESERVERPLUGIN_REMOVEIGNOREPATH_ENTRY );
    WRITELOG( "CFastFindFileServerPlugin::RemoveIgnorePath()" );
    TInt err = KErrNone;
    
    TFastFindFSPStatusPckg pckg;
    TRAP( err, aRequest.ReadParam1L(pckg) );
    
    if ( err != KErrNone )
        {
        OstTraceFunctionExit0( CFASTFINDFILESERVERPLUGIN_REMOVEIGNOREPATH_EXIT );
        return err;
        }
    
    TFastFindFSPStatus& status = pckg();
    
    if ( status.iFileName.Length() > 0 )
        {
        // check if already exist
        if ( iIgnorePaths.Count() > 0 )
            {
            for ( TInt i = 0; i < iIgnorePaths.Count(); i++ )
                {
                TFileName* tf = iIgnorePaths[i];
                if ( tf->Compare(status.iFileName) == 0 )
                    {
                    OstTraceExt1( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_REMOVEIGNOREPATH, "CFastFindFileServerPlugin::RemoveIgnorePath;remove path=%S", status.iFileName );
                    WRITELOG1( "CFastFindFileServerPlugin::RemoveIgnorePath() - remove path: %S", &status.iFileName );
                    delete tf;
                    tf = NULL;
                    iIgnorePaths.Remove( i );
                    }
                }
            }
        }
    else
        {
        err = KErrNotFound;
        }
        
    OstTraceFunctionExit0( DUP1_CFASTFINDFILESERVERPLUGIN_REMOVEIGNOREPATH_EXIT );
    return err;
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
TBool CFastFindFileServerPlugin::CheckPath( TFileName& aFileName )
    {
    // check if ignored pathlist
    if ( iIgnorePaths.Count() > 0 )
        {
        for ( TInt i = 0; i < iIgnorePaths.Count(); i++ )
            {    
            TFileName* pathName = iIgnorePaths[i];
            pathName->LowerCase();
            TFileName tempFileName;
            tempFileName.Copy( aFileName );
            tempFileName.LowerCase();
            // WRITELOG1("CFastFindFileServerPlugin::CheckPath() - search ignore path: %S", pathName);
            if ( tempFileName.Find(*pathName) != KErrNotFound )
                {
                // WRITELOG( "CFastFindFileServerPlugin::CheckPath() - is ignore path" );
                return EFalse;
                }
            }
        }
    
    // check if notification path
    if ( iPaths.Count() > 0 )
        {
        for ( TInt i = 0; i < iPaths.Count(); i++ )
            {    
            TFileName* pathName = iPaths[i];
            pathName->LowerCase();
            TFileName tempFileName;
            tempFileName.Copy( aFileName );
            tempFileName.LowerCase();
            //WRITELOG1("CFastFindFileServerPlugin::CheckPath() - search path: %S", &pathName);
            if ( tempFileName.Find(*pathName) != KErrNotFound )
                {
                //WRITELOG( "CFastFindFileServerPlugin::CheckPath() - path found" );
                return ETrue;
                }
            }
        }
    else
        {
        // WRITELOG( "CFastFindFileServerPlugin::CheckPath() - no notification paths" );
        return ETrue;
        }
            
    return EFalse;
    }
    
//-----------------------------------------------------------------------------
// CFastFindFileServerPlugin::CheckAttribs()
//-----------------------------------------------------------------------------
//
TBool CFastFindFileServerPlugin::CheckAttribs( TFileName& aFileName )
    {
    TParse parse;
    parse.Set( aFileName, NULL, NULL );
    TPath path( parse.DriveAndPath() );
    
    TUint att = 0;
    
    // check if path is hidden or system path
    TInt err = iFsSession.Att( path, att );
    if ( err == KErrNone )
        {
        if ( att & KEntryAttHidden || att & KEntryAttSystem )
            {
            return EFalse;
            }
        }
        
    // or is the file hidden or system file
    att = 0;
    err = iFsSession.Att( aFileName, att );
    if ( err == KErrNone )
        {
        if ( att & KEntryAttHidden || att & KEntryAttSystem )
            {
            return EFalse;
            }
        }
        
    return ETrue;
    }
    
//-----------------------------------------------------------------------------
// CFastFindFileServerPluginFactory implementation
//-----------------------------------------------------------------------------
//
class CFastFindFileServerPluginFactory : public CFsPluginFactory
    {
    public:
        CFastFindFileServerPluginFactory();
        virtual TInt Install();            
        virtual CFsPlugin* NewPluginL();
        virtual TInt UniquePosition();
    };
    
// Constructor for the plugin factory
// @internalComponent
CFastFindFileServerPluginFactory::CFastFindFileServerPluginFactory()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINFACTORY_CFASTFINDFILESERVERPLUGINFACTORY, "CFastFindFileServerPluginFactory::CFastFindFileServerPluginFactory()" );
    WRITELOG( "CFastFindFileServerPluginFactory::CFastFindFileServerPluginFactory()" );
    }

// Install function for the plugin factory
// @internalComponent
TInt CFastFindFileServerPluginFactory::Install()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINFACTORY_INSTALL, "CFastFindFileServerPluginFactory::Install()" );
    WRITELOG( "CFastFindFileServerPluginFactory::Install()" );
    iSupportedDrives = KPluginAutoAttach;
    
    return( SetName(&KFastFindFileServerPlugin) );
    }

// @internalComponent
TInt CFastFindFileServerPluginFactory::UniquePosition()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINFACTORY_UNIQUEPOSITION, "CFastFindFileServerPluginFactory::UniquePosition()" );
    WRITELOG( "CFastFindFileServerPluginFactory::UniquePosition()" );
    return( KFastFindFSPluginPosition );
    }

// Plugin factory function
// @internalComponent
CFsPlugin* CFastFindFileServerPluginFactory::NewPluginL()
    {
    OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGINFACTORY_NEWPLUGINL, "CFastFindFileServerPluginFactory::NewPluginL()" );
    WRITELOG( "CFastFindFileServerPluginFactory::NewPluginL()" );
    return CFastFindFileServerPlugin::NewL();
    }

// Create a new Plugin
// @internalComponent
extern "C"
    {
    EXPORT_C CFsPluginFactory* CreateFileSystem()
        {
        OstTrace0( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGINFACTORY_NEWPLUGINL, "CFastFindFileServerPluginFactory::CreateFileSystem" );
        WRITELOG( "CFastFindFileServerPluginFactory::CreateFileSystem" );
        return( new CFastFindFileServerPluginFactory() );
        }
    }

#ifdef _DEBUG_EVENTS

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPlugin::RegisterDebugEventsL()
    {
    User::LeaveIfError( RegisterIntercept( EFsFileDuplicate, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileCreate, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileWrite, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileFlush, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileSetAtt, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileChangeMode, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsDelete, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileAdopt, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileReplace, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileLock, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileSize, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileRename, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsRename, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsMkDir, EPrePostIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsRmDir, EPrePostIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileOpen, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileTemp,    EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileUnLock, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileSetSize, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileDrive, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsReplace, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileSubClose, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileRead, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileSeek, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileAtt, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileSet, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsFileName, EPreIntercept) );
    User::LeaveIfError( RegisterIntercept( EFsDirOpen, EPreIntercept) );
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPlugin::UnregisterDebugEventsL()
    {
    User::LeaveIfError( UnregisterIntercept( EFsFileDuplicate, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileCreate, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileWrite, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileFlush, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileSetAtt, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileChangeMode, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsDelete, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileAdopt, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileReplace, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileLock, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileSize, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileRename, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsRename, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsMkDir, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsRmDir, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileOpen, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileTemp, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileUnLock, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileSetSize, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileDrive, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsReplace, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileSubClose, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileRead, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileSeek, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileAtt, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileSet, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsFileName, EPrePostIntercept) );
    User::LeaveIfError( UnregisterIntercept( EFsDirOpen, EPrePostIntercept) );
    }
    
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void CFastFindFileServerPlugin::PrintDebugEvents( TInt aFunction )
    {
    switch ( aFunction )
        {
        case EFsFileDuplicate:
            OstTrace0( TRACE_NORMAL, CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileDuplicate" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileDuplicate" );
            break;            
        case EFsFileCreate:
            OstTrace0( TRACE_NORMAL, DUP1_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileCreate" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileCreate" );
            break;
        case EFsFileWrite:
            OstTrace0( TRACE_NORMAL, DUP2_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileWrite" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileWrite" );
            break;
        case EFsFileFlush:
            OstTrace0( TRACE_NORMAL, DUP3_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileFlush" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileFlush" );
            break;
        case EFsFileSetAtt:
            OstTrace0( TRACE_NORMAL, DUP4_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSetAtt" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSetAtt" );
            break;
        case EFsFileChangeMode:
            OstTrace0( TRACE_NORMAL, DUP5_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileChangeMode" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileChangeMode" );
            break;    
        case EFsDelete:
            OstTrace0( TRACE_NORMAL, DUP6_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsDelete" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsDelete" );
            break;    
        case EFsFileAdopt:
            OstTrace0( TRACE_NORMAL, DUP7_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileAdopt" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileAdopt" );
            break;    
        case EFsFileReplace:
            OstTrace0( TRACE_NORMAL, DUP8_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileReplace" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileReplace" );
            break;    
        case EFsFileLock:
            OstTrace0( TRACE_NORMAL, DUP9_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileLock" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileLock" );
            break;    
        case EFsFileSize:
            OstTrace0( TRACE_NORMAL, DUP10_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSize" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSize" );
            break;    
        case EFsFileModified:
            OstTrace0( TRACE_NORMAL, DUP11_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileModified" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileModified" );
            break;    
        case EFsFileRename:
            OstTrace0( TRACE_NORMAL, DUP12_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileRename" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileRename" );
            break;    
        case EFsRename:
            OstTrace0( TRACE_NORMAL, DUP13_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsRename" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsRename" );
            break;
        case EFsMkDir:
            OstTrace0( TRACE_NORMAL, DUP14_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsMkDir" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsMkDir" );
            break;
        case EFsRmDir:
            OstTrace0( TRACE_NORMAL, DUP15_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsRmDir" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsRmDir" );
            break;
        case EFsFileOpen:
            OstTrace0( TRACE_NORMAL, DUP16_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileOpen" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileOpen" );
            break;    
        case EFsFileTemp:
            OstTrace0( TRACE_NORMAL, DUP17_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileTemp" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileTemp" );
            break;    
        case EFsFileUnLock:
            OstTrace0( TRACE_NORMAL, DUP18_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileUnLock" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileUnLock" );
            break;    
        case EFsFileSetSize:
            OstTrace0( TRACE_NORMAL, DUP19_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSetSize" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSetSize" );
            break;    
        case EFsFileSetModified:
            OstTrace0( TRACE_NORMAL, DUP20_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSetModified" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSetModified" );
            break;
        case EFsFileDrive:
            OstTrace0( TRACE_NORMAL, DUP21_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileDrive" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileDrive" );
            break;    
        case EFsReplace:
            OstTrace0( TRACE_NORMAL, DUP22_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsReplace" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsReplace" );
            break;    
        case EFsFileSubClose:
            OstTrace0( TRACE_NORMAL, DUP23_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSubClose" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSubClose" );
            break;    
        case EFsFileRead:
            OstTrace0( TRACE_NORMAL, DUP24_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileRead" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileRead" );
            break;
        case EFsFileSeek:
            OstTrace0( TRACE_NORMAL, DUP25_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSeek" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSeek" );
            break;    
        case EFsFileAtt:
            OstTrace0( TRACE_NORMAL, DUP26_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileAtt" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileAtt" );
            break;    
        case EFsFileSet:
            OstTrace0( TRACE_NORMAL, DUP27_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSet" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileSet" );
            break;    
        case EFsFileName:
            OstTrace0( TRACE_NORMAL, DUP28_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileName" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsFileName" );
            break;    
        case EFsDirOpen:
            OstTrace0( TRACE_NORMAL, DUP29_CFASTFINDFILESERVERPLUGIN_PRINTDEBUGEVENTS, "CFastFindFileServerPlugin::PrintDebugEvents() - EFsDirOpen" );
            WRITELOG( "CFastFindFileServerPlugin::PrintDebugEvents() - EFsDirOpen" );
            break;    
        default:
        break;
        }
    }
#endif

//#ifdef _FORCEDEBUG
//#undef _DEBUG
//#undef _FORCEDEBUG
//#endif
