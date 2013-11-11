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
* Description:   Global WLAN settings Adapter
*
*/



#include "globalwlanadapter.h"
#include "nsmldebug.h"
#include <implementationproxy.h> // For TImplementationProxy definition
#include <commdb.h>
#include <WlanCdbCols.h>
#include <utf.h>
#include <cdbstore.h>
#include <cdbcols.h>
#include <f32file.h>
#include <e32const.h>
#include <s32strm.h>
#include <e32base.h>
#include <sysutil.h>
#include <e32cmn.h>
#include <featmgr.h>

#include <centralrepository.h>
#include <commsdattypesv1_1.h>


#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(0x1315DBE, CGlobalWLANAdapter::NewL)
    };

//------------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
//------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CGlobalWLANAdapter: begin");

    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    _DBG_FILE("ImplementationGroupProxy() for CGlobalWLANAdapter: end");
    return ImplementationTable;
    }


//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::NewL( )
//-----------------------------------------------------------------------------
CGlobalWLANAdapter* CGlobalWLANAdapter::NewL( MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CGlobalWLANAdapter::NewL(): begin");

    CGlobalWLANAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop( self );

    _DBG_FILE("CGlobalWLANAdapter::NewL(): end");
    return self;
    }

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::NewLC( )
//-----------------------------------------------------------------------------
CGlobalWLANAdapter* CGlobalWLANAdapter::NewLC( MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CGlobalWLANAdapter::NewLC(): begin");
    
    CGlobalWLANAdapter* self = new(ELeave) CGlobalWLANAdapter(aDmCallback);
    CleanupStack::PushL( self );

    self->iCallBack = aDmCallback;
    _DBG_FILE("CGlobalWLANAdapter::NewLC(): call constructL");
    self->ConstructL( );

    _DBG_FILE("CGlobalWLANAdapter::NewLC(): end");
    return self;
    }

//-----------------------------------------------------------------------------
// void CGlobalWLAdapter::ConstructL( )
// Second phase constructor
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::ConstructL(  )
	{
    _DBG_FILE("CGlobalWLANAdapter::ConstructL(): begin");
    
    // checks if Wlan feature is supported
    FeatureManager::InitializeLibL();
    iWlanSupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    FeatureManager::UnInitializeLib();
    
    iWriteSettings = new(ELeave) TGlobalSettings;
    iReadSettings = new(ELeave) TGlobalSettings;
    iReadStatuses = new(ELeave) TStatusResult;
    iWriteStatuses = new(ELeave) TStatusResult;
    
    // set "present" variables to false, since symbian inits thems as true
    InitMemberVariables();
     
    _DBG_FILE("CGlobalWLANAdapter::ConstructL(): end");
    }

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter::CGlobalWLANAdapter( )
// Constructor
//-----------------------------------------------------------------------------
CGlobalWLANAdapter::CGlobalWLANAdapter( TAny* aEcomArguments )
    : CSmlDmAdapter(aEcomArguments)
    {
    _DBG_FILE("CGlobalWLANAdapter::CGlobalWLANAdapter(): begin");
    _DBG_FILE("CGlobalWLANAdapter::CGlobalWLANAdapter(): end");
    }

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter::~CGlobalWLANAdapter( )
// Destructor
//-----------------------------------------------------------------------------
CGlobalWLANAdapter::~CGlobalWLANAdapter( )
    {
    _DBG_FILE("CGlobalWLANAdapter::~CGlobalWLANAdapter(): begin");
    
    delete iWriteSettings;
    delete iReadSettings;
    delete iReadStatuses;
    delete iWriteStatuses;
    
    _DBG_FILE("CGlobalWLANAdapter::~CGlobalWLANAdapter(): end");
    }


//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::DDFVersionL( CBufBase& aDDFVersion )
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    _DBG_FILE("CGlobalWLANAdapter::DDFVersionL(): begin");
    
    aDDFVersion.InsertL( 0, KNSmGlobalWlanAdapterDDFversion );
    
    _DBG_FILE("CGlobalWLANAdapter::DDFVersionL(): end");
    }

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    _DBG_FILE("CGlobalWLANAdapter::DDFStructureL(): begin");
    if( !iWlanSupported )
        {
        _DBG_FILE("CGlobalWLANAdapter::DDFStructureL(): WLAN not supported.");
        return;
        }  
//
// Set Get, add and replace as acceptable operations
//

    TSmlDmAccessTypes accessTypesGetAddReplace;     
    accessTypesGetAddReplace.SetAdd();
    accessTypesGetAddReplace.SetGet();
    accessTypesGetAddReplace.SetReplace();
    
    //WLAN
    MSmlDmDDFObject& rootWLan = aDDF.AddChildObjectL( KNSmlWLan );
    FillNodeInfoL( rootWLan,
               	accessTypesGetAddReplace,
               	MSmlDmDDFObject::EZeroOrOne,
               	MSmlDmDDFObject::EDynamic,
               	MSmlDmDDFObject::ENode,
               	KNSmlEmpty );

    //WLAN/InternetConnectivityTest
    MSmlDmDDFObject& internetConnectivityTest = rootWLan.AddChildObjectL( KNSmlWLanInternetConnectivityTest );
    FillNodeInfoL( internetConnectivityTest,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    
    //WLAN/UseDefaultSettings
    MSmlDmDDFObject& useDefaultSettings = rootWLan.AddChildObjectL( KNSmlWLanUseDefaultSettings );
    FillNodeInfoL( useDefaultSettings,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EBool,
    			KNSmlEmpty );

     //WLAN/LongRetryLimit
    MSmlDmDDFObject& longRetryLimit = rootWLan.AddChildObjectL( KNSmlWLanLongRetryLimit );
    FillNodeInfoL( longRetryLimit,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/ShortRetryLimit
    MSmlDmDDFObject& shortRetryLimit = rootWLan.AddChildObjectL( KNSmlWLanShortRetryLimit );
    FillNodeInfoL( shortRetryLimit,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    			
    //WLAN/RTSThreshold
    MSmlDmDDFObject& RTSThreshold = rootWLan.AddChildObjectL( KNSmlWLanRTSThreshold );
    FillNodeInfoL( RTSThreshold,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/TXPowerLevel
    MSmlDmDDFObject& TXPowerLevel = rootWLan.AddChildObjectL( KNSmlWLanTXPowerLevel );
    FillNodeInfoL( TXPowerLevel,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/PowerSaving
    MSmlDmDDFObject& powerSaving = rootWLan.AddChildObjectL( KNSmlWLanPowerSaving );
    FillNodeInfoL( powerSaving,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EBool,
    			KNSmlEmpty );
    			
    //WLAN/BackgroundScanInterval
    MSmlDmDDFObject& backgroundScanInterval = rootWLan.AddChildObjectL( KNSmlWLanBackgroundScanInterval );
    FillNodeInfoL( backgroundScanInterval,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/ScanRate
    MSmlDmDDFObject& scanRate = rootWLan.AddChildObjectL( KNSmlWLanScanRate );
    FillNodeInfoL( scanRate,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/RCPITrigger
    MSmlDmDDFObject& RCPITrigger = rootWLan.AddChildObjectL( KNSmlWLanRCPITrigger );
    FillNodeInfoL( RCPITrigger,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/MinActiveChannelTime
    MSmlDmDDFObject& minActiveChannelTime = rootWLan.AddChildObjectL( KNSmlWLanMinActiveChannelTime );
    FillNodeInfoL( minActiveChannelTime,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
     //WLAN/MaxActiveChannelTime
    MSmlDmDDFObject& maxActiveChannelTime = rootWLan.AddChildObjectL( KNSmlWLanMaxActiveChannelTime );
    FillNodeInfoL( maxActiveChannelTime,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
     //WLAN/MaxTxMSDULifeTime
    MSmlDmDDFObject& maxTxMSDULifeTime = rootWLan.AddChildObjectL( KNSmlWLanMaxTxMSDULifeTime );
    FillNodeInfoL( maxTxMSDULifeTime,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/ScanExpirationTimer
    MSmlDmDDFObject& scanExpirationTimer = rootWLan.AddChildObjectL( KNSmlWLanScanExpirationTimer );
    FillNodeInfoL( scanExpirationTimer,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/UnloadDriverTimer
    MSmlDmDDFObject& unloadDriverTimer = rootWLan.AddChildObjectL( KNSmlWLanUnloadDriverTimer );
    FillNodeInfoL( unloadDriverTimer,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/RoamTimer
    MSmlDmDDFObject& roamTimer = rootWLan.AddChildObjectL( KNSmlWLanRoamTimer );
    FillNodeInfoL( roamTimer,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/KNSmlWLanRCPIDifference
    MSmlDmDDFObject& RCPIDifference = rootWLan.AddChildObjectL( KNSmlWLanRCPIDifference );
    FillNodeInfoL( RCPIDifference,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/ConnRegainTimer
    MSmlDmDDFObject& connRegainTimer = rootWLan.AddChildObjectL( KNSmlWLanConnRegainTimer );
    FillNodeInfoL( connRegainTimer,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/MaxTriesToFindNw
    MSmlDmDDFObject& maxTriesToFindNw = rootWLan.AddChildObjectL( KNSmlWLanMaxTriesToFindNw );
    FillNodeInfoL( maxTriesToFindNw,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/DelayBetweenFindNw
    MSmlDmDDFObject& delayBetweenFindNw = rootWLan.AddChildObjectL( KNSmlWLanDelayBetweenFindNw );
    FillNodeInfoL( delayBetweenFindNw,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/AllowRadioMeasurements
    MSmlDmDDFObject& allowRadioMeasurements = rootWLan.AddChildObjectL( KNSmlWLanAllowRadioMeasurements );
    FillNodeInfoL( allowRadioMeasurements,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EBool,
    			KNSmlEmpty );

    //WLAN/MinPassiveChannelTime
    MSmlDmDDFObject& minPassiveChannelTime = rootWLan.AddChildObjectL( KNSmlWLanMinPassiveChannelTime );
    FillNodeInfoL( minPassiveChannelTime,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/MaxPassiveChannelTime
    MSmlDmDDFObject& maxPassiveChannelTime = rootWLan.AddChildObjectL( KNSmlWLanMaxPassiveChannelTime );
    FillNodeInfoL( maxPassiveChannelTime,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/MaxApFailureCount
    MSmlDmDDFObject& maxApFailureCount = rootWLan.AddChildObjectL( KNSmlWLanMaxApFailureCount );
    FillNodeInfoL( maxApFailureCount,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/LongBeaconFindCount
    MSmlDmDDFObject& longBeaconFindCount = rootWLan.AddChildObjectL( KNSmlWLanLongBeaconFindCount );
    FillNodeInfoL( longBeaconFindCount,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );
    			
    //WLAN/QosNullFrameInterval
    MSmlDmDDFObject& qosNullFrameInterval = rootWLan.AddChildObjectL( KNSmlWLanQosNullFrameInterval );
    FillNodeInfoL( qosNullFrameInterval,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/QosNullFrameTimeout
    MSmlDmDDFObject& qosNullFrameTimeout = rootWLan.AddChildObjectL( KNSmlWLanQosNullFrameTimeout );
    FillNodeInfoL( qosNullFrameTimeout,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );

    //WLAN/MTU
    MSmlDmDDFObject& MTU = rootWLan.AddChildObjectL( KNSmlWLanMTU );
    FillNodeInfoL( MTU,
    			accessTypesGetAddReplace,
    			MSmlDmDDFObject::EZeroOrOne,
    			MSmlDmDDFObject::EDynamic,
    			MSmlDmDDFObject::EInt,
    			KNSmlEmpty );


    _DBG_FILE("CGlobalWLANAdapter::DDFStructureL(): end");
    }
    
      

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::AddLeafObjectL( const TDesC& aURI,const 
// TDesC& aParentLUID, const TDesC8& aObject, const TDesC& aType, 
// const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::AddLeafObjectL( const TDesC8& aURI,
                                       const TDesC8& aParentLUID, 
                                       const TDesC8& aObject, 
                                       const TDesC8& /*aType*/, 
                                       const TInt aStatusRef )
    {
    DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL - <%S> <%S>"), &aURI, &aParentLUID );

    // Get the last uri segment and the number of uri items in the input
    TInt uriSegs = NumOfUriSegs( aURI );
    TInt err(KErrNone);
    TPtrC8 lastUriSeg = GetLastUriSeg( aURI );
    
    //**************************************************************************
    // Check which leaf is in question
    //**************************************************************************
    if( aURI.Match( _L8("WLAN/*" ) ) != KErrNotFound && uriSegs==2 )
    	{  
      	// *****************************************************************
      	if( lastUriSeg.Match( KNSmlWLanInternetConnectivityTest ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      	    	//store the parameter
      	    	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->internetConnectivityTest = err;
      	    		iWriteSettings->internetConnectivityTestPresent = ETrue;
      	    		iWriteStatuses->internetConnectivityTestStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanUseDefaultSettings ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		    //store the parameter 
      		    if( aObject.MatchF( _L8("True")) != KErrNotFound ||
      		    	aObject.MatchF( _L8("true")) != KErrNotFound )
                	{
                	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
                	iWriteSettings->useDefaultSettings = ETrue;
                	iWriteSettings->useDefaultSettingsPresent = ETrue;
                	iWriteStatuses->useDefaultSettingsStatus = aStatusRef;
                	}
            	else if ( aObject.MatchF( _L8("False")) != KErrNotFound ||
            			  aObject.MatchF( _L8("false")) != KErrNotFound )
                	{
                	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
                	iWriteSettings->useDefaultSettings = EFalse;
                	iWriteSettings->useDefaultSettingsPresent = ETrue;
                	iWriteStatuses->useDefaultSettingsStatus = aStatusRef;
                	}
              	else 
              		{
              		err = KErrArgument;
              		}
            	}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanLongRetryLimit ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      	    	//store the parameter
      	    	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->longRetryLimit = err;
      	    		iWriteSettings->longRetryLimitPresent = ETrue;
      	    		iWriteStatuses->longRetryLimitStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanShortRetryLimit ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
				//store the parameter
				DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->shortRetryLimit = err;
      	    		iWriteSettings->shortRetryLimitPresent = ETrue;
      	    		iWriteStatuses->shortRetryLimitStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanRTSThreshold ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      			DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->RTSThreshold = err;
      	    		iWriteSettings->RTSThresholdPresent = ETrue;
      	    		iWriteStatuses->RTSThresholdStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanTXPowerLevel ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      			DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      		  	//store the parameter
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0)
      	    		{
      	    		iWriteSettings->TXPowerLevel = err;
      	    		iWriteSettings->TXPowerLevelPresent = ETrue;
      	    		iWriteStatuses->TXPowerLevelStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
       	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanPowerSaving ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter 
      		  	if( aObject.MatchF( _L8("True")) != KErrNotFound ||
      		  		aObject.MatchF( _L8("true")) != KErrNotFound )
              		{
              		DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
              		iWriteSettings->powerSaving = ETrue;
              		iWriteSettings->powerSavingPresent = ETrue;
              		iWriteStatuses->powerSavingStatus = aStatusRef;
              		}
           		else if ( aObject.MatchF( _L8("False")) != KErrNotFound ||
           				  aObject.MatchF( _L8("false")) != KErrNotFound )
              		{
              		DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
              		iWriteSettings->powerSaving = EFalse;
              		iWriteSettings->powerSavingPresent = ETrue;
              		iWriteStatuses->powerSavingStatus = aStatusRef;
              		}
            	else
            		{
            		err = KErrArgument;
            		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanBackgroundScanInterval ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
				//store the parameter
				DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->backgroundScanInterval = err;
      	    		iWriteSettings->backgroundScanIntervalPresent = ETrue;
      	    		iWriteStatuses->backgroundScanIntervalStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanScanRate ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      			//store the parameter
      			DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->scanRate = err;
      	    		iWriteSettings->scanRatePresent = ETrue;
      	    		iWriteStatuses->scanRateStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanRCPITrigger ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0)
      	    		{
      	    		iWriteSettings->RCPITrigger = err;
      	    		iWriteSettings->RCPITriggerPresent = ETrue;
      	    		iWriteStatuses->RCPITriggerStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMinActiveChannelTime ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->minActiveChannelTime = err;
      	    		iWriteSettings->minActiveChannelTimePresent = ETrue;
      	    		iWriteStatuses->minActiveChannelTimeStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxActiveChannelTime ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0)
      	    		{
      	    		iWriteSettings->maxActiveChannelTime = err;
      	    		iWriteSettings->maxActiveChannelTimePresent = ETrue;
      	    		iWriteStatuses->maxActiveChannelTimeStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxTxMSDULifeTime ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->maxTxMSDULifeTime = err;
      	    		iWriteSettings->maxTxMSDULifeTimePresent = ETrue;
      	    		iWriteStatuses->maxTxMSDULifeTimeStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanScanExpirationTimer ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      			DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->scanExpirationTimer = err;
      	    		iWriteSettings->scanExpirationTimerPresent = ETrue;
      	    		iWriteStatuses->scanExpirationTimerStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanUnloadDriverTimer ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->unloadDriverTimer = err;
      	    		iWriteSettings->unloadDriverTimerPresent = ETrue;
      	    		iWriteStatuses->unloadDriverTimerStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanRoamTimer ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->roamTimer = err;
      	    		iWriteSettings->roamTimerPresent = ETrue;
      	    		iWriteStatuses->roamTimerStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
       	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanRCPIDifference ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->RCPIDifference = err;
      	    		iWriteSettings->RCPIDifferencePresent = ETrue;
      	    		iWriteStatuses->RCPIDifferenceStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanConnRegainTimer ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->connRegainTimer = err;
      	    		iWriteSettings->connRegainTimerPresent = ETrue;
      	    		iWriteStatuses->connRegainTimerStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxTriesToFindNw ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0)
      	    		{
      	    		iWriteSettings->maxTriesToFindNw = err;
      	    		iWriteSettings->maxTriesToFindNwPresent = ETrue;
      	    		iWriteStatuses->maxTriesToFindNwStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanDelayBetweenFindNw ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->delayBetweenFindNw = err;
      	    		iWriteSettings->delayBetweenFindNwPresent = ETrue;
      	    		iWriteStatuses->delayBetweenFindNwStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanAllowRadioMeasurements ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	if( aObject.MatchF( _L8("True")) != KErrNotFound ||
      		  		aObject.MatchF( _L8("true")) != KErrNotFound )
              		{
              		DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
              		iWriteSettings->allowRadioMeasurements = ETrue;
              		iWriteSettings->allowRadioMeasurementsPresent = ETrue;
              		iWriteStatuses->allowRadioMeasurementsStatus = aStatusRef;
              		}
           		else if ( aObject.MatchF( _L8("False")) != KErrNotFound ||
           			      aObject.MatchF( _L8("false")) != KErrNotFound )
              		{
              		DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
              		iWriteSettings->allowRadioMeasurements = EFalse;
              		iWriteSettings->allowRadioMeasurementsPresent = ETrue;
              		iWriteStatuses->allowRadioMeasurementsStatus = aStatusRef;
              		}
           		else
           			{
           			err = KErrArgument;
      				}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMinPassiveChannelTime ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->minPassiveChannelTime = err;
      	    		iWriteSettings->minPassiveChannelTimePresent = ETrue;
      	    		iWriteStatuses->minPassiveChannelTimeStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxPassiveChannelTime ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->maxPassiveChannelTime = err;
      	    		iWriteSettings->maxPassiveChannelTimePresent = ETrue;
      	    		iWriteStatuses->maxPassiveChannelTimeStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxApFailureCount ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->maxApFailureCount = err;
      	    		iWriteSettings->maxApFailureCountPresent = ETrue;
      	    		iWriteStatuses->maxApFailureCountStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanLongBeaconFindCount ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->longBeaconFindCount = err;
      	    		iWriteSettings->longBeaconFindCountPresent = ETrue;
      	    		iWriteStatuses->longBeaconFindCountStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanQosNullFrameInterval ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->qosNullFrameInterval = err;
      	    		iWriteSettings->qosNullFrameIntervalPresent = ETrue;
      	    		iWriteStatuses->qosNullFrameIntervalStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanQosNullFrameTimeout ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->qosNullFrameTimeout = err;
      	    		iWriteSettings->qosNullFrameTimeoutPresent = ETrue;
      	    		iWriteStatuses->qosNullFrameTimeoutStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMTU ) != KErrNotFound )
      		{
      		// check whether input is of legal size
      		if ( aObject.Size() <= KInputMaxLength )
      			{
      		  	//store the parameter
      		  	DBG_ARGS8(_S8("CGlobalWLANAdapter::AddLeafObjectL, add leaf - <%S>"), &aURI );
      	    	err = DesToInt( aObject );
      	    	if ( err >= 0 )
      	    		{
      	    		iWriteSettings->MTU = err;
      	    		iWriteSettings->MTUPresent = ETrue;
      	    		iWriteStatuses->MTUStatus = aStatusRef;
      	    		err = KErrNone;
      	    		}
      			}
      		else
      			{
      			err = KErrTooBig;
      			}
      		}
      	// *****************************************************************
     	// The given leaf was not found
	  	else
  			{
			err = KErrNotFound;
  			}
    	}
	else
    	{
        // Something went wrong
    	_DBG_FILE("CGlobalWLANAdapter::AddLeafObjectL(): Leaf addition failed");
        err = KErrGeneral;
    	}   	

    // Update the possible failure to the status, success is updated when the 
    // actual writing to db is done
    switch ( err )
    	{
    	case KErrTooBig:
    		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::ETooLargeObject );
    		break;	
    		
    	case KErrNotFound:
    		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
    		break;
    		
    	case KErrGeneral:
    		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    		break;
    		
    	case KErrArgument:
			iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
			break;
			
    	case KErrOverflow:
    		iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
    		break;
    	}

    _DBG_FILE("CGlobalWLANAdapter::AddLeafObjectL(): end");
    }

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::UpdateLeafObjectL( const TDesC& aURI, 
//    const TDesC& aLUID, const TDesC8& aObject, const TDesC& aType, 
//    const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                          const TDesC8& aLUID, 
                                          const TDesC8& aObject, 
                                          const TDesC8& aType, 
                                          const TInt aStatusRef )
    {
    _DBG_FILE("CGlobalWLANAdapter::UpdateLeafObjectL(): begin");
    DBG_ARGS8(_S8("CGlobalWLANAdapter::UpdateLeafObjectL - <%S> <%S>"), &aURI, &aLUID );
    
    AddLeafObjectL( aURI, aLUID, aObject, aType, aStatusRef );

    _DBG_FILE("CGlobalWLANAdapter::UpdateLeafObjectL(): end");
    }

//------------------------------------------------------------------------------
// CGlobalWLANAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
// const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType )
// Not supported
//------------------------------------------------------------------------------

void CGlobalWLANAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                          const TDesC8& /*aLUID*/,
                                          RWriteStream*& /*aStream*/, 
                                          const TDesC8& /*aType*/,
                                          TInt aStatusRef )
    {
    _DBG_FILE("CGlobalWLANAdapter::UpdateLeafObjectL(): streaming: begin");
    
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    
    _DBG_FILE("CGlobalWLANAdapter::UpdateLeafObjectL() streaming: end");
    }

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::DeleteObjectL( const TDesC8& aURI, 
// const TDesC8& aLUID, const TInt aStatusRef )
// Not supported
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::DeleteObjectL( const TDesC8& /*aURI*/, 
                                      const TDesC8& /*aLUID*/, 
                                      const TInt aStatusRef )
    {
    _DBG_FILE("CGlobalWLANAdapter::DeleteObjectL(): begin");
    
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError); 
       
    _DBG_FILE("CGlobalWLANAdapter::DeleteObjectL(): end");
    }

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
//                     const TDesC8& aLUID, const TDesC8& aType, 
//                     const TInt aResultsRef, const TInt aStatusRef )
// not supported
//-----------------------------------------------------------------------------

void CGlobalWLANAdapter::FetchLeafObjectSizeL( const TDesC8& /*aURI*/, 
                                                 const TDesC8& /*aLUID*/,
                                                 const TDesC8& /*aType*/, 
                                                 TInt /*aResultsRef*/,
                                                 TInt /*aStatusRef*/ )
    {
    _DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectSizeL(): begin");
    _DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectSizeL(): end");
    return;
    }                                                


//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::FetchLeafObjectL( const TDesC8& aURI, 
//                     const TDesC8& aLUID, const TDesC8& aType, 
//                     const TInt aResultsRef, const TInt aStatusRef )
// 
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                        const TDesC8& aLUID, 
                                        const TDesC8& /*aType*/, 
                                        const TInt aResultsRef, 
                                        const TInt aStatusRef )
    {
    _DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): begin");
    DBG_ARGS8(_S8("Fetch aURI - %S - %S"), &aURI, &aLUID);
    
    // Store the required parameters into the struct. Actual reading will be done in
    // when CompleteOutstandingCommands is called
    
    // Get the last uri segment
    TPtrC8 lastUriSeg = GetLastUriSeg( aURI );
    DBG_ARGS8(_S8("Fetch leaf, lasturiseg - %S"), &lastUriSeg);
    
    //**************************************************************************
    // Check which leaf is in question
    //**************************************************************************
    if( aURI.Match( _L8("WLAN/*" ) ) != KErrNotFound )
    	{
        _DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): WLAN/* found in uri");
      	// *****************************************************************
      	if( lastUriSeg.Match( KNSmlWLanInternetConnectivityTest ) != KErrNotFound )
      		{	
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->internetConnectivityTestPresent = ETrue;
      	   	iReadStatuses->internetConnectivityTestResult = aResultsRef;
      	   	iReadStatuses->internetConnectivityTestStatus = aStatusRef;
      		}
      	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanUseDefaultSettings ) != KErrNotFound )
      		{	
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
            iReadSettings->useDefaultSettingsPresent = ETrue;  
            iReadStatuses->useDefaultSettingsResult = aResultsRef;
            iReadStatuses->useDefaultSettingsStatus = aStatusRef;
            }
	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanLongRetryLimit ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	    iReadSettings->longRetryLimitPresent = ETrue;
      	    iReadStatuses->longRetryLimitResult = aResultsRef;
      	    iReadStatuses->longRetryLimitStatus = aStatusRef;
      		}

      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanShortRetryLimit ) != KErrNotFound )
      		{	
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	    iReadSettings->shortRetryLimitPresent = ETrue;
      	    iReadStatuses->shortRetryLimitResult = aResultsRef;
      	    iReadStatuses->shortRetryLimitStatus = aStatusRef;
      		}
	
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanRTSThreshold ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	    iReadSettings->RTSThresholdPresent = ETrue;
      	    iReadStatuses->RTSThresholdResult = aResultsRef;
      	    iReadStatuses->RTSThresholdStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanTXPowerLevel ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->TXPowerLevelPresent = ETrue;
      	   	iReadStatuses->TXPowerLevelResult = aResultsRef;
      	   	iReadStatuses->TXPowerLevelStatus = aStatusRef;
      		}
      		
       	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanPowerSaving ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
            iReadSettings->powerSavingPresent = ETrue;
            iReadStatuses->powerSavingResult = aResultsRef;
            iReadStatuses->powerSavingStatus = aStatusRef;
      		}
      			
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanBackgroundScanInterval ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->backgroundScanIntervalPresent = ETrue;
      	   	iReadStatuses->backgroundScanIntervalResult = aResultsRef;
      	   	iReadStatuses->backgroundScanIntervalStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanScanRate ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->scanRatePresent = ETrue;
      	   	iReadStatuses->scanRateResult = aResultsRef;
      	   	iReadStatuses->scanRateStatus = aStatusRef;
      		}
      			
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanRCPITrigger ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->RCPITriggerPresent = ETrue;
      	   	iReadStatuses->RCPITriggerResult = aResultsRef;
      	   	iReadStatuses->RCPITriggerStatus = aStatusRef;
      		}
      			
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMinActiveChannelTime ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->minActiveChannelTimePresent = ETrue;
      	   	iReadStatuses->minActiveChannelTimeResult = aResultsRef;
      	   	iReadStatuses->minActiveChannelTimeStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxActiveChannelTime ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->maxActiveChannelTimePresent = ETrue;
      	   	iReadStatuses->maxActiveChannelTimeResult = aResultsRef;
      	   	iReadStatuses->maxActiveChannelTimeStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxTxMSDULifeTime ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->maxTxMSDULifeTimePresent = ETrue;
      	   	iReadStatuses->maxTxMSDULifeTimeResult = aResultsRef;
      	   	iReadStatuses->maxTxMSDULifeTimeStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanScanExpirationTimer ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->scanExpirationTimerPresent = ETrue;
      	   	iReadStatuses->scanExpirationTimerResult = aResultsRef;
      	   	iReadStatuses->scanExpirationTimerStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanUnloadDriverTimer ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->unloadDriverTimerPresent = ETrue;
      	   	iReadStatuses->unloadDriverTimerResult = aResultsRef;
      	   	iReadStatuses->unloadDriverTimerStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanRoamTimer ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->roamTimerPresent = ETrue;
      	   	iReadStatuses->roamTimerResult = aResultsRef;
      	   	iReadStatuses->roamTimerStatus = aStatusRef;
      		}
      		
       	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanRCPIDifference ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->RCPIDifferencePresent = ETrue;
      	   	iReadStatuses->RCPIDifferenceResult = aResultsRef;
      	   	iReadStatuses->RCPIDifferenceStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanConnRegainTimer ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->connRegainTimerPresent = ETrue;
      	   	iReadStatuses->connRegainTimerResult = aResultsRef;
      	   	iReadStatuses->connRegainTimerStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxTriesToFindNw ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->maxTriesToFindNwPresent = ETrue;
      	   	iReadStatuses->maxTriesToFindNwResult = aResultsRef;
      	   	iReadStatuses->maxTriesToFindNwStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanDelayBetweenFindNw ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->delayBetweenFindNwPresent = ETrue;
      	   	iReadStatuses->delayBetweenFindNwResult = aResultsRef;
      	   	iReadStatuses->delayBetweenFindNwStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanAllowRadioMeasurements ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
            iReadSettings->allowRadioMeasurementsPresent = ETrue;
           	iReadStatuses->allowRadioMeasurementsResult = aResultsRef;
      	   	iReadStatuses->allowRadioMeasurementsStatus = aStatusRef;
            }
              
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMinPassiveChannelTime ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->minPassiveChannelTimePresent = ETrue;
      	   	iReadStatuses->minPassiveChannelTimeResult = aResultsRef;
      	   	iReadStatuses->minPassiveChannelTimeStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxPassiveChannelTime ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->maxPassiveChannelTimePresent = ETrue;
      	   	iReadStatuses->maxPassiveChannelTimeResult = aResultsRef;
      	   	iReadStatuses->maxPassiveChannelTimeStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMaxApFailureCount ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->maxApFailureCountPresent = ETrue;
      	   	iReadStatuses->maxApFailureCountResult = aResultsRef;
      	   	iReadStatuses->maxApFailureCountStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanLongBeaconFindCount ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->longBeaconFindCountPresent = ETrue;
      	   	iReadStatuses->longBeaconFindCountResult = aResultsRef;
      	   	iReadStatuses->longBeaconFindCountStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanQosNullFrameInterval ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->qosNullFrameIntervalPresent = ETrue;
      	   	iReadStatuses->qosNullFrameIntervalResult = aResultsRef;
      	   	iReadStatuses->qosNullFrameIntervalStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanQosNullFrameTimeout ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->qosNullFrameTimeoutPresent = ETrue;
      	   	iReadStatuses->qosNullFrameTimeoutResult = aResultsRef;
      	   	iReadStatuses->qosNullFrameTimeoutStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
      	else if( lastUriSeg.Match( KNSmlWLanMTU ) != KErrNotFound )
      		{
      		_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): leaf"); 
      	   	iReadSettings->MTUPresent = ETrue;
      	   	iReadStatuses->MTUResult = aResultsRef;
      	   	iReadStatuses->MTUStatus = aStatusRef;
      		}
      		
      	// *****************************************************************
   		// The given leaf was not found
	  	else
  			{
  			_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): No matchin leaf was found");
			iCallBack->SetStatusL(aStatusRef, ENotFound);
  			}
    	}
	else
    	{
        // Something went wrong
    	_DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): No matchin node was found");
        iCallBack->SetStatusL(aStatusRef, EInvalidObject);
    	}   

    _DBG_FILE("CGlobalWLANAdapter::FetchLeafObjectL(): end");

    }
    

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::ChildURIListL( const TDesC& aURI, 
// const TDesC& aLUID, const CArrayFix<TNSmlDmMappingInfo>& aPreviousURISegmentList, 
// const TInt aResultsRef, const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::ChildURIListL( const TDesC8& aURI, 
                                      const TDesC8& /*aLUID*/, 
                                      const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, 
                                      const TInt aResultsRef, 
                                      const TInt aStatusRef )
    {
    _DBG_FILE("CGlobalWLANAdapter::ChildURIListL(): begin");
    DBG_ARGS8(_S8("globalwlanadapter::ChildUriList - <%S>"), &aURI );
 
    CBufBase* currentUriSegmentList = CBufFlat::NewL( KUriSegListLength );
    CleanupStack::PushL( currentUriSegmentList );
   	TEntry currentEntry;
   	TInt uriSegs = NumOfUriSegs( aURI );
    
    // Check whether WLAN node in question	
    if ( aURI.Match(_L8("WLAN"  ))!= KErrNotFound && uriSegs == 1 )
    	{
    	_DBG_FILE("CGlobalWLANAdapter::ChildURIListL(): WLAN");
    	currentUriSegmentList->InsertL( 0, KNSmlWLanNode );
      	iCallBack->SetResultsL( aResultsRef, *currentUriSegmentList, KNullDesC8 );
    	iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
    	_DBG_FILE("CGlobalWLANAdapter::ChildURIListL(): WLAN leafs returned");
    	}
    
    // Some illegal node/leaf in question
    else
    	{
        iCallBack->SetStatusL(aStatusRef,CSmlDmAdapter::ENotFound);
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): ENotFound end");
    	}
    
    // delete the list
    CleanupStack::PopAndDestroy( currentUriSegmentList );
    _DBG_FILE("CGlobalWLANAdapter::ChildURIListL(): end");
    }

//-----------------------------------------------------------------------------
// void CGlobalWLANAdapter::AddNodeObjectL( const TDesC& aURI, const TDesC& aParentLUID, 
// const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::AddNodeObjectL( const TDesC8& aURI, 
                                       const TDesC8& aParentLUID, 
                                       const TInt aStatusRef )
    {
 
    DBG_ARGS8(_S8("CGlobalWLANAdapter::AddNodeObjectL - uri: <%S> to aParentLuid: <%S>"), 
                &aURI, &aParentLUID ); 
                
    TInt uriSegs = NumOfUriSegs( aURI );
                
    // The only node is WLAN, lets check if that is in question
    if( aURI.Match( _L8("WLAN" ) ) != KErrNotFound && uriSegs==1 )
    	{
    	// No actions needed, the table is created and filled in the
    	// Execution function.
    	iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
    	}
    else
    	{
    	iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    	}

    _DBG_FILE("CGlobalWLANAdapter::AddNodeObjectL(): end");
    }
    
//------------------------------------------------------------------------------
// CGlobalWLANAdapter::ExecuteCommandL
// not supported
//------------------------------------------------------------------------------
void CGlobalWLANAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                              const TDesC8& /*aLUID*/,
                              const TDesC8& /*aArgument*/, 
                              const TDesC8& /*aType*/,
                              TInt aStatusRef )
    {
    _DBG_FILE("CGlobalWLANAdapter::ExecuteCommandL(): begin");
    
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    
    _DBG_FILE("CGlobalWLANAdapter::ExecuteCommandL(): end");
    }

//------------------------------------------------------------------------------
// CGlobalWLANAdapter::ExecuteCommandL ( .. RWriteStream ..)
// not supported
//------------------------------------------------------------------------------
void CGlobalWLANAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                              const TDesC8& /*aLUID*/,
                              RWriteStream*& /*aStream*/, 
                              const TDesC8& /*aType*/,
                              TInt aStatusref )
    {
    _DBG_FILE("CGlobalWLANAdapter::ExecuteCommandL(): begin");
     
   	iCallBack->SetStatusL( aStatusref, CSmlDmAdapter::EError );
   	
    _DBG_FILE("CGlobalWLANAdapter::ExecuteCommandL(): end");
    }

//------------------------------------------------------------------------------
// CGlobalWLANAdapter::CopyCommandL
// not supported
//------------------------------------------------------------------------------
void CGlobalWLANAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/,
                                     const TDesC8& /*aTargetLUID*/,
                                     const TDesC8& /* aSourceURI*/,
                                     const TDesC8& /*aSourceLUID*/, 
                                     const TDesC8& /*aType*/,
                                     TInt aStatusRef )
    {
    _DBG_FILE("CGlobalWLANAdapter::CopyCommandL(): begin");
    
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    
    _DBG_FILE("CGlobalWLANAdapter::CopyCommandL(): end");
    }
//------------------------------------------------------------------------------
// CGlobalWLANAdapter::StartAtomicL
// not supported
//------------------------------------------------------------------------------
void CGlobalWLANAdapter::StartAtomicL()
    {
    _DBG_FILE("CGlobalWLANAdapter::StartAtomicL(): begin");
    _DBG_FILE("CGlobalWLANAdapter::StartAtomicL(): end");
    }

//------------------------------------------------------------------------------
// CGlobalWLANAdapter::CommitAtomicL
// not supported
//------------------------------------------------------------------------------
void CGlobalWLANAdapter::CommitAtomicL()
    {
    _DBG_FILE("CGlobalWLANAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CGlobalWLANAdapter::CommitAtomicL(): end");
    }

//------------------------------------------------------------------------------
// CGlobalWLANAdapter::RollbackAtomicL
// returns EError
//------------------------------------------------------------------------------
void CGlobalWLANAdapter::RollbackAtomicL()
    {
    _DBG_FILE("CGlobalWLANAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CGlobalWLANAdapter::RollbackAtomicL(): end");
    }


//------------------------------------------------------------------------------
// CGlobalWLANAdapter::StreamingSupport
// returns ETrue, streaming is supported
//------------------------------------------------------------------------------

TBool CGlobalWLANAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    _DBG_FILE("CGlobalWLANAdapter::StreamingSupport(): begin");
    _DBG_FILE("CGlobalWLANAdapter::StreamingSupport(): end");
    return EFalse;
    }

//------------------------------------------------------------------------------
// CGlobalWLANAdapter::StreamCommittedL
// not used in this adapter
//------------------------------------------------------------------------------

void CGlobalWLANAdapter::StreamCommittedL()
    {
    _DBG_FILE("CGlobalWLANAdapter::StreamCommittedL(): begin");
    _DBG_FILE("CGlobalWLANAdapter::StreamCommittedL(): end");
    }

//-----------------------------------------------------------------------------
// CGlobalWLANAdapter* CGlobalWLANAdapter::CompleteOutstandingCmdsL( )
//-----------------------------------------------------------------------------
void CGlobalWLANAdapter::CompleteOutstandingCmdsL( )
    {
    _DBG_FILE("CGlobalWLANAdapter::CompleteOutStandingCmdsL(): begin");
    
    ExecuteBufferL( ETrue );  
    
    _DBG_FILE("CGlobalWLANAdapter::CompleteOutStandingCmdsL(): end");
    }


// -------------------------------------------------------------------------------------
// CGlobalWLANAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CGlobalWLANAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,
                                          TSmlDmAccessTypes aAccTypes,
                                          MSmlDmDDFObject::TOccurence aOccurrence, 
                                          MSmlDmDDFObject::TScope aScope, 
                                          MSmlDmDDFObject::TDFFormat aFormat,
                                          const TDesC8& aDescription )
                                          
    {
    aNode.SetAccessTypesL(aAccTypes);
    aNode.SetOccurenceL(aOccurrence);
    aNode.SetScopeL(aScope);
    aNode.SetDFFormatL(aFormat);
    if(aFormat!=MSmlDmDDFObject::ENode)
        {
        aNode.AddDFTypeMimeTypeL(KNSmlMimeType);
        }
    aNode.SetDescriptionL(aDescription);
    }


// ------------------------------------------------------------------------------------------------
// void CGlobalWLANAdapter::ExecuteBufferL( TBool aFinal )
// Executes buffered commands
// ------------------------------------------------------------------------------------------------
void CGlobalWLANAdapter::ExecuteBufferL( TBool /*aFinal*/ )
    {
    
    _DBG_FILE("CGlobalWLANAdapter::ExecuteBuffer(): begin");
  
  	// Store the given values into cenrep and commsdb and
  	// read the required parameters from cenrep and commsdb
  	TRAPD (err1, StoreCenRepParamsL());
  	TRAPD (err2, StoreCommsDBParamsL());
  	TRAPD (err3, ReadCenRepParamsL());
  	TRAPD (err4, ReadCommsDBParamsL());
  	
  	// Init the member variables after updates
  	InitMemberVariables();
  	
  	// If some of the operations failed, then leave
  	User::LeaveIfError( err1 );
  	User::LeaveIfError( err2 );
  	User::LeaveIfError( err3 );
  	User::LeaveIfError( err4 );
  	
    _DBG_FILE("CGlobalWLANAdapter::ExecuteBuffer(): End");
    }


// ------------------------------------------------------------------------------------------------
// TBool CGlobalWLANAdapter::StoreCenRepParamsL( )
// Stores the parameters to central repository
// ------------------------------------------------------------------------------------------------
void CGlobalWLANAdapter::StoreCenRepParamsL()
	{
	
	_DBG_FILE("CGlobalWLANAdapter::StoreCenRepParams(): begin");

 	CRepository* repository = NULL;
 	CRepository* conSettRep = NULL;
 	TInt err = KErrNone;
    

	// Write KWlanInternetConnectivityTest
    if ( iWriteSettings->internetConnectivityTestPresent )
    	{
    	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanInternetConnectivityTest start " ); 
    	// Open the cenrep for internet connectivity settings. This is the only global wlan parameter
    	// from this cenrep
        conSettRep = CRepository::NewL( KCRUidInternetConnectivitySettings );
        
        // There is no pointer to the repository, leave
        if ( conSettRep == NULL )
        	{
        	User::LeaveIfError( KErrGeneral );
        	}
        
        CleanupStack::PushL(conSettRep);
        
    	err = conSettRep->Set( KIctsTestPermission, static_cast<TInt>( iWriteSettings->internetConnectivityTest ) );
    	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanInternetConnectivityTest set to cenRep " ); 
    	if( err == KErrNone ) 
        	{ 
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanInternetConnectivityTest" ); 
        	iCallBack->SetStatusL(iWriteStatuses->internetConnectivityTestStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanInternetConnectivityTest failed " ); 
       	 	iCallBack->SetStatusL(iWriteStatuses->internetConnectivityTestStatus, CSmlDmAdapter::EError);
       	 	}
    	CleanupStack::PopAndDestroy(conSettRep);
       	}
    
    
    // The rest of the parameters are stored to Wlan device settings cenRep
    repository = CRepository::NewL( KCRUidWlanDeviceSettingsRegistryId );
    
    // There is no pointer to the repository, leave
    if ( repository == NULL )
    	{
    	User::LeaveIfError( KErrGeneral );
    	}
    
    CleanupStack::PushL(repository);
    
    if ( iWriteSettings->MTUPresent )
    	{
    	err = repository->Set( KWlanMTU, static_cast<TInt>( iWriteSettings->MTU ) );
    		
    	DBG_ARGS8(_S8("CGlobalWLANAdapter::StoreCenRepParams, MTU status - (%d)"), err );
    	if( err == KErrNone  ) 
        	{ 
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() MTU" ); 
        	iCallBack->SetStatusL(iWriteStatuses->MTUStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() MTU failed " ); 
       	 	iCallBack->SetStatusL(iWriteStatuses->MTUStatus, CSmlDmAdapter::EError);
       	 	}
       	}

    // Write KWlanScanRate
    if ( iWriteSettings->scanRatePresent )
    	{
    	err = repository->Set( KWlanScanRate, static_cast<TInt>( iWriteSettings->scanRate ) );
    		
    	DBG_ARGS8(_S8("CGlobalWLANAdapter::StoreCenRepParams, scanrate status - (%d)"), err );
    	if( err == KErrNone ) 
        	{ 
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() WlanScanRate" ); 
        	iCallBack->SetStatusL(iWriteStatuses->scanRateStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() WlanScanRate failed " ); 
       	 	iCallBack->SetStatusL(iWriteStatuses->scanRateStatus, CSmlDmAdapter::EError);
       	 	}
       	}

    // Write KWlanRcpiTrigger
   	if ( iWriteSettings->RCPITriggerPresent )
    	{
    	err = repository->Set( KWlanRcpiTrigger, static_cast<TInt>( iWriteSettings->RCPITrigger ) );
    		
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanRcpiTrigger" );
    	    iCallBack->SetStatusL(iWriteStatuses->RCPITriggerStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanRcpiTrigger failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->RCPITriggerStatus, CSmlDmAdapter::EError);
       	 	}
    	}

    // Write KWlanMinActiveChannelTime
    if ( iWriteSettings->minActiveChannelTimePresent )
    	{
    	err = repository->Set( KWlanMinActiveChannelTime, static_cast<TInt>( iWriteSettings->minActiveChannelTime ) );
    		
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMinActiveChannelTime" );
        	iCallBack->SetStatusL(iWriteStatuses->minActiveChannelTimeStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMinActiveChannelTime failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->minActiveChannelTimeStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanMaxActiveChannelTime
    if ( iWriteSettings->maxActiveChannelTimePresent )
    	{
    	err = repository->Set( KWlanMaxActiveChannelTime, static_cast<TInt>( iWriteSettings->maxActiveChannelTime ) );
    		
    	if( err == KErrNone ) 
        	{ 
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMaxActiveChannelTime" );
        	iCallBack->SetStatusL(iWriteStatuses->maxActiveChannelTimeStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMaxActiveChannelTime failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->maxActiveChannelTimeStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanMaxTxMSDULifeTime
    if ( iWriteSettings->maxTxMSDULifeTimePresent )
    	{
    	err = repository->Set( KWlanMaxTxMSDULifeTime, static_cast<TInt>( iWriteSettings->maxTxMSDULifeTime ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams()KWlanMaxTxMSDULifeTime" );
        	iCallBack->SetStatusL(iWriteStatuses->maxTxMSDULifeTimeStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams()KWlanMaxTxMSDULifeTime failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->maxTxMSDULifeTimeStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanScanExpirationTimer
    if ( iWriteSettings->scanExpirationTimerPresent )
    	{
    	err = repository->Set( KWlanScanExpirationTimer, static_cast<TInt>( iWriteSettings->scanExpirationTimer ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams()KWlanScanExpirationTimer" );
        	iCallBack->SetStatusL(iWriteStatuses->scanExpirationTimerStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams()KWlanScanExpirationTimer failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->scanExpirationTimerStatus, CSmlDmAdapter::EError);
       	 	}
		}
				
    // Write KWlanUnloadDriverTimer
    if ( iWriteSettings->unloadDriverTimerPresent )
    	{
    	err = repository->Set( KWlanUnloadDriverTimer, static_cast<TInt>( iWriteSettings->unloadDriverTimer ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams()KWlanUnloadDriverTimer" );
        	iCallBack->SetStatusL(iWriteStatuses->unloadDriverTimerStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams()KWlanUnloadDriverTimer failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->unloadDriverTimerStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanRoamTimer
    if ( iWriteSettings->roamTimerPresent )
    	{
    	err = repository->Set( KWlanRoamTimer, static_cast<TInt>( iWriteSettings->roamTimer ) );
    	
    	if( err == KErrNone) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanRoamTimer" );
        	iCallBack->SetStatusL(iWriteStatuses->roamTimerStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanRoamTimer failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->roamTimerStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanRcpiDifference
    if ( iWriteSettings->RCPIDifferencePresent )
    	{
    	err = repository->Set( KWlanRcpiDifference, static_cast<TInt>( iWriteSettings->RCPIDifference ) );
    	
    	DBG_ARGS8(_S8("CGlobalWLANAdapter::StoreCenRepParams, rcpidifference status - (%d)"), err );
    	if( err == KErrNone) 
        	{ 
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanRcpiDifference" );
        	iCallBack->SetStatusL(iWriteStatuses->RCPIDifferenceStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanRcpiDifference failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->RCPIDifferenceStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanConnRegainTimer
    if ( iWriteSettings->connRegainTimerPresent )
    	{
    	err = repository->Set( KWlanConnRegainTimer, static_cast<TInt>( iWriteSettings->connRegainTimer ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanConnRegainTimer" );
        	iCallBack->SetStatusL(iWriteStatuses->connRegainTimerStatus, CSmlDmAdapter::EOk);
       	 	}
       	else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanConnRegainTimer failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->connRegainTimerStatus, CSmlDmAdapter::EError);
       	 	}
		}
				
    // Write KWlanMaxTriesToFindNw
   	if ( iWriteSettings->maxTriesToFindNwPresent )
    	{
    	err = repository->Set( KWlanMaxTriesToFindNw, static_cast<TInt>( iWriteSettings->maxTriesToFindNw ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMaxTriesToFindNw ");
        	iCallBack->SetStatusL(iWriteStatuses->maxTriesToFindNwStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMaxTriesToFindNw failed");
       	 	iCallBack->SetStatusL(iWriteStatuses->maxTriesToFindNwStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanDelayBetweenFindNw
    if ( iWriteSettings->delayBetweenFindNwPresent )
    	{
    	err = repository->Set( KWlanDelayBetweenFindNw, static_cast<TInt>( iWriteSettings->delayBetweenFindNw ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanDelayBetweenFindNw" );
        	iCallBack->SetStatusL(iWriteStatuses->delayBetweenFindNwStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanDelayBetweenFindNw failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->delayBetweenFindNwStatus, CSmlDmAdapter::EError);
       	 	}
		}
				
    // Write KWlanMinPassiveChannelTime
    if ( iWriteSettings->minPassiveChannelTimePresent )
    	{
    	err = repository->Set( KWlanMinPassiveChannelTime, static_cast<TInt>( iWriteSettings->minPassiveChannelTime ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMinPassiveChannelTime" );
        	iCallBack->SetStatusL(iWriteStatuses->minPassiveChannelTimeStatus, CSmlDmAdapter::EOk);
       	 	}
       	 else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMinPassiveChannelTime failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->minPassiveChannelTimeStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanMaxPassiveChannelTime
    if ( iWriteSettings->maxPassiveChannelTimePresent )
    	{
    	err = repository->Set( KWlanMaxPassiveChannelTime, static_cast<TInt>( iWriteSettings->maxPassiveChannelTime ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMaxPassiveChannelTime" );
        	iCallBack->SetStatusL(iWriteStatuses->maxPassiveChannelTimeStatus, CSmlDmAdapter::EOk);
       	 	}
       	else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMaxPassiveChannelTime failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->maxPassiveChannelTimeStatus, CSmlDmAdapter::EError);
       	 	}
		}
				
    // Write KWlanMaxApFailureCount
    if ( iWriteSettings->maxApFailureCountPresent )
    	{
    	err = repository->Set( KWlanMaxApFailureCount, static_cast<TInt>( iWriteSettings->maxApFailureCount ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMaxApFailureCount" );
        	iCallBack->SetStatusL(iWriteStatuses->maxApFailureCountStatus, CSmlDmAdapter::EOk);
       	 	}
        else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanMaxApFailureCount failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->maxApFailureCountStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanLongBeaconFindCount
    if ( iWriteSettings->longBeaconFindCountPresent )
    	{
    	err = repository->Set( KWlanLongBeaconFindCount, static_cast<TInt>( iWriteSettings->longBeaconFindCount ) );
    	
    	if( err == KErrNone ) 
      		{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanLongBeaconFindCount" );
        	iCallBack->SetStatusL(iWriteStatuses->longBeaconFindCountStatus, CSmlDmAdapter::EOk);
       	 	}
       	else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanLongBeaconFindCount failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->longBeaconFindCountStatus, CSmlDmAdapter::EError);
       	 	}
		}
				
    // Write KWlanQosNullFrameInterval
    if ( iWriteSettings->qosNullFrameIntervalPresent )
    	{
    	err = repository->Set( KWlanQosNullFrameInterval, static_cast<TInt>( iWriteSettings->qosNullFrameInterval ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanQosNullFrameInterval" );
        	iCallBack->SetStatusL(iWriteStatuses->qosNullFrameIntervalStatus, CSmlDmAdapter::EOk);
       	 	}
       	else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanQosNullFrameInterval failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->qosNullFrameIntervalStatus, CSmlDmAdapter::EError);
       	 	}
        }

    // Write KWlanQosNullFrameTimeout
    if ( iWriteSettings->qosNullFrameTimeoutPresent )
    	{
    	err = repository->Set( KWlanQosNullFrameTimeout, static_cast<TInt>( iWriteSettings->qosNullFrameTimeout ) );
    	
    	if( err == KErrNone ) 
        	{
        	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanQosNullFrameTimeout" );
        	iCallBack->SetStatusL(iWriteStatuses->qosNullFrameTimeoutStatus, CSmlDmAdapter::EOk);
       	 	}
       	else
       	 	{
       	 	_DBG_FILE("CGlobalWLANAdapter:StoreCenRepParams() KWlanQosNullFrameTimeout failed" );
       	 	iCallBack->SetStatusL(iWriteStatuses->qosNullFrameTimeoutStatus, CSmlDmAdapter::EError);
       	 	}
       	}

    // Cleanup
   	CleanupStack::PopAndDestroy(repository);
    _DBG_FILE("CGlobalWLANAdapter::StoreCenRepParams(): end");
    return;
	}
	
// ------------------------------------------------------------------------------------------------
// void CGlobalWLANAdapter::StoreCommsDBParams( )
// Stores the parameters to central repository
// ------------------------------------------------------------------------------------------------
void CGlobalWLANAdapter::StoreCommsDBParamsL()
	{
	
	_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): begin");
	
	CCommsDatabase* db = CCommsDatabase::NewL();
	CleanupStack::PushL(db);
			
	// User defined WLAN device settings table from Comms database
    CCommsDbTableView* usrTable;
    
    // Open user settings.
    usrTable = db->OpenViewMatchingUintLC( TPtrC(WLAN_DEVICE_SETTINGS),
											TPtrC(WLAN_DEVICE_SETTINGS_TYPE),
											KWlanUserSettings );
		
	// if record did not exist, then it is created.
    if ( usrTable->GotoFirstRecord() != KErrNone )
        {
        _DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): Record creation");
    	TUint32 id;
    	User::LeaveIfError( usrTable->InsertRecord( id ) );
    
    	usrTable->WriteUintL( TPtrC(WLAN_DEVICE_SETTINGS_TYPE), KWlanUserSettings );  
    	//usrTable->WriteUintL( TPtrC(WLAN_TABLE_VERSION), KWlanDeviceSettingsTableVersion );
    	User::LeaveIfError( usrTable->PutRecordChanges() ); // End and save changes.
        }
			
	// Write CommsDat settings
    User::LeaveIfError( usrTable->UpdateRecord() ); // Begin changes.
	
	// Check which parameters are present in the structure, store them into commsdb and
	// update the status to OK/Error accordingly	
	if ( iWriteSettings->backgroundScanIntervalPresent )
		{
		_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): write KBgScanInterval");
    	TRAPD( err, usrTable->WriteUintL( TPtrC(WLAN_BG_SCAN_INTERVAL), iWriteSettings->backgroundScanInterval ));
    	if ( err == KErrNone)
    		{
    		iCallBack->SetStatusL(iWriteStatuses->backgroundScanIntervalStatus, CSmlDmAdapter::EOk);
    		}
    	else
    		{
    		iCallBack->SetStatusL(iWriteStatuses->backgroundScanIntervalStatus, CSmlDmAdapter::EError);	
    		}
		}
		
    if ( iWriteSettings->useDefaultSettingsPresent )
    	{
    	_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): write KUseDefaultSettings");
    	TRAPD( err, usrTable->WriteBoolL( TPtrC(WLAN_USE_DEFAULT_SETTINGS), iWriteSettings->useDefaultSettings ));
    	if ( err == KErrNone)
    		{
    		iCallBack->SetStatusL(iWriteStatuses->useDefaultSettingsStatus, CSmlDmAdapter::EOk);
    		}
    	else
    		{
    		iCallBack->SetStatusL(iWriteStatuses->useDefaultSettingsStatus, CSmlDmAdapter::EError);	
    		}
		}
		
    if ( iWriteSettings->longRetryLimitPresent )
   		{
   		_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): write KWlanLongRetry");
    	TRAPD( err, usrTable->WriteUintL( TPtrC(WLAN_LONG_RETRY), iWriteSettings->longRetryLimit ));
    	if ( err == KErrNone)
    		{
    		iCallBack->SetStatusL(iWriteStatuses->longRetryLimitStatus, CSmlDmAdapter::EOk);
    		}
    	else
    		{
    		iCallBack->SetStatusL(iWriteStatuses->longRetryLimitStatus, CSmlDmAdapter::EError);	
    		}
		}
		
    if ( iWriteSettings->shortRetryLimitPresent )
    	{
    	_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): write KWlanShortRetry");
    	TRAPD( err, usrTable->WriteUintL( TPtrC(WLAN_SHORT_RETRY), iWriteSettings->shortRetryLimit ));
    	if ( err == KErrNone)
    		{
    		iCallBack->SetStatusL(iWriteStatuses->shortRetryLimitStatus, CSmlDmAdapter::EOk);
    		}
    	else
    		{
    		iCallBack->SetStatusL(iWriteStatuses->shortRetryLimitStatus, CSmlDmAdapter::EError);	
    		}
		}
		
    if ( iWriteSettings->RTSThresholdPresent )
   		{
   		_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): write KWlanRTSThreshold");
    	TRAPD( err, usrTable->WriteUintL( TPtrC(WLAN_RTS_THRESHOLD), iWriteSettings->RTSThreshold ));
    	if ( err == KErrNone)
    		{
    		iCallBack->SetStatusL(iWriteStatuses->RTSThresholdStatus, CSmlDmAdapter::EOk);
    		}
    	else
    		{
    		iCallBack->SetStatusL(iWriteStatuses->RTSThresholdStatus, CSmlDmAdapter::EError);	
    		}
		}
		
    if ( iWriteSettings->TXPowerLevelPresent )
    	{
    	_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): write KWlanTxPowerLevel");
    	TRAPD( err, usrTable->WriteUintL( TPtrC(NU_WLAN_TX_POWER_LEVEL), iWriteSettings->TXPowerLevel ));
    	if ( err == KErrNone)
    		{
    		iCallBack->SetStatusL(iWriteStatuses->TXPowerLevelStatus, CSmlDmAdapter::EOk);
    		}
    	else
    		{
    		iCallBack->SetStatusL(iWriteStatuses->TXPowerLevelStatus, CSmlDmAdapter::EError);	
    		}
		}
		
    if ( iWriteSettings->allowRadioMeasurementsPresent )
    	{
    	_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): write KWlanAllowRadioMeasurements");
    	TRAPD( err, usrTable->WriteBoolL( TPtrC(WLAN_ALLOW_RADIO_MEASUREMENTS), iWriteSettings->allowRadioMeasurements ));
        if ( err == KErrNone)
    		{
    		iCallBack->SetStatusL(iWriteStatuses->allowRadioMeasurementsStatus, CSmlDmAdapter::EOk);
    		}
    	else
    		{
    		iCallBack->SetStatusL(iWriteStatuses->allowRadioMeasurementsStatus, CSmlDmAdapter::EError);	
    		}
		}
    
    if ( iWriteSettings->powerSavingPresent )
    	{
    	_DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): write KWlanPowerSaving");
    	TRAPD( err, usrTable->WriteBoolL( TPtrC(WLAN_POWER_MODE), iWriteSettings->powerSaving ));
        if ( err == KErrNone)
    		{
    		iCallBack->SetStatusL(iWriteStatuses->powerSavingStatus, CSmlDmAdapter::EOk);
    		}
    	else
    		{
    		iCallBack->SetStatusL(iWriteStatuses->powerSavingStatus, CSmlDmAdapter::EError);	
    		}
		}
    
    _DBG_FILE("CGlobalWLANAdapter::StoreCommsDBParams(): end");
    User::LeaveIfError( usrTable->PutRecordChanges() ); // End and save changes.
    CleanupStack::PopAndDestroy( usrTable );
    CleanupStack::PopAndDestroy( db );
    return;
    
	}
			

// ------------------------------------------------------------------------------------------------
// void CGlobalWLANAdapter::ReadCenRepParams( )
// Reads parameters from central repository
// ------------------------------------------------------------------------------------------------
void CGlobalWLANAdapter::ReadCenRepParamsL()
	{
		
	_DBG_FILE("CGlobalWLANAdapter::ReadCenRepParams(): begin");
	
	TInt err = KErrNone;
    CRepository* repository = NULL;
    TBuf8<KInputMaxLength> value;
    TBuf8<KTypeMaxLength> type;
    TInt temp = 0;
     
    // InternetConnectivityTest parameter is in a different cenRep than the other parameters
    // Write KWlanScanRate
    if ( iReadSettings->internetConnectivityTestPresent )
    	{
    	// This parameter is in the internet connectivity settings cenRep, open the repository
        repository = CRepository::NewL( KCRUidInternetConnectivitySettings );
        CleanupStack::PushL(repository);
      
    	value.Zero();
    	type.Zero();
    	
    	// if opening the cenrtral repository failed, then leave this function
    	if ( repository == NULL )
    		{
    		User::LeaveIfError( KErrGeneral );
    		}
    	
    	err = repository->Get( KIctsTestPermission, temp );    	
       	value.Num(temp);
       	type.Append( KNSmlWLanInternetConnectivityTest );
       	UpdateStatusAndResultL( iReadStatuses->internetConnectivityTestResult, iReadStatuses->internetConnectivityTestStatus, 
       					value, type, err  );
       	
       	CleanupStack::PopAndDestroy(repository);
    	}
    
    // The rest of the parameters are in WlanDeviceSettings cenRep
    repository = CRepository::NewL( KCRUidWlanDeviceSettingsRegistryId );
    CleanupStack::PushL(repository);
    
    // if opening the cenrtral repository failed, then leave this function
	if ( repository == NULL )
		{
		User::LeaveIfError( KErrGeneral );
		}
    
    if ( iReadSettings->MTUPresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanMTU, temp );
       	value.Num(temp);
       	type.Append( KNSmlWLanMTU );
       	UpdateStatusAndResultL( iReadStatuses->MTUResult, iReadStatuses->MTUStatus, 
       					value, type, err  );
    	}
    
    // Write KWlanScanRate
    if ( iReadSettings->scanRatePresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanScanRate, temp );
       	value.Num(temp);
       	type.Append( KNSmlWLanScanRate );
       	UpdateStatusAndResultL( iReadStatuses->scanRateResult, iReadStatuses->scanRateStatus, 
       					value, type, err  );
    	}

    // Write KWlanRcpiTrigger
   	if ( iReadSettings->RCPITriggerPresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanRcpiTrigger, temp );
       	value.Num(temp);
       	type.Append( KNSmlWLanRCPITrigger );
       	UpdateStatusAndResultL( iReadStatuses->RCPITriggerResult, iReadStatuses->RCPITriggerStatus, 
       					value, type, err  );
       	}

    // Write KWlanMinActiveChannelTime
    if ( iReadSettings->minActiveChannelTimePresent )
   	 	{
   	 	value.Zero();
   	 	type.Zero();
   	 	err = repository->Get( KWlanMinActiveChannelTime, temp );
       	value.Num(temp);
       	type.Append( KNSmlWLanMinActiveChannelTime );
       	UpdateStatusAndResultL( iReadStatuses->minActiveChannelTimeResult, iReadStatuses->minActiveChannelTimeStatus, 
       					value, type, err  );
       	}
       	
    // Write KWlanMaxActiveChannelTime
    if ( iReadSettings->maxActiveChannelTimePresent )
   	 	{
   	 	value.Zero();
   	 	type.Zero();
   	 	err = repository->Get( KWlanMaxActiveChannelTime, temp );
		value.Num(temp);
		type.Append( KNSmlWLanMaxActiveChannelTime );
       	UpdateStatusAndResultL( iReadStatuses->maxActiveChannelTimeResult, iReadStatuses->maxActiveChannelTimeStatus, 
       					value, type, err  );
       	}

    // Write KWlanMaxTxMSDULifeTime
    if ( iReadSettings->maxTxMSDULifeTimePresent )
   	 	{
   	 	value.Zero();
   	 	type.Zero();
   	 	err = repository->Get( KWlanMaxTxMSDULifeTime, temp );
		value.Num(temp);
		type.Append( KNSmlWLanMaxTxMSDULifeTime );
       	UpdateStatusAndResultL( iReadStatuses->maxTxMSDULifeTimeResult, iReadStatuses->maxTxMSDULifeTimeStatus, 
       					value, type, err  );
       	}

    // Write KWlanScanExpirationTimer
    if ( iReadSettings->scanExpirationTimerPresent )
   	 	{
   	 	value.Zero();
   	 	type.Zero();
   	 	err = repository->Get( KWlanScanExpirationTimer, temp );
		value.Num(temp);
		type.Append( KNSmlWLanScanExpirationTimer );
       	UpdateStatusAndResultL( iReadStatuses->scanExpirationTimerResult, iReadStatuses->scanExpirationTimerStatus, 
       					value, type, err  );
       	}
				
    // Write KWlanUnloadDriverTimer
    if ( iReadSettings->unloadDriverTimerPresent )
   	 	{
   	 	value.Zero();
   	 	type.Zero();
   	 	err = repository->Get( KWlanUnloadDriverTimer, temp );
		value.Num(temp);
		type.Append( KNSmlWLanUnloadDriverTimer );
       	UpdateStatusAndResultL( iReadStatuses->unloadDriverTimerResult, iReadStatuses->unloadDriverTimerStatus, 
       					value, type, err  );
       	}

    // Write KWlanRoamTimer
    if ( iReadSettings->roamTimerPresent )
    	{
    	value.Zero();
		type.Zero();
    	err = repository->Get( KWlanRoamTimer, temp );
    	value.Num(temp);
		type.Append( KNSmlWLanRoamTimer );
       	UpdateStatusAndResultL( iReadStatuses->roamTimerResult, iReadStatuses->roamTimerStatus, 
       					value, type, err  );
       	}

    // Write KWlanRcpiDifference
    if ( iReadSettings->RCPIDifferencePresent )
    	{
    	value.Zero();
    	type.Zero();
    	type.Append( KNSmlWLanRCPIDifference );
    	err = repository->Get( KWlanRcpiDifference, temp );
		value.Num(temp); 	
       	UpdateStatusAndResultL( iReadStatuses->RCPIDifferenceResult, iReadStatuses->RCPIDifferenceStatus, 
       					value, type, err  );
       	}

    // Write KWlanConnRegainTimer
    if ( iReadSettings->connRegainTimerPresent )
    	{
    	value.Zero();
    	type.Zero();
    	type.Append( KNSmlWLanConnRegainTimer );
    	err = repository->Get( KWlanConnRegainTimer, temp );
		value.Num(temp);
       	UpdateStatusAndResultL( iReadStatuses->connRegainTimerResult, iReadStatuses->connRegainTimerStatus, 
       					value, type, err  );
       	}
				
    // Write KWlanMaxTriesToFindNw
   	if ( iReadSettings->maxTriesToFindNwPresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanMaxTriesToFindNw, temp );
		value.Num(temp);
		type.Append( KNSmlWLanMaxTriesToFindNw );
       	UpdateStatusAndResultL( iReadStatuses->maxTriesToFindNwResult, iReadStatuses->maxTriesToFindNwStatus, 
       					value, type, err  );
       	}

    // Write KWlanDelayBetweenFindNw
    if ( iReadSettings->delayBetweenFindNwPresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanDelayBetweenFindNw, temp );
		value.Num(temp);
		type.Append( KNSmlWLanDelayBetweenFindNw );
       	UpdateStatusAndResultL( iReadStatuses->delayBetweenFindNwResult, iReadStatuses->delayBetweenFindNwStatus, 
       					value, type, err  );
       	}
				
    // Write KWlanMinPassiveChannelTime
    if ( iReadSettings->minPassiveChannelTimePresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanMinPassiveChannelTime, temp );
		value.Num(temp);
		type.Append( KNSmlWLanRCPITrigger );
       	UpdateStatusAndResultL( iReadStatuses->minPassiveChannelTimeResult, iReadStatuses->minPassiveChannelTimeStatus, 
       					value, type, err  );
       	}

    // Write KWlanMaxPassiveChannelTime
    if ( iReadSettings->maxPassiveChannelTimePresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanMaxPassiveChannelTime, temp );
		value.Num(temp);
		type.Append( KNSmlWLanMinPassiveChannelTime );
       	UpdateStatusAndResultL( iReadStatuses->maxPassiveChannelTimeResult, iReadStatuses->maxPassiveChannelTimeStatus, 
       					value, type, err  );
       	}
       			
    // Write KWlanMaxApFailureCount
    if ( iReadSettings->maxApFailureCountPresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanMaxApFailureCount, temp );
		value.Num(temp);
		type.Append( KNSmlWLanMaxApFailureCount );
       	UpdateStatusAndResultL( iReadStatuses->maxApFailureCountResult, iReadStatuses->maxApFailureCountStatus, 
       					value, type, err  );
       	}

    // Write KWlanLongBeaconFindCount
    if ( iReadSettings->longBeaconFindCountPresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanLongBeaconFindCount, temp );
		value.Num(temp);
		type.Append( KNSmlWLanLongBeaconFindCount );
       	UpdateStatusAndResultL( iReadStatuses->longBeaconFindCountResult, iReadStatuses->longBeaconFindCountStatus, 
       					value, type, err  );
       	}
				
    // Write KWlanQosNullFrameInterval
    if ( iReadSettings->qosNullFrameIntervalPresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanQosNullFrameInterval, temp );
		value.Num(temp);
		type.Append( KNSmlWLanQosNullFrameInterval );
       	UpdateStatusAndResultL( iReadStatuses->qosNullFrameIntervalResult, iReadStatuses->qosNullFrameIntervalStatus, 
       					value, type, err  );
       	}

    // Write KWlanQosNullFrameTimeout
    if ( iReadSettings->qosNullFrameTimeoutPresent )
    	{
    	value.Zero();
    	type.Zero();
    	err = repository->Get( KWlanQosNullFrameTimeout, temp );
		value.Num(temp);
		type.Append( KNSmlWLanQosNullFrameTimeout );
       	UpdateStatusAndResultL( iReadStatuses->qosNullFrameTimeoutResult, iReadStatuses->qosNullFrameTimeoutStatus, 
       					value, type, err  );
       	}

    // Cleanup
    CleanupStack::PopAndDestroy(repository);
    _DBG_FILE("CGlobalWLANAdapter::ReadCenRepParams(): end"); 
    return;
	
	}

// ------------------------------------------------------------------------------------------------
// void CGlobalWLANAdapter::UpdateStatusAndResultL( )
// Reads parameters from cenrep and updates result/status
// ------------------------------------------------------------------------------------------------
void CGlobalWLANAdapter::UpdateStatusAndResultL( TInt aResult, TInt aStatus, TDesC8& aValue, TDesC8& aType, TInt aStoreErr )
	{
	
	_DBG_FILE("CGlobalWLANAdapter::UpdateStatusAndResultL(): begin");

	CBufBase *lObject = CBufFlat::NewL(KInputMaxLength);
    CleanupStack::PushL(lObject);

    // If reading went ok, then we can set the results and statuses
    if ( aStoreErr == KErrNone )
    	{
    	lObject->InsertL(0, aValue);
    	iCallBack->SetResultsL( aResult,*lObject,aType );
    	iCallBack->SetStatusL( aStatus, CSmlDmAdapter::EOk);
    	}
    else
    	{
    	iCallBack->SetStatusL( aStatus, CSmlDmAdapter::EError );
    	}
    
    _DBG_FILE("CGlobalWLANAdapter::UpdateStatusAndResultL(): end");
    CleanupStack::PopAndDestroy(lObject);
    return;
   	}

// ------------------------------------------------------------------------------------------------
// void CGlobalWLANAdapter::ReadCommsDBParams( )
// Reads parameters from commsDB
// ------------------------------------------------------------------------------------------------
void CGlobalWLANAdapter::ReadCommsDBParamsL()
	{
	_DBG_FILE("CGlobalWLANAdapter::ReadCommsDBParams() begin" );  
   
  	CCommsDatabase* db = CCommsDatabase::NewL();
  	CleanupStack::PushL(db);
			
	// User defined WLAN device settings table from Comms database
    CCommsDbTableView* usrTable;
    
    // Open user settings.
    usrTable = db->OpenViewMatchingUintLC( TPtrC(WLAN_DEVICE_SETTINGS),
    										TPtrC(WLAN_DEVICE_SETTINGS_TYPE),
    										KWlanUserSettings );

    TUint32 intVal;
    TBool boolVal = EFalse;
    TBuf8<KInputMaxLength> value;
    TBuf8<KTypeMaxLength> type;
    TInt err = KErrNone;
    TInt readErr = KErrNone;
    _LIT8(KTrue,"True");
    _LIT8(KFalse,"False");
		
	// if record did not exist, then we don't do anything
    if ( usrTable == NULL )
    	readErr = KErrGeneral;
   	else
   		readErr = usrTable->GotoFirstRecord();

    // Check whether parameter was requested to be fetched
   	if ( iReadSettings->backgroundScanIntervalPresent )
   		{
   		// The reading is done if the commdb record was opened successfully, otherwise the err
   		// is updated so that the ReadAndSetStatus function can update thet status correctly
   		if ( readErr == KErrNone )
   			{
    		TRAP( err, usrTable->ReadUintL( TPtrC(WLAN_BG_SCAN_INTERVAL), intVal ));
   			}
    	else
    		{
            err=readErr;
    		}
   		
   		value.Zero();
   		type.Zero();
    	value.Num(intVal); 
    	type.Append( KNSmlWLanBackgroundScanInterval );
    	UpdateStatusAndResultL( iReadStatuses->backgroundScanIntervalResult, iReadStatuses->backgroundScanIntervalStatus, 
       					value, type, err );
   		}
 
    if ( iReadSettings->useDefaultSettingsPresent )
    	{
   		if ( readErr == KErrNone )
   			{
    		TRAP( err, usrTable->ReadBoolL( TPtrC(WLAN_USE_DEFAULT_SETTINGS), boolVal ));
   			}
    	else
    		{
            err=readErr;
    		}
    	
		value.Zero();
		type.Zero();
    	if ( boolVal == 0 )
    		value.Copy(KFalse);
    	else
    		value.Copy(KTrue);
    	
    	type.Append( KNSmlWLanUseDefaultSettings );
    	_DBG_FILE("CGlobalWLANAdapter::ReadCommsDBParams() go set status" );
    	UpdateStatusAndResultL( iReadStatuses->useDefaultSettingsResult, iReadStatuses->useDefaultSettingsStatus, 
       					value, type, err );
    	}
    
    if ( iReadSettings->longRetryLimitPresent )
    	{
    	if ( readErr == KErrNone )
    		{
    		TRAP( err, usrTable->ReadUintL( TPtrC(WLAN_LONG_RETRY), intVal ));
    		}
   		else
   			{
   			err = readErr;
   			}
    	
    	value.Zero();
    	value.Num(intVal);
    	type.Zero();
    	type.Append( KNSmlWLanLongRetryLimit );
    	UpdateStatusAndResultL( iReadStatuses->longRetryLimitResult, iReadStatuses->longRetryLimitStatus, 
       					value, type, err );
    	}
    
    if ( iReadSettings->shortRetryLimitPresent )
    	{
    	if ( readErr == KErrNone )
    		{
    		TRAP( err, usrTable->ReadUintL( TPtrC(WLAN_SHORT_RETRY), intVal ));
    		}
   		else
   			{
   			err = readErr;
   			}
    	
    	value.Zero();
    	type.Zero();
    	value.Num(intVal);
    	type.Append( KNSmlWLanShortRetryLimit );
    	UpdateStatusAndResultL( iReadStatuses->shortRetryLimitResult, iReadStatuses->shortRetryLimitStatus, 
       					value, type, err );
    	}
    	
    if ( iReadSettings->RTSThresholdPresent )
    	{
    	if ( readErr == KErrNone )
    		{
    		TRAP( err, usrTable->ReadUintL( TPtrC(WLAN_RTS_THRESHOLD), intVal ));
    		}
   		else
   			{
   			err = readErr;
   			}
    	
    	value.Zero();
    	type.Zero();
    	value.Num(intVal);
    	type.Append( KNSmlWLanRTSThreshold );
    	UpdateStatusAndResultL( iReadStatuses->RTSThresholdResult, iReadStatuses->RTSThresholdStatus, 
       					value, type, err );
    	}
    
    if ( iReadSettings->TXPowerLevelPresent )
    	{
		if ( readErr == KErrNone )
			{
    		TRAP( err, usrTable->ReadUintL( TPtrC(NU_WLAN_TX_POWER_LEVEL), intVal ));
			}
   		else
   			{
   			err = readErr;
   			}
		
		value.Zero();
		type.Zero();
    	value.Num(intVal);
    	type.Append( KNSmlWLanTXPowerLevel );
    	UpdateStatusAndResultL( iReadStatuses->TXPowerLevelResult, iReadStatuses->TXPowerLevelStatus, 
       					value, type, err );
    	}
    
    if ( iReadSettings->allowRadioMeasurementsPresent )
    	{
		if ( readErr == KErrNone )
			{
    		TRAP( err, usrTable->ReadBoolL( TPtrC(WLAN_ALLOW_RADIO_MEASUREMENTS), boolVal ));
			}
   		else
   			{
   			err = readErr;
   			}
		
		value.Zero();
		type.Zero();
    	if ( boolVal )
    		value.Copy(_L8("True"));
    	else
    		value.Copy(_L8("False"));
    	
    	type.Append( KNSmlWLanAllowRadioMeasurements );
    	UpdateStatusAndResultL( iReadStatuses->allowRadioMeasurementsResult, iReadStatuses->allowRadioMeasurementsStatus, 
       					value, type, err );
    	}
    
    if ( iReadSettings->powerSavingPresent )
    	{
		if ( readErr == KErrNone )
    		{
    		TRAP( err, usrTable->ReadBoolL( TPtrC(WLAN_POWER_MODE), boolVal ));
    		}
   		else
   			{
   			err = readErr;
   			}
		
		value.Zero();
		type.Zero();
    	if ( boolVal )
    		value.Copy(_L8("True"));
    	else
    		value.Copy(_L8("False"));
    	
    	type.Append( KNSmlWLanPowerSaving );
    	UpdateStatusAndResultL( iReadStatuses->powerSavingResult, iReadStatuses->powerSavingStatus, 
       					value, type, err );
    	}

	CleanupStack::PopAndDestroy( usrTable );
    CleanupStack::PopAndDestroy( db );
	_DBG_FILE("CGlobalWLANAdapter::ReadCommsDBParams() end" );  
	return;
	}



// ------------------------------------------------------------------------------------------------
// TInt CGlobalWLANAdapter::NumOfUriSegs( const TDesC8& aUri )
// Return count of URI segments of aUri
// ------------------------------------------------------------------------------------------------
TInt CGlobalWLANAdapter::NumOfUriSegs( const TDesC8& aUri )
    {
    _DBG_FILE("CGlobalWLANAdapter::NumOfUriSegs(): begin");

    TInt count = 1;
    for( TInt i=0; i<aUri.Length(); i++ )
        {
        if( aUri[i] == '/' )
            count ++;
        }

    _DBG_FILE("CGlobalWLANAdapter::NumOfUriSegs(): end");
    return count;
    }


// ------------------------------------------------------------------------------------------------
// TPtrC8 CGlobalWLANAdapter::GetLastUriSeg(const TDesC8& aURI)
// Returns only the last uri segemnt
// ------------------------------------------------------------------------------------------------
TPtrC8 CGlobalWLANAdapter::GetLastUriSeg( const TDesC8& aURI )
    {
    _DBG_FILE("CGlobalWLANAdapter::GetLastUriSeg(): begin");

	// search for the segment after the last slash
    TInt i;
    for( i = aURI.Length() - 1; i >= 0; i-- )
        {
        if( aURI[i] == '/' )
            {
            break;
            }
        }

    if( i == 0 )
        {
        _DBG_FILE("CGlobalWLANAdapter::GetLastUriSeg(): end");
        return aURI;
        }
    else
        {
        _DBG_FILE("CGlobalWLANAdapter::GetLastUriSeg(): end");
        return aURI.Mid( i + 1 );
        }
    }
    

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::DesToInt( const TDesC8& aNumber )
// Returns aLuid as integer value
// ------------------------------------------------------------------------------------------------
TInt CGlobalWLANAdapter::DesToInt( const TDesC8& aNumber )
    {
    _DBG_FILE("CGlobalWLANAdapter::DesToInt(): begin");
    
    TInt err = KErrNone;
    TLex8 lex(aNumber);
    TInt value = 0;
    err = lex.Val( value );

    DBG_ARGS8(_S8("CGlobalWLANAdapter::DesToInt() - Des: <%S> Int: <%D>"), &aNumber, value );
    _DBG_FILE("CGlobalWLANAdapter::DesToInt(): end");
   
    // the input might have been illegal. Return error code if conversion failed.
    if ( err == KErrNone )
    	return value;
    else
    	return err;
    }
    
    
// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::InitMemberVariables
// Inits the member variable structures
// ------------------------------------------------------------------------------------------------
void CGlobalWLANAdapter::InitMemberVariables()
    {
    _DBG_FILE("CGlobalWLANAdapter::InitMemberVariables(): begin");
    
    iWriteSettings->RCPITriggerPresent=EFalse;
  	iWriteSettings->internetConnectivityTestPresent=EFalse;
	iWriteSettings->useDefaultSettingsPresent=EFalse;
	iWriteSettings->longRetryLimitPresent=EFalse;
	iWriteSettings->shortRetryLimitPresent=EFalse;
	iWriteSettings->RTSThresholdPresent=EFalse;
	iWriteSettings->TXPowerLevelPresent=EFalse;
	iWriteSettings->powerSavingPresent=EFalse;
	iWriteSettings->backgroundScanIntervalPresent=EFalse;
	iWriteSettings->scanRatePresent=EFalse;
	iWriteSettings->RCPITriggerPresent=EFalse;
	iWriteSettings->minActiveChannelTimePresent=EFalse;
	iWriteSettings->maxActiveChannelTimePresent=EFalse;
	iWriteSettings->maxTxMSDULifeTimePresent=EFalse;
	iWriteSettings->scanExpirationTimerPresent=EFalse;
	iWriteSettings->unloadDriverTimerPresent=EFalse;
	iWriteSettings->roamTimerPresent=EFalse;
	iWriteSettings->RCPIDifferencePresent=EFalse;
	iWriteSettings->connRegainTimerPresent=EFalse;
	iWriteSettings->maxTriesToFindNwPresent=EFalse;
	iWriteSettings->delayBetweenFindNwPresent=EFalse;
	iWriteSettings->allowRadioMeasurementsPresent=EFalse;
	iWriteSettings->minPassiveChannelTimePresent=EFalse;
	iWriteSettings->maxPassiveChannelTimePresent=EFalse;
	iWriteSettings->maxApFailureCountPresent=EFalse;
	iWriteSettings->longBeaconFindCountPresent=EFalse;
	iWriteSettings->qosNullFrameIntervalPresent=EFalse;
	iWriteSettings->qosNullFrameTimeoutPresent=EFalse;
	iWriteSettings->MTUPresent=EFalse;
		 
	iReadSettings->RCPITriggerPresent=EFalse;
  	iReadSettings->internetConnectivityTestPresent=EFalse;
	iReadSettings->useDefaultSettingsPresent=EFalse;
	iReadSettings->longRetryLimitPresent=EFalse;
	iReadSettings->shortRetryLimitPresent=EFalse;
	iReadSettings->RTSThresholdPresent=EFalse;
	iReadSettings->TXPowerLevelPresent=EFalse;
	iReadSettings->powerSavingPresent=EFalse;
	iReadSettings->backgroundScanIntervalPresent=EFalse;
	iReadSettings->scanRatePresent=EFalse;
	iReadSettings->RCPITriggerPresent=EFalse;
	iReadSettings->minActiveChannelTimePresent=EFalse;
	iReadSettings->maxActiveChannelTimePresent=EFalse;
	iReadSettings->maxTxMSDULifeTimePresent=EFalse;
	iReadSettings->scanExpirationTimerPresent=EFalse;
	iReadSettings->unloadDriverTimerPresent=EFalse;
	iReadSettings->roamTimerPresent=EFalse;
	iReadSettings->RCPIDifferencePresent=EFalse;
	iReadSettings->connRegainTimerPresent=EFalse;
	iReadSettings->maxTriesToFindNwPresent=EFalse;
	iReadSettings->delayBetweenFindNwPresent=EFalse;
	iReadSettings->allowRadioMeasurementsPresent=EFalse;
	iReadSettings->minPassiveChannelTimePresent=EFalse;
	iReadSettings->maxPassiveChannelTimePresent=EFalse;
	iReadSettings->maxApFailureCountPresent=EFalse;
	iReadSettings->longBeaconFindCountPresent=EFalse;
	iReadSettings->qosNullFrameIntervalPresent=EFalse;
	iReadSettings->qosNullFrameTimeoutPresent=EFalse;
	iReadSettings->MTUPresent=EFalse;

	_DBG_FILE("CGlobalWLANAdapter::InitMemberVariables(): begin");
    }

// end of file
