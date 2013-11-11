/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Global WLAN settings adapter
*
*/



#ifndef __GLOBALWLANADAPTER_H__
#define __GLOBALWLANADAPTER_H__


// -----------------------------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------------------------
#include <utf.h>
#include <commdb.h>
#include <EapSettings.h>
#include <internetconnectivitycrkeys.h>
#include <smldmadapter.h>


// -----------------------------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------------------------

// The nodes of the DDF structure
_LIT8( KNSmlWLan, "WLAN" );
_LIT8( KNSmlWLanInternetConnectivityTest, "InternetConnectivityTest" );
_LIT8( KNSmlWLanUseDefaultSettings, "UseDefaultSettings" );
_LIT8( KNSmlWLanLongRetryLimit, "LongRetryLimit" );
_LIT8( KNSmlWLanShortRetryLimit, "ShortRetryLimit" );
_LIT8( KNSmlWLanRTSThreshold, "RTSThreshold" );
_LIT8( KNSmlWLanTXPowerLevel, "TXPowerLevel" );
_LIT8( KNSmlWLanPowerSaving, "PowerSaving" );
_LIT8( KNSmlWLanBackgroundScanInterval, "BackgroundScanInterval" );
_LIT8( KNSmlWLanScanRate, "ScanRate" );
_LIT8( KNSmlWLanRCPITrigger, "RCPITrigger" );
_LIT8( KNSmlWLanMinActiveChannelTime, "MinActiveChannelTime" );
_LIT8( KNSmlWLanMaxActiveChannelTime, "MaxActiveChannelTime" );
_LIT8( KNSmlWLanMaxTxMSDULifeTime, "MaxTxMSDULifeTime" );
_LIT8( KNSmlWLanScanExpirationTimer, "ScanExpirationTimer" );
_LIT8( KNSmlWLanUnloadDriverTimer, "UnloadDriverTimer" );
_LIT8( KNSmlWLanRoamTimer, "RoamTimer" );
_LIT8( KNSmlWLanRCPIDifference, "RCPIDifference" );
_LIT8( KNSmlWLanConnRegainTimer, "ConnRegainTimer" );
_LIT8( KNSmlWLanMaxTriesToFindNw, "MaxTriesToFindNw" );
_LIT8( KNSmlWLanDelayBetweenFindNw, "DelayBetweenFindNw" );
_LIT8( KNSmlWLanAllowRadioMeasurements, "AllowRadioMeasurements" );
_LIT8( KNSmlWLanMinPassiveChannelTime, "MinPassiveChannelTime" );
_LIT8( KNSmlWLanMaxPassiveChannelTime, "MaxPassiveChannelTime" );
_LIT8( KNSmlWLanMaxApFailureCount, "MaxApFailureCount" );
_LIT8( KNSmlWLanLongBeaconFindCount, "LongBeaconFindCount" );
_LIT8( KNSmlWLanQosNullFrameInterval, "QosNullFrameInterval" );
_LIT8( KNSmlWLanQosNullFrameTimeout, "QosNullFrameTimeout" );
_LIT8( KNSmlWLanMTU, "MTU" );
_LIT8( KNSmlWLanNode, "InternetConnectivityTest/UseDefaultSettings/LongRetryLimit/ShortRetryLimit/RTSThreshold/TXPowerLevel/PowerSaving/BackgroundScanInterval/ScanRate/RCPITrigger/MinActiveChannelTime/MaxActiveChannelTime/MaxTxMSDULifeTime/ScanExpirationTimer/UnloadDriverTimer/RoamTimer/RCPIDifference/ConnRegainTimer/MaxTriesToFindNw/DelayBetweenFindNw/AllowRadioMeasurements/MinPassiveChannelTime/MaxApFailureCount/LongBeaconFindCount/QosNullFrameInterval/QosNullFrameTimeout" );

//DDF version number
_LIT8( KNSmGlobalWlanAdapterDDFversion, "1.0");
_LIT8( KNSmlMimeType, "text/plain" );
_LIT8( KNSmlEmpty, "");


const TInt KUriSegListLength = 128;
// Maximum input size for nodes
const TInt KInputMaxLength = 50;
// Max lenght of the DDF type
const TInt KTypeMaxLength = 40;
// Repository id
const TUid KCRUidWlanDeviceSettingsRegistryId = {0x101f8e44};


// WLan engine central repository parameters
const TUint32 KWlanScanRate =                            0x00000004;
const TUint32 KWlanRcpiTrigger =                         0x00000005;
const TUint32 KWlanMinActiveChannelTime =                0x00000006;
const TUint32 KWlanMaxActiveChannelTime =                0x00000007;
const TUint32 KWlanMaxTxMSDULifeTime =                   0x00000008;
const TUint32 KWlanScanExpirationTimer =                 0x00000009;
const TUint32 KWlanUnloadDriverTimer =                   0x0000000A;
const TUint32 KWlanRoamTimer =                           0x0000000B;
const TUint32 KWlanRcpiDifference =                      0x0000000C;
const TUint32 KWlanConnRegainTimer =                     0x0000000D;
const TUint32 KWlanMaxTriesToFindNw =                    0x0000000E;
const TUint32 KWlanDelayBetweenFindNw =                  0x0000000F;
const TUint32 KWlanMTU =								 0x00000010;
const TUint32 KWlanMinPassiveChannelTime =               0x00000011;
const TUint32 KWlanMaxPassiveChannelTime =               0x00000012;
const TUint32 KWlanMaxApFailureCount =                   0x00000013;
const TUint32 KWlanLongBeaconFindCount =                 0x00000014;
const TUint32 KWlanQosNullFrameInterval =                0x00000015;
const TUint32 KWlanQosNullFrameTimeout =                 0x00000016;

// -----------------------------------------------------------------------------------------------
// Structures
// -----------------------------------------------------------------------------------------------

// Structure for storing the settings to be stored into cenrep and commsdb
struct TGlobalSettings
{
	TUint32 internetConnectivityTest;
	TBool internetConnectivityTestPresent;
	TBool useDefaultSettings;
	TBool useDefaultSettingsPresent;
	TUint32 longRetryLimit;
	TBool longRetryLimitPresent;
	TUint32 shortRetryLimit;
	TBool shortRetryLimitPresent;
	TUint32 RTSThreshold;
	TBool RTSThresholdPresent;
	TUint32 TXPowerLevel;
	TBool TXPowerLevelPresent;
	TBool powerSaving;
	TBool powerSavingPresent;
	TUint32 backgroundScanInterval;
	TBool backgroundScanIntervalPresent;
	TUint32 scanRate;
	TBool scanRatePresent;
	TUint32 RCPITrigger;
	TBool RCPITriggerPresent;
	TUint32 minActiveChannelTime;
	TBool minActiveChannelTimePresent;
	TUint32 maxActiveChannelTime;
	TBool maxActiveChannelTimePresent;
	TUint32 maxTxMSDULifeTime;
	TBool maxTxMSDULifeTimePresent;
	TUint32 scanExpirationTimer;
	TBool scanExpirationTimerPresent;
	TUint32 unloadDriverTimer;
	TBool unloadDriverTimerPresent;
	TUint32 roamTimer;
	TBool roamTimerPresent;
	TUint32 RCPIDifference;
	TBool RCPIDifferencePresent;
	TUint32 connRegainTimer;
	TBool connRegainTimerPresent;
	TUint32 maxTriesToFindNw;
	TBool maxTriesToFindNwPresent;
	TUint32 delayBetweenFindNw;
	TBool delayBetweenFindNwPresent;
	TBool allowRadioMeasurements;
	TBool allowRadioMeasurementsPresent;
	TUint32 minPassiveChannelTime;
	TBool minPassiveChannelTimePresent;
	TUint32 maxPassiveChannelTime;
	TBool maxPassiveChannelTimePresent;
	TUint32 maxApFailureCount;
	TBool maxApFailureCountPresent;
	TUint32 longBeaconFindCount;
	TBool longBeaconFindCountPresent;
	TUint32 qosNullFrameInterval;
	TBool qosNullFrameIntervalPresent;
	TUint32 qosNullFrameTimeout;
	TBool qosNullFrameTimeoutPresent;
	TUint32 MTU;
	TBool MTUPresent;
};

// Structure for storing the result and status variables from every addition/fetch command
struct TStatusResult
{
	TInt internetConnectivityTestStatus;
	TInt internetConnectivityTestResult;
	TInt useDefaultSettingsStatus;
	TInt useDefaultSettingsResult;
	TInt longRetryLimitStatus;
	TInt longRetryLimitResult;
	TInt shortRetryLimitStatus;
	TInt shortRetryLimitResult;
	TInt RTSThresholdStatus;
	TInt RTSThresholdResult;
	TInt TXPowerLevelStatus;
	TInt TXPowerLevelResult;
	TInt powerSavingStatus;
	TInt powerSavingResult;
	TInt backgroundScanIntervalStatus;
	TInt backgroundScanIntervalResult;
	TInt scanRateStatus;
	TInt scanRateResult;
	TInt RCPITriggerStatus;
	TInt RCPITriggerResult;
	TInt minActiveChannelTimeStatus;
	TInt minActiveChannelTimeResult;
	TInt maxActiveChannelTimeStatus;
	TInt maxActiveChannelTimeResult;
	TInt maxTxMSDULifeTimeStatus;
	TInt maxTxMSDULifeTimeResult;
	TInt scanExpirationTimerStatus;
	TInt scanExpirationTimerResult;
	TInt unloadDriverTimerStatus;
	TInt unloadDriverTimerResult;
	TInt roamTimerStatus;
	TInt roamTimerResult;
	TInt RCPIDifferenceStatus;
	TInt RCPIDifferenceResult;
	TInt connRegainTimerStatus;
	TInt connRegainTimerResult;
	TInt maxTriesToFindNwStatus;
	TInt maxTriesToFindNwResult;
	TInt delayBetweenFindNwStatus;
	TInt delayBetweenFindNwResult;
	TInt allowRadioMeasurementsStatus;
	TInt allowRadioMeasurementsResult;
	TInt minPassiveChannelTimeStatus;
	TInt minPassiveChannelTimeResult;
	TInt maxPassiveChannelTimeStatus;
	TInt maxPassiveChannelTimeResult;
	TInt maxApFailureCountStatus;
	TInt maxApFailureCountResult;
	TInt longBeaconFindCountStatus;
	TInt longBeaconFindCountResult;
	TInt qosNullFrameIntervalStatus;
	TInt qosNullFrameIntervalResult;
	TInt qosNullFrameTimeoutStatus;
	TInt qosNullFrameTimeoutResult;
	TInt MTUStatus;
	TInt MTUResult;
};


// -----------------------------------------------------------------------------------------------
// Enumerations
// -----------------------------------------------------------------------------------------------
        
   
// ------------------------------------------------------------------------------------------------
// CGlobalWLanAdapter 
// ------------------------------------------------------------------------------------------------
class CGlobalWLANAdapter : public CSmlDmAdapter
        {
public:
        static CGlobalWLANAdapter* NewL( MSmlDmCallback* aDmCallback );
        static CGlobalWLANAdapter* NewLC( MSmlDmCallback* aDmCallback );
        void ConstructL( );
        virtual ~CGlobalWLANAdapter();

// Adapter interface
        void DDFVersionL( CBufBase& aDDFVersion );
        void DDFStructureL( MSmlDmDDFObject& aDDF );
        void AddLeafObjectL( const TDesC8& aURI,
                             const TDesC8& aParentLUID, 
                             const TDesC8& aObject, 
                             const TDesC8& aType, 
                             const TInt aStatusRef );  
        void UpdateLeafObjectL( const TDesC8& aURI, 
                                const TDesC8& aLUID, 
                            const TDesC8& aObject, 
                            const TDesC8& aType, 
                            const TInt aStatusRef );
       void UpdateLeafObjectL( const TDesC8& aURI, 
                            const TDesC8& aLUID,
                            RWriteStream*& aStream, 
                            const TDesC8& aType,
                            TInt aStatusRef );
        void DeleteObjectL( const TDesC8& aURI, 
                            const TDesC8& aLUID, 
                            const TInt aStatusRef ); 
        void FetchLeafObjectL( const TDesC8& aURI, 
                               const TDesC8& aLUID, 
                               const TDesC8& aType, 
                               const TInt aResultsRef, 
                               const TInt aStatusRef ); 
        void FetchLeafObjectSizeL( const TDesC8& aURI, 
                               const TDesC8& aLUID,
                               const TDesC8& aType, 
                               TInt aResultsRef,
                               TInt aStatusRef );
        void ChildURIListL( const TDesC8& aURI, 
                            const TDesC8& aLUID, 
                            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                            const TInt aResultsRef, 
                            const TInt aStatusRef ); 
        void AddNodeObjectL( const TDesC8& aURI, 
                             const TDesC8& aParentLUID, 
                             const TInt aStatusRef );
        void ExecuteCommandL( const TDesC8& aURI, 
                              const TDesC8& aLUID,
                              const TDesC8& aArgument, 
                              const TDesC8& aType,
                              TInt aStatusRef );
        void ExecuteCommandL( const TDesC8& aURI, 
                              const TDesC8& aLUID,
                              RWriteStream*& aStream, 
                              const TDesC8& aType,
                              TInt aStatusRef );
        void CopyCommandL( const TDesC8& aTargetURI, 
                           const TDesC8& aTargetLUID, 
                           const TDesC8& aSourceURI,
                           const TDesC8& aSourceLUID, 
                           const TDesC8& aType,
                           TInt aStatusRef );
        void StartAtomicL();
        void CommitAtomicL();
        void RollbackAtomicL();
        TBool StreamingSupport( TInt& aItemSize );
        void StreamCommittedL();
        void CompleteOutstandingCmdsL();


private:
        CGlobalWLANAdapter(TAny* aEcomArguments);
        void FillNodeInfoL( MSmlDmDDFObject& aNode,
                            TSmlDmAccessTypes aAccTypes,
                            MSmlDmDDFObject::TOccurence aOccurrence, 
                            MSmlDmDDFObject::TScope aScope, 
                            MSmlDmDDFObject::TDFFormat aFormat,
                            const TDesC8& aDescription );
        void ExecuteBufferL( TBool aFinal = EFalse ); 
        
        /**
     	* Stores the central repository parameters
     	*
     	* @return 
     	*/
        void StoreCenRepParamsL();
        
        /**
     	* Stores the CommsDB parameters
     	*
     	* @return 
     	*/
  		void StoreCommsDBParamsL();
  			
  		/**
     	* Reads the central repository parameters
     	*
     	* @return 
     	*/
        void ReadCenRepParamsL();
        
        /**
     	* Returns the central repository parameters and updates statuses
     	*
     	* @return 
     	*/
        void UpdateStatusAndResultL( TInt aResult, TInt aStatus, TDesC8& aValue, TDesC8& aType, TInt aStoreErr );
        
        /**
     	* Reads the CommsDB parameters
     	*
     	* @return 
     	*/
  		void ReadCommsDBParamsL();
                          
        /**
     	* Gets the number of uri segments in the URI
     	*
     	* @since S60 ?S60_version
     	* @param aUri The URI whose segments are counted
     	* @return The amount of segments
     	*/
        TInt NumOfUriSegs( const TDesC8& aUri );
        
        /**
     	* Gets the last uri segment
     	*
     	* @since S60 ?S60_version
     	* @param aUri The URI whose segments are counted
     	* @return The last segment
     	*/        
        TPtrC8 GetLastUriSeg(const TDesC8& aURI);
        
        TInt DesToInt( const TDesC8& aLuid );
        void InitMemberVariables();

        MSmlDmCallback* iCallBack;
        
        // Variable to store the received parameters
        TGlobalSettings* iWriteSettings;
        // Variable to store whick parameters are to be fetched
        TGlobalSettings* iReadSettings;
        // Status and result information related to fetches and additions
        TStatusResult* iReadStatuses;
        TStatusResult* iWriteStatuses;
        
        TBool iWlanSupported;

        };

#endif __GLOBALWLANADAPTER_H__
