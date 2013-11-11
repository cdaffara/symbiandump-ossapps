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
* Description:   Bearer management DM adapter. Provides handling of 
                Bearer management related settings in Device management
*
*/



#include <ecom/implementationproxy.h>
#include <cmdestinationext.h>

#include "nsmldmbmadapter.h"
#include "bmadapterlogger.h"

// ======== MEMBER FUNCTIONS ========


// --------------------------------------------------------------------------
// Maps plugin UID to its factory function (constructor)
// --------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x101f6d2f, 
                               CNSmlDmBmAdapter::NewL)
    };

// --------------------------------------------------------------------------
// For ecom plugin implementation
// -------------------------------------------------------------------------

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::NewL
// --------------------------------------------------------------------------
CNSmlDmBmAdapter* CNSmlDmBmAdapter::NewL( MSmlDmCallback* aDmCallback )
    {
    BMADAPTERLOGSTRING("CNSmlDmBmAdapter::NewL: Start")
    CNSmlDmBmAdapter* self = new (ELeave) CNSmlDmBmAdapter( aDmCallback );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    BMADAPTERLOGSTRING("CNSmlDmBmAdapter::NewL: End")
    return self;
    }

// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::CNSmlDmBmAdapter
// --------------------------------------------------------------------------

CNSmlDmBmAdapter::CNSmlDmBmAdapter( MSmlDmCallback* aDmCallback ) 
    : CSmlDmAdapter( aDmCallback )
    {
    }

// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::ConstructL
// --------------------------------------------------------------------------
void CNSmlDmBmAdapter::ConstructL()
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::CNSmlDmBmAdapter: Start" )
    iSettingStore = CNSmlDmBmSettingStore::NewL( this );
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::CNSmlDmBmAdapter: End" )
    }

// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::~CNSmlDmBmAdapter
// --------------------------------------------------------------------------
CNSmlDmBmAdapter::~CNSmlDmBmAdapter()
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::~CNSmlDmBmAdapter: Start" )
    iBuffer.ResetAndDestroy();
    iBuffer.Close();
    delete iSettingStore;
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::~CNSmlDmBmAdapter: End" )
    }
    
// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::DDFVersionL
// Returns ddf version nr
// --------------------------------------------------------------------------
void CNSmlDmBmAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::DDFVersionL: Start" )
    aDDFVersion.InsertL( 0, KNSmlDmBMAdapterDDFVersion );
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::DDFVersionL:End" )
    }
   
// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::DDFStructureL
//  Constructs the DDF structure
// --------------------------------------------------------------------------
void CNSmlDmBmAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::DDFStructureL: Start" )
    TSmlDmAccessTypes accessTypes;

    // BearerManagementSNAP
    MSmlDmDDFObject& bm = aDDF.AddChildObjectL( KNSmlDmBMAdapterBM );
    accessTypes.SetGet();
    accessTypes.SetAdd();
    bm.SetAccessTypesL( accessTypes );
    bm.SetOccurenceL( MSmlDmDDFObject::EOne );
    bm.SetScopeL( MSmlDmDDFObject::EPermanent );
    bm.SetDFFormatL( MSmlDmDDFObject::ENode );
    bm.AddDFTypeMimeTypeL( KNSmlDmBMAdapterTextPlain );
    bm.SetDescriptionL( KNSmlDmBMAdapterBMDescription );
    accessTypes.Reset();

    
    // BearerManagementSNAP/<X>
    MSmlDmDDFObject& dynamic1 = bm.AddChildObjectGroupL();
    accessTypes.SetAdd();
    accessTypes.SetDelete();
    accessTypes.SetGet();
    accessTypes.SetReplace();
    dynamic1.SetAccessTypesL( accessTypes );
    dynamic1.SetOccurenceL( MSmlDmDDFObject::EZeroOrMore );
    dynamic1.SetScopeL( MSmlDmDDFObject::EDynamic );
    dynamic1.SetDFFormatL( MSmlDmDDFObject::ENode );
    dynamic1.AddDFTypeMimeTypeL( KNSmlDmBMAdapterTextPlain );
    dynamic1.SetDescriptionL( KNSmlDmBMAdapterDynamicDescription );
    accessTypes.Reset();


    // BearerManagementSNAP/<X>/Name
    MSmlDmDDFObject& name = dynamic1.AddChildObjectL( KNSmlDmBMAdapterName );
    accessTypes.SetAdd();
    accessTypes.SetGet();
    accessTypes.SetReplace();
    name.SetAccessTypesL( accessTypes );
    name.SetOccurenceL( MSmlDmDDFObject::EOne );
    name.SetScopeL( MSmlDmDDFObject::EDynamic );
    name.SetDFFormatL( MSmlDmDDFObject::EChr );
    name.SetDescriptionL( KNSmlDmBMAdapterNameDescription );
    accessTypes.Reset();

    // BearerManagementSNAP/<X>/IAPPrioityList
    MSmlDmDDFObject& iapList = 
    dynamic1.AddChildObjectL( KNSmlDmBMAdapterIAPPriorityList );
    accessTypes.SetAdd();
    accessTypes.SetGet();
    accessTypes.SetReplace();
    iapList.SetAccessTypesL( accessTypes );
    iapList.SetOccurenceL( MSmlDmDDFObject::EOne );
    iapList.SetScopeL( MSmlDmDDFObject::EDynamic );
    iapList.SetDFFormatL( MSmlDmDDFObject::EChr );
    iapList.SetDescriptionL( KNSmlDmBMAdapterIAPPriorityListDescription );
    accessTypes.Reset();
        
    // BearerManagementSNAP/<X>/EmbeddedSNAP
    MSmlDmDDFObject& embedded = 
    dynamic1.AddChildObjectL( KNSmlDmBMAdapterEmbeddedSNAP );
    accessTypes.SetAdd();
    accessTypes.SetDelete();
    accessTypes.SetGet();
    accessTypes.SetReplace();
    embedded.SetAccessTypesL( accessTypes );
    embedded.SetOccurenceL( MSmlDmDDFObject::EZeroOrOne );
    embedded.SetScopeL( MSmlDmDDFObject::EDynamic );
    embedded.SetDFFormatL( MSmlDmDDFObject::EChr );
    embedded.SetDescriptionL( KNSmlDmBMAdapterEmbeddedSNAPDescription );
    accessTypes.Reset();

    // BearerManagementSNAP/<X>/Metadata
    MSmlDmDDFObject& metadata = 
    dynamic1.AddChildObjectL( KNSmlDmBMAdapterMetadata );
    accessTypes.SetAdd();
    accessTypes.SetGet();
    accessTypes.SetReplace();
    metadata.SetAccessTypesL( accessTypes );
    metadata.SetOccurenceL( MSmlDmDDFObject::EOne );
    metadata.SetScopeL( MSmlDmDDFObject::EDynamic );
    metadata.SetDFFormatL( MSmlDmDDFObject::EInt );
    metadata.SetDescriptionL( KNSmlDmBMAdapterMetadataDescription );
    accessTypes.Reset();
 
 
    // BearerManagementSNAP/<X>/Protected
    MSmlDmDDFObject& protection = 
    dynamic1.AddChildObjectL( KNSmlDmBMAdapterProtected );
    accessTypes.SetAdd();
    accessTypes.SetGet();
    accessTypes.SetReplace();
    protection.SetAccessTypesL( accessTypes );
    protection.SetOccurenceL( MSmlDmDDFObject::EOne );
    protection.SetScopeL( MSmlDmDDFObject::EDynamic );
    protection.SetDFFormatL( MSmlDmDDFObject::EInt );
    protection.SetDescriptionL( KNSmlDmBMAdapterProtectedDescription );
    accessTypes.Reset(); 
    
    // BearerManagementSNAP/<X>/Hidden
    MSmlDmDDFObject& hidden = dynamic1.AddChildObjectL( KNSmlDmBMAdapterHidden );
    accessTypes.SetAdd();
    accessTypes.SetGet();
    accessTypes.SetReplace();
    hidden.SetAccessTypesL( accessTypes );
    hidden.SetOccurenceL( MSmlDmDDFObject::EOne );
    hidden.SetScopeL( MSmlDmDDFObject::EDynamic );
    hidden.SetDFFormatL( MSmlDmDDFObject::EBool );
    hidden.SetDescriptionL( KNSmlDmBMAdapterHiddenDescription );
    accessTypes.Reset(); 
    
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::DDFStructureL: End" )
    }

// -------------------------------------------------------------------------
// CNSmlDmBmAdapter::UpdateLeafObjectL
//  Adds or updates leaf node value. If snap object is 
//  in buffer this command is also buffered. Also if leaf node 
//  is embedded snap and that snap is buffered, the argument command
//  is buffered.
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                          const TDesC8& aLUID, 
                                          const TDesC8& aObject, 
                                          const TDesC8& /*aType*/, 
                                          TInt aStatusRef )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::UpdateLeafObjectL: Start" )
    BMADAPTERLOGSTRING4( "\tUpdateLeafObjectL  \tURI: %S, \n\tLUID: %S,\
                         \tobject: %S ", 
                        &aURI, &aLUID, &aObject )

    TInt snapId = MapPredefinedSnapsL( aURI, aLUID );
                        
    // Check if parent snap node is found in the buffered settings
    CSmlDmBmSettingsElement* snap = 
    GetSnapIfBuffered( RemoveLastURISeg( aURI ) );

    // data for buffered SNAP -> Store command into buffer    
    if ( snap )
        {
        CSmlDmBmCommandElement* cmd = 
        CSmlDmBmCommandElement::NewLC( ETrue, 
                                       aStatusRef,
                                       KNSmlDmBmInvalidRef, 
                                       CNSmlDmBmAdapter::EAddCmd,
                                       LastURISeg( aURI ), 
                                       aObject);
        snap->NodeBuf().AppendL(cmd);
        CleanupStack::Pop( cmd );
        BMADAPTERLOGSTRING2( "\tSnap %S found, buffering cmd", 
                            snap->MappingName() )

        }
    // updates for an existing SNAP
    else
        {

        // If setting embedded SNAP node and the embedded SNAP node is still 
        // buffered -> Buffer also this command
        if( !LastURISeg( aURI ).Compare( KNSmlDmBMAdapterEmbeddedSNAP ) )
            {
            snap = GetSnapIfBuffered( aObject );

                // embedded SNAP is buffered
                if ( snap )
                    {
                    CSmlDmBmCommandElement* cmd = 
                    CSmlDmBmCommandElement::NewLC( ETrue, 
                                                   aStatusRef,
                                                   KNSmlDmBmInvalidRef, 
                                                   CNSmlDmBmAdapter::EAddCmd,
                                                   LastURISeg( aURI ), 
                                                   aObject);

                    // buffer this command
                    CSmlDmBmSettingsElement* setting = 
                    CSmlDmBmSettingsElement::NewLC( RemoveLastURISeg( aURI ) );
                    
                    // Check if SNAP was predefined or not
                    if( snapId == KErrNotFound )
                        {
                        snapId = DesToIntL( aLUID );
                        }
                    setting->SetLuid( snapId );
                    iBuffer.AppendL( setting );
                    CleanupStack::Pop( setting );
                    setting->NodeBuf().AppendL(cmd);
                    CleanupStack::Pop( cmd );
                    BMADAPTERLOGSTRING2( "\tEmbedded Snap %S buffered, buffering cmd", 
                                         snap->MappingName() )
                    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::UpdateLeafObjectL: End" )
                    return;
                    }
            }
        // execute command and return status  
        CSmlDmBmCommandElement* cmd = 
        CSmlDmBmCommandElement::NewLC( ETrue, 
                                       aStatusRef,
                                       KNSmlDmBmInvalidRef, 
                                       CNSmlDmBmAdapter::EAddCmd,
                                       LastURISeg( aURI ), 
                                       aObject);
        if( snapId == KErrNotFound )
            {
            snapId = DesToIntL( aLUID );
            }
        iSettingStore->ExecuteCmdL( *cmd, snapId );
        BMADAPTERLOGSTRING2( "\tCmd executed with status: %d", 
                              cmd->Status() )
        Callback().SetStatusL( aStatusRef, cmd->Status() );
        CleanupStack::PopAndDestroy( cmd );

        }
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::UpdateLeafObjectL: End" )
    }
// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::DeleteObjectL
//  Deletes node or leaf object. If the snap this command relates to is
//  buffered, also the delete command is buffered.
// --------------------------------------------------------------------------

void CNSmlDmBmAdapter::DeleteObjectL( const TDesC8& aURI, 
                                      const TDesC8& aLUID,
                                      TInt aStatusRef )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::DeleteObjectL: Start" )
    BMADAPTERLOGSTRING3( "\tDeleteObjectL  \tURI: %S, \tLUID: %S", 
                         &aURI, &aLUID )
    
    CSmlDmBmCommandElement* cmd;
    
    TInt snapId = MapPredefinedSnapsL( aURI, aLUID );
    // check if node or leaf object and create command accordingly
    // when updating, make sure that cmd is constructed
    if ( NumOfURISegs( aURI  ) == KNsmlDmBmSNAPNodeDepth )
        {
        cmd = CSmlDmBmCommandElement::NewLC( EFalse, 
                                             aStatusRef,
                                             KNSmlDmBmInvalidRef, 
                                             CNSmlDmBmAdapter::EDeleteCmd,
                                             KNullDesC8, KNullDesC8 );
        }
    else if ( ! LastURISeg( aURI ).Compare( KNSmlDmBMAdapterEmbeddedSNAP() ) )
        {
        cmd = CSmlDmBmCommandElement::NewLC( ETrue, 
                                             aStatusRef,
                                             KNSmlDmBmInvalidRef, 
                                             CNSmlDmBmAdapter::EDeleteCmd,
                                             LastURISeg( aURI ), 
                                             KNullDesC8 );
        }
    else
        {
        // unsupported node, framework should check this
        Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
        BMADAPTERLOGSTRING( "\tCNSmlDmBmAdapter::DeleteObjectL: \
        Error, unsupported node" )
        return;
        }

    // If this SNAP still in buffer -> buffer command for later execution
    // Check if SNAP is found in the buffered settings
    CSmlDmBmSettingsElement* snap = NULL;
    if( cmd->Leaf() )
        {
        snap = GetSnapIfBuffered( RemoveLastURISeg( aURI ) );
        }
    else
        {
        snap = GetSnapIfBuffered( aURI );
        }
    // when updating make sure that cmd is removed from cleanup 
    // stack and deleted if necessary
    
    // if cmd for buffered snap append cmd to buffer
    if ( snap )
        {
        snap->NodeBuf().AppendL( cmd );
        CleanupStack::Pop( cmd );
        BMADAPTERLOGSTRING2( "\tSnap %S found, buffering cmd", 
                            snap->MappingName() )
        }
    // if existing node, execute command
    else if( aLUID.Length() > 0 || snapId != KErrNotFound )
        {
        if( snapId == KErrNotFound )
            {
            snapId = DesToIntL( aLUID );
            }
        iSettingStore->ExecuteCmdL( *cmd, snapId );
        BMADAPTERLOGSTRING2( "\tCmd executed with status: %d ", 
                              cmd->Status() )
        Callback().SetStatusL( aStatusRef, cmd->Status() );
        CleanupStack::PopAndDestroy( cmd );
        }
        // unknown node    
    else
        {
        BMADAPTERLOGSTRING( "\tUnknown node\n" )
        Callback().SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        CleanupStack::PopAndDestroy( cmd );
        }
    
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::DeleteObjectL: End" )
    }
// -------------------------------------------------------------------------
// CNSmlDmBmAdapter::FetchLeafObjectL
//  Fetches leaf object value. If the snap is buffered also this command 
//  will
//  be buffered.
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                         const TDesC8& aLUID, 
                                         const TDesC8& /*aType*/, 
                                         const TInt aResultsRef, 
                                         const TInt aStatusRef )
    {
    BMADAPTERLOGSTRING( "NSmlDmBMAdapter::FetchLeafObjectL: Start" )
    BMADAPTERLOGSTRING3( "\tFetchLeafObjectL  \tURI: %S, \tLUID: %S,", 
                        &aURI, &aLUID )

    TInt snapId = MapPredefinedSnapsL( aURI, aLUID );                         
                        
    // check if parent snap node is buffered 
    CSmlDmBmSettingsElement* snap = 
    GetSnapIfBuffered( RemoveLastURISeg( aURI ) );

    // data for buffered SNAP -> Store command into buffer    
    if ( snap )
        {
        CSmlDmBmCommandElement* cmd = 
        CSmlDmBmCommandElement::NewLC( ETrue, 
                                       aStatusRef, 
                                       aResultsRef, 
                                       CNSmlDmBmAdapter::EGetCmd,
                                       LastURISeg( aURI ), 
                                       KNullDesC8);
        snap->NodeBuf().AppendL( cmd );
        CleanupStack::Pop( cmd );
        BMADAPTERLOGSTRING2( "\tSnap %S found, buffering cmd", 
                            snap->MappingName() )
        }
    // otherwise execute cmd right away
    else
        {
                
        CSmlDmBmCommandElement* cmd = 
        CSmlDmBmCommandElement::NewLC( ETrue, 
                                       aStatusRef, 
                                       aResultsRef, 
                                       CNSmlDmBmAdapter::EGetCmd,
                                       LastURISeg( aURI ), 
                                       KNullDesC8);
        if( snapId == KErrNotFound )
            {
            snapId = DesToIntL( aLUID );
            }

        iSettingStore->ExecuteCmdL( *cmd, snapId );
        BMADAPTERLOGSTRING2( "\tCmd executed with status: %d ", 
                              cmd->Status() )
        // if executed return status
        if ( cmd->Executed() ) 
            {
            Callback().SetStatusL( aStatusRef, cmd->Status() );
            // if successful set results
            if ( cmd->Status() == CSmlDmAdapter::EOk )
                {
                BMADAPTERLOGSTRING2( "\tCmd executed with result: %S ", 
                                      cmd->Data() )
                CBufBase* result = CBufFlat::NewL( cmd->Data()->Size() );
                CleanupStack::PushL( result );
                result->InsertL( 0, *cmd->Data() );
                Callback().SetResultsL( aResultsRef, *result, KNullDesC8 );
                CleanupStack::PopAndDestroy( result );
                }
            }
        else
            {
            // failed to execute command
            Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
            }
        CleanupStack::PopAndDestroy( cmd );
        }
    
    BMADAPTERLOGSTRING( "NSmlDmBMAdapter::FetchLeafObjectL: End" )
    }

// --------------------------------------------------------------------------
//  CNSmlDmBmAdapter::FetchLeafObjectSizeL
//  Fetches leaf object size. If the snap is buffered also this command will
//  be buffered.
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                             const TDesC8& aLUID,
                                             const TDesC8& /* aType */, 
                                             TInt aResultsRef,
                                             TInt aStatusRef )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::FetchLeafObjectSizeL: Start" )
    BMADAPTERLOGSTRING3( "\tFetchLeafObjectSizeL  \tURI: %S, \tLUID: %S,", 
                         &aURI, &aLUID )
   
    TInt snapId = MapPredefinedSnapsL( aURI, aLUID );
    
    // check if parent snap node is buffered 
    CSmlDmBmSettingsElement* snap = 
    GetSnapIfBuffered( RemoveLastURISeg( aURI ) );

    // data for buffered SNAP -> Store command into buffer    
    if ( snap )
        {
        CSmlDmBmCommandElement* cmd = 
        CSmlDmBmCommandElement::NewLC( ETrue, 
                                       aStatusRef, 
                                       aResultsRef, 
                                       CNSmlDmBmAdapter::EGetSizeCmd,
                                       LastURISeg( aURI ), 
                                       KNullDesC8);
        snap->NodeBuf().AppendL( cmd );
        CleanupStack::Pop( cmd ); 
        BMADAPTERLOGSTRING2( "\tSnap %S found, buffering cmd", 
                             snap->MappingName() )
        }
    // otherwise execute get size cmd right away
    else
        {
        CSmlDmBmCommandElement* cmd = 
        CSmlDmBmCommandElement::NewLC( ETrue, 
                                       aStatusRef, 
                                       aResultsRef, 
                                       CNSmlDmBmAdapter::EGetSizeCmd,
                                       LastURISeg( aURI ), 
                                       KNullDesC8);
        if( snapId == KErrNotFound )
            {
            snapId = DesToIntL( aLUID );
            }
        iSettingStore->ExecuteCmdL( *cmd, snapId );
        BMADAPTERLOGSTRING2( "\tCmd executed with status: %d ", 
                              cmd->Status() )
        // if executed get status
        if ( cmd->Executed() ) 
            {
            Callback().SetStatusL( aStatusRef, cmd->Status() );
            // if successful get results
            if ( cmd->Status() == CSmlDmAdapter::EOk )
                {
                BMADAPTERLOGSTRING2( "\tCmd executed with result: %S ", 
                                      cmd->Data() )
                CBufBase* result = CBufFlat::NewL( cmd->Data()->Size() );
                CleanupStack::PushL( result );
                result->InsertL( 0, *cmd->Data() );
                Callback().SetResultsL( aResultsRef, *result, KNullDesC8 );
                CleanupStack::PopAndDestroy( result );
                }
            }
        else
            {
            // failed to execute command
            Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
            }
        
        CleanupStack::PopAndDestroy( cmd );
        }
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::FetchLeafObjectSizeL: End" )
    }

// ------------------------------------------------------------------------
//  CNSmlDmBmAdapter::ChildURIListL
//  Fetches child nodes of a node. these may be either all snap nodes
//  or leaf nodes under a snap node. 
// ------------------------------------------------------------------------
void CNSmlDmBmAdapter::ChildURIListL( const TDesC8& aURI, 
                                      const TDesC8& aLUID,
                                      const CArrayFix<TSmlDmMappingInfo>& 
                                      aPreviousURISegmentList,
                                      TInt aResultsRef, 
                                      TInt aStatusRef )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::ChildURIListL: Start" )
    BMADAPTERLOGSTRING3( "\tChildURIListL  URI: %S, LUID: %S ", 
                        &aURI, &aLUID )
                        
    TInt snapId = MapPredefinedSnapsL( aURI, aLUID );
    
    CBufBase* resultList = CBufFlat::NewL( KSmlMaxURISegLen );
    CleanupStack::PushL( resultList );
    // get all snap nodes
    if ( !aURI.Compare(KNSmlDmBMAdapterBM) )
        {
        RArray<TUint32> snaps = RArray<TUint32>( KNSmlDmBMGranularity );
        CleanupClosePushL( snaps );
        iSettingStore->GetSnapsL( snaps );
        TInt currentSNAP( -1 );
        for ( TInt i( 0 ); i < snaps.Count(); i++)
            {
            currentSNAP = snaps[i];
            BMADAPTERLOGSTRING2( "\tSnap id: %d, ", currentSNAP )

            // check that not previously mapped
            TBool found = EFalse;
            
            for ( TInt j( 0 ); j < aPreviousURISegmentList.Count(); j++ )
                {
                TSmlDmMappingInfo mapping = aPreviousURISegmentList.At( j );
                // snap already has mapping
                if ( currentSNAP == DesToIntL( mapping.iURISegLUID ) )
                    {
                    BMADAPTERLOGSTRING2( " URI: %S exists ", &mapping.iURISeg )
                    found = ETrue;
                    // add to result list
                    resultList->InsertL( resultList->Size(), 
                                         mapping.iURISeg);
                    break;
                    }
                }
            if ( !found )
                {
                // This is new snap -> create URI and mapping and add 
                // node name to result
                HBufC8* uri = HBufC8::NewLC( aURI.Length() + 
                                             KNSmlDmBmSeparator().Length() +
                                             KSmlMaxURISegLen );
                    
                TPtr8 uriPtr = uri->Des(); 
                uriPtr.Format( aURI );
                uriPtr.Append( KNSmlDmBmSeparator );
                uriPtr.Append( KNSmlDmBMSNAPNamePrefix ); 
                HBufC8* luidBuf = IntToDes8L( currentSNAP );
                CleanupStack::PushL( luidBuf );
                uriPtr.Append( *luidBuf );

                Callback().SetMappingL( uriPtr, *luidBuf );
                resultList->InsertL( resultList->Size(), 
                                     LastURISeg( uriPtr ) );
                BMADAPTERLOGSTRING2( "Creating mapping for URI %S ", &uriPtr ) 
                CleanupStack::PopAndDestroy( luidBuf );
                CleanupStack::PopAndDestroy( uri );
                }
            // insert separator if not last    
            if ( i < snaps.Count() - 1 )
                {
                resultList->InsertL( resultList->Size(), 
                                     KNSmlDmBmSeparator );
                }
            }
        CleanupStack::PopAndDestroy( &snaps );
        Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        Callback().SetResultsL( aResultsRef, *resultList, KNullDesC8 );
        }
    // get child nodes of a snap node
    else if ( NumOfURISegs( aURI ) == KNsmlDmBmSNAPNodeDepth )
        {        
        CSmlDmBmSettingsElement* snap = GetSnapIfBuffered( aURI );
        // if buffered, add this command to buffer
        if( snap )
            {
            CSmlDmBmCommandElement* cmd = 
            CSmlDmBmCommandElement::NewLC( EFalse, 
                                           aStatusRef,
                                           aResultsRef, 
                                           CNSmlDmBmAdapter::EGetCmd, 
                                           KNullDesC8, 
                                           KNullDesC8 );
            snap->NodeBuf().AppendL( cmd );
            CleanupStack::Pop( cmd );
            BMADAPTERLOGSTRING2( "\tSnap %S found, buffering cmd", 
                                 snap->MappingName() )
            }
        // existing snap must have luid mapped
        else if( aLUID.Length() > 0 || snapId != KErrNotFound )
            {
            CSmlDmBmCommandElement* cmd = 
            CSmlDmBmCommandElement::NewLC( EFalse, 
                                           aStatusRef,
                                           aResultsRef, 
                                           CNSmlDmBmAdapter::EGetCmd, 
                                           KNullDesC8, 
                                           KNullDesC8 );
            if( snapId == KErrNotFound )
                {
                snapId = DesToIntL( aLUID );
                }
            
            iSettingStore->ExecuteCmdL( *cmd, snapId );
            BMADAPTERLOGSTRING2( "\tCmd executed with status: %d ", 
                                  cmd->Status() )
            Callback().SetStatusL( aStatusRef, cmd->Status() );
            if( cmd->Status() == CSmlDmAdapter::EOk )
                {
                resultList->InsertL( 0, *cmd->Data() );
                Callback().SetResultsL( aResultsRef, 
                                        *resultList, 
                                        KNullDesC8 );
                } 
            CleanupStack::PopAndDestroy( cmd );
            }
        // invalid snap object
        else
            {
            BMADAPTERLOGSTRING( "\tCNSmlDmBmAdapter::ChildURIListL: Error, \
            invalid snap node" )
            Callback().SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
            }           
        }
    else
        {
        BMADAPTERLOGSTRING( "\tChildURIListL: Invalid URI" )
        }
    CleanupStack::PopAndDestroy( resultList );
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::ChildURIListL: End" )
    }
 
// --------------------------------------------------------------------------
//  CNSmlDmBmAdapter::AddNodeObjectL
//  Adds a snap node. The node is buffered until all data is received.
// --------------------------------------------------------------------------
void CNSmlDmBmAdapter::AddNodeObjectL( const TDesC8& aURI, 
                                       const TDesC8& aParentLUID,
                                       TInt aStatusRef )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::AddNodeObjectL: Start" )
    BMADAPTERLOGSTRING3( "\tAddNodeObjectL  \tURI: %S, \tparent LUID: %S,", 
                        &aURI, &aParentLUID )
    if ( NumOfURISegs( aURI ) == KNsmlDmBmSNAPNodeDepth )
        {
        // if same node already exists command fails
        CSmlDmBmSettingsElement* setting = GetSnapIfBuffered( aURI );
        if( setting || aParentLUID.Compare( KNullDesC8) )
            {
            BMADAPTERLOGSTRING( "\t Snap exists" )
            Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
            }
        else if ( LastURISeg( aURI ).Length() > KSmlMaxURISegLen )
            {
            BMADAPTERLOGSTRING( "\t Node name too long" )
            Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
            }
        else
            {
            // Store command in buffer for later processing
            setting = CSmlDmBmSettingsElement::NewLC( aURI );
            iBuffer.AppendL( setting );
            CleanupStack::Pop( setting );
        
            // store the add node command
            CSmlDmBmCommandElement* command = 
            CSmlDmBmCommandElement::NewLC( EFalse, 
                                           aStatusRef,
                                           KNSmlDmBmInvalidRef, 
                                           CNSmlDmBmAdapter::EAddCmd, 
                                           KNullDesC8, 
                                           KNullDesC8 );
            TInt index( iBuffer.Count() - 1 );
            iBuffer[index]->NodeBuf().AppendL( command ); 
            CleanupStack::Pop( command );
            BMADAPTERLOGSTRING( "\t Cmd buffered" )
            }
        }
      
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::AddNodeObjectL: End" )
    }

// -------------------------------------------------------------------------
//  CNSmlDmBmAdapter::UpdateLeafObjectL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::UpdateLeafObjectL( const TDesC8& /* aURI */, 
                                          const TDesC8& /* aLUID */, 
                                          RWriteStream*& /* aStream */, 
                                          const TDesC8& /* aType */, 
                                          TInt /* aStatusRef */ )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::UpdateLeafObjectL: Start" )
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::UpdateLeafObjectL: End" )
    }



// -------------------------------------------------------------------------
//  CNSmlDmBmAdapter::ExecuteCommandL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::ExecuteCommandL( const TDesC8& /* aURI */, 
                                        const TDesC8& /* aLUID */, 
                                        const TDesC8& /* aArgument */, 
                                        const TDesC8& /* aType */, 
                                        TInt aStatusRef )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::ExecuteCommandL: Start" )
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::ExecuteCommandL: End" )
    }
// -------------------------------------------------------------------------
//  CNSmlDmBmAdapter::ExecuteCommandL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::ExecuteCommandL( const TDesC8& /* aURI */, 
                                        const TDesC8& /* aLUID */, 
                                        RWriteStream*& /* aStream */, 
                                        const TDesC8& /* aType */, 
                                        TInt aStatusRef )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::ExecuteCommandL: Start" )
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::ExecuteCommandL: End" )
    }
 
// -------------------------------------------------------------------------
//  CNSmlDmBmAdapter::CopyCommandL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::CopyCommandL( const TDesC8& /* aTargetURI */ , 
                                     const TDesC8& /* aTargetLUID */ , 
                                     const TDesC8& /* aSourceURI */ , 
                                     const TDesC8& /* aSourceLUID */, 
                                     const TDesC8& /* aType */ , 
                                     TInt aStatusRef )
    {
    //not supported
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::CopyCommandL: Sart" )
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::CopyCommandL: End" )
    }
// -------------------------------------------------------------------------
//  CNSmlDmBmAdapter::StartAtomicL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::StartAtomicL()
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::StartAtomicL: Start" )
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::StartAtomicL: End" )
    }
// -------------------------------------------------------------------------
//  CNSmlDmBmAdapter::CommitAtomicL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::CommitAtomicL()
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::CommitAtomicL: Start" )
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::CommitAtomicL: End" )
    }

// -------------------------------------------------------------------------
//  CNSmlDmBmAdapter::RollbackAtomicL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::RollbackAtomicL()
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::RollbackAtomicL: Start" )
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::RollbackAtomicL: End" )
    }

TBool CNSmlDmBmAdapter::StreamingSupport( TInt& /* aItemSize */ )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::StreamingSupport: Start" )
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::StreamingSupport: End" )
    return EFalse;
    }
// -------------------------------------------------------------------------
//  CNSmlDmBmAdapter::StreamCommittedL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::StreamCommittedL()
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::StreamCommittedL: Start" )
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::StreamCommittedL: End" )
    }

// -------------------------------------------------------------------------
// CNSmlDmBmAdapter::CompleteOutstandingCmdsL
//  CNSmlDmBmAdapter::CompleteOutstandingCmdsL
// -------------------------------------------------------------------------
void CNSmlDmBmAdapter::CompleteOutstandingCmdsL()
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::CompleteOutstandingCmdsL: Start" )
    CSmlDmBmSettingsElement* settings( NULL );
    CSmlDmBmCommandElement* cmd( NULL );
    TInt err(0);
    // Read through the buffer and execute commands
    for ( TInt i(0); i < iBuffer.Count(); )
        {
        settings = iBuffer[i];
        if( settings->NodeBuf().Count() )
            {
            // the setting may be executed already if it has been an embedded
            // snap for another snap
            if ( !settings->Executed() ) 
                {
                //execute commands for snap
                TRAP( err, iSettingStore->StoreSnapL( *settings )); 
                } 
            // if can't recover from error, don't continue processing 
            // commands
            if( err == KErrNoMemory || 
                err == KErrDiskFull )
                {
                BMADAPTERLOGSTRING2( "\tCNSmlDmBmAdapter::\
                CompleteOutstandingCmdsL: Error %d", err )
                User::Leave( err );
                }
            // if this is a new snap and created successfully, map luid
            if ( !settings->NodeBuf()[0]->Leaf() &&
                 settings->NodeBuf()[0]->CmdType() == CNSmlDmBmAdapter::EAddCmd && 
                 settings->NodeBuf()[0]->Executed() &&
                 settings->NodeBuf()[0]->Status() == CSmlDmAdapter::EOk )
                {
                HBufC8* luid = IntToDes8L( settings->Luid() );
                CleanupStack::PushL( luid );
                Callback().SetMappingL( *settings->MappingName(), *luid );
                BMADAPTERLOGSTRING3( "\t Setting mapping %S - %S ",
                settings->MappingName(), luid )
                CleanupStack::PopAndDestroy( luid );
                }
            // set status for all commands executed and remove the command
            for ( TInt j( 0 ); j < settings->NodeBuf().Count(); )
                {
                
                cmd = settings->NodeBuf()[j];
                 
                // return status values if executed
                if ( cmd->Executed() )
                    {
                    
                    Callback().SetStatusL( cmd->StatusRef(),
                                  cmd->Status() );
                    // for certain errors, don't continue command execution
                    if( cmd->Status() == CSmlDmAdapter::ENoMemory )
                        {
                        // commands are deleted     
                        iBuffer.ResetAndDestroy();
                        BMADAPTERLOGSTRING( "\tCmd failed with CSmlDmAdapter::ENoMemory" )
                        User::Leave( KErrNoMemory );
                        }
                   else if( cmd->Status() == CSmlDmAdapter::EDiskFull )
                        {
                        // commands are deleted   
                        iBuffer.ResetAndDestroy();
                        BMADAPTERLOGSTRING( "\tCmd failed with CSmlDmAdapter::EDiskFull" )
                        User::Leave( KErrDiskFull );
                        }
                   else
                        {
                        BMADAPTERLOGSTRING2( 
                            "\tCmd failed with CSmlDmAdapter::%d", 
                            cmd->Status() )
                        }
                    // for get commands, also return the results
                   if ( cmd->CmdType() == CNSmlDmBmAdapter::EGetCmd || 
                        cmd->CmdType() == CNSmlDmBmAdapter::EGetSizeCmd )
                       {
                       CBufBase* result = CBufFlat::NewL( cmd->Data()->Size() );
                       CleanupStack::PushL( result );
                       result->InsertL( 0, *cmd->Data() ); 
                       Callback().SetResultsL( cmd->ResultRef(), 
                                               *result, 
                                               KNullDesC8 );
                       CleanupStack::PopAndDestroy( result );
                       }
                   }
               else
                   {
                   Callback().SetStatusL( cmd->StatusRef(),
                                 CSmlDmAdapter::EError );
                   }
               settings->NodeBuf().Remove( j );
               delete cmd;
               }
            }
            
        // delete settings element and remove it from buffer 
        delete settings;
        iBuffer.Remove( i );
        }
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::CompleteOutstandingCmdsL: End" )
    }



// -------------------------------------------------------------------------
// CNSmlDmBmAdapter::StoreSnapIfBufferedL
// If argument snap is found in the buffer iSettingStore->StoreSNAPL is 
// called 
// for it and LUID is mapped. SNAP is set as executed.
// This is called when checking if snap has embedded snap reference to 
// another 
// snap which is stored further in the command buffer. 
// All embedded snap commands of this type are buffered and execution only 
// happens with CompleteOutstandingCmdsL call. 
// -------------------------------------------------------------------------
    
void CNSmlDmBmAdapter::StoreSnapIfBufferedL( const TDesC8& aURI )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::StoreSnapIfBufferedL: Start" )
    CSmlDmBmSettingsElement* snap = GetSnapIfBuffered( aURI );
    // if snap is in buffer and is a new snap
    if( snap && !snap->Luid() )
        {
        iSettingStore->StoreSnapL( *snap );
        // set mapping if succesfully executed
        if ( snap->NodeBuf()[0]->Executed() &&
             snap->NodeBuf()[0]->Status() == CSmlDmAdapter::EOk )
            {
            HBufC8* luid = IntToDes8L( snap->Luid() );
            CleanupStack::PushL( luid );
            Callback().SetMappingL( *snap->MappingName(), *luid);
            CleanupStack::PopAndDestroy( luid );
            }
        }
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::StoreSnapIfBufferedL: End" )
    }
// -------------------------------------------------------------------------
// CNSmlDmBmAdapter::GetSnapUriFromLuidL
// Returns heap desciptor holding snap node URI matching the argument LUID
// -------------------------------------------------------------------------
HBufC8* CNSmlDmBmAdapter::GetSnapUriFromLuidL( const TUint aSnapId )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::GetSnapURIFromLUIDL: Start" )
    HBufC8* uri( NULL );
    MSmlDmAdapter::TError err( MSmlDmAdapter::EOk );
    CBufBase* fetchRes = CBufFlat::NewL( KSmlMaxURISegLen );
    CleanupStack::PushL( fetchRes );
    // fetch all snap nodes
    Callback().FetchLinkL( KNSmlDmBMAdapterBMURI, *fetchRes, err );

    TPtr8 snapNodes = fetchRes->Ptr( 0 );

    TInt numSnaps = NumOfURISegs( snapNodes );
        
    // get uri-luid mappings for snap nodes
    for ( TInt i( 0 ); i < numSnaps; i++ )
        {
        TPtrC8 node = LastURISeg( snapNodes );

        HBufC8* uriSeg = HBufC8::NewLC( KNSmlDmBmAdapterURIMaxLength + 
                                        node.Length());
        uriSeg->Des().Append( KNSmlDmBMAdapterBM() );
        uriSeg->Des().Append( KNSmlDmBmSeparator() );
        uriSeg->Des().Append( node ); 

        HBufC8* luid = Callback().GetLuidAllocL( *uriSeg );
        CleanupStack::PopAndDestroy( uriSeg );
        CleanupStack::PushL( luid );
        // node may not have luid mapped if it's just created
        if( luid->Size() )
            {
            TInt id = DesToIntL( *luid );
            CleanupStack::PopAndDestroy( luid ); 
            if ( aSnapId ==  id ) 
                {
                uri = HBufC8::NewLC( KNSmlDmBMAdapterBMURI().Size() + 
                               KNSmlDmBmSeparator().Size() + 
                               node.Size() );

                uri->Des().Append( KNSmlDmBMAdapterBMURI() );
                uri->Des().Append( KNSmlDmBmSeparator() );
                uri->Des().Append( node );
                break;
                }
            }
        else
            {
            CleanupStack::PopAndDestroy( luid );
            }
        snapNodes = RemoveLastURISeg( snapNodes );
        }
          
    if( uri )
        {
         CleanupStack::Pop( uri );
        }

    CleanupStack::PopAndDestroy( fetchRes );

    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::IdArrayToIAPList: End" )
    return uri;
    }

// -------------------------------------------------------------------------
// CNSmlDmBmAdapter::GetIapListL
// Returns heap desciptor holding a comma separated list of AP node URIs
// matching the luids in the argument array.
// -------------------------------------------------------------------------
HBufC8* CNSmlDmBmAdapter::GetIapListL( const RArray<TUint>& aIdArray )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::GetIapListL: Start" )
    HBufC8* iapList = HBufC8::NewLC( aIdArray.Count() * 
                                    ( KNSmlDmBmAdapterAPURIMaxLength + 
                                      KSmlMaxURISegLen ) );
    MSmlDmAdapter::TError err;
    CBufBase* fetchRes = CBufFlat::NewL( KSmlMaxURISegLen );
    CleanupStack::PushL( fetchRes );
    // fetch dynamic AP nodes, so that LUID mapping is created
    Callback().FetchLinkL( KNSmlDmBMAdapterIAPURI, *fetchRes, err );

    // list of all nodes has a trailing '/' unlike regular uris, remove that
    // for further handling 
    TPtr8 iapNodes = fetchRes->Ptr( 0 );
    if ( iapNodes.Length() > 0 && iapNodes[iapNodes.Length() - 1] == '/')
        {
        iapNodes = iapNodes.Left( iapNodes.Length() - 1 );
        }

    TInt numAPs = NumOfURISegs( iapNodes );
    
    CArrayFixFlat<TSmlDmMappingInfo>* mappings = 
        new (ELeave) CArrayFixFlat<TSmlDmMappingInfo>( numAPs * 
                                                       sizeof( TSmlDmMappingInfo ) );
    CleanupStack::PushL( mappings );
    
   // get uri-luid mappings for all ap nodes
    HBufC8* luid = NULL;
    for ( TInt i( 0 ); i < numAPs; i++ )
        {
        TSmlDmMappingInfo m;
        m.iURISeg = LastURISeg( iapNodes );
        
        HBufC8* uriSeg = HBufC8::NewLC( KNSmlDmBmAdapterAPURIMaxLength + 
                                        m.iURISeg.Length() );
        uriSeg->Des().Replace( 0, 
                               uriSeg->Length(), 
                               KNSmlDmBMAdapterIAPNode() );
        uriSeg->Des().Append( KNSmlDmBmSeparator() );
        uriSeg->Des().Append( m.iURISeg ); 

        luid = Callback().GetLuidAllocL( *uriSeg );
        CleanupStack::PopAndDestroy( uriSeg );
        CleanupStack::PushL( luid );
        m.iURISegLUID.Set( *luid );
        mappings->AppendL( m ); 
        iapNodes = RemoveLastURISeg( iapNodes );
        }
        
    // find out URIs matching argument IAP ids
    for (TInt i(0); i < aIdArray.Count(); i++)
        {
        TBool found = EFalse;

        for ( TInt j( 0 ); j < mappings->Count(); j++ )
            {
            if ( aIdArray[i] == DesToIntL( mappings->At( j ).iURISegLUID ) )
                {
                // add to result
                iapList->Des().Append( KNSmlDmBMAdapterIAPURI() );
                iapList->Des().Append( KNSmlDmBmSeparator() );
                iapList->Des().Append( mappings->At(j).iURISeg );
                // not last -> add separator
                if ( i < aIdArray.Count() - 1 )
                    {
                    iapList->Des().Append( KNSmlDmBmUriListSeparator );
                    }
                found = ETrue;
                break;
                }
            }
        if ( ! found )
            {
            BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::GetIapListL: \
            Error IAP matching id not found" )
            User::Leave( KErrNotFound );
            }
        }
  
    // cleanup each allocated luid
    CleanupStack::PopAndDestroy( numAPs ); 
    CleanupStack::PopAndDestroy( mappings );
    CleanupStack::PopAndDestroy( fetchRes );
    CleanupStack::Pop( iapList );
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::GetIapListL: End" )
    return iapList;
    }
    
// -------------------------------------------------------------------------
// CNSmlDmBmAdapter::GetIdArrayL
// Returns array holding LUIDs matching the AP node in the argument list.
// -------------------------------------------------------------------------
RArray<TUint> CNSmlDmBmAdapter::GetIdArrayL( const TDesC8& aIAPList )
    {
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::GetIdArrayL: Start" )
    TPtrC8 uri;
    RArray<TUint> iaps;
    CleanupClosePushL( iaps );
    uri.Set( aIAPList );
    // fetch AP nodes so that mappings are created
    MSmlDmAdapter::TError err;
    CBufBase* fetchRes = CBufFlat::NewL( KSmlMaxURISegLen );
    CleanupStack::PushL( fetchRes );
    // fetch dynamic AP nodes, so that LUID mappings are created
    Callback().FetchLinkL( KNSmlDmBMAdapterIAPURI, *fetchRes, err );
    
    for ( TInt i(0), n = NumOfURIs( aIAPList ); i < n; i++ )
        {
        TPtrC8 thisURI = FirstURI( uri );
        // remove ./ from beginning
        TPtrC8 apUri = thisURI.Mid( KNSmlDmBmAdapterURIPrefix().Length() );
        HBufC8* luid = Callback().GetLuidAllocL( apUri );
        CleanupStack::PushL( luid );
        TInt id = DesToIntL( *luid );
        CleanupStack::PopAndDestroy( luid );
        iaps.Append( id );
        uri.Set( RemoveFirstURI( uri ) );
        }
    
    CleanupStack::PopAndDestroy( fetchRes );
    CleanupStack::Pop( &iaps );
    BMADAPTERLOGSTRING( "CNSmlDmBmAdapter::GetIdArrayL: End" )
    return iaps;
    }

// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::IntToDes8L
// Converts integer to 8bit descriptor
// --------------------------------------------------------------------------
HBufC8* CNSmlDmBmAdapter::IntToDes8L( TInt aLuid ) const
    {
    HBufC8* buf = HBufC8::NewL( KMaxLengthOf32bitInteger ); 
    TPtr8 ptrBuf = buf->Des();

    // Unless aLuid is casted as TInt64, PC-Lint reports: 
    // Significant prototype coercion
    // 
    ptrBuf.Num( TInt64( aLuid ) );
    return buf;
    }

// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::DesToIntL
// Converts 8bit descriptor to int
// --------------------------------------------------------------------------
TUint CNSmlDmBmAdapter::DesToIntL( const TDesC8& aLuid ) const
    {
    __UHEAP_MARK;
    TLex8 lex( aLuid );
    TUint value( 0 );
    User::LeaveIfError( lex.Val( value ) );
    __UHEAP_MARKEND;
    return value;
    }
    
// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::DesToBool
// Converts 8bit descriptor to a boolean value
// --------------------------------------------------------------------------
TBool CNSmlDmBmAdapter::DesToBool(const TDesC8& aBoolDes, TBool& aBool ) const
    {
    __UHEAP_MARK;
    if ( !aBoolDes.Compare( KNSmlDmBMBooleanTrue ) )
        {
        aBool = ETrue;
        }
    else if ( !aBoolDes.Compare( KNSmlDmBMBooleanFalse ) )
        {
        aBool = EFalse;
        }
    else
        {
        __UHEAP_MARKEND;
        return EFalse;
        }
    __UHEAP_MARKEND;
    return ETrue;
    }

// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::BoolToDes8LC
// Converts boolean value to 8bit descriptor
// --------------------------------------------------------------------------

HBufC8* CNSmlDmBmAdapter::BoolToDes8LC( TBool aBool) const
    {
    HBufC8* ret;
    if( aBool )
        {
        ret = HBufC8::NewLC( KNSmlDmBMBooleanTrue().Size() );
        ret->Des().Append( KNSmlDmBMBooleanTrue() ); 
        }
    else
        {
        ret = HBufC8::NewLC( KNSmlDmBMBooleanFalse().Size() );
        ret->Des().Append( KNSmlDmBMBooleanFalse() ); 
        }
    return ret;
    }


// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::GetLuidAllocL
// The function calls the GetLuidAllocL function of the MSmlDmCallback to get
// the LUID which is mapped to aURI.
// --------------------------------------------------------------------------
HBufC8* CNSmlDmBmAdapter::GetLuidAllocL( const TDesC8& aURI )
    {
    return Callback().GetLuidAllocL( aURI );
    }

   
// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::GetSnapIfBuffered
// Returns a pointer to the snap CSmlDmBmSettingsElement object if found in 
// the buffer 
// --------------------------------------------------------------------------
CSmlDmBmSettingsElement* CNSmlDmBmAdapter::GetSnapIfBuffered( const TDesC8& 
                                                              aMappingName )
    {
    CSmlDmBmSettingsElement* settings = NULL;
    for ( TInt i( 0 ); i<iBuffer.Count(); i++ )
        {
        if ( !iBuffer[i]->MappingName()->Compare( aMappingName ) )
            {
            settings = iBuffer[i];
            break;
            }
        }
    return settings;
    }
// -------------------------------------------------------------------------
// CNSmlDmBmAdapter::FirstURI
// Returns the first URI from a list of URIs
// -------------------------------------------------------------------------
TPtrC8 CNSmlDmBmAdapter::FirstURI( const TDesC8& aURIList ) const
    {
    __UHEAP_MARK;
    TInt i;
    for ( i = 0; i < aURIList.Length(); i++ )
        {
        if ( aURIList[i] == ',' )
            {
            break;
            }
        }
    if ( i == aURIList.Length() - 1 )
        {
        __UHEAP_MARKEND;
        return aURIList;
        }
    else
        {
        __UHEAP_MARKEND;
        return aURIList.Mid( 0, i );
        }
    }
    
// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::NumOfURIs
// Returns the num of uris
// --------------------------------------------------------------------------
TInt CNSmlDmBmAdapter::NumOfURIs( const TDesC8& aURIList ) const
    {
    __UHEAP_MARK;
    if ( !aURIList.Length() )
        {
        return 0;
        }
    TInt numOfURIs( 1 );
    for ( TInt i(0); i < aURIList.Length(); i++ )
        {
        if ( aURIList[i] == ',' )
            {
            numOfURIs++;
            }
        }
    __UHEAP_MARKEND;
    return numOfURIs;
    }
    
// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::RemoveFirstURI
// extracts first uri an returns the remaining part of the uri list 
// --------------------------------------------------------------------------
TPtrC8 CNSmlDmBmAdapter::RemoveFirstURI( const TDesC8& aURIList ) const
    {
    __UHEAP_MARK;
    TInt i;
    for ( i = 0; i < aURIList.Length(); i++ )
        {
        if ( aURIList[i] == ',' )
            {
            break;
            }
        }
    if ( i <  aURIList.Length() ) 
        {
        __UHEAP_MARKEND;
        return aURIList.Mid( i + 1 );
        }
    else
        {
        __UHEAP_MARKEND;
        return aURIList.Mid( i ); 
        }
        
    }
// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::LastURISeg
// Returns the last uri segemnt of a uri. 
// --------------------------------------------------------------------------
TPtrC8 CNSmlDmBmAdapter::LastURISeg( const TDesC8& aURI ) const
    {
    __UHEAP_MARK;
    TInt i;
    for ( i=aURI.Length()-1; i >= 0; i-- )
        {
        if ( aURI[i]=='/' )
            {
            break;
            }
        }
    if ( i==0 )
        {
        __UHEAP_MARKEND;
        return aURI;
        }
    else
        {
        __UHEAP_MARKEND;
        return aURI.Mid( i + 1 );
        }
    }

// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::RemoveLastURISeg
// returns parent uri, i.e. removes last uri segment
// --------------------------------------------------------------------------
TPtrC8 CNSmlDmBmAdapter::RemoveLastURISeg( const TDesC8& aURI ) const
    {
    __UHEAP_MARK;
    TInt i;
    for ( i = aURI.Length()-1; i >= 0; i-- )
        {
        if ( aURI[i]=='/' )
            {
            break;
            }
        }
    if ( i > -1 )
        {
        __UHEAP_MARKEND;
        return aURI.Left( i );
        }
    else
        {
        __UHEAP_MARKEND;
        return aURI;
        }
    
    }



// --------------------------------------------------------------------------
// CNSmlDmBmAdapter::NumOfURISegs
// Returns the num of uri segs
// --------------------------------------------------------------------------
TInt CNSmlDmBmAdapter::NumOfURISegs( const TDesC8& aURI ) const
    {
    __UHEAP_MARK;
    TInt numOfURISegs( 1 );
    for ( TInt i(0); i < aURI.Length(); i++ )
        {
        if ( aURI[i]=='/' )
            {
            numOfURISegs++;
            }
        }
    __UHEAP_MARKEND;
    return numOfURISegs;
    }

TInt CNSmlDmBmAdapter::MapPredefinedSnapsL( const TDesC8& aURI, const TDesC8& aLUID )
{
    TInt snapId( KErrNotFound ); 
    // If this is an add to pre-defined SNAPs then LUID we get is empty, but we still need
    // to handle it.
    if( aLUID.Length() == 0 && ( ( snapId = GetPredefinedSnapIdFromUri( aURI ) ) != KErrNotFound ) )
        {               
        // Check if SNAP with given ID exists (from CmManager)
        iCmManagerExt.OpenL();
        TRAPD( err, RCmDestinationExt dest = iCmManagerExt.DestinationL( snapId ) );
        iCmManagerExt.Close();
        
        // If exists -> Call SetMapping to set LUID.
        if( err == KErrNone )
            {
            HBufC8* uri = HBufC8::NewLC( KNSmlDmBMAdapterBM().Length() + 
                                         KNSmlDmBmSeparator().Length() +
                                         KSmlMaxURISegLen );
                
            TPtr8 uriPtr = uri->Des(); 
            uriPtr.Format( KNSmlDmBMAdapterBM );
            uriPtr.Append( KNSmlDmBmSeparator );
            uriPtr.Append( KNSmlDmBMSNAPNamePrefix ); 
            HBufC8* luidBuf = IntToDes8L( snapId );
            CleanupStack::PushL( luidBuf );
            uriPtr.Append( *luidBuf );
    
            Callback().SetMappingL( uriPtr, *luidBuf );
            
            BMADAPTERLOGSTRING2( "Creating mapping for URI %S ", &uriPtr ) 
            CleanupStack::PopAndDestroy( luidBuf );
            CleanupStack::PopAndDestroy( uri );
            }                                          
        }                       
    return snapId;
}

TInt CNSmlDmBmAdapter::GetPredefinedSnapIdFromUri( const TDesC8& aURI ) const
    {    
    TInt ret = KErrNotFound;
    
    // First check that URI starts with BearerManagementSNAP
    TInt startIndex = aURI.Find( KNSmlDmBMAdapterBM );
    
    if( startIndex == KErrNotFound )
        {
        return KErrNotFound;
        }
    
    // Then assume there is separator
    startIndex += KNSmlDmBMAdapterBM().Length() + KNSmlDmBmSeparator().Length();
    
    if( startIndex >= aURI.Length() )
        {
        // No separator. This was an access to ./BearerManagementSNAP
        return KErrNotFound;
        }
    
    TPtrC8 predefinedUriStart = aURI.Mid( startIndex  );
    
    startIndex = predefinedUriStart.Find( KNSmlDmBMSNAPNamePrefix );
    
    // Result should be zero ( = start of string)
    if( startIndex != 0 )
        {
        return KErrNotFound;
        }
    
    startIndex += KNSmlDmBMSNAPNamePrefix().Length();     
    
    TInt endIndex = 0;
    
    endIndex = predefinedUriStart.Find( KNSmlDmBmSeparator );
    
    
    
    if ( endIndex == KErrNotFound )
        {
        TPtrC8 snapId = predefinedUriStart.Mid( startIndex );
        TRAPD( err, ret = DesToIntL( snapId ) );
        if( err != KErrNone )
            {
            ret = KErrNotFound;
            }       
        }
    else
        {
        TPtrC8 snapId = predefinedUriStart.Mid( startIndex, endIndex - startIndex );
        TRAPD( err, ret = DesToIntL( snapId ) );
        if( err != KErrNone )
            {
            ret = KErrNotFound;
            }       
        }    
    return ret;    
    }



//---------------------- CSmlDmBmCommandElement -------------------------//

// --------------------------------------------------------------------------
// CSmlDmBmCommandElement* CSmlDmBmCommandElement::NewLC
// --------------------------------------------------------------------------

CSmlDmBmCommandElement* CSmlDmBmCommandElement::NewLC( TBool aLeaf,
                                                       TInt aStatusRef, 
                                                       TInt aResultRef, 
                                                       CNSmlDmBmAdapter::TCommandType aCmdType,
                                                       const TDesC8& aLastUriSeg, 
                                                       const TDesC8& aData )
    {
    CSmlDmBmCommandElement* self = new (ELeave) CSmlDmBmCommandElement( aLeaf, 
                                                                        aStatusRef, 
                                                                        aResultRef, 
                                                                        aCmdType );
    CleanupStack::PushL(self);
    self->ConstructL( aLastUriSeg, aData );
    return self;
    }
    
// --------------------------------------------------------------------------
// CSmlDmBmCommandElement::~CSmlDmBmCommandElement
// --------------------------------------------------------------------------
CSmlDmBmCommandElement::~CSmlDmBmCommandElement()
    {
    delete iLastUriSeg;
    delete iData;
    }
    
// --------------------------------------------------------------------------
// CSmlDmBmCommandElement::CSmlDmBmCommandElement
// --------------------------------------------------------------------------
CSmlDmBmCommandElement::CSmlDmBmCommandElement( TBool aLeaf,
                                                TInt aStatusRef, 
                                                TInt aResultRef, 
                                                CNSmlDmBmAdapter::TCommandType  aCmdType )
                                                : iStatus( CSmlDmAdapter::EError ),
                                                iLeaf( aLeaf ),
                                                iStatusRef( aStatusRef ),
                                                iResultRef( aResultRef ),
                                                iCmdType( aCmdType ),
                                                iData( NULL )
                                                
    {
    }

// --------------------------------------------------------------------------
// CSmlDmBmCommandElement::ConstructL
// --------------------------------------------------------------------------
void CSmlDmBmCommandElement::ConstructL( const TDesC8& aLastUriSeg, 
                                         const TDesC8& aData )
    {
    iLastUriSeg = aLastUriSeg.AllocL();
    iData = aData.AllocL();
    }

//---------------------- CSmlDmBmSettingsElement -------------------------//

// --------------------------------------------------------------------------
// CSmlDmBmSettingsElement* CSmlDmBmSettingsElement::NewLC
// --------------------------------------------------------------------------
CSmlDmBmSettingsElement* CSmlDmBmSettingsElement::NewLC( const TDesC8& aMappingName )
    {
    CSmlDmBmSettingsElement* self = new (ELeave) CSmlDmBmSettingsElement();
    CleanupStack::PushL(self);
    self->ConstructL( aMappingName );
    return self;
    }
// --------------------------------------------------------------------------
// CSmlDmBmSettingsElement::~CSmlDmBmSettingsElement
// --------------------------------------------------------------------------
CSmlDmBmSettingsElement::~CSmlDmBmSettingsElement()
    {
    // deallocate all elements
    iNodeBuf.ResetAndDestroy();
    iNodeBuf.Close();
    delete iMappingName;
    }

// --------------------------------------------------------------------------
// CSmlDmBmSettingsElement::CSmlDmBmSettingsElement
// --------------------------------------------------------------------------
CSmlDmBmSettingsElement::CSmlDmBmSettingsElement()
                                                : iExecuted( EFalse ),
                                                  iLuid( 0 ),
                                                  iMappingName( NULL ) 
    {
    
    }
// --------------------------------------------------------------------------
// CSmlDmBmSettingsElement::ConstructL
// --------------------------------------------------------------------------
void CSmlDmBmSettingsElement::ConstructL( const TDesC8& aMappingName )
    {
    iMappingName = aMappingName.AllocL();
    }
