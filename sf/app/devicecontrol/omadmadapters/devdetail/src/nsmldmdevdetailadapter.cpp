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
* Description:    DM DevDetail Adapter Source Code
*
*/




// INCLUDE FILES
#include <implementationproxy.h> // For TImplementationProxy definition

#include "nsmldmimpluids.h"
#include "nsmldmdevdetailadapter.h"
#include "nsmldebug.h"
#include "nsmlphoneinfo.h"
#include "nsmlunicodeconverter.h"
#include "nsmlconstants.h"


// ------------------------------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CNSmlDmDevDetailAdapter* CNSmlDmDevDetailAdapter::NewL(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::NewL(): begin");
    CNSmlDmDevDetailAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop();
    _DBG_FILE("CNSmlDmDevDetailAdapter::NewL(): end");
    return self;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CNSmlDmDevDetailAdapter* CNSmlDmDevDetailAdapter::NewLC(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::NewLC(): begin");
    CNSmlDmDevDetailAdapter* self = new (ELeave) CNSmlDmDevDetailAdapter(aDmCallback);
    CleanupStack::PushL(self);
    self->iDmCallback=aDmCallback;
    _DBG_FILE("CNSmlDmDevDetailAdapter::NewLC(): end");
    return self;
    }

    
// ------------------------------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::CNSmlDmDevDetailAdapter(TAny* aEcomArguments)
// ------------------------------------------------------------------------------------------------
CNSmlDmDevDetailAdapter::CNSmlDmDevDetailAdapter(TAny* aEcomArguments):CSmlDmAdapter(aEcomArguments)
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::CNSmlDmDevDetailAdapter(): begin");
    _DBG_FILE("CNSmlDmDevDetailAdapter::CNSmlDmDevDetailAdapter(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::~CNSmlDmDevDetailAdapter()
// -----------------------------------------------------------------------------
CNSmlDmDevDetailAdapter::~CNSmlDmDevDetailAdapter()
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::~CNSmlDmDevDetailAdapter(): begin");
    _DBG_FILE("CNSmlDmDevDetailAdapter::~CNSmlDmDevDetailAdapter(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::DevDetailValueCrcL()
// -----------------------------------------------------------------------------
TInt CNSmlDmDevDetailAdapter::DevDetailValueCrcL()
    {
    CBufBase* crcBuf = CBufFlat::NewL( 1 );
    CleanupStack::PushL( crcBuf );
    
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailDevTypNodeName() );
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailOEMNodeName() );
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailFwVNodeName() );
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailSwVNodeName() );
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailHwVNodeName() );
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailLrgObjNodeName() );
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailMaxDepthNodeName() );
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailMaxTotLenNodeName() );
    AddObjectToBufferL( *crcBuf, KNSmlDMDevDetailMaxSegLenNodeName() );
    
    TUint16 crc = 0;
    Mem::Crc( crc, crcBuf->Ptr(0).Ptr(), crcBuf->Size() );
    CleanupStack::PopAndDestroy();  // crcBuf
    return crc;
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::AddObjectToBufferL()
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::AddObjectToBufferL( CBufBase& aCrcBuf, 
                                                  const TDesC8& aURI )
    {
    CBufBase* buf = CBufFlat::NewL( 1 );
    CleanupStack::PushL( buf );
    FetchLeafObjectL( aURI, *buf );
    aCrcBuf.InsertL( aCrcBuf.Size(), buf->Ptr(0) );
    _LIT8( KNSmlDmSeparator, ";" );
    aCrcBuf.InsertL( aCrcBuf.Size(), KNSmlDmSeparator );
    CleanupStack::PopAndDestroy(); //buf    
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::FetchLeafObjectL()
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmDevDetailAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                                                 CBufBase& aObject )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::FetchLeafObjectL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;

    TInt ret = aURI.LocateReverse(KNSmlDMDevDetailSeparator()[0]);
    if ( ret == KErrNotFound ) 
        {
        ret = -1;
        }
    TInt len = aURI.Length() - ( ret + 1 );
    TPtrC8 segment = aURI.Right( len );
    if ( segment == KNSmlDMDevDetailDevTypNodeName )
        {
        aObject.InsertL( 0, KNSmlDMDevDetailDevTypValue );
        }
    else
    if ( segment == KNSmlDMDevDetailOEMNodeName )
        {
        CNSmlPhoneInfo* phoneInfo = CNSmlPhoneInfo::NewLC();
        HBufC* manufacturer = HBufC::NewLC( 50 );
        TPtr manufacturerPtr = manufacturer->Des();  
        phoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneManufacturer, manufacturerPtr );
        HBufC8* manufacturerInUTF8 = NULL;
        NSmlUnicodeConverter::HBufC8InUTF8LC( *manufacturer, manufacturerInUTF8 );
        aObject.InsertL( 0, *manufacturerInUTF8 );
        CleanupStack::PopAndDestroy( 3 ); //manufacturerInUTF8, manufacturer, phoneInfo
        }
    else
    if ( segment == KNSmlDMDevDetailSwVNodeName )
        {
        // fetch software version
        GetDevDetailDataL( aObject, ESwVersion );
        }
    else
    if ( segment == KNSmlDMDevDetailHwVNodeName )
        {
        // fetch hardware version
        GetDevDetailDataL( aObject, EHwVersion );
        }
    else
    if ( segment == KNSmlDMDevDetailLrgObjNodeName )
        {
        aObject.InsertL( 0, KNSmlDMDevDetailLrgObjValue );
        }
    else
    if ( segment == KNSmlDMDevDetailMaxDepthNodeName )
        {
        aObject.InsertL( 0, KNSmlDMDevDetailMaxDepthValue );
        }
    else
    if ( segment == KNSmlDMDevDetailMaxTotLenNodeName )
        {
        aObject.InsertL( 0, KNSmlDMDevDetailMaxTotLenValue );
        }
    else
    if ( segment == KNSmlDMDevDetailMaxSegLenNodeName )
        {
        aObject.InsertL( 0, KNSmlDMDevDetailMaxSegLenValue );
        }
    else
    if ( segment != KNSmlDMDevDetailFwVNodeName )
        {
        retValue = CSmlDmAdapter::EError;
        }
        
    _DBG_FILE("CNSmlDmDevDetailAdapter::FetchLeafObjectL(): end");
    return retValue;
    }
// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::SetLeafPropertiesL()
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::SetLeafPropertiesL( MSmlDmDDFObject& aObject, 
                                                  const TSmlDmAccessTypes& aAccessTypes,  
                                                  const TDesC8& aDescription ) const
    {
    aObject.SetAccessTypesL( aAccessTypes );
    aObject.SetScopeL( MSmlDmDDFObject::EPermanent );
    aObject.SetDFFormatL( MSmlDmDDFObject::EChr );
    aObject.AddDFTypeMimeTypeL( KNSmlDMDevDetailTextPlain );
    aObject.SetDescriptionL( aDescription );
    }



// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::DDFVersionL()
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::DDFVersionL(TDes& aDDFVersion): begin");
    aDDFVersion.InsertL(0,KNSmlDMDevDetailDDFVersion);
    _DBG_FILE("CNSmlDmDevDetailAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }


// -----------------------------------------------------------------------------
// void CNSmlDmDevDetailAdapter::DDFStructureL()
//
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::DDFStructureL(): begin");
    
    TSmlDmAccessTypes accessTypes;
    
    accessTypes.SetGet();
    // DevDetail
    MSmlDmDDFObject& devDetail = aDDF.AddChildObjectL( KNSmlDMDevDetailNodeName );
    devDetail.SetAccessTypesL( accessTypes ); 
    devDetail.SetScopeL( MSmlDmDDFObject::EPermanent );
    devDetail.SetDescriptionL( KNSmlDMDevDetailDescription );

    // DevTyp   
    MSmlDmDDFObject& devTyp = devDetail.AddChildObjectL(KNSmlDMDevDetailDevTypNodeName );
    SetLeafPropertiesL( devTyp, accessTypes, KNSmlDMDevDetailDevTypDescription );
    // OEM  
    MSmlDmDDFObject& oem = devDetail.AddChildObjectL(KNSmlDMDevDetailOEMNodeName);
    SetLeafPropertiesL( oem, accessTypes, KNSmlDMDevDetailOEMDescription );
    // FwV  
    MSmlDmDDFObject& fwv = devDetail.AddChildObjectL(KNSmlDMDevDetailFwVNodeName);
    SetLeafPropertiesL( fwv, accessTypes, KNSmlDMDevDetailFwVDescription );
    // SwV  
    MSmlDmDDFObject& swv = devDetail.AddChildObjectL( KNSmlDMDevDetailSwVNodeName );
    SetLeafPropertiesL( swv, accessTypes, KNSmlDMDevDetailSwVDescription );
    // HwV  
    MSmlDmDDFObject& hwv = devDetail.AddChildObjectL(KNSmlDMDevDetailHwVNodeName );
    SetLeafPropertiesL( hwv, accessTypes, KNSmlDMDevDetailHwVDescription );
    // LrgObj   
    MSmlDmDDFObject& lrgObj = devDetail.AddChildObjectL(KNSmlDMDevDetailLrgObjNodeName );
    SetLeafPropertiesL( lrgObj, accessTypes, KNSmlDMDevDetailLrgObjDescription );
    lrgObj.SetDFFormatL( MSmlDmDDFObject::EBool );
    // URI
    MSmlDmDDFObject& uri = devDetail.AddChildObjectL(KNSmlDMDevDetailURINodeName);
    uri.SetAccessTypesL( accessTypes );
    uri.SetScopeL( MSmlDmDDFObject::EPermanent );
    uri.SetDescriptionL( KNSmlDMDevDetailURIDescription );
    // MaxDepth
    MSmlDmDDFObject& maxDepth = uri.AddChildObjectL( KNSmlDMDevDetailMaxDepthNodeName);
    SetLeafPropertiesL( maxDepth, accessTypes, KNSmlDMDevDetailMaxDepthDescription );
    // MaxTotLen
    MSmlDmDDFObject& maxTotLen = uri.AddChildObjectL( KNSmlDMDevDetailMaxTotLenNodeName );
    SetLeafPropertiesL( maxTotLen, accessTypes, KNSmlDMDevDetailMaxTotLenDescription );
    // MaxSegLen
    MSmlDmDDFObject& maxSegLen = uri.AddChildObjectL( KNSmlDMDevDetailMaxSegLenNodeName);
    SetLeafPropertiesL( maxSegLen, accessTypes, KNSmlDMDevDetailMaxSegLenDescription );
        
    _DBG_FILE("CNSmlDmDevDetailAdapter::DDFStructureL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void  CNSmlDmDevDetailAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                                  const TDesC8& /*aLUID*/, 
                                                  const TDesC8& /*aObject*/, 
                                                  const TDesC8& /*aType*/,
                                                  const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::UpdateLeafObjectL(): begin");
    _DBG_FILE("CNSmlDmDevDetailAdapter::UpdateLeafObjectL(): end");
    iDmCallback->SetStatusL(aStatusRef,CSmlDmAdapter::EError);
    }


// -----------------------------------------------------------------------------
//  CNSmlDmDevDetailAdapter::DeleteObjectL()
// -----------------------------------------------------------------------------
void  CNSmlDmDevDetailAdapter::DeleteObjectL( const TDesC8& /*aURI*/, 
                                              const TDesC8& /*aLUID*/,
                                              const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::DeleteLeafObjectL( ): begin");
    _DBG_FILE("CNSmlDmDevDetailAdapter::DeleteLeafObjectL( ): end");
    iDmCallback->SetStatusL(aStatusRef,CSmlDmAdapter::EError);
    }


// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::FetchLeafObjectL()
// -----------------------------------------------------------------------------
void  CNSmlDmDevDetailAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                                 const TDesC8& /*aLUID*/, 
                                                 const TDesC8& aType, 
                                                 const TInt aResultsRef, 
                                                 const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::FetchLeafObjectL(): begin");
    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    CSmlDmAdapter::TError retValue = FetchLeafObjectL( aURI, *object );

    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType);
    CleanupStack::PopAndDestroy(); //object
        
    _DBG_FILE("CNSmlDmDevDetailAdapter::FetchLeafObjectL(): end");
    }


// -----------------------------------------------------------------------------
// CSmlDmAdapter::TError CNSmlDmDevDetailAdapter::ChildURIListL()
// -----------------------------------------------------------------------------

void  CNSmlDmDevDetailAdapter::ChildURIListL( const TDesC8& aURI, 
                                              const TDesC8& /*aLUID*/, 
                                              const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, 
                                              const TInt aResultsRef, 
                                              const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::ChildURIListL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    CBufBase *currentURISegmentList = CBufFlat::NewL(64);
    CleanupStack::PushL(currentURISegmentList);

    TInt ret = aURI.LocateReverse(KNSmlDMDevDetailSeparator()[0]);
    if ( ret == KErrNotFound ) 
        {
        ret = -1;
        }
    TInt len = aURI.Length() - ( ret + 1 );
    TPtrC8 segment = aURI.Right( len );

    if ( segment == KNSmlDMDevDetailNodeName )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailDevTypNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailOEMNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailFwVNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSwVNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSeparator() );
         currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailHwVNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailLrgObjNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSeparator() );
         currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailURINodeName() );
        }
    else
    if ( segment == KNSmlDMDevDetailURINodeName )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailMaxDepthNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailMaxTotLenNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevDetailMaxSegLenNodeName() );
        }
    else
        {
        retValue = CSmlDmAdapter::EError;
        }

    iDmCallback->SetStatusL(aStatusRef,retValue);
    iDmCallback->SetResultsL(aResultsRef,*currentURISegmentList,KNullDesC8);
    CleanupStack::PopAndDestroy(); //currentURISegmentList;
    _DBG_FILE("CNSmlDmDevDetailAdapter::ChildURIListL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::AddNodeObjectL()
// -----------------------------------------------------------------------------
void  CNSmlDmDevDetailAdapter::AddNodeObjectL( const TDesC8& /*aURI*/, 
                                               const TDesC8& /*aParentLUID*/,
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::AddNodeObjectL(): begin");
    _DBG_FILE("CNSmlDmDevDetailAdapter::AddNodeObjectL(): end");
    iDmCallback->SetStatusL(aStatusRef,CSmlDmAdapter::EError);
    }

// -----------------------------------------------------------------------------
//  CNSmlDmDevDetailAdapter::UpdateLeafObjectL(const TDesC8& aURI, 
// const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, 
// const TInt aStatusRef)
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                                 const TDesC8& /*aLUID*/, 
                                                 RWriteStream*& /*aStream*/, 
                                                 const TDesC8& /*aType*/, 
                                                 const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::UpdateLeafObjectL(): stream: begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmDevInfoAdapter::UpdateLeafObjectL(): stream: end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmDevDetailAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
// const TDesC8& aLUID, const TDesC8& aType, const TInt aResultsRef, 
// const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                                    const TDesC8& /*aLUID*/, 
                                                    const TDesC8& aType, 
                                                    const TInt aResultsRef, 
                                                    const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::FetchLeafObjectSizeL(): begin");

    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    CSmlDmAdapter::TError retValue = FetchLeafObjectL( aURI, *object );

    TInt objSizeInBytes = object->Size();
    TBuf8<KNSmlMaxSizeBufferLength> stringObjSizeInBytes;
    stringObjSizeInBytes.Num( objSizeInBytes );
    object->Reset();
    object->InsertL( 0, stringObjSizeInBytes );
    
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType);
    CleanupStack::PopAndDestroy(); //object 

            
    _DBG_FILE("CNSmlDmDevDetailAdapter::FetchLeafObjectSizeL(): end");
    }
    
// -----------------------------------------------------------------------------
//  CNSmlDmDevDetailAdapter::ExecuteCommandL( const TDesC8& aURI, 
//  const TDesC8& aParentLUID, TDesC8& aArgument, const TDesC8& aType, 
//  TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, const TDesC8& /*aType*/, const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::ExecuteCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmDevInfoAdapter::ExecuteCommandL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmDevDetailAdapter::ExecuteCommandL( const TDesC8& aURI, 
//  const TDesC8& aParentLUID, RWriteStream*& aStream, const TDesC8& aType, 
//  const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                               const TDesC8& /*aParentLUID*/, 
                                               RWriteStream*& /*aStream*/, 
                                               const TDesC8& /*aType*/, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::ExecuteCommandL(): stream: begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmDevInfoAdapter::ExecuteCommandL(): stream: end");
    }
// -----------------------------------------------------------------------------
//  CNSmlDmDevDetailAdapter::CopyCommandL( const TDesC8& aTargetURI, 
//  const TDesC8& aSourceURI, const TDesC8& aSourceLUID, const TDesC8& aType, 
//  const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                                            const TDesC8& /*aTargetLUID*/, 
                                            const TDesC8& /*aSourceURI*/, 
                                            const TDesC8& /*aSourceLUID*/, 
                                            const TDesC8& /*aType*/, 
                                            TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::CopyCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmDevInfoAdapter::CopyCommandL(): end");
    }

// ----------------------------------------
//  CNSmlDmDevDetailAdapter::StartAtomicL()
// ----------------------------------------
void CNSmlDmDevDetailAdapter::StartAtomicL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::StartAtomicL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::StartAtomicL(): end");
    }
    
// -----------------------------------------
//  CNSmlDmDevDetailAdapter::CommitAtomicL()
// -----------------------------------------
void CNSmlDmDevDetailAdapter::CommitAtomicL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::CommitAtomicL(): end");
    }

// -------------------------------------------
//  CNSmlDmDevDetailAdapter::RollbackAtomicL()
// -------------------------------------------
void CNSmlDmDevDetailAdapter::RollbackAtomicL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::RollbackAtomicL(): end");
    }

// -------------------------------------------------------------
//  CNSmlDmDevDetailAdapter::StreamingSupport( TInt& aItemSize )
// -------------------------------------------------------------    
TBool CNSmlDmDevDetailAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::StreamingSupport(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::StreamingSupport(): end");
    return EFalse;
    }

// --------------------------------------------
//  CNSmlDmDevDetailAdapter::StreamCommittedL()
// --------------------------------------------
void CNSmlDmDevDetailAdapter::StreamCommittedL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::StreamCommittedL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::StreamCommittedL(): end");
    }

// ----------------------------------------------------
//  CNSmlDmDevDetailAdapter::CompleteOutstandingCmdsL()
// ---------------------------------------------------- 
void CNSmlDmDevDetailAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::CompleteOutstandingCmdsL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::CompleteOutstandingCmdsL(): end");
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
        IMPLEMENTATION_PROXY_ENTRY(KNSmlDMDevDetailAdapterImplUid, 
                                   CNSmlDmDevDetailAdapter::NewL)
    };

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmDevDetailAdapter: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmDevDetailAdapter: end");
    return ImplementationTable;
    }

