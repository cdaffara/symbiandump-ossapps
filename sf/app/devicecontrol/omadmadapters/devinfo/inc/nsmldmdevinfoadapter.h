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






#ifndef __NSMLDMDEVINFOADAPTER_H__
#define __NSMLDMDEVINFOADAPTER_H__

// INCLUDES
#include <e32base.h>
#include <smldmadapter.h>
#include "nsmldmconstants.h"

// MACROS

// the DDF version must be changed if any changes in DDF structure 
// ( built in DDFStructureL() function )
_LIT8( KNSmlDMDevInfoDDFVersion, "1.0" ); 

_LIT8( KNSmlDMDevInfoTextPlain, "text/plain" );
_LIT8( KNSmlDMDevInfoNodeName, "DevInfo" );
_LIT8( KNSmlDMDevInfoDescription, "The interior object holds all DevInfo objects" );

_LIT8( KNSmlDMDevInfoManNodeName, "Man" );
_LIT8( KNSmlDMDevInfoManDescription, "The name of the device manufacturer" );

_LIT8( KNSmlDMDevInfoModNodeName, "Mod" );
_LIT8( KNSmlDMDevInfoModDescription, "The name of the device model" );

_LIT8( KNSmlDMDevInfoDevIdNodeName, "DevId" );
_LIT8( KNSmlDMDevInfoDevIdDescription, "The unique device identifier" );

_LIT8( KNSmlDMDevInfoLangNodeName, "Lang" );
_LIT8( KNSmlDMDevInfoLangDescription, "The current language setting of the device" );

_LIT8( KNSmlDMDevInfoDmVNodeName, "DmV" );
_LIT8( KNSmlDMDevInfoDmVDescription, "The current DM client revision of the device" );

_LIT8( KNSmlDMDevInfoExtNodeName, "Ext" );
_LIT8( KNSmlDMDevInfoExtDescription, "The extendable DevInfo branch" );

_LIT8( KNSmlDMDevInfoModDDFNodeName, "ModDDF" );
_LIT8( KNSmlDMDevInfoModDDFDescription, "The checksum counted from DDF" );

_LIT8( KNSmlDMDevInfoModDevDetNodeName, "ModDevDet" );
_LIT8( KNSmlDMDevInfoModDevDetDescription, "The checksum counted from DevDetail values" );

_LIT8( KNSmlDmDDFChangeUri, "DDFCHANGE" );

_LIT8( KNSmlDMDevInfoSeparator, "/" );

_LIT8( KNSmlDMEnglish, "en" );
_LIT8( KNSmlDMFrench, "fr" );
_LIT8( KNSmlDMGerman, "de" );
_LIT8( KNSmlDMSpanish, "es" );
_LIT8( KNSmlDMItalian, "it" );
_LIT8( KNSmlDMSwedish, "sv" );
_LIT8( KNSmlDMDanish, "da" );
_LIT8( KNSmlDMNorwegian, "no" ); 
_LIT8( KNSmlDMFinnish, "fi" );
_LIT8( KNSmlDMPortuguese, "pt" );
_LIT8( KNSmlDMTurkish, "tr" );
_LIT8( KNSmlDMIcelandic, "is" );
_LIT8( KNSmlDMRussian, "ru" );
_LIT8( KNSmlDMHungarian, "hu" );
_LIT8( KNSmlDMDutch, "nl" );
_LIT8( KNSmlDMCzech, "cs" );
_LIT8( KNSmlDMSlovak, "sk" );
_LIT8( KNSmlDMPolish, "pl" );
_LIT8( KNSmlDMSlovenian, "sl" );
_LIT8( KNSmlDMChinese, "ch" );
_LIT8( KNSmlDMJapanese, "ja" );
_LIT8( KNSmlDMThai, "th" );
_LIT8( KNSmlDMAfrikaans, "af" );
_LIT8( KNSmlDMAlbanian, "sq" );
_LIT8( KNSmlDMAmharic, "am" );
_LIT8( KNSmlDMArabic, "ar" );
_LIT8( KNSmlDMArmenian, "hy" );
_LIT8( KNSmlDMTagalog, "tl" );
_LIT8( KNSmlDMBelarussian, "be" );
_LIT8( KNSmlDMBengali, "bn" );
_LIT8( KNSmlDMBulgarian, "bg" );
_LIT8( KNSmlDMBurmese, "my" );
_LIT8( KNSmlDMCatalan, "ca" );
_LIT8( KNSmlDMCroatian, "hr" );
_LIT8( KNSmlDMEstonian, "et" );
_LIT8( KNSmlDMFarsi, "fo" );
_LIT8( KNSmlDMScotsGaelic, "gl" );
_LIT8( KNSmlDMGeorgian, "ka" );
_LIT8( KNSmlDMGreek, "el" );
_LIT8( KNSmlDMGujarati, "gu" );
_LIT8( KNSmlDMHebrew, "he" );
_LIT8( KNSmlDMHindi, "hi" );
_LIT8( KNSmlDMIndonesian, "id" );
_LIT8( KNSmlDMIrish, "ga" );
_LIT8( KNSmlDMKannada, "kn" );
_LIT8( KNSmlDMKazakh, "kk" );
_LIT8( KNSmlDMKorean, "ko" );
_LIT8( KNSmlDMLao, "lo" );
_LIT8( KNSmlDMLatvian, "lv" );
_LIT8( KNSmlDMLithuanian, "lt" );
_LIT8( KNSmlDMMacedonian, "mk" );
_LIT8( KNSmlDMMalay, "ms" );
_LIT8( KNSmlDMMalayalam, "ml" );
_LIT8( KNSmlDMMarathi, "mr" );
_LIT8( KNSmlDMMoldavian, "mo" );
_LIT8( KNSmlDMMongolian, "mn" );
_LIT8( KNSmlDMPunjabi, "pa" );
_LIT8( KNSmlDMRomanian, "ro" );
_LIT8( KNSmlDMSerbian, "sr" );
_LIT8( KNSmlDMSinhalese, "si" );                
_LIT8( KNSmlDMSomali, "so" );
_LIT8( KNSmlDMSwahili, "sw" );
_LIT8( KNSmlDMTamil, "ta" );
_LIT8( KNSmlDMTelugu, "te" );
_LIT8( KNSmlDMTibetan, "bo" );
_LIT8( KNSmlDMTigrinya, "ti" );
_LIT8( KNSmlDMTurkmen, "tk" );
_LIT8( KNSmlDMUkrainian, "uk" );
_LIT8( KNSmlDMUrdu, "ur" ); 
_LIT8( KNSmlDMVietnamese, "vi" );
_LIT8( KNSmlDMWelsh, "cy" );
_LIT8( KNSmlDMZulu, "zu" );


// CLASS DECLARATION
class CNSmlDmDevInfoAdapter : public CSmlDmAdapter
    {
public: // constructors and destructor
    static CNSmlDmDevInfoAdapter* NewL(MSmlDmCallback* aDmCallback );
    static CNSmlDmDevInfoAdapter* NewLC(MSmlDmCallback* aDmCallback );

    virtual ~CNSmlDmDevInfoAdapter();

private: ///new functions

    void SetLeafPropertiesL( MSmlDmDDFObject& aObject, 
                 const TSmlDmAccessTypes& aAccessTypes, 
                 const TDesC8& aDescription ) const;
    CNSmlDmDevInfoAdapter(TAny* aEcomArguments);
    CSmlDmAdapter::TError FetchLeafObjectL( const TDesC8& aURI, CBufBase& aObject );
    
private: // from base classes
    // Adapter interface from CSmlDmAdapter
    void DDFVersionL( CBufBase& aDDFVersion );
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                const TDesC8& aObject, const TDesC8& aType, 
                const TInt aStatusRef );
    void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef ); 
    void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                   const TDesC8& aType, const TInt aResultsRef, 
                   const TInt aStatusRef ); 
    void ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID, 
                const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                const TInt aResultsRef, const TInt aStatusRef );  
    void AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, 
                 const TInt aStatusRef );
    
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                RWriteStream*& aStream, const TDesC8& aType, 
                const TInt aStatusRef );
    void FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, 
                   const TDesC8& aType, const TInt aResultsRef, 
                   const TInt aStatusRef );
     
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
                          const TDesC8& aArgument, 
                          const TDesC8& aType,
                          const TInt aStatusRef );
                                  
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
                          RWriteStream*& aStream, 
                          const TDesC8& aType,
                          const TInt aStatusRef );
    
    void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID,
                       const TDesC8& aSourceURI, 
                       const TDesC8& aSourceLUID,
                       const TDesC8& aType, TInt aStatusRef );
    void StartAtomicL();
    void CommitAtomicL();
    void RollbackAtomicL();
    TBool StreamingSupport( TInt& aItemSize );
    void StreamCommittedL();
    void CompleteOutstandingCmdsL();
    
    
private:
    MSmlDmCallback* iDmCallback;
    };

#endif // __NSMLDMDEVINFOADAPTER_H__
