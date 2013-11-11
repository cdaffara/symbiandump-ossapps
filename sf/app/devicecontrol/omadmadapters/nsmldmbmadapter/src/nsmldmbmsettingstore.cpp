/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Provides settings management in CommsDat.
*
*/




#include <utf.h>
#include <cmdestinationext.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerdef.h>
#include <commsdat.h>

#include "nsmldmbmsettingstore.h"
#include "nsmldmbmadapter.h"
#include "bmadapterlogger.h"

// -----------------------------------------------------------------------------
// CNSmlDmBmSettingStore::NewL
// -----------------------------------------------------------------------------
CNSmlDmBmSettingStore* CNSmlDmBmSettingStore::NewL( CNSmlDmBmAdapter * aAdapter )
    {
    CNSmlDmBmSettingStore* self = new (ELeave) CNSmlDmBmSettingStore( aAdapter );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CNSmlDmBmSettingStore::CNSmlDmBmSettingStore
// -----------------------------------------------------------------------------
CNSmlDmBmSettingStore::CNSmlDmBmSettingStore( CNSmlDmBmAdapter * aAdapter ) 
    : iAdapter( aAdapter )
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ConstructL
// -----------------------------------------------------------------------------
void CNSmlDmBmSettingStore::ConstructL()
    {
    iCmManagerExt.OpenL();
    }

// -----------------------------------------------------------------------------
// CNSmlDmBmSettingStore::~CNSmlDmBmSettingStore
// -----------------------------------------------------------------------------
CNSmlDmBmSettingStore::~CNSmlDmBmSettingStore()
    {
    iCmManagerExt.Close();
    }

// -----------------------------------------------------------------------------
// CNSmlDmBmSettingStore::StoreSnapL
// Stores data related to a snap object. Executes commands for the snap and fills 
// in status and for get commands result values.
// -----------------------------------------------------------------------------
void CNSmlDmBmSettingStore::StoreSnapL( CSmlDmBmSettingsElement& aSettings )
    {
    // this snap has been handled
    aSettings.SetExecuted( ETrue );

    // error empty settings object
    if ( !aSettings.NodeBuf().Count() )
      {
      BMADAPTERLOGSTRING( "StoreSnapL: Error, No commands found" )
        return;
        }
       
    TInt nameCmdInd( -1 );
    TInt startInd( 0 );
    RCmDestinationExt snap;
    TInt snapId( 0 );
    
    // New SNAP to be stored, first command is add node
    if( !aSettings.NodeBuf()[0]->Leaf() && 
      aSettings.NodeBuf()[0]->CmdType() == CNSmlDmBmAdapter::EAddCmd)
        {
        CSmlDmBmCommandElement* snapCmd( NULL );
        CSmlDmBmCommandElement* nameCmd( NULL );
      
        snapCmd = aSettings.NodeBuf()[0];
        startInd = 1;

        // find name command
        for ( TInt i( 0 ); i < aSettings.NodeBuf().Count(); i++)
            {
            if ( aSettings.NodeBuf()[i]->Leaf() && 
                 !aSettings.NodeBuf()[i]->LastUriSeg()->Compare( KNSmlDmBMAdapterName ) )
                {
                nameCmd = aSettings.NodeBuf()[i];
                nameCmdInd = i;
                break;
                }
            }
        // Snap can be created when name cmd exists
        if ( snapCmd && nameCmd )
            {
            HBufC* name = HBufC::NewLC( nameCmd->Data()->Size() );
            TPtr namePtr = name->Des();
            CnvUtfConverter::ConvertToUnicodeFromUtf8( namePtr, *nameCmd->Data() );
            snap = iCmManagerExt.CreateDestinationL( namePtr );
            CleanupStack::PopAndDestroy( name );
            CleanupClosePushL( snap );
                              
            snap.UpdateL();
            snapCmd->SetStatus( CSmlDmAdapter::EOk );
            nameCmd->SetStatus( CSmlDmAdapter::EOk );
            snapCmd->SetExecuted( ETrue );
            nameCmd->SetExecuted( ETrue );
            aSettings.SetLuid( snap.Id() );
            }
        else
            {
            BMADAPTERLOGSTRING( "StoreSnapL: Error, SNAP doesn't have necessary data and can't be stored" )
            return;
            }
        }
    // existing SNAP
    else
        {
        snap = iCmManagerExt.DestinationL( aSettings.Luid() );
        CleanupClosePushL( snap );
        }
    snapId = snap.Id();
    TInt err( KErrNone );
    // execute all remaining commands for the snap 
    // starting after add node cmd
    for ( TInt i( startInd ); i < aSettings.NodeBuf().Count(); i++ )
        {
        // name command is already executed -> skipped
        if ( i != nameCmdInd )
            {
            CSmlDmBmCommandElement* currentCmd = aSettings.NodeBuf()[i];
            // handle leaf commands
            // leaves are trapped and if possible 
            // execution continues with remaining commands
            if ( currentCmd->Leaf() )
                {
                // name
                if ( !currentCmd->LastUriSeg()->Compare( KNSmlDmBMAdapterName() ) )
                    {
                    // name is set more than once within same message
                    TRAP( err, ExecuteNameCmdL( *currentCmd, snap ) );
                    }
                // metadata 
                else if ( !currentCmd->LastUriSeg()->Compare( KNSmlDmBMAdapterMetadata ) )
                    {
                    TRAP( err, ExecuteMetadataCmdL( *currentCmd, snap ) );
                    }
                // protected
                else if ( !currentCmd->LastUriSeg()->Compare( KNSmlDmBMAdapterProtected ) )
                    {
                    TRAP( err, ExecuteProtectionCmdL( *currentCmd, snap ) );
                    }
                // hidden
                else if ( !currentCmd->LastUriSeg()->Compare( KNSmlDmBMAdapterHidden ) )
                    {
                    TRAP( err, ExecuteHiddenCmdL( *currentCmd, snap ) );
                    }
                // iap list 
                else if ( !currentCmd->LastUriSeg()->Compare( KNSmlDmBMAdapterIAPPriorityList ) )
                    {
                    TRAP( err, ExecuteIapsCmdL( *currentCmd, snap ) );
                    }
                // embedded snap  
                else if ( !currentCmd->LastUriSeg()->Compare( KNSmlDmBMAdapterEmbeddedSNAP ) )
                    {
                    TRAP( err, ExecuteEmbeddedSnapCmdL( *currentCmd, snap ) );
                    }
                else
                    {
                    // invalid node
                    BMADAPTERLOGSTRING( "StoreSnapL: Error, Invalid node name" )
                    currentCmd->SetStatus( CSmlDmAdapter::ENotFound );
                    err = KErrNone;
                    }
                }
            // snap node    
            else
                {
                TRAP( err, ExecuteSnapCmdL( *currentCmd, snap.Id() ) );
                // no more commands will be executed
                // after snap is deleted
                if( currentCmd->CmdType() == CNSmlDmBmAdapter::EDeleteCmd )
                    {
                    break;
                    }
                }
  
            // handle errors, check if execution may continue
            if( err != KErrNone )
                {
                // cases where not worth continuing
                // attempt to return status codes however
                if( err == KErrNoMemory )
                    {
                    currentCmd->SetStatus( CSmlDmAdapter::ENoMemory );
                    currentCmd->SetExecuted( ETrue );
                    break;
                    }
                else if( err == KErrDiskFull )
                    {
                    currentCmd->SetStatus( CSmlDmAdapter::EDiskFull );
                    currentCmd->SetExecuted( ETrue );
                    break;
                    }
                // cases where command execution continues
                else if( err == KErrNotFound )
                    {
                    currentCmd->SetStatus( CSmlDmAdapter::ENotFound );
                    }
                else if( err == KErrArgument )
                    {
                    currentCmd->SetStatus( CSmlDmAdapter::EInvalidObject );
                    }
                else
                    {
                    currentCmd->SetStatus( CSmlDmAdapter::EError );
                    }
                // if error occurred, reopen snap to dismis any changes
                // continue with rest of buffered commands    
                CleanupStack::PopAndDestroy( &snap );
                snap = iCmManagerExt.DestinationL( snapId );
                CleanupClosePushL( snap );
                }
            // if leaf cmd execution was successful and data written -> update the snap
            else if( currentCmd->Leaf() &&
                currentCmd->Status() == CSmlDmAdapter::EOk && 
                currentCmd->CmdType() != CNSmlDmBmAdapter::EGetCmd && 
                currentCmd->CmdType() != CNSmlDmBmAdapter::EGetSizeCmd )
                {
                snap.UpdateL();
                }
            else
                {
                BMADAPTERLOGSTRING( 
                    "StoreSnapL: Leaf cmd execution unsuccessful" )
                }
            currentCmd->SetExecuted( ETrue );

            }
        }
    CleanupStack::PopAndDestroy( &snap );
    }

// -----------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ExecuteCmdL
// Executes a single command
// -----------------------------------------------------------------------------
void CNSmlDmBmSettingStore::ExecuteCmdL( CSmlDmBmCommandElement& aCmd, TUint aLuid)
    {
    if( !aCmd.Leaf() )
        {
        ExecuteSnapCmdL( aCmd, aLuid );
        }
    else 
        {
        // snap can be opened
        RCmDestinationExt snap = iCmManagerExt.DestinationL( aLuid );
        CleanupClosePushL( snap );
        // name
        if ( !aCmd.LastUriSeg()->Compare( KNSmlDmBMAdapterName ) )
            {
            ExecuteNameCmdL( aCmd, snap );
            }
        // iap list
        else if ( !aCmd.LastUriSeg()->Compare( KNSmlDmBMAdapterIAPPriorityList ) )
            {
            ExecuteIapsCmdL( aCmd, snap );
            } 
        // metadata
        else if ( !aCmd.LastUriSeg()->Compare( KNSmlDmBMAdapterMetadata ) )
            {
            ExecuteMetadataCmdL( aCmd, snap ); 
            } 
        // protected
        else if ( !aCmd.LastUriSeg()->Compare( KNSmlDmBMAdapterProtected ) )
            {
            ExecuteProtectionCmdL( aCmd, snap ); 
            }
        // hidden 
        else if ( !aCmd.LastUriSeg()->Compare( KNSmlDmBMAdapterHidden ) )
            {
            ExecuteHiddenCmdL( aCmd, snap ); 
            }
        // embedded snap
        else if ( !aCmd.LastUriSeg()->Compare( KNSmlDmBMAdapterEmbeddedSNAP ) )
            {
            ExecuteEmbeddedSnapCmdL( aCmd, snap );
            }
        else
            {
            // invalid node
            BMADAPTERLOGSTRING( "ExecuteCmdL: Error, Invalid node name" )
            aCmd.SetStatus( CSmlDmAdapter::ENotFound );
            }
        // update snap if necessary              
        if( aCmd.Status() == CSmlDmAdapter::EOk &&
            aCmd.CmdType() != CNSmlDmBmAdapter::EGetCmd && 
            aCmd.CmdType() != CNSmlDmBmAdapter::EGetSizeCmd)
            {
            snap.UpdateL();
            }
        aCmd.SetExecuted( ETrue );
        CleanupStack::PopAndDestroy( &snap );
        }
    }

   
// -----------------------------------------------------------------------------
// CNSmlDmBmSettingStore::GetSnapsL
// Fetches all snap ids
// -----------------------------------------------------------------------------
void CNSmlDmBmSettingStore::GetSnapsL( RArray<TUint32>& aLUIDArray )
        {
        iCmManagerExt.AllDestinationsL( aLUIDArray);
        }
 
// -----------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ExecuteSnapCmdL
// Executes a command for snap node. Either delete or get, adds are handled 
// in StoreSNAPL
// -----------------------------------------------------------------------------
void CNSmlDmBmSettingStore::ExecuteSnapCmdL( CSmlDmBmCommandElement& aCmd, TUint aLuid )
    {
    RCmDestinationExt snap = iCmManagerExt.DestinationL( aLuid );
    CleanupClosePushL( snap );
    if ( aCmd.CmdType() == CNSmlDmBmAdapter::EGetCmd )
        {
        TBool found( EFalse );
        // get child node list

        // check if snap has embedded snap        
        for ( TInt i(0), c = snap.ConnectionMethodCount(); i < c; i++ )
            {
            RCmConnectionMethodExt cm = snap.ConnectionMethodL( i );
            if ( cm.GetBoolAttributeL( CMManager::ECmDestination ) )
                {
                found = ETrue;
                break;
                }
            }
        if( found )
            {
            aCmd.SetDataL( KNSmlDmBMAllLeafNodes );
            }
        else
            {
            aCmd.SetDataL( KNSmlDmBMAllLeafNodesNoEmbedded );
            }

        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        CleanupStack::PopAndDestroy( &snap );
        }
    else if ( aCmd.CmdType() == CNSmlDmBmAdapter::EDeleteCmd )
        {
        snap.DeleteLD();
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        CleanupStack::Pop( &snap );
        }
    else
        {
        BMADAPTERLOGSTRING( "ExecuteSnapCmdL: Error, Invalid cmd type" )
        aCmd.SetStatus( CSmlDmAdapter::EError );
        CleanupStack::PopAndDestroy( &snap );
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ExecuteIapsCmdL
// Executes an iap list command (add or get).
// ---------------------------------------------------------------------------
//    
void CNSmlDmBmSettingStore::ExecuteIapsCmdL( CSmlDmBmCommandElement& aCmd, 
                                             RCmDestinationExt& aSnap )
    {
    // for add command replace the list of iaps with the new one
    if ( aCmd.CmdType() == CNSmlDmBmAdapter::EAddCmd )
        {
        // remove old iaps
        for( TInt i(0); i < aSnap.ConnectionMethodCount(); )
            {
            RCmConnectionMethodExt cm = aSnap.ConnectionMethodL( i );
            // if iap, remove
            if ( !cm.GetBoolAttributeL( CMManager::ECmDestination ) )
                {
                aSnap.RemoveConnectionMethodL( cm );
                }
            // otherwise skip the ebedded snap entry  
            else
                {
                i++;
                }
            }
        // add new list of iaps 
        RArray<TUint> iaps = iAdapter->GetIdArrayL( *aCmd.Data() );
        CleanupClosePushL( iaps );
        TInt wildcard(0);
        for ( TInt i( 0 ); i < iaps.Count(); i++ )
            {
            RCmConnectionMethodExt cm = iCmManagerExt.ConnectionMethodL( iaps[i] );
            aSnap.AddConnectionMethodL( cm );
            if ( aSnap.PriorityL( cm ) != CMManager::KDataMobilitySelectionPolicyPriorityWildCard )
                {
                // set the priority of iap as it's index in the list
                // but preceding any iaps with wildcard priority.
                aSnap.ModifyPriorityL( cm, i - wildcard );
                }
            else
                {
                wildcard++;
                }
            }
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        CleanupStack::PopAndDestroy( &iaps );
        }
    // for get command fetch the list of iaps
    else if ( aCmd.CmdType() == CNSmlDmBmAdapter::EGetCmd || 
              aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
        {
        RArray<TUint> ids;
        CleanupClosePushL( ids );
        for ( TInt i(0), c = aSnap.ConnectionMethodCount(); i < c; i++ )
            {
            // if not embedded snap or hidden, insert into iap list 
            // (hidden iaps not handled by internet adapter, so link won't be found)
            if( !aSnap.ConnectionMethodL(i).GetBoolAttributeL( CMManager::ECmDestination ) &&
                !aSnap.ConnectionMethodL(i).GetBoolAttributeL( CMManager::ECmHidden ) )
                {
                ids.Insert( aSnap.ConnectionMethodL(i).GetIntAttributeL( 
                                                       CMManager::ECmIapId ), i );
                }
            }
        if ( ids.Count() > 0)
            {
            HBufC8* iapList = iAdapter->GetIapListL( ids );
            CleanupStack::PushL( iapList );
            aCmd.SetDataL( *iapList );
            CleanupStack::PopAndDestroy( iapList );
            }
        else
            {
            aCmd.SetDataL( KNullDesC8 );
            }
        CleanupStack::PopAndDestroy( &ids );
        // for size command, set the command data to be the 
        // size of the fetched data
        if( aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
            {
            HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
            CleanupStack::PushL( size );
            aCmd.SetDataL( *size );
            CleanupStack::PopAndDestroy( size );
            }
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        BMADAPTERLOGSTRING( "ExecuteIapsCmdL: Error, Only Add, Get and Get size commands supported" )
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ExecuteProtectionCmdL
// Executes an protected command (add or get).
// ---------------------------------------------------------------------------
//
void CNSmlDmBmSettingStore::ExecuteProtectionCmdL( CSmlDmBmCommandElement& aCmd, 
                                                   RCmDestinationExt& aSnap )
    {
    if ( aCmd.CmdType() == CNSmlDmBmAdapter::EAddCmd )
        {
        __UHEAP_MARK;
        TInt data( iAdapter->DesToIntL( *aCmd.Data() ) );
        // is proper protection level
        if ( data == CMManager::EProtLevel0 )
            {
            aSnap.SetProtectionL( CMManager::EProtLevel0 );
            aCmd.SetStatus( CSmlDmAdapter::EOk );
            }
        else if ( data == CMManager::EProtLevel1 )
            {
            aSnap.SetProtectionL( CMManager::EProtLevel1 );
            aCmd.SetStatus( CSmlDmAdapter::EOk );
            }
        else if ( data == CMManager::EProtLevel2 )
            {
            aSnap.SetProtectionL( CMManager::EProtLevel2 );
            aCmd.SetStatus( CSmlDmAdapter::EOk );
            }
        else
            {
            aCmd.SetStatus( CSmlDmAdapter::EInvalidObject );
            }
        __UHEAP_MARKEND;
        }
    else if ( aCmd.CmdType() == CNSmlDmBmAdapter::EGetCmd ||
              aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
        {
        HBufC8* data = iAdapter->IntToDes8L( aSnap.ProtectionLevel() );
        CleanupStack::PushL( data );
        aCmd.SetDataL( *data );
        CleanupStack::PopAndDestroy( data );
        
        // for size command, set the command data to be the 
        // size of the fetched data
        if( aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
            {
            HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
            CleanupStack::PushL( size );
            aCmd.SetDataL( *size );
            CleanupStack::PopAndDestroy( size );
            }

        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        BMADAPTERLOGSTRING( "ExecuteProtectionCmdL: Error, Only Add, Get and Get size commands supported" )
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ExecuteHiddenCmdL
// Executes hidden command (add or get)
// ---------------------------------------------------------------------------
//
void CNSmlDmBmSettingStore::ExecuteHiddenCmdL( CSmlDmBmCommandElement& aCmd, 
                                               RCmDestinationExt& aSnap )
    {
    if ( aCmd.CmdType() == CNSmlDmBmAdapter::EAddCmd )
        {
        __UHEAP_MARK;
        TBool data; 
        if( iAdapter->DesToBool( *aCmd.Data(), data ) )
            {
            aSnap.SetHiddenL( data );
            aCmd.SetStatus( CSmlDmAdapter::EOk );
            }
        else
            {
            aCmd.SetStatus( CSmlDmAdapter::EInvalidObject );
            }
        __UHEAP_MARKEND;
        }
    else if ( aCmd.CmdType() == CNSmlDmBmAdapter::EGetCmd ||
              aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
        {
        HBufC8* data = iAdapter->BoolToDes8LC( aSnap.IsHidden() );
        aCmd.SetDataL( *data );
        CleanupStack::PopAndDestroy( data ); 
        // for size command, set the command data to be the 
        // size of the fetched data
        if( aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
            {
            HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
            CleanupStack::PushL( size );
            aCmd.SetDataL( *size );
            CleanupStack::PopAndDestroy( size );
            }
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        BMADAPTERLOGSTRING( "ExecuteHiddenCmdL: Error, Only Add, Get and Get size commands supported" )
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ExecuteMetadataCmdL
// Executes metadata command (add or get)
// ---------------------------------------------------------------------------
//
void CNSmlDmBmSettingStore::ExecuteMetadataCmdL( CSmlDmBmCommandElement& aCmd, 
                                                 RCmDestinationExt& aSnap )
    {
    if ( aCmd.CmdType() == CNSmlDmBmAdapter::EAddCmd )
        {
        __UHEAP_MARK;
        
        TInt data( iAdapter->DesToIntL( *aCmd.Data() ) );
        // AND with inversed bit masks to check if data 
        // has bits set that are not defined for metadata
        if( ( data & 
              ~CMManager::ESnapMetadataHighlight &
              ~CMManager::ESnapMetadataHiddenAgent & 
              ~CMManager::ESnapMetadataPurpose ) )
            {
            aCmd.SetStatus( CSmlDmAdapter::EError );
            }
        // set value for each metadata bit
        else
            {
            aSnap.SetMetadataL( CMManager::ESnapMetadataHighlight, 
                                CMManager::ESnapMetadataHighlight & data );
            aSnap.SetMetadataL( CMManager::ESnapMetadataHiddenAgent, 
                                CMManager::ESnapMetadataHiddenAgent & data );
            aSnap.SetMetadataL( CMManager::ESnapMetadataPurpose, 
                              ( CMManager::ESnapMetadataPurpose & data ) >> KShift8 );
            aCmd.SetStatus( CSmlDmAdapter::EOk );
            }
        __UHEAP_MARKEND;
        }
    else if ( aCmd.CmdType() == CNSmlDmBmAdapter::EGetCmd ||
              aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
        {
        // get all metadata values
        TInt metadata(0);
        metadata = aSnap.MetadataL( CMManager::ESnapMetadataHighlight ) +
                   aSnap.MetadataL( CMManager::ESnapMetadataHiddenAgent ) + 
                 ( aSnap.MetadataL( CMManager::ESnapMetadataPurpose ) << KShift8 );
        HBufC8* data = iAdapter->IntToDes8L( metadata );
        CleanupStack::PushL( data );
        aCmd.SetDataL( *data ); 
        CleanupStack::PopAndDestroy( data );
        
        // for size command, set the command data to be the 
        // size of the fetched data
        if( aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
            {
            HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
            CleanupStack::PushL( size );
            aCmd.SetDataL( *size );
            CleanupStack::PopAndDestroy( size );
            }
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        BMADAPTERLOGSTRING( "ExecuteMetadataCmdL: Error, Only Add, Get and Get size commands supported" )
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ExecuteEmbeddedSnapCmdL
// Executes embedded snap command (add, get or delete)
// ---------------------------------------------------------------------------
//
void CNSmlDmBmSettingStore::ExecuteEmbeddedSnapCmdL( CSmlDmBmCommandElement& aCmd, 
                                                     RCmDestinationExt& aSnap )
    {
    if ( aCmd.CmdType() == CNSmlDmBmAdapter::EAddCmd )
        {
        // Remove ./ from beginning of the URI
        TPtrC8 embeddedUri = aCmd.Data()->Mid( KNSmlDmBmAdapterURIPrefix().Length() );

        // if the embedded snap is still in buffer it's stored here
        iAdapter->StoreSnapIfBufferedL( embeddedUri );
        // LUID mapping is assumed done by this time

        // get SNAP id for the URI 
        HBufC8* luid = iAdapter->GetLuidAllocL( embeddedUri );
        CleanupStack::PushL( luid );
        if ( luid->Size() > 0 )
            {
            TInt id = iAdapter->DesToIntL( *luid );

            // open snap with the id
            RCmDestinationExt embedded = iCmManagerExt.DestinationL( id );
            CleanupClosePushL( embedded );

            // check if snap has an existing embedded snap                       
            for ( TInt i(0), c = aSnap.ConnectionMethodCount(); i < c; i++ )
                {
                RCmConnectionMethodExt cm = aSnap.ConnectionMethodL( i );
                // remove existing embedded snap before adding new one
                if ( cm.GetBoolAttributeL( CMManager::ECmDestination ) )
                    {
                    aSnap.DeleteConnectionMethodL( cm );
                    break;
                }
            }
            aSnap.AddEmbeddedDestinationL( embedded ); 
            CleanupStack::PopAndDestroy( &embedded );
            aCmd.SetStatus( CSmlDmAdapter::EOk );
            }
        else
            {
            // no luid for the uri
            BMADAPTERLOGSTRING( "ExecuteEmbeddedSnapCmdL: No LUID found for embedded snap" )
            aCmd.SetStatus( CSmlDmAdapter::EInvalidObject );
            }
        aCmd.SetExecuted( ETrue );
        CleanupStack::PopAndDestroy( luid );
        }
    else if ( aCmd.CmdType() == CNSmlDmBmAdapter::EGetCmd ||
              aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
        {
        // get destination
        TInt embeddedId( -1 );
        for ( TInt i( 0 ), c = aSnap.ConnectionMethodCount(); i < c; i++ )
            {
            RCmConnectionMethodExt cm = aSnap.ConnectionMethodL( i );
            // check if embedded
            if ( cm.GetBoolAttributeL( CMManager::ECmDestination ) )
                {
                // get snap id
                embeddedId = cm.GetIntAttributeL( CMManager::ECmId );

                // CMManager::ECmId returns element id, 
                // GetSnapUriFromLuidL needs record id
                embeddedId = ( embeddedId & KCDMaskShowRecordId ) >> KShift8;

                HBufC8* embeddedUri = iAdapter->GetSnapUriFromLuidL( embeddedId );
                CleanupStack::PushL( embeddedUri );
                
                if( !embeddedUri )
                    {
                    embeddedId = -1;
                    break;
                    }
                aCmd.SetDataL( *embeddedUri );
                CleanupStack::PopAndDestroy( embeddedUri );
                
                // for size command, set the command data to be the 
                // size of the fetched data
                if( aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
                    {
                    HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
                    CleanupStack::PushL( size );
                    aCmd.SetDataL( *size );
                    CleanupStack::PopAndDestroy( size );
                    }
                aCmd.SetStatus( CSmlDmAdapter::EOk );
                aCmd.SetExecuted( ETrue );
                break;
                }
            }
        // if no embedded snap exists return not found
        if ( embeddedId < 0 )
            {
            BMADAPTERLOGSTRING( "ExecuteEmbeddedSnapCmdL: Get, No embedded snap found" )
            aCmd.SetStatus( CSmlDmAdapter::ENotFound );
            aCmd.SetExecuted( ETrue );
            }
        }
    else if ( aCmd.CmdType() == CNSmlDmBmAdapter::EDeleteCmd )
        {
        TBool found( EFalse);
        for ( TInt i( 0 ), c = aSnap.ConnectionMethodCount(); i < c; i++ )
            {
            RCmConnectionMethodExt cm = aSnap.ConnectionMethodL( i );
            // find embedded snap and delete it
            if ( cm.GetBoolAttributeL( CMManager::ECmDestination ) )
                {
                found = ETrue;
                aSnap.DeleteConnectionMethodL( cm );
                aCmd.SetStatus( CSmlDmAdapter::EOk );
                break;
                }
            }
        if ( !found )
            {
            BMADAPTERLOGSTRING( "ExecuteEmbeddedSnapCmdL: Delete, No embedded snap found" )
            aCmd.SetStatus( CSmlDmAdapter::ENotFound );
            aCmd.SetExecuted( ETrue );
            }
        }
    else    
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        aCmd.SetExecuted( ETrue );
        BMADAPTERLOGSTRING( "ExecuteEmbeddedSnapCmdL: Error, command not supported" )
        }
    }

// ---------------------------------------------------------------------------
// CNSmlDmBmSettingStore::ExecuteNameCmdL
// Executes name command (add or get )
// ---------------------------------------------------------------------------
//
void CNSmlDmBmSettingStore::ExecuteNameCmdL( CSmlDmBmCommandElement& aCmd, 
                                             RCmDestinationExt& aSnap )
    {
    if ( aCmd.CmdType() == CNSmlDmBmAdapter::EAddCmd )
        {
        HBufC* name = HBufC::NewLC( aCmd.Data()->Size() );
        TPtr namePtr = name->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( namePtr, *aCmd.Data() );
        aSnap.SetNameL( namePtr );
        CleanupStack::PopAndDestroy( name );
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        }
    else if ( aCmd.CmdType() == CNSmlDmBmAdapter::EGetCmd ||
              aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
        {
        HBufC* data = aSnap.NameLC();
        HBufC8* data8 = HBufC8::NewLC( data->Size() );
        TPtr8 toPtr = data8->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( toPtr, *data );
        aCmd.SetDataL( *data8 );
        CleanupStack::PopAndDestroy( data8 );
        CleanupStack::PopAndDestroy( data );
       
        // for size command, set the command data to be the 
        // size of the fetched data
        if( aCmd.CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
            {
            HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
            CleanupStack::PushL( size );
            aCmd.SetDataL( *size );
            CleanupStack::PopAndDestroy( size );
            }
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else    
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        BMADAPTERLOGSTRING( "ExecuteEmbeddedSnapCmdL: Error, Only Add, Get and Get size commands supported" )
        }
    }

