/*
*  Name        : nsmldmstreamingadapter.h
*  Part of     : nsmldmstreamingadapter
*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of dm adapters
* 	This is part of omadmextensions.
*
*/


#ifndef __NSMLDMSTREAMINGADAPTER_H__
#define __NSMLDMSTREAMINGADAPTER_H__

// INCLUDES
#include <e32base.h>
#include <smldmadapter.h>
#include "nsmldmconstants.h"

const TInt KMaxLengthStreamingName = 255;
const TInt KMaxLengthToProxy = 255;
const TInt KMaxLengthNetInfo = 255;
const TInt KMaxLengthTempInfo = 10;

const TInt KMinPort = 1024;
const TInt KMaxPort = 65535;
const TInt KPortDiff = 5;
const TUint KNSmlDMStreamingAdapterImplUid = 0x20024311;

const TInt KMnMaxBwGPRS = 9050;
const TInt KMxMaxBwGPRS = 40200;
const TInt KMnSusBwGPRS = 6335;
const TInt KMxSusBwGPRS = 28140;

const TInt KMnMaxBwEGPRS = 44800;
const TInt KMxMaxBwEGPRS = 108800;
const TInt KMnSusBwEGPRS = 26880;
const TInt KMxSusBwEGPRS = 65280;

const TInt KMnMaxBwWCDMA = 64000;
const TInt KMxMaxBwWCDMA = 384000;
const TInt KMnSusBwWCDMA = 44800;
const TInt KMxSusBwWCDMA = 268800;

const TInt KMnMaxBwHSDPA = 600000;
const TInt KMxMaxBwHSDPA = 3600000;
const TInt KMnSusBwHSDPA = 420000;
const TInt KMxSusBwHSDPA = 2520000;

const TInt KMnMaxBwWLAN = 64000;
const TInt KMxMaxBwWLAN = 3000000;
const TInt KMnSusBwWLAN = 44800;
const TInt KMxSusBwWLAN = 2100000;

const TInt KMnMaxBwCDMA = 9050;
const TInt KMxMaxBwCDMA = 40200;
const TInt KMnSusBwCDMA = 6335;
const TInt KMxSusBwCDMA = 28140;

const TInt KMnMaxBwCDMA2000 = 9050;
const TInt KMxMaxBwCDMA2000 = 40200;
const TInt KMnSusBwCDMA2000 = 6335;
const TInt KMxSusBwCDMA2000 = 28140;


/*
const TInt KMnMaxBw = 5000;

const TInt KMnMaxBwGPRS = 99995; // <
const TInt KMnMaxBw3G = 1920001; // < 
const TInt KMnMaxBwEGPRS = 199995; // < 
const TInt KMnMaxBwWLAN = 4300001; // <
const TInt KMnMaxBwHSDPA = 3999995; // < 
const TInt KMnMaxBwCDMA = 99995; // <
const TInt KMnMaxBwCDMA2000 = 153601; // < */




// MACROS

class CMPSettingsModel;

// the DDF version must be changed if any changes in DDF structure 
// ( built in DDFStructureL() function )
_LIT8( KNSmlDMStreamingAdapterDDFVersion, "1.0" ); 

_LIT8 (KNSmlURISeparator ,"/");

_LIT8( KNSmlDMStreamingAdapterTextPlain, "text/plain" );
_LIT8( KNSmlDMStreamingAdapterNodeName, "Streaming" );
_LIT8( KNSmlDMStreamingAdapterDescription, "The parent object holding all Streaming Settings" );

_LIT8( KNSmlDMStreamingAdapterVal, "Name/To-Proxy/To-NapID/NetInfo/MIN-UDP-PORT/MAX-UDP-PORT" );
_LIT8( KNSmlDMStreamingAdapterName, "Name" );
_LIT8( KNSmlDMStreamingAdapterNameDescription, "Streaming configuration name" );
_LIT8( KNSmlDMStreamingAdapterToProxy, "To-Proxy" );
_LIT8( KNSmlDMStreamingAdapterToProxyDescription, "Streaming proxy server ID" );
_LIT8( KNSmlDMStreamingAdapterToNapID, "To-NapID" );
_LIT8( KNSmlDMStreamingAdapterToNapIDDescription, "The APN that is used for Streaming" );
_LIT8( KNSmlDMStreamingAdapterNetInfo, "NetInfo" );
_LIT8( KNSmlDMStreamingAdapterNetInfoVal, "GPRS/EGPRS/WCDMA/CDMA/CDMA2000/WLAN/HSDPA" );

_LIT8( KNSmlDMStreamingAdapterNetInfoGPRS, "GPRS" );
_LIT8( KNSmlDMStreamingAdapterNetInfoEGPRS, "EGPRS" );
_LIT8( KNSmlDMStreamingAdapterNetInfoWCDMA, "WCDMA" );
_LIT8( KNSmlDMStreamingAdapterNetInfoCDMA2000, "CDMA2000" );
_LIT8( KNSmlDMStreamingAdapterNetInfoWLAN, "WLAN" );
_LIT8( KNSmlDMStreamingAdapterNetInfoCDMA, "CDMA" );
_LIT8( KNSmlDMStreamingAdapterNetInfoHSDPA, "HSDPA" );

_LIT8( KNSmlDMStreamingAdapterNetInfoDescription, "The parameter which contains networks information" );
_LIT8( KNSmlDMStreamingAdapterMinUdpPort, "MIN-UDP-PORT" );
_LIT8( KNSmlDMStreamingAdapterMinUdpPortDescription, "The min port number used for streaming" );
_LIT8( KNSmlDMStreamingAdapterMaxUdpPort, "MAX-UDP-PORT" );
_LIT8( KNSmlDMStreamingAdapterMaxUdpPortDescription, "The max port number used for streaming" );

_LIT8( KCommaSep,",");



// CLASS DECLARATION
class CNSmlDmStreamingAdapter : public CSmlDmAdapter
    {
public: // constructors and destructor
    static CNSmlDmStreamingAdapter* NewL(MSmlDmCallback* aDmCallback );
    static CNSmlDmStreamingAdapter* NewLC(MSmlDmCallback* aDmCallback );

    virtual ~CNSmlDmStreamingAdapter();

//private: ///new functions
public: ///new functions
    
    void SetNetInfoL(const TDesC8& aObject,const TDesC8 &aDes,TDataBearer aBearer,const TInt aStatusRef,const TInt aMnMaxBw, const TInt aMxMaxBw,const TInt aMnSusBw, const TInt aMxSusBw);
		
    CSmlDmAdapter::TError GetNetInfoL(CBufBase& aObject,TDataBearer aBearer,const TDesC8 &aDes);

    void SetLeafPropertiesL( MSmlDmDDFObject& aObject, 
                 const TSmlDmAccessTypes& aAccessTypes, 
                 const TDesC8& aDescription ) const;
    CNSmlDmStreamingAdapter(TAny* aEcomArguments);
    /**
     * Two-phased constructor.
     */
    void ConstructL();
    CSmlDmAdapter::TError FetchLeafObjectL( const TDesC8& aURI, CBufBase& aObject );
    
    void SetMinUDPPortL(const TDesC8& aObject,const TInt aStatusRef);
    
    void SetMaxUDPPortL(const TDesC8& aObject,const TInt aStatusRef);
    
    void SetDefaultApL(const TDesC8& aObject,const TInt aStatusRef);
    
    CSmlDmAdapter::TError GetDefaultApL(CBufBase& aObject);
    
//private: // from base classes
public: // from base classes
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
    
    static void Cleanup( TAny* aAny );
    
    /**
    * Fills the given information to a DDF Object node.
    * @param aNode The node whose data is filled.
    * @param aAccTypes The access types of the node.
    * @param aOccurance Occurance of the node.
    * @param aScope The scope of the node.
    * @param aFormat The format of the node's data, i.e. node/chr/bin/...
    * @param aDescription Informal description of the node.
    */
    void FillNodeInfoL( MSmlDmDDFObject& aNode, 
                        const TSmlDmAccessTypes& aAccTypes, 
                        MSmlDmDDFObject::TOccurence aOccurrence, 
                        MSmlDmDDFObject::TScope aScope, 
                        MSmlDmDDFObject::TDFFormat aFormat,
                        const TDesC8& aDescription ) const;
    
private:
    MSmlDmCallback* iDmCallback;
    
    CMPSettingsModel* iModel;
    
    };

#endif // __NSMLDMSTREAMINGADAPTER_H__
