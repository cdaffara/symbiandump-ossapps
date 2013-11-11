/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConML parser/generator
*
*/

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------    
#include "sconconmlgenerator.h"
#include "sconconmlhandlererror.h"
#include "sconconmldtd.h"
#include "sconxmlelement.h"
#include "debug.h"

// -----------------------------------------------------------------------------
// CSConConMLGenerator
// -----------------------------------------------------------------------------
CSConConMLGenerator::CSConConMLGenerator()
    {
    }
    
// -----------------------------------------------------------------------------
// ~CSConConMLGenerator
// -----------------------------------------------------------------------------
CSConConMLGenerator::~CSConConMLGenerator()
    {
    if (iCmdStack)
        {
        iCmdStack->Reset();
        delete iCmdStack;
        iCmdStack = NULL;
        }
        
    if ( iWBXMLWorkspace )
        {
        delete iWBXMLWorkspace;
        iWBXMLWorkspace = NULL;
        }
        
    if ( iXMLWorkspace )
        {
        delete iXMLWorkspace;
        iXMLWorkspace = NULL;
        }
        
    if (iCleanupStack)
        {
        iCleanupStack->ResetAndDestroy();
        delete iCleanupStack;
        iCleanupStack = NULL;
        }

    iElemStack.Close();
    }
    
// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
CSConConMLGenerator* CSConConMLGenerator::NewL ()
    {
    LOGGER_ENTERFN( "CSConConMLGenerator::NewL()" );
    CSConConMLGenerator* self = new ( ELeave ) CSConConMLGenerator();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    LOGGER_LEAVEFN( "CSConConMLGenerator::NewL()" );
    return self;
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::ConstructL()
    {
    LOGGER_ENTERFN( "CSConConMLGenerator::ConstructL()" );
    iCmdStack = CXMLStack<CXMLElement>::NewL();
    iCleanupStack = CXMLStack<CXMLElement>::NewL();
    iWBXMLWorkspace = CXMLWorkspace::NewL();
    iXMLWorkspace = CXMLWorkspace::NewL();
    LOGGER_LEAVEFN( "CSConConMLGenerator::ConstructL()" );
    }
    
// -----------------------------------------------------------------------------
// SetCallbacks
// -----------------------------------------------------------------------------
void CSConConMLGenerator::SetCallback ( MWBXMLConMLCallback* aCallback )
    {
    iCallback = aCallback;
    }

// -----------------------------------------------------------------------------
// StartDocument
// -----------------------------------------------------------------------------    
void CSConConMLGenerator::StartDocument( 
    TUint8 /*aVersion*/, TInt32 /*aPublicId*/, TUint32 /*aCharset*/ )
    {
    LOGGER_ENTERFN( "CSConConMLGenerator::StartDocumentL()" );
    iCmdStack->Reset();
    LOGGER_LEAVEFN( "CSConConMLGenerator::StartDocumentL()" );
    }

// -----------------------------------------------------------------------------
// StartDocument
// -----------------------------------------------------------------------------
void CSConConMLGenerator::StartDocument( 
    TUint8 /*aVersion*/, const TDesC8& /*aPublicIdStr*/, TUint32 /*aCharset*/ )
    {
    LOGGER_ENTERFN( "CSConConMLGenerator::StartDocumentL()" );
    iCmdStack->Reset();
    LOGGER_LEAVEFN( "CSConConMLGenerator::StartDocumentL()" );
    }
    
// -----------------------------------------------------------------------------
// StartElementL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::StartElementL( TWBXMLTag aTag )
    {
    if( iCmdStack->Top() != 0 )
        {
        AddElementL(iCmdStack->Top()->BeginElementL( 
            aTag, TXMLElementParams(iCallback, iCmdStack, iCleanupStack ) ) );
        }
    else
        {
        if( aTag == EConML )
            {
            ConML_ConML_t* element = new (ELeave) ConML_ConML_t();
            CleanupStack::PushL( element );
            AddElementL( element );
            CleanupStack::Pop( element );
            }
        else
            {
            LOGGER_WRITE( "CSConConMLGenerator::StartElementL() : Leave KWBXMLParserErrorInvalidTag" );
            User::Leave(KWBXMLParserErrorInvalidTag);
            }
        }
    }
    
// -----------------------------------------------------------------------------
// AddElement
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AddElementL( CXMLElement* aElement )
    {
    if( aElement )
        {
        iCmdStack->PushL(aElement);
        if( aElement->NeedsCleanup() )
            {
            iCleanupStack->PushL(aElement);
            }
        }
    }

// -----------------------------------------------------------------------------
// CharactersL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::CharactersL( const TDesC8& aBuffer )
    {
    if( iCmdStack->Top() != 0 )
        {
        iCmdStack->Top()->SetDataL(aBuffer);
        }
    else
        {
        LOGGER_WRITE( "CSConConMLGenerator::CharactersL() : Leave KWBXMLParserErrorInvalidTag" );
        User::Leave(KWBXMLParserErrorInvalidTag);
        }
    }
    
// -----------------------------------------------------------------------------
// EndElementL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::EndElementL( TWBXMLTag aTag )
    {
    if( iCmdStack->Top() != 0 )
        {
        CXMLElement::TAction action = iCmdStack->Top()->EndElementL( 
            iCallback, aTag );
        if( action != CXMLElement::ENone )
            {
            CXMLElement* elem = iCmdStack->Pop();
            if( iCleanupStack->Top() == elem )
                {
                iCleanupStack->Pop();
                }
            if( action == CXMLElement::EPopAndDestroy )
                {
                delete elem;
                }
            }
        }
    }
    
// -----------------------------------------------------------------------------
// WriteMUint32L
// -----------------------------------------------------------------------------
void CSConConMLGenerator::WriteMUint32L( TUint32 aValue )
    {
    TUint8 temp[5];
    TInt i(4);
    
    temp[i--] = TUint8(aValue & 0x7F);
    aValue >>= 7;
    while( aValue > 0 )
        {
        temp[i--] = TUint8((aValue & 0x7F) | 0x80);
        aValue >>= 7;
        }
            
    while( i < 4 )
        {
        iWBXMLWorkspace->WriteL(temp[++i]);
        }
    }

// -----------------------------------------------------------------------------
// WriteOpaqueDataL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::WriteOpaqueDataL( const TDesC8& aData )
    {
    iWBXMLWorkspace->WriteL( OPAQUE );
    WriteMUint32L( aData.Size() );
    iWBXMLWorkspace->WriteL( aData );
    iXMLWorkspace->WriteL( aData );
    }
    
// -----------------------------------------------------------------------------
// WriteInlineStringL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::WriteInlineStringL( const TDesC8& aData )
    {
    iWBXMLWorkspace->WriteL( STR_I );
    iWBXMLWorkspace->WriteL( aData );
    iWBXMLWorkspace->WriteL( 0 );
    iXMLWorkspace->WriteL( aData );
    }
    
// -----------------------------------------------------------------------------
// IndentL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::IndentL()
    {
    for( TInt i = 0; i < iElemStack.Count() + iInitialIndentLevel; i++ )
        {
        iXMLWorkspace->WriteL(KXMLIndent());
        }
    }

// -----------------------------------------------------------------------------
// TranslateElement
// -----------------------------------------------------------------------------    
TPtrC8 CSConConMLGenerator::TranslateElement( TUint8 aElement )
    {
    TPtrC8 buf( KConMLElements );
    while( aElement-- )
        {
        TInt pos = buf.Find(KXMLElemenentSeparator());
        if( pos == KErrNotFound )
            {
            return TPtrC8();
            }
        buf.Set(buf.Right(buf.Length() - pos - 1));
        }

    TInt pos = buf.Find(KXMLElemenentSeparator());
    
    if( pos != KErrNotFound )
        {
        buf.Set(buf.Left(pos));
        }
    return buf;
    }
    
// -----------------------------------------------------------------------------
// EndDocument
// -----------------------------------------------------------------------------
void CSConConMLGenerator::EndDocument()
    {
    LOGGER_ENTERFN( "CSConConMLGenerator::EndDocument()" );
    LOGGER_LEAVEFN( "CSConConMLGenerator::EndDocument()" );
    }

    
// -----------------------------------------------------------------------------
// GenerateConMLDocument
// -----------------------------------------------------------------------------
TInt CSConConMLGenerator::GenerateConMLDocument ( ConML_ConMLPtr_t aContent )
    {
    LOGGER_ENTERFN( "CSConConMLGenerator::GenerateConMLDocument()" );
    iWBXMLWorkspace->Reset();
    iWBXMLWorkspace->BeginTransaction();
    iXMLWorkspace->Reset();
    iXMLWorkspace->BeginTransaction();
    TRAPD(result, AppendConMLL(aContent));
    LOGGER_WRITE_1( "CSConConMLGenerator::GenerateConMLDocument()\
     : returned %d", result );
    return HandleResult(result);
    }
    
// -----------------------------------------------------------------------------
// AppendConMLL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendConMLL( ConML_ConMLPtr_t aContent )
    {
    BeginDocumentL(KSConConMLVersion, KSConConMLPublicId, KSConConMLUTF8);
    BeginElementL(EConML, ETrue);
    AppendExecuteL( aContent->execute );
    AppendGetStatusL( aContent->getStatus );
    AppendCancelL( aContent->cancel );
    AppendStatusL( aContent->status );
    EndElementL(); // EConML
    }

// -----------------------------------------------------------------------------
// AppendExecuteL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendExecuteL( ConML_ExecutePtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLExecute, ETrue );
        AppendPCDataL( EConMLID, aContent->id );
        AppendInstallL( aContent->install );
        AppendUnInstallL( aContent->unInstall );
        AppendListInstalledAppsL( aContent->listInstalledApps );
        AppendListDataOwnersL( aContent->listDataOwners );
        AppendSetBURModeL( aContent->setBurMode );
        AppendGetDataSizeL( aContent->getDataSize );
        AppendRequestDataL( aContent->requestData );
        AppendUpdateDeviceInfoL( aContent->updateDeviceInfo );
        AppendListPublicFilesL( aContent->listPublicFiles );
        AppendSupplyDataL( aContent->supplyData );
        AppendGetDataOwnerStatusL( aContent->getDataOwnerStatus );
        AppendGetMetadataL( aContent->getMetadata );
        
        if ( aContent->reboot )
            {
            BeginElementL( EConMLReboot );
            }
        EndElementL(); // EConMLExecute
        }
    }

// -----------------------------------------------------------------------------
// AppendSupplyDataL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendSupplyDataL( ConML_SupplyDataPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLSupplyData, ETrue );
        AppendSIDListL( aContent->sid );
        AppendResultsL( aContent->results );
        EndElementL(); // EConMLSupplyData
        }
    }
// -----------------------------------------------------------------------------
// AppendInstallL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendInstallL( ConML_InstallPtr_t aContent )
    {
    if ( aContent)
        {
        BeginElementL( EConMLInstall, ETrue );
        AppendPCDataL( EConMLName, aContent->name );
        if ( aContent->instParams) 
            {
            if ( aContent->instParams->param )
                {
                AppendInstParamsL( aContent->instParams );  
                }
            else 
                {
                BeginElementL( EConMLInstParams );
                }
            }
        AppendResultsL( aContent->results );
        EndElementL(); // EConMLInstall
        }
    }

// -----------------------------------------------------------------------------
// AppendCancelL
// -----------------------------------------------------------------------------    
void CSConConMLGenerator::AppendCancelL ( ConML_CancelPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLCancel, ETrue );
        AppendPCDataL ( EConMLID, aContent->id );
        AppendPCDataL( EConMLAll, aContent->all);
        EndElementL(); // EConMLCancel
        }
    }

// -----------------------------------------------------------------------------
// AppendStatusL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendStatusL ( ConML_StatusPtr_t aContent )
    {
    if ( aContent )
        {
        if ( aContent->task )
            {
            BeginElementL( EConMLStatus, ETrue );
            AppendTaskListL( aContent->task );
            EndElementL(); // EConMLStatus
            }
        else
            {
            BeginElementL( EConMLStatus );  
            }
        }
    }
    
// -----------------------------------------------------------------------------
// AppendGetStatusL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendGetStatusL( ConML_GetStatusPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLGetStatus, ETrue );
        AppendPCDataL( EConMLID, aContent->id);
        if ( aContent->all )
            {
            BeginElementL( EConMLAll );
            }
        EndElementL(); // EConMLGetStatus
        }
    }

// -----------------------------------------------------------------------------
// AppendRebootL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendRebootL( ConML_RebootPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLReboot, ETrue );
        AppendResultsL( aContent->results );
        EndElementL(); // EConMLReboot
        }
    }
    
// -----------------------------------------------------------------------------
// AppendTaskL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendTaskL( ConML_TaskPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLTask, ETrue );
        AppendPCDataL( EConMLID, aContent->id );
        AppendInstallL(aContent->install );
        AppendUnInstallL( aContent->unInstall );
        AppendListInstalledAppsL ( aContent->listInstalledApps );
        AppendListDataOwnersL ( aContent->listDataOwners );
        AppendSetBURModeL ( aContent->setBurMode );
        AppendGetDataSizeL ( aContent->getDataSize );
        AppendRequestDataL ( aContent->requestData );
        AppendUpdateDeviceInfoL ( aContent->updateDeviceInfo);
        AppendListPublicFilesL ( aContent->listPublicFiles );
        AppendGetDataOwnerStatusL( aContent->getDataOwnerStatus );
        AppendSupplyDataL( aContent->supplyData );
        AppendRebootL( aContent->reboot );
        AppendGetMetadataL( aContent->getMetadata );
        EndElementL();
        }
    }
    
// -----------------------------------------------------------------------------
// AppendTaskListL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendTaskListL( ConML_TaskListPtr_t aContent )
    {
    for ( ConML_TaskListPtr_t p = aContent; p && p->data; p=p->next )
        {
        AppendTaskL( p->data );
        }
    }
    
// -----------------------------------------------------------------------------
// AppendListInstalledAppsL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendListInstalledAppsL ( 
    ConML_ListInstalledAppsPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLListInstalledApps, ETrue );
        AppendDrivesL( aContent->drives );
        if ( aContent ->all )
            {
            BeginElementL( EConMLAll );
            }
        AppendResultsL( aContent->results );
        EndElementL(); // EConMLListInstalledApps
        }
    }

// -----------------------------------------------------------------------------
// AppendListDataOwnersL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendListDataOwnersL ( 
    ConML_ListDataOwnersPtr_t aContent )
    {
    if ( aContent )
        {
        if ( aContent->results )
            {
            BeginElementL( EConMLListDataOwners, ETrue );
            AppendResultsL( aContent->results );
            EndElementL(); // EConMLListDataOwners
            }
        else
            {
            BeginElementL( EConMLListDataOwners );          
            }
        }
    }

// -----------------------------------------------------------------------------
// CSConConMLGanerator::AppendBUROptionsL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendBUROptionsL( ConML_BUROptionsPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLBUROptions, ETrue );
        if ( aContent->requiresReboot )
            {
            BeginElementL( EConMLRequiresReboot );
            }
        if ( aContent->hasFiles )
            {
            AppendPCDataL( EConMLHasFiles, aContent->hasFiles );
            }
        if ( aContent->supportsInc )
            {
            BeginElementL( EConMLSupportsInc );
            }
        if ( aContent->delayToPrepareData )
            {
            BeginElementL( EConMLDelayToPrepareData );
            }
        EndElementL(); // EConMLBUROptions
        }
    }

// -----------------------------------------------------------------------------
// AppendSetBURModeL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendSetBURModeL( ConML_SetBURModePtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLSetBURMode, ETrue );
        AppendDrivesL( aContent->drives );
        AppendPCDataL( EConMLPartialType, aContent->partialType );
        AppendPCDataL( EConMLIncType, aContent->incType );
        AppendResultsL( aContent->results );
        EndElementL(); //EConMLSetBURMode
        }
    }
    
// -----------------------------------------------------------------------------
// AppendUnInstallL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendUnInstallL( ConML_UnInstallPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLUnInstall, ETrue );
        AppendApplicationsL( aContent->applications );
        AppendInstParamsL( aContent->instParams );
        AppendResultsL( aContent->results );
        EndElementL(); // EConMLUnInstall
        }
    }

// -----------------------------------------------------------------------------
// AppendGetDataSizeL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendGetDataSizeL( ConML_GetDataSizePtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLGetDataSize, ETrue );
        AppendDataOwnersL( aContent->dataOwners );
        AppendResultsL( aContent->results );
        EndElementL(); //EConMLGetDataSize
        }
    }

// -----------------------------------------------------------------------------
// AppendRequestDataL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendRequestDataL( ConML_RequestDataPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLRequestData, ETrue );
        AppendSIDListL( aContent->sid );
        AppendResultsL( aContent->results );
        EndElementL();//EConMLRequestData
        }
    }

// -----------------------------------------------------------------------------
// AppendUpdateDeviceInfoL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendUpdateDeviceInfoL( 
    ConML_UpdateDeviceInfoPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL(EConMLUpdateDeviceInfo, ETrue);
        AppendDeviceInfoL( aContent-> deviceInfo );
        AppendResultsL( aContent->results );
        EndElementL();// EConMLUpdateDeviceInfo
        }
    }

// -----------------------------------------------------------------------------
// AppendListPublicFilesL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendListPublicFilesL( 
    ConML_ListPublicFilesPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLListPublicFiles, ETrue );
        AppendSIDListL ( aContent->sid );
        AppendResultsL( aContent->results );
        EndElementL(); // EConMLListPublicFiles
        }
    }

// -----------------------------------------------------------------------------
// AppendApplicationL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendApplicationL( ConML_ApplicationPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL(EConMLApplication, ETrue );
        AppendPCDataL( EConMLName, aContent->name );
        AppendPCDataL( EConMLUID, aContent->uid );
        EndElementL(); //EConMLApplication
        }
    }

// -----------------------------------------------------------------------------
// AppendApplicationListL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendApplicationListL( 
    ConML_ApplicationListPtr_t aContent )
    {
    for ( ConML_ApplicationListPtr_t p = aContent; p && p->data; p = p->next )
        {
        AppendApplicationL( p->data );
        }
    }

// -----------------------------------------------------------------------------
// AppendApplicationsL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendApplicationsL( 
    ConML_ApplicationsPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLApplications, ETrue );
        AppendApplicationListL( aContent->application );
        EndElementL(); // EConMLApplications
        }
    }

// -----------------------------------------------------------------------------
// AppendParamL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendParamL( ConML_ParamPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLParam, ETrue );
        AppendPCDataL( EConMLName, aContent->name );
        AppendPCDataL( EConMLValue, aContent->value );
        EndElementL(); // EConMLParam
        }
    }

// -----------------------------------------------------------------------------
// AppendParamListL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendParamListL( ConML_ParamListPtr_t aContent )
    {
    for ( ConML_ParamListPtr_t p = aContent; p && p->data; p = p->next )
        {
        AppendParamL( p-> data );
        }
    }

// -----------------------------------------------------------------------------
// AppendInstParamsL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendInstParamsL( ConML_InstParamsPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLInstParams, ETrue );
        AppendParamListL( aContent->param );
        EndElementL(); //EConMLInstParams
        }
    }

// -----------------------------------------------------------------------------
// AppendProgressL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendProgressL( ConML_ProgressPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLProgress, ETrue );
        AppendPCDataL( EConMLValue, aContent->value );
        EndElementL(); // EconMLProgress
        }
    }
    
// -----------------------------------------------------------------------------
// AppendResultsL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendResultsL( ConML_ResultsPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLResults, ETrue );
        if ( aContent->complete )
            {
            BeginElementL( EConMLComplete );
            }
        AppendProgressL( aContent->progress );
        AppendApplicationsL( aContent->applications );
        AppendDataOwnersL( aContent->dataOwners );
        if ( aContent->filename )
            {
            AppendPCDataL( EConMLFilename, aContent->filename );
            }
        AppendPCDataL( EConMLData, aContent->data );
        if ( aContent->moreData )
            {
            BeginElementL( EConMLMoreData );
            }
        AppendDeviceInfoL( aContent->deviceInfo );
        AppendFilesL( aContent->files );
        EndElementL(); //EConMLResults
        }
    }

// -----------------------------------------------------------------------------
// AppendDriveL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendDriveL( ConML_DrivePtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLDrive, ETrue );
        AppendPCDataL( EConMLName, aContent->name );
        EndElementL(); //EConMLDrive
        }
    }

// -----------------------------------------------------------------------------
// AppendDriveListL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendDriveListL( ConML_DriveListPtr_t aContent )
    {
    for ( ConML_DriveListPtr_t p =  aContent; p && p->data; p=p->next )
        {
        AppendDriveL( p->data );
        }
    }

// -----------------------------------------------------------------------------
// AppendDrivesL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendDrivesL( ConML_DrivesPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLDrives, ETrue );
        AppendDriveListL( aContent->drive );
        EndElementL(); // EConMLDrives
        }
    }

// -----------------------------------------------------------------------------
// AppendDataOwnersL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendDataOwnersL( ConML_DataOwnersPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLDataOwners, ETrue );
        AppendSIDListL( aContent->sid );
        EndElementL(); //EConMLDataOwners
        }
    }

// -----------------------------------------------------------------------------
// AppendGetDataOwnerStatusL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendGetDataOwnerStatusL
    ( ConML_GetDataOwnerStatusPtr_t aContent )
    {
    if ( aContent)
        {
        BeginElementL( EConMLGetDataOwnerStatus, ETrue );
        AppendDataOwnersL( aContent->dataOwners );
        AppendResultsL( aContent->results );
        EndElementL(); // EconMLGetDataOwnerStatus
        }
    }

// -----------------------------------------------------------------------------
// AppendGetMetadataL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendGetMetadataL
    ( ConML_GetMetadataPtr_t aContent )
    {
    if ( aContent)
        {
        BeginElementL( EConMLGetMetadata, ETrue );
        AppendPCDataL( EConMLFilename, aContent->filename );
        AppendResultsL( aContent->results );
        EndElementL(); // EConMLGetMetadata
        }
    }

// -----------------------------------------------------------------------------
// AppendPackageInfoL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendPackageInfoL ( ConML_PackageInfoPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLPackageInfo, ETrue );
        AppendPCDataL( EConMLName, aContent->name );
        EndElementL(); // EConMLPackageInfo
        }
    }

// -----------------------------------------------------------------------------
// AppendSIDL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendSIDL( ConML_SIDPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLSID, ETrue );
        AppendPCDataL( EConMLType, aContent->type );
        AppendPCDataL( EConMLUID, aContent->uid );
        AppendPCDataL( EConMLDataOwnerStatus, aContent->dataOwnerStatus );
        AppendDrivesL( aContent->drives );
        if ( aContent->size )
            {
            AppendPCDataL( EConMLSize, aContent->size ); 
            }
        AppendPackageInfoL( aContent->packageInfo );
        AppendBUROptionsL( aContent->burOptions );
        AppendPCDataL( EConMLTransferDataType, aContent->transferDataType );
        AppendPCDataL( EConMLData, aContent->data );
        if ( aContent->moreData)
            {
            BeginElementL(EConMLMoreData );
            }
        EndElementL(); // EconMLSID
        }
    }
    
// -----------------------------------------------------------------------------
// AppendSIDListL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendSIDListL( ConML_SIDListPtr_t aContent )
    {
    for ( ConML_SIDListPtr_t p = aContent; p && p->data; p=p->next )
        {
        AppendSIDL( p->data );
        }
    }

// -----------------------------------------------------------------------------
// AppendDeviceInfoL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendDeviceInfoL( ConML_DeviceInfoPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLDeviceInfo, ETrue );
        AppendPCDataL( EConMLVersion, aContent->version );
        AppendSupportedMethodsL ( aContent->supportedMethods );
        AppendPCDataL(EConMLMaxObjectSize, aContent->maxObjectSize );
        EndElementL(); // EConMLDeviceInfo
        }
    }

// -----------------------------------------------------------------------------
// AppendFilesL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendFilesL( ConML_FilesPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLFiles, ETrue );
        AppendFileListL( aContent->file );
        EndElementL(); // EConMLFiles
        }
    }

// -----------------------------------------------------------------------------
// AppendSupportedMethodsL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendSupportedMethodsL
    ( ConML_SupportedMethodsPtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLSupportedMethods, ETrue );
        if  ( aContent->install )
            {
            BeginElementL( EConMLInstall );
            }
        if ( aContent->unInstall )
            {
            BeginElementL( EConMLUnInstall );
            }
        if ( aContent->listInstalledApps )
            {
            BeginElementL( EConMLListInstalledApps );
            }
        if ( aContent->listDataOwners )
            {
            BeginElementL( EConMLListDataOwners );
            }
        if ( aContent->setBurMode )
            {
            BeginElementL( EConMLSetBURMode );
            }
        if ( aContent->getDataSize )
            {
            BeginElementL( EConMLGetDataSize );
            }
        if ( aContent->requestData )
            {
            BeginElementL( EConMLRequestData );
            }
        if ( aContent->supplyData )
            {
            BeginElementL( EConMLSupplyData );
            }
        if ( aContent->reboot )
            {
            BeginElementL( EConMLReboot );
            }
        EndElementL(); // EConMLSupportedMethods
        }
    }

// -----------------------------------------------------------------------------
// AppendFileListL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendFileListL( ConML_FileListPtr_t  aContent )
    {
    if ( aContent )
        {
        for ( ConML_FileListPtr_t p = aContent; p && p->data; p = p->next )
            {
            AppendFileL(p->data );
            }
        }
    }

// -----------------------------------------------------------------------------
// AppendFileL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendFileL( ConML_FilePtr_t aContent )
    {
    if ( aContent )
        {
        BeginElementL( EConMLFile, ETrue );
        AppendPCDataL( EConMLName, aContent->name );
        AppendPCDataL( EConMLModified, aContent->modified );
        AppendPCDataL( EConMLSize, aContent->size );
        AppendPCDataL( EConMLUserPerm, aContent->userPerm );
        EndElementL(); // EConMLFile
        }
    }

// -----------------------------------------------------------------------------
// HandleResult
// -----------------------------------------------------------------------------
TInt CSConConMLGenerator::HandleResult( TInt aResult, TInt aTreshold )
    {
    LOGGER_ENTERFN( "CSConConMLGenerator::HandleResult()" );
    switch( aResult )
        {
        case KErrNone:
            if( iWBXMLWorkspace->FreeSize() < aTreshold )
                {               
                iWBXMLWorkspace->Rollback();
                return KWBXMLGeneratorBufferFull;
                }
            iWBXMLWorkspace->Commit();
            return KWBXMLGeneratorOk;

        case KErrTooBig:
            iWBXMLWorkspace->Rollback();
            return KWBXMLGeneratorBufferFull;
        }
    LOGGER_WRITE_1( "CSConConMLGenerator::HandleResult() : returned %d", aResult);
    return aResult;
    }

// -----------------------------------------------------------------------------
// BeginDocumentL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::BeginDocumentL( 
    TUint8 aVersion, 
    TInt32 aPublicId, 
    TUint32 aCharset, 
    const TDesC8& aStringTbl )
    {
    LOGGER_ENTERFN( "CSConConMLGenerator::BeginDocumentL()" );
    iWBXMLWorkspace->WriteL(aVersion);

    if( aPublicId <= 0 )
        {
        iWBXMLWorkspace->WriteL(0);
        WriteMUint32L(-aPublicId);
        }
    else
        {
        WriteMUint32L(aPublicId);
        }
    WriteMUint32L(aCharset);
    WriteMUint32L(aStringTbl.Size());
    iWBXMLWorkspace->WriteL(aStringTbl);
    LOGGER_LEAVEFN( "CSConConMLGenerator::BeginDocumentL()" );
    }

// -----------------------------------------------------------------------------
// BeginElementL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::BeginElementL( 
    TUint8 aElement, TBool aHasContent, TBool aHasAttributes )
    {
    IndentL();
    iXMLWorkspace->WriteL(KXMLTagStart());
    iXMLWorkspace->WriteL(TranslateElement(aElement));
        
    if( aHasAttributes )
        {
        aElement |= KWBXMLHasAttributes;
        }
        
    if( aHasContent )
        {
        iXMLWorkspace->WriteL(KXMLTagEnd());
        iElemStack.Insert(aElement, 0);
        aElement |= KWBXMLHasContent;
        }
    else
        {
        iXMLWorkspace->WriteL(KXMLTagEndNoContent());
        }

    if( !iDontNewLine )
        {
        iXMLWorkspace->WriteL(KXMLNewLine());
        }
    iDontNewLine = EFalse;

    WriteMUint32L(aElement);
    }

// -----------------------------------------------------------------------------
// EndElementL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::EndElementL()
    {
    TUint8 elem = iElemStack[0];
    iElemStack.Remove(0);
    if( !iDontIndent )
        {
        IndentL();
        }
    iDontIndent = EFalse;
    iXMLWorkspace->WriteL(KXMLTagStartEndTag());
    iXMLWorkspace->WriteL(TranslateElement(elem));
    iXMLWorkspace->WriteL(KXMLTagEnd());
    iXMLWorkspace->WriteL(KXMLNewLine());
    
    iWBXMLWorkspace->WriteL(END);
    }

// -----------------------------------------------------------------------------
// AddElementL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AddElementL( 
    TUint8 aElement, const TDesC8& aContent, const TWBXMLContentFormat aFormat )
    {
    iDontNewLine = ETrue;
    BeginElementL(aElement, ETrue);
    if( aFormat == EWBXMLContentFormatOpaque )
        {
        WriteOpaqueDataL(aContent);
        }
    else
        {
        WriteInlineStringL(aContent);
        }
    iDontIndent = ETrue;
    EndElementL();
    }

// -----------------------------------------------------------------------------
// AppendPCDataL
// -----------------------------------------------------------------------------
void CSConConMLGenerator::AppendPCDataL( TUint8 aElement, pcdataPtr_t aContent )
    {
    if( !aContent )
        {
        return;
        }

    if( aContent->contentType == SML_PCDATA_OPAQUE )
        {
        AddElementL(aElement, 
                    TPtrC8((TUint8*)aContent->content, 
                    aContent->length));
        }
    else
        {
        LOGGER_WRITE( "CSConConMLGenerator::AppendPCDataL() : Data type not Opaque - ignoring " );
        }
    }
    
// -----------------------------------------------------------------------------
// WBXMLDocument
// -----------------------------------------------------------------------------
TPtrC8 CSConConMLGenerator::WBXMLDocument()
    {
    return iWBXMLWorkspace->Buffer();
    }
    
// -----------------------------------------------------------------------------
// XMLDocument
// -----------------------------------------------------------------------------
TPtrC8 CSConConMLGenerator::XMLDocument()
    {
    return iXMLWorkspace->Buffer();
    }
