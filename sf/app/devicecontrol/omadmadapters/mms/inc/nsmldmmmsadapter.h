/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    DM MMS Settings Adapter
 *
*/





#ifndef __NSMLDMMMSADAPTER_H__
#define __NSMLDMMMSADAPTER_H__

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include <mtclreg.h>
#include <mmssettings.h>

// --------------------------------------------------------------------------
// Class forwards
// --------------------------------------------------------------------------
class CClientMtmRegistry;
class CMsvSession;
class CMmsClientMtm;

// --------------------------------------------------------------------------
// Constant definitions
// --------------------------------------------------------------------------

const TInt KNSmlDMMMSDefaultExpiry = 72 * 3600; //3 days
const TInt KNSmlDMMMSExpiryHour = 3600; //1 hour
const TInt KNSmlDMMMSExpirySixHours = 6*3600; //6 hours
const TInt KNSmlDMMMSExpiryDay = 24*3600; //24 hours
const TInt KNSmlDMMMSExpiryThreeDays = 3*24*3600; //3 days
const TInt KNSmlDMMMSExpiryWeek = 7*24*3600; //1 week
const TInt KNSmlDMMMSExpiryMaximum = 0;
//Iap id which is not found from commsdb
const TUint32 KNSmlDMMMSIapNotDefined = 999999; 
const TInt KNSmlMMSUndefinedStatusRef = -1;

const TInt KNSmlDMMMSFirstAccessPointId = 0;
const TInt KNSmlDMMMSSecondAccessPointId = 1;
const TUint8 KNSmlDMMMSSeparatorDef = 0x2f; //forward slash
const TUint8 KNSmlDmMmsObjectSizeWidth = 4;

// the DDF version must be changed if any changes in DDF structure 
// ( built in DDFStructureL() function )

// --------------------------------------------------------------------------
// Literal definitions
// --------------------------------------------------------------------------

_LIT8( KNSmlDMMMSDDFVersion, "1.1" ); 

_LIT8( KNSmlDMMMSNodeName, "MMS" );
_LIT8( KNSmlDMMMSName, "Name" );
_LIT8( KNSmlDMMMSMMRecep, "MMRecep" );
_LIT8( KNSmlDMMMSAMsg, "AMsg" );
_LIT8( KNSmlDMMMSRcvAds, "RcvAds" );
_LIT8( KNSmlDMMMSImgSize, "ImgSize" );
_LIT8( KNSmlDMMMSDRepSnd, "DRepSnd" );
_LIT8( KNSmlDMMMSRcvRep, "RcvRep" );
_LIT8( KNSmlDMMMSMsgVal, "MsgVal" );
_LIT8( KNSmlDMMMSCon, "Con" );
_LIT8( KNSmlDMMMSMToNapID, "MToNapID" );
_LIT8( KNSmlDMMMSMToNapIDL, "MToNapIDL" );
_LIT8( KNSmlDMMMSCreMode, "CreMode" );

_LIT8( KNSmlDMMMSConSlash, "Con/" );
_LIT8( KNSmlDMMMSTextPlain, "text/plain" );

_LIT8( KNSmlDMMMSNodeNameDescription, "The interior object holds all MMS objects" );
_LIT8( KNSmlDMMMSDynamicNodeDescription, "Placeholder for one set of MMS settings" );
_LIT8( KNSmlDMMMSNameDescription, "Displayable Account Name for the MMS Settings" );
_LIT8( KNSmlDMMMSMMRecepDescription, "Level of MMS protocol operations" );
_LIT8( KNSmlDMMMSAMsgDescription, "Anonymous messages allowed or not" );
_LIT8( KNSmlDMMMSRcvAdsDescription, "Receiving adverts allowed or not" );
_LIT8( KNSmlDMMMSImgSizeDescription, "Image  resolution in sending messages" );
_LIT8( KNSmlDMMMSDRepSndDescription, "Sending of Delivery Reports is allowed or not" );
_LIT8( KNSmlDMMMSRcvRepDescription, "Defines whether the delivery report is requested by default from each recipient or not" );
_LIT8( KNSmlDMMMSMsgValDescription, "Message validity time" );
_LIT8( KNSmlDMMMSMMSSAddrDescription, "MMS server address" );

_LIT8( KNSmlDMMMSConNodeDescription, "Parent for MMS connectivity object" );
_LIT8( KNSmlDMMMSConRtNodeDescription, "Place holder for one or more connectivity objects" );
_LIT8( KNSmlDMMMSMToNapIDDescription, "Parent to multiple NAP connection reference objects" );
_LIT8( KNSmlDMMMSMToNapIDLDescription, "Link to internet access point" );
_LIT8( KNSmlDMMMSCreModeDescription, "MMS creation mode" );
_LIT8( KNSmlDMMMSTextPlainDescription, "text/plain" );

_LIT8( KNSmlDMMMSAccount8, "MMSAcc" );
_LIT( KNSmlDMMMSAccount16, "MMSAcc" );
_LIT8( KNSmlDMMMSListOfLeafsLo, "MMRecep/CreMode/AMsg/RcvAds/ImgSize/DRepSnd/RcvRep/MsgVal/Con" );
_LIT8( KNSmlDMMMSListOfLeafsMid, "MMSSAddr/MToNapID" );
_LIT8( KNSmlDMMMSListOfLeafsHi, "MToNapIDL" );
_LIT8( KNSmlDMMMSConDynamicName8, "DCon" );
_LIT8( KNSmlDMMMSConDynamicName16, "DCon" );
_LIT8( KNSmlDMMMSMMSSAddr, "MMSSAddr" );

_LIT8( KNSmlDMMMSDynamicPrimary, "Primary" );


_LIT( KNSmlDMMMSSeparator16, "/" );
_LIT8( KNSmlDMMMSSeparator8, "/" );

_LIT8( KNSmlDMMMSValueTrue, "True" );
_LIT8( KNSmlDMMMSValueFalse, "False" );
_LIT8( KNSmlDMMMSValueAlwaysOn, "Always on" );

_LIT8( KNSmlDMMMSValueAutomaticAtHome, "Automatic at home" );
_LIT8( KNSmlDMMMSValueAutomaticAtHomeShort, "Aut" );
_LIT8( KNSmlDMMMSValueAlwaysManual, "Always Manual" );
_LIT8( KNSmlDMMMSValueAlwaysManual2, "Always manual" );  
_LIT8( KNSmlDMMMSValueOff, "Off" );
_LIT8( KNSmlDMMMSValueFree, "Free" );
_LIT8( KNSmlDMMMSValueRestricted, "Restricted" );
_LIT8( KNSmlDMMMSValueWarning, "Warning" );

_LIT8( KNSmlDMMMSValueLow, "Low" );
_LIT8( KNSmlDMMMSValueNormal, "Normal" );
_LIT8( KNSmlDMMMSValueHigh, "High" );
_LIT8( KNSmlDMMMSValueSmall, "Small" );
_LIT8( KNSmlDMMMSValueLarge, "Large" );
_LIT8( KNSmlDMMMSValueOriginal, "Original" );

_LIT8( KNSmlDMMMSValueHour, "one hour" );
_LIT8( KNSmlDMMMSValueSixHours, "six hours" );
_LIT8( KNSmlDMMMSValueDay, "24 hours" );
_LIT8( KNSmlDMMMSValueThreeDays, "three days" );
_LIT8( KNSmlDMMMSValueWeek, "a week" );

_LIT8( KNSmlDMMMSValueHourShort, "hour" );
_LIT8( KNSmlDMMMSValueSixHoursShort, "six" );
_LIT8( KNSmlDMMMSValueSixHoursShortNum, "6" );
_LIT8( KNSmlDMMMSValueDayShort, "24" );
_LIT8( KNSmlDMMMSValueThreeDaysShort, "three" );
_LIT8( KNSmlDMMMSValueThreeDaysShortNum, "3" );
_LIT8( KNSmlDMMMSValueWeekShort, "week" );
_LIT8( KNSmlDMMMSValueMaximumTimeShort, "max" );
_LIT8( KNSmlDMMMSValueMaximumTime , "maximum time" );

_LIT8( KNSmlDMMMSValuePrimary, "Primary" );
_LIT8( KNSmlDMMMSValueSecondary, "Secondary" );
_LIT8( KNSmlMMSUriDotSlash, "./");
_LIT8( KNSmlDMMMSIAPUri, "AP" );

_LIT8( KNSmlDMMMSLevel3Parents, "MMS/MMSAcc" );
_LIT8( KNSmlDMMMSLevel5Parents, "MMS/MMSAcc/Con/DCon" );
_LIT8( KNSmlDMMMSLevel7Parents1, "MMS/MMSAcc/Con/DCon/MToNapID/Primary" );
_LIT8( KNSmlDMMMSLevel7Parents2, "MMS/MMSAcc/Con/DCon/MToNapID/Secondary" );

_LIT8( KNSmlDMMMSAclName, "MMS/MMSAcc/Name" );
_LIT8( KNSmlDMMMSAclMMRecep, "MMS/MMSAcc/MMRecep" );
_LIT8( KNSmlDMMMSAclCreMode, "MMS/MMSAcc/CreMode" );
_LIT8( KNSmlDMMMSAclAMsg, "MMS/MMSAcc/AMsg" );
_LIT8( KNSmlDMMMSAclRcvAds, "MMS/MMSAcc/RcvAds" );
_LIT8( KNSmlDMMMSAclImgSize, "MMS/MMSAcc/ImgSize" );
_LIT8( KNSmlDMMMSAclDRepSnd, "MMS/MMSAcc/DRepSnd" );
_LIT8( KNSmlDMMMSAclRcvRep, "MMS/MMSAcc/RcvRep" );
_LIT8( KNSmlDMMMSAclMsgVal, "MMS/MMSAcc/MsgVal" );
_LIT8( KNSmlDMMMSAclMMSSAddr, "MMS/MMSAcc/Con/DCon/MMSSAddr" );
_LIT8( KNSmlDMMMSAclPrimaryMToNapIDL, "MMS/MMSAcc/Con/DCon/MToNapID/Primary/MToNapIDL" );

_LIT8(KNSmlDmMMSAclDefault, "Get=*&Replace=*");
_LIT8(KNSmlDmMMSAclGet, "Get=*");

/**
* The main class of the SyncML DM MMS adapter.
*
*  @since 
*/

class CNSmlDmMMSAdapter : public CSmlDmAdapter, public MMsvSessionObserver
    {
public:
    static CNSmlDmMMSAdapter* NewL(MSmlDmCallback* aDmCallback );

    ~CNSmlDmMMSAdapter();
    
    // Adapter interface from CSmlDmAdapter
    void DDFVersionL( CBufBase& aDDFVersion );
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                            const TDesC8& aObject, const TDesC8& aType, 
                            TInt aStatusRef );
    void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                        const TInt aStatusRef ); 
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
                          const TDesC8& aArgument, const TDesC8& aType, 
                          const TInt aStatusRef );
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, 
                          RWriteStream*& aStream, const TDesC8& aType, 
                          const TInt aStatusRef );
    void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, 
                       const TDesC8& aSourceURI, const TDesC8& aSourceLUID, 
                       const TDesC8& aType, TInt aStatusRef );
    void StartAtomicL();
    void CommitAtomicL();
    void RollbackAtomicL();
    TBool StreamingSupport( TInt& aItemSize );
    void StreamCommittedL();
    void CompleteOutstandingCmdsL();
        
    // from MMsvSessionObserver
    void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, 
                              TAny* aArg3 );

private:
/**
 * Default constructor
 * @param   aDmCallback Pointer to callback interface
 * @return    
 */
    CNSmlDmMMSAdapter( MSmlDmCallback* aDmCallback );

/**
* Second level constructor
* @param    
* @return    
*/
    void ConstructL();

/**
* Converts a descriptor to integer.
* @param    aLuid Descriptor to convert.
* @return    TInt value
*/
    TInt DesToInt( const TDesC& aLuid );
/**
* Removes the last uri segment, ie. everything after and including  the last "/" character.
* @param    aURI The URI to process.
* @return    The modified aURI
*/

    TPtrC8 RemoveLastURISeg( const TDesC8& aURI );
/**
* Calculates the number of the URI segments.
* @param    aURI URI to process.
* @return    The number of segments.
*/

    TInt NumOfURISegs( const TDesC8& aURI );
/**
* Parses the last segment of URI, ie everything after the last "/" character.
* @param    aURI The URI to parse.
* @return    The last segment of the URI.
*/

    TPtrC8 LastURISeg( const TDesC8& aURI );
/**
* Performs the actual UPDATE operation.
* @param    aURI The URI to update.
* @param    aParentLUID LUID of the object.
* @param    aObject The new data to update.
* @param    aStatusRef The command reference.
* @return    Status of the UPDATE operation
*/

    CSmlDmAdapter::TError UpdateDataL( const TDesC8& aURI,
                                       const TDesC8& aParentLUID, 
                                       const TDesC8& aObject, 
                                       TInt aStatusRef );
/**
* Replaces accesspoint in iMmsSettings's accesspoint array.
* @param    aIapId The new IAP id to update.
* @param    aPos A position to update IAP id
* @return    
*/

    void ReplaceAPL( TInt aIapId, TInt aPos );
/**
* Performs updating of MMSAddr field.
* @param    
* @return   Status of the update. 
*/

    CSmlDmAdapter::TError UpdateMMSAddrL();
/**
* Retrieves the paramters to iMmsSettings class from phones permanent storage.
* @param  
* @return    
*/

    void CreateMMSClientL();

/**
* Updates the parameters of given DDF node
* @param    aNode The node to update.
* @param    aAccTypes Access types of the node.
* @param    aOccurrence Occurrance of the node.
* @param    aScope Scope of the node.
* @param    aFormat Format of the node.
* @param    aDescription A description of the node.
* @return    
*/

    void FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
                        MSmlDmDDFObject::TOccurence aOccurrence, 
                        MSmlDmDDFObject::TScope aScope, 
                        MSmlDmDDFObject::TDFFormat aFormat,
                        const TDesC8& aDescription);

/**
* Fetches a value from iMmsSettings class.
* @param    aURI The URI to fetch
* @param    aObject The container for the result.
* @return    Status of the operation.
*/

    CSmlDmAdapter::TError FetchObjectL( const TDesC8& aURI, CBufBase& aObject );
/**
* Stores the contents of iMmsSettings class to phones permanen storage.
* @param    
* @return    
*/

    void StoreSettingsL();
/**
* Checks the URI validity.
* @param    aURI The URI to check.
* @return    Status of the operation.
*/

    CSmlDmAdapter::TError ValidateUriL(const TDesC8& aURI );
    
    TBool IsWAPAccessPointRecordLockedL(TUint32 aRecord);

/**
* Gets the IAP ID using CMManager
* @param    aUid the accesspointID
* @return   the IAP ID 
*/
    TUint32 GetIapIdL( TUint32 aUid );
    
    
	  TInt DoProtectWAPAccessRecordL(TUint32 aRecord,TBool aProtect);
private:
    CClientMtmRegistry* iClientMtmRegistry;
    CMsvSession* iMsvSession;
    CMmsClientMtm *iMmsClient;
    CMmsSettings* iMmsSettings;
    MSmlDmCallback* iCallback;
    TInt iStatusRef;
    HBufC8 *iMMSSAddr;
    TInt iImgSizeStatusRef;
    CCommsDatabase* iDatabase;
    };

#endif // __NSMLDMMMSADAPTER_H__

// End of File
