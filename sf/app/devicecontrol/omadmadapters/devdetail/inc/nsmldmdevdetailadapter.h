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
* Description:    DM DevDetail Adapter
*
*/




#ifndef __NSMLDMDEVDETAILADAPTER_H__
#define __NSMLDMDEVDETAILADAPTER_H__

// INCLUDES

#include <e32base.h>
#include <smldmadapter.h>
#include "nsmldmconstants.h"

// the DDF version must be changed if any changes in DDF structure 
// ( built in DDFStructureL() function )
_LIT8( KNSmlDMDevDetailDDFVersion, "1.0" ); 

_LIT8( KNSmlDMDevDetailTextPlain, "text/plain" );
_LIT8( KNSmlDMDevDetailNodeName, "DevDetail" );
_LIT8( KNSmlDMDevDetailDescription, "The interior object holds all DevDetail objects" );

_LIT8( KNSmlDMDevDetailDevTypNodeName, "DevTyp" );
_LIT8( KNSmlDMDevDetailDevTypDescription, "The type of the device" );
_LIT8( KNSmlDMDevDetailDevTypValue, "smartphone" );

_LIT8( KNSmlDMDevDetailOEMNodeName, "OEM" );
_LIT8( KNSmlDMDevDetailOEMDescription, "Original Equipment Manufacturer" );

_LIT8( KNSmlDMDevDetailFwVNodeName, "FwV" );
_LIT8( KNSmlDMDevDetailFwVDescription, "The firmware revision of the device" );

_LIT8( KNSmlDMDevDetailSwVNodeName, "SwV" );
_LIT8( KNSmlDMDevDetailSwVDescription, "The software revision of the device" );

_LIT8( KNSmlDMDevDetailHwVNodeName, "HwV" );
_LIT8( KNSmlDMDevDetailHwVDescription, "The hardware revision of the device" );

_LIT8( KNSmlDMDevDetailLrgObjNodeName, "LrgObj" );
_LIT8( KNSmlDMDevDetailLrgObjDescription, "Large object handling supported if value is true" );
_LIT8( KNSmlDMDevDetailLrgObjValue, "true" );

_LIT8( KNSmlDMDevDetailURINodeName, "URI" );
_LIT8( KNSmlDMDevDetailURIDescription, "The tree limitations branch" );

_LIT8( KNSmlDMDevDetailMaxDepthNodeName, "MaxDepth" );
_LIT8( KNSmlDMDevDetailMaxDepthDescription, "Maximum tree depth" );
_LIT8( KNSmlDMDevDetailMaxDepthValue, "0" );

_LIT8( KNSmlDMDevDetailMaxTotLenNodeName, "MaxTotLen" );
_LIT8( KNSmlDMDevDetailMaxTotLenDescription, "Maximum URI length" );
_LIT8( KNSmlDMDevDetailMaxTotLenValue, "0" );

_LIT8( KNSmlDMDevDetailMaxSegLenNodeName, "MaxSegLen" );
_LIT8( KNSmlDMDevDetailMaxSegLenDescription, "Maximum URI segment length" );
_LIT8( KNSmlDMDevDetailMaxSegLenValue, "9" );

_LIT8( KNSmlDMDevDetailSeparator, "/" );

const TInt KNSmlMaxRequestBufferLength = 128;
const TInt KNSmlMaxSizeBufferLength = 4;

_LIT( KNSmlDevDetailPanic,      "DevDetail" );

// CLASS DECLARATION
class CNSmlDmDevDetailAdapter : public CSmlDmAdapter
    {
public:  // contructors and destructor
    static CNSmlDmDevDetailAdapter* NewL(MSmlDmCallback* aDmCallback );
    static CNSmlDmDevDetailAdapter* NewLC(MSmlDmCallback* aDmCallback );

    virtual ~CNSmlDmDevDetailAdapter();
    virtual TInt DevDetailValueCrcL();

private: // new functions
    CNSmlDmDevDetailAdapter(TAny* aEcomArguments);
    void AddObjectToBufferL( CBufBase& aCrcBuf, const TDesC8& aURI ); 
    CSmlDmAdapter::TError FetchLeafObjectL( const TDesC8& aURI, CBufBase& aObject ); 
    void SetLeafPropertiesL( MSmlDmDDFObject& aObject, 
                             const TSmlDmAccessTypes& aAccessTypes, 
                             const TDesC8& aDescription ) const;
            
private: // functions from base classes
    // from CSmlDmAdapter
    void DDFVersionL( CBufBase& aVersion );
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                            const TDesC8& aObject, const TDesC8& aType, 
                            const TInt aStatusRef );
    void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TInt aStatusRef ); 
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
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, 
                          const TDesC8& aType, const TInt aStatusRef );
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, 
                          const TDesC8& aType, const TInt aStatusRef );
    void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, 
                       const TDesC8& aSourceURI, const TDesC8& aSourceLUID, 
                       const TDesC8& aType, TInt aStatusRef );
    void StartAtomicL();
    void CommitAtomicL();
    void RollbackAtomicL();
    TBool StreamingSupport( TInt& aItemSize );
    void StreamCommittedL();
    void CompleteOutstandingCmdsL();

// FOTA
private:

    /**
    * Enumeration specifies the data that can be fetched using
    * GetDevDetailDataL().
    */
    enum TNSmlDevDetailData
        {
        ESwVersion,
        EHwVersion
        };

    /**
    * Fetches and/or forms DevDetail data specified by aElement.
    * In WINSCW this data is read from file. In ARM builds the 
    * data is fetched from EInfo.
    * @param aObject The buffer where fetched data is inserted.
    * @param aElement Specifies the data that should be fetched.
    */
    void GetDevDetailDataL( CBufBase& aObject, TNSmlDevDetailData aElement ) const;
	
// FOTA end

private:

    MSmlDmCallback* iDmCallback;
    
    };

#endif // __NSMLDMDEVDETAILADAPTER_H__
