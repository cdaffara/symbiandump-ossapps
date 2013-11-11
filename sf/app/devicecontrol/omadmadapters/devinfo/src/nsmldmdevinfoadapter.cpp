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
* Description:    DM DevInfo Adapter
*
*/





//INCLUDE FILES
#include <implementationproxy.h> // For TImplementationProxy definition
#include "nsmldmimpluids.h"
#include <featmgr.h>

#include "nsmldmdevinfoadapter.h"
#include "nsmldebug.h"
#include "nsmlphoneinfo.h"
#include "nsmlunicodeconverter.h"
#include "nsmlconstants.h"
#include "nsmldmconst.h"
#include "nsmldmdevdetailadapter.h"


// -----------------------------------------------------------------------------
// CNSmlDmDevInfoAdapter* CNSmlDmDevInfoAdapter::NewL( )
// -----------------------------------------------------------------------------
CNSmlDmDevInfoAdapter* CNSmlDmDevInfoAdapter::NewL(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::NewL(): begin");
    CNSmlDmDevInfoAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop();
    _DBG_FILE("CNSmlDmDevInfoAdapter::NewL(): end");
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevInfoAdapter* CNSmlDmDevInfoAdapter::NewLC( )
// -----------------------------------------------------------------------------
CNSmlDmDevInfoAdapter* CNSmlDmDevInfoAdapter::NewLC(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::NewLC(): begin");
    CNSmlDmDevInfoAdapter* self = new (ELeave) CNSmlDmDevInfoAdapter(aDmCallback);
    CleanupStack::PushL(self);
    self->iDmCallback = aDmCallback;
    FeatureManager::InitializeLibL();
    _DBG_FILE("CNSmlDmDevInfoAdapter::NewLC(): end");
    return self;
    }



// -----------------------------------------------------------------------------
// CNSmlDmDevInfoAdapter::~CNSmlDmDevInfoAdapter()
// -----------------------------------------------------------------------------
CNSmlDmDevInfoAdapter::~CNSmlDmDevInfoAdapter()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::~CNSmlDmDevInfoAdapter(): begin");
    FeatureManager::UnInitializeLib();
    _DBG_FILE("CNSmlDmDevInfoAdapter::~CNSmlDmDevInfoAdapter(): end");
    }
    
// -----------------------------------------------------------------------------
// CNSmlDmDevInfoAdapter::CNSmlDmDevInfoAdapter()
// -----------------------------------------------------------------------------

CNSmlDmDevInfoAdapter::CNSmlDmDevInfoAdapter(TAny* aEcomArguments):CSmlDmAdapter(aEcomArguments)
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::CNSmlDmDevInfoAdapter(aEcomArguments): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::CNSmlDmDevInfoAdapter(aEcomArguments): end"); 
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevInfoAdapter::SetLeafPropertiesL()
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::SetLeafPropertiesL( MSmlDmDDFObject& aObject, 
                                                const TSmlDmAccessTypes& aAccessTypes, 
                                                const TDesC8& aDescription ) const
    {
    aObject.SetAccessTypesL( aAccessTypes );
    aObject.SetScopeL( MSmlDmDDFObject::EPermanent );
    aObject.SetDFFormatL( MSmlDmDDFObject::EChr );
    aObject.AddDFTypeMimeTypeL( KNSmlDMDevInfoTextPlain );
    aObject.SetDescriptionL( aDescription );
    }


// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::DDFVersionL()
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::DDFVersionL(TDes& aDDFVersion): begin");
    aDDFVersion.InsertL(0,KNSmlDMDevInfoDDFVersion);
    _DBG_FILE("CNSmlDmDevInfoAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::DDFStructureL()
//
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::DDFStructureL(): begin");
    
    TSmlDmAccessTypes accessTypes;
    accessTypes.SetGet();
    // DevInfo
    MSmlDmDDFObject& devInfo = aDDF.AddChildObjectL( KNSmlDMDevInfoNodeName );
    devInfo.SetAccessTypesL( accessTypes ); 
    devInfo.SetScopeL( MSmlDmDDFObject::EPermanent );
    devInfo.SetDescriptionL( KNSmlDMDevInfoDescription );
    // Man  
    MSmlDmDDFObject& man = devInfo.AddChildObjectL(KNSmlDMDevInfoManNodeName);
    SetLeafPropertiesL( man, accessTypes, KNSmlDMDevInfoManDescription );
    // Mod  
    MSmlDmDDFObject& mod = devInfo.AddChildObjectL(KNSmlDMDevInfoModNodeName );
    SetLeafPropertiesL( mod, accessTypes, KNSmlDMDevInfoModDescription );
    // DevId    
    MSmlDmDDFObject& devid = devInfo.AddChildObjectL( KNSmlDMDevInfoDevIdNodeName);
    SetLeafPropertiesL( devid, accessTypes, KNSmlDMDevInfoDevIdDescription ); 
    // Lang 
    MSmlDmDDFObject& lang = devInfo.AddChildObjectL(KNSmlDMDevInfoLangNodeName );
    SetLeafPropertiesL( lang, accessTypes, KNSmlDMDevInfoLangDescription ); 
    // DmV  
    MSmlDmDDFObject& dmv = devInfo.AddChildObjectL(KNSmlDMDevInfoDmVNodeName);
    SetLeafPropertiesL( dmv, accessTypes, KNSmlDMDevInfoDmVDescription );
    // Ext
    MSmlDmDDFObject& ext = devInfo.AddChildObjectL(KNSmlDMDevInfoExtNodeName );
    ext.SetAccessTypesL( accessTypes );
    ext.SetScopeL( MSmlDmDDFObject::EPermanent );
    ext.SetDescriptionL( KNSmlDMDevInfoExtDescription );
    // ModDDF   
    MSmlDmDDFObject& modDDF = ext.AddChildObjectL( KNSmlDMDevInfoModDDFNodeName);
    SetLeafPropertiesL( modDDF, accessTypes, KNSmlDMDevInfoModDDFDescription );
    // ModDevDet    
    MSmlDmDDFObject& modDevDet = ext.AddChildObjectL(KNSmlDMDevInfoModDevDetNodeName );
    SetLeafPropertiesL( modDevDet, accessTypes, KNSmlDMDevInfoModDevDetDescription );
    _DBG_FILE("CNSmlDmDevInfoAdapter::DDFStructureL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                               const TDesC8& /*aLUID*/, 
                                               const TDesC8& /*aObject*/, 
                                               const TDesC8& /*aType*/, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::UpdateLeafObjectL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::UpdateLeafObjectL(): end");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }


// -----------------------------------------------------------------------------
// CNSmlDmDevInfoAdapter::DeleteObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::DeleteObjectL( const TDesC8& /*aURI*/, 
                                           const TDesC8& /*aLUID*/, 
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::DeleteLeafObjectL( ): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::DeleteLeafObjectL( ): end");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }


// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
// const TDesC8& aType, const TInt aResultsRef, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                              const TDesC8& /*aLUID*/, 
                                              const TDesC8& aType, 
                                              const TInt aResultsRef, 
                                              const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::FetchLeafObjectL(): begin");
    
    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    
    CSmlDmAdapter::TError retValue = FetchLeafObjectL( aURI, *object );
    
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType );
    CleanupStack::PopAndDestroy(); //object
    _DBG_FILE("CNSmlDmDevInfoAdapt+er::FetchLeafObjectL(): end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::ChildURIListL( const TDesC& aURI, 
// const TDesC& aParentLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
// const TInt aResultsRef, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::ChildURIListL( const TDesC8& aURI, 
                                           const TDesC8& /*aParentLUID*/, 
                                           const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, 
                                           const TInt aResultsRef, 
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::ChildURIListL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    CBufBase* currentURISegmentList = CBufFlat::NewL( 1 );
    CleanupStack::PushL( currentURISegmentList );
    TInt ret = aURI.LocateReverse(KNSmlDMDevInfoSeparator()[0]);
    if ( ret == KErrNotFound ) 
        {
        ret = -1;
        }
    TInt len = aURI.Length() - ( ret + 1 );
    TPtrC8 segment = aURI.Right( len );

    if ( segment == KNSmlDMDevInfoNodeName )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoManNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoModNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoDevIdNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoLangNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoDmVNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoExtNodeName() );
        }
    else
    if ( segment == KNSmlDMDevInfoExtNodeName )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoModDDFNodeName() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoSeparator() );
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMDevInfoModDevDetNodeName() );
        }
    else
        {
        retValue = CSmlDmAdapter::EError;
        }
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
    CleanupStack::PopAndDestroy(); //currentURISegmentList
    _DBG_FILE("CNSmlDmDevInfoAdapter::ChildURIListL(): end");
    }


// -----------------------------------------------------------------------------
// CNSmlDmDevInfoAdapter::AddNodeObjectL( const TDesC8& aURI, 
// const TDesC8& aParentLUID, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::AddNodeObjectL( const TDesC8& /*aURI*/, 
                                            const TDesC8& /*aParentLUID*/, 
                                            const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::AddNodeObjectL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmDevInfoAdapter::AddNodeObjectL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                               const TDesC8& /*aLUID*/, 
                                               RWriteStream*& /*aStream*/, 
                                               const TDesC8& /*aType*/, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::UpdateLeafObjectL(): stream: begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::UpdateLeafObjectL(): stream: end");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }


// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
//  const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                                  const TDesC8& /*aLUID*/, 
                                                  const TDesC8& aType, 
                                                  const TInt aResultsRef, 
                                                  const TInt aStatusRef )
    {

    _DBG_FILE("CNSmlDmDevInfoAdapter::FetchLeafObjectSizeL(): begin");
    
    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    CSmlDmAdapter::TError retValue = FetchLeafObjectL( aURI, *object );
        
    TInt objSizeInBytes = object->Size();
    TBuf8<2> stringObjSizeInBytes;
    stringObjSizeInBytes.Num( objSizeInBytes );
    object->Reset();
    object->InsertL( 0, stringObjSizeInBytes );
    
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType );
    CleanupStack::PopAndDestroy(); //object
    _DBG_FILE("CNSmlDmDevInfoAdapter::FetchLeafObjectSizeL(): end");
    }
    

// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::ExecuteCommandL( const TDesC8& aURI, 
//  const TDesC8& aParentLUID, TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, const TDesC8& /*aType*/, const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::ExecuteCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmDevInfoAdapter::ExecuteCommandL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmDevInfoAdapter::ExecuteCommandL( const TDesC8& aURI, 
//  const TDesC8& aParentLUID, RWriteStream*& aStream, const TDesC8& aType, 
//  TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
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
//  CNSmlDmDevInfoAdapter::CopyCommandL( const TDesC8& aTargetURI, const TDesC8& 
//  aSourceURI, const TDesC8& aSourceLUID, const TDesC8& /*aType*/, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmDevInfoAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& 
                                          /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, 
                                          const TDesC8& /*aSourceLUID*/, 
                                          const TDesC8& /*aType*/, TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::CopyCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmDevInfoAdapter::CopyCommandL(): end");
    }

// --------------------------------------
//  CNSmlDmDevInfoAdapter::StartAtomicL()
// --------------------------------------
void CNSmlDmDevInfoAdapter::StartAtomicL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::StartAtomicL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::StartAtomicL(): end");
    }
    
// ---------------------------------------
//  CNSmlDmDevInfoAdapter::CommitAtomicL()
// ---------------------------------------
void CNSmlDmDevInfoAdapter::CommitAtomicL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::CommitAtomicL(): end");
    }

// -----------------------------------------
//  CNSmlDmDevInfoAdapter::RollbackAtomicL()
// -----------------------------------------
void CNSmlDmDevInfoAdapter::RollbackAtomicL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::RollbackAtomicL(): end");
    }

// -----------------------------------------------------------
//  CNSmlDmDevInfoAdapter::StreamingSupport( TInt& aItemSize )
// -----------------------------------------------------------  
TBool CNSmlDmDevInfoAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::StreamingSupport(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::StreamingSupport(): end");
    return EFalse;
    }

// ------------------------------------------
//  CNSmlDmDevInfoAdapter::StreamCommittedL()
// ------------------------------------------
void CNSmlDmDevInfoAdapter::StreamCommittedL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::StreamCommittedL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::StreamCommittedL(): end");
    }

// --------------------------------------------------
//  CNSmlDmDevInfoAdapter::CompleteOutstandingCmdsL()
// --------------------------------------------------   
void CNSmlDmDevInfoAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE("CNSmlDmDevInfoAdapter::CompleteOutstandingCmdsL(): begin");
    _DBG_FILE("CNSmlDmDevInfoAdapter::CompleteOutstandingCmdsL(): end");    
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::FetchLeafObjectL()
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmDevInfoAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                                               CBufBase& aObject )
    {
    _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    TInt ret = aURI.LocateReverse(KNSmlDMDevInfoSeparator()[0]);
    if ( ret == KErrNotFound ) 
        {
        retValue = CSmlDmAdapter::EError;
        }
    else
        {
        TInt len = aURI.Length() - (ret + 1);
        TPtrC8 segment = aURI.Right( len );
        if ( segment == KNSmlDMDevInfoManNodeName ||
             segment == KNSmlDMDevInfoModNodeName ||
             segment == KNSmlDMDevInfoDevIdNodeName )
            {
            CNSmlPhoneInfo* phoneInfo = CNSmlPhoneInfo::NewLC();
            HBufC* phonedata = HBufC::NewLC( 50 );
            TPtr phonedataPtr = phonedata->Des();  
            if ( segment == KNSmlDMDevInfoManNodeName )
                {
                phoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneManufacturer, phonedataPtr );
                }
            if ( segment == KNSmlDMDevInfoModNodeName )
                {
                phoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneModelId, phonedataPtr );
                }
            if ( segment == KNSmlDMDevInfoDevIdNodeName )
                {
                phoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneSerialNumber, phonedataPtr );
                }
            HBufC8* phonedataInUTF8 = NULL;
            NSmlUnicodeConverter::HBufC8InUTF8LC( *phonedata, phonedataInUTF8 );
            aObject.InsertL( 0, *phonedataInUTF8 );
            CleanupStack::PopAndDestroy( 3 ); //phonedataInUTF8, phonedata, phoneInfo
            }
        else
        if ( segment == KNSmlDMDevInfoLangNodeName )
            { 
            TLanguage language = User::Language();
            TBuf8<2> langCode;
            switch ( language )
                {
                case ELangEnglish: 
                case ELangAmerican:
                case ELangAustralian:
                case ELangNewZealand:
                case ELangCanadianEnglish:
                case ELangInternationalEnglish:
                case ELangSouthAfricanEnglish:
                    langCode = KNSmlDMEnglish;
                    break; 
                case ELangFrench:
                case ELangSwissFrench:
                case ELangBelgianFrench:
                case ELangInternationalFrench:
                case ELangCanadianFrench:
                    langCode = KNSmlDMFrench;
                    break;
                case ELangGerman:
                case ELangSwissGerman:
                case ELangAustrian:
                    langCode = KNSmlDMGerman;
                    break;
                case ELangSpanish:
                case ELangInternationalSpanish:
                case ELangLatinAmericanSpanish:
                    langCode = KNSmlDMSpanish;
                    break;
                case ELangItalian:
                case ELangSwissItalian:
                    langCode = KNSmlDMItalian;
                    break;
                case ELangSwedish:
                case ELangFinlandSwedish:
                    langCode = KNSmlDMSwedish;
                    break;
                case ELangDanish:
                    langCode = KNSmlDMDanish;
                    break;
                case ELangNorwegian:
                case ELangNorwegianNynorsk:
                    langCode = KNSmlDMNorwegian;
                    break;
                case ELangFinnish:
                    langCode = KNSmlDMFinnish;
                    break;
                case ELangPortuguese:
                case ELangBrazilianPortuguese:
                    langCode = KNSmlDMPortuguese;
                    break;
                case ELangTurkish:
                case ELangCyprusTurkish:
                    langCode = KNSmlDMTurkish;
                    break;
                case ELangIcelandic:
                    langCode = KNSmlDMIcelandic;
                    break;
                case ELangRussian:
                    langCode = KNSmlDMRussian;
                    break;
                case ELangHungarian:
                    langCode = KNSmlDMHungarian;
                    break;
                case ELangDutch:
                case ELangBelgianFlemish:
                    langCode = KNSmlDMDutch;
                    break;
                case ELangCzech:
                    langCode = KNSmlDMCzech;
                    break;
                case ELangSlovak:
                    langCode = KNSmlDMSlovak;
                    break;
                case ELangPolish:
                    langCode = KNSmlDMPolish;
                    break;
                case ELangSlovenian:
                    langCode = KNSmlDMSlovenian;
                    break;
                case ELangTaiwanChinese:
                case ELangHongKongChinese:
                case ELangPrcChinese:
                    langCode = KNSmlDMChinese;
                    break;
                case ELangJapanese:
                    langCode = KNSmlDMJapanese;
                    break;
                case ELangThai:
                    langCode = KNSmlDMThai;
                    break;
                case ELangAfrikaans:
                    langCode = KNSmlDMAfrikaans;
                    break;
                case ELangAlbanian:
                    langCode = KNSmlDMAlbanian;
                    break;
                case ELangAmharic:
                    langCode = KNSmlDMAmharic;
                    break;
                case ELangArabic:
                    langCode = KNSmlDMArabic;
                    break;
                case ELangArmenian:
                    langCode = KNSmlDMArmenian;
                    break;
                case ELangTagalog:
                    langCode = KNSmlDMTagalog;
                    break;
                case ELangBelarussian:
                    langCode = KNSmlDMBelarussian;
                    break;
                case ELangBengali:
                    langCode = KNSmlDMBengali;
                    break;
                case ELangBulgarian:
                    langCode = KNSmlDMBulgarian;
                    break;
                case ELangBurmese:
                    langCode = KNSmlDMBurmese;
                    break;
                case ELangCatalan:
                    langCode = KNSmlDMCatalan;
                    break;
                case ELangCroatian:
                    langCode = KNSmlDMCroatian;
                    break;
                case ELangEstonian:
                    langCode = KNSmlDMEstonian;
                    break;
                case ELangFarsi:
                    langCode = KNSmlDMFarsi;
                    break;
                case ELangScotsGaelic:
                    langCode = KNSmlDMScotsGaelic;
                    break;
                case ELangGeorgian:
                    langCode = KNSmlDMGeorgian;
                    break;
                case ELangGreek:
                case ELangCyprusGreek:
                    langCode = KNSmlDMGreek;
                    break;
                case ELangGujarati:
                    langCode = KNSmlDMGujarati;
                    break;
                case ELangHebrew:
                    langCode = KNSmlDMHebrew;
                    break;
                case ELangHindi:
                    langCode = KNSmlDMHindi;
                    break;
                case ELangIndonesian:
                    langCode = KNSmlDMIndonesian;
                    break;
                case ELangIrish:
                    langCode = KNSmlDMIrish;
                    break;
                case ELangKannada:
                    langCode = KNSmlDMKannada;
                    break;
                case ELangKazakh:
                    langCode = KNSmlDMKazakh;
                    break;
                case ELangKorean:
                    langCode = KNSmlDMKorean;
                    break;
                case ELangLao:
                    langCode = KNSmlDMLao;
                    break;
                case ELangLatvian:
                    langCode = KNSmlDMLatvian;
                    break;
                case ELangLithuanian:
                    langCode = KNSmlDMLithuanian;
                    break;
                case ELangMacedonian:
                    langCode = KNSmlDMMacedonian;
                    break;
                case ELangMalay:
                    langCode = KNSmlDMMalay;
                    break;
                case ELangMalayalam:
                    langCode = KNSmlDMMalayalam;
                    break;
                case ELangMarathi:
                    langCode = KNSmlDMMarathi;
                    break;
                case ELangMoldavian:
                    langCode = KNSmlDMMoldavian;
                    break;
                case ELangMongolian:
                    langCode = KNSmlDMMongolian;
                    break;
                case ELangPunjabi:
                    langCode = KNSmlDMPunjabi;
                    break;
                case ELangRomanian:
                    langCode = KNSmlDMRomanian;
                    break;
                case ELangSerbian:
                    langCode = KNSmlDMSerbian;
                    break;
                case ELangSinhalese:
                    langCode = KNSmlDMSinhalese;
                    break;
                case ELangSomali:
                    langCode = KNSmlDMSomali;
                    break;
                case ELangSwahili:
                    langCode = KNSmlDMSwahili;
                    break;
                case ELangTamil:
                    langCode = KNSmlDMTamil;
                    break;
                case ELangTelugu:
                    langCode = KNSmlDMTelugu;
                    break;
                case ELangTibetan:
                    langCode = KNSmlDMTibetan;
                    break;
                case ELangTigrinya:
                    langCode = KNSmlDMTigrinya;
                    break;
                case ELangTurkmen:
                    langCode = KNSmlDMTurkmen;
                    break;
                case ELangUkrainian:
                    langCode = KNSmlDMUkrainian;
                    break;
                case ELangUrdu:
                    langCode = KNSmlDMUrdu;
                    break;
                case ELangVietnamese:
                    langCode = KNSmlDMVietnamese;
                    break;
                case ELangWelsh:
                    langCode = KNSmlDMWelsh;
                    break;
                case ELangZulu:
                    langCode = KNSmlDMZulu;
                    break;
                default:
                    langCode = KNSmlDMEnglish;
                }
            aObject.InsertL( 0, langCode );
            }
        else
        if ( segment == KNSmlDMDevInfoDmVNodeName )
            {
            if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
                aObject.InsertL( 0, KNSmlDMDmV12 );
            else
            aObject.InsertL( 0, KNSmlDMDmV );
            }
        else
        if ( segment == KNSmlDMDevInfoModDDFNodeName )
            {

            }
        else
        if ( segment == KNSmlDMDevInfoModDevDetNodeName )
            {
            TUid implUID;
            implUID.iUid = KNSmlDMDevDetailAdapterImplUid;
            
            TAny* nPtr = NULL;
            MSmlDmCallback& aDmNull = reinterpret_cast<MSmlDmCallback&>(nPtr);
            CNSmlDmDevDetailAdapter* devDetAdapter = 
                reinterpret_cast<CNSmlDmDevDetailAdapter*>(CSmlDmAdapter::NewL( implUID, aDmNull ) );
            CleanupStack::PushL(devDetAdapter);
            TInt checkSumDevDet = devDetAdapter->DevDetailValueCrcL();
            TBuf8<16> stringCheckSumDevDet; 
            stringCheckSumDevDet.Num( checkSumDevDet );
            aObject.InsertL( 0, stringCheckSumDevDet );
            CleanupStack::PopAndDestroy(); // devDetAdapter
            }
        else
            {
            retValue = CSmlDmAdapter::EError;
            }
        }
    _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): end");
    return retValue;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
        IMPLEMENTATION_PROXY_ENTRY(KNSmlDMDevInfoAdapterImplUid, CNSmlDmDevInfoAdapter::NewL)
    };

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmDevInfoAdapter: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmDevInfoAdapter: end");
    return ImplementationTable;
    }

// End of file

