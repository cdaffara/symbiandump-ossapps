/*
*  Name        : nsmldmstreamingadapter.cpp
*  Part of     : nsmldmstreamingadapter
*  Interface   : ecom / SmlDmAdapter
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


//INCLUDE FILES
#include <implementationproxy.h> // For TImplementationProxy definition
#include <centralrepository.h>
#include <commdb.h>
#include <cdbcols.h>             // CommsDB columname defs
#include <e32base.h>
#include <e32std.h>
#include <e32const.h>
#include <mpsettingsmodel.h>
#include <e32msgqueue.h>


#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldmconst.h"
#include "nsmlstreamingadapter.h"
#include "nsmldmiapmatcher.h"
#include "streamingadapterCRKeys.h"

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


// -----------------------------------------------------------------------------
// CNSmlDmStreamingAdapter* CNSmlDmStreamingAdapter::NewL( )
// -----------------------------------------------------------------------------
CNSmlDmStreamingAdapter* CNSmlDmStreamingAdapter::NewL(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::NewL(): begin");
    CNSmlDmStreamingAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop();
    _DBG_FILE("CNSmlDmStreamingAdapter::NewL(): end");
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDmStreamingAdapter* CNSmlDmStreamingAdapter::NewLC( )
// -----------------------------------------------------------------------------
CNSmlDmStreamingAdapter* CNSmlDmStreamingAdapter::NewLC(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::NewLC(): begin");
    CNSmlDmStreamingAdapter* self = new (ELeave) CNSmlDmStreamingAdapter(aDmCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->iDmCallback = aDmCallback;
    _DBG_FILE("CNSmlDmStreamingAdapter::NewLC(): end");
    return self;
    }



// -----------------------------------------------------------------------------
// CNSmlDmStreamingAdapter::~CNSmlDmStreamingAdapter()
// -----------------------------------------------------------------------------
CNSmlDmStreamingAdapter::~CNSmlDmStreamingAdapter()
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::~CNSmlDmStreamingAdapter(): begin");
    if(iModel)
        delete iModel;
    _DBG_FILE("CNSmlDmStreamingAdapter::~CNSmlDmStreamingAdapter(): end");
    }
    
// -----------------------------------------------------------------------------
// CNSmlDmStreamingAdapter::CNSmlDmStreamingAdapter()
// -----------------------------------------------------------------------------

CNSmlDmStreamingAdapter::CNSmlDmStreamingAdapter(TAny* aEcomArguments):CSmlDmAdapter(aEcomArguments)
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::CNSmlDmStreamingAdapter(aEcomArguments): begin");
    _DBG_FILE("CNSmlDmStreamingAdapter::CNSmlDmStreamingAdapter(aEcomArguments): end"); 
    }


void CNSmlDmStreamingAdapter::ConstructL()
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::ConstructL: begin");
        if ( !iModel )
                {
                RImplInfoPtrArray impl; 
                CleanupStack::PushL( TCleanupItem( Cleanup, &impl ) );
                CMPSettingsModel::ListImplementationsL( impl );
                if( impl.Count() > 0 )
                    {
                    _DBG_FILE("CStreamingAdapter::ConstructL: Creating CMPSettingsModel");
                    // using the first available implementation
                    iModel= CMPSettingsModel::NewL( impl[0]->ImplementationUid() );
                    _DBG_FILE("CStreamingAdapter::ConstructL: Creating CMPSettingsModel Done" );
                    }
                CleanupStack::PopAndDestroy(); // implArray
                // Load default values
                //iModel->LoadSettingsL(EConfigDefault);
                if(iModel)
                	iModel->LoadSettingsL(EConfigUser);
                }
        _DBG_FILE("CNSmlDmStreamingAdapter::ConstructL: end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmStreamingAdapter::SetLeafPropertiesL()
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::SetLeafPropertiesL( MSmlDmDDFObject& aObject, 
                                                const TSmlDmAccessTypes& aAccessTypes, 
                                                const TDesC8& aDescription ) const
    {
    aObject.SetAccessTypesL( aAccessTypes );
    aObject.SetScopeL( MSmlDmDDFObject::EPermanent );
    aObject.SetDFFormatL( MSmlDmDDFObject::EChr );
    aObject.AddDFTypeMimeTypeL( KNSmlDMStreamingAdapterTextPlain );
    aObject.SetDescriptionL( aDescription );
    }


// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::DDFVersionL()
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFVersionL(TDes& aDDFVersion): begin");
    aDDFVersion.InsertL(0,KNSmlDMStreamingAdapterDDFVersion);
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::DDFStructureL()
//
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): begin");
    
    TSmlDmAccessTypes accessTypes;
    accessTypes.SetGet();
    
    TSmlDmAccessTypes accessNoDeleteTypes;
    accessNoDeleteTypes.SetGet();
    accessNoDeleteTypes.SetReplace();
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNodeName");
    MSmlDmDDFObject& dmStreamingAdap = aDDF.AddChildObjectL( KNSmlDMStreamingAdapterNodeName );
    dmStreamingAdap.SetAccessTypesL( accessTypes ); 
    dmStreamingAdap.SetScopeL( MSmlDmDDFObject::EPermanent );
    dmStreamingAdap.SetDescriptionL( KNSmlDMStreamingAdapterDescription );
  
  
  	_DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterName");
    MSmlDmDDFObject& confName = dmStreamingAdap.AddChildObjectL(KNSmlDMStreamingAdapterName);
    FillNodeInfoL(  confName, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterNameDescription );
    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterToProxy");
    MSmlDmDDFObject& toProxyID = dmStreamingAdap.AddChildObjectL(KNSmlDMStreamingAdapterToProxy);
    FillNodeInfoL(  toProxyID, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterToProxyDescription );
    
     
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterToNapID");
    MSmlDmDDFObject& toNapID = dmStreamingAdap.AddChildObjectL(KNSmlDMStreamingAdapterToNapID);
    FillNodeInfoL(  toNapID, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterToNapIDDescription );
    
    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNetInfo");
    MSmlDmDDFObject& netInfo = dmStreamingAdap.AddChildObjectL(KNSmlDMStreamingAdapterNetInfo);
    FillNodeInfoL(  netInfo, 
                    accessTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::ENode,
                    KNSmlDMStreamingAdapterNetInfo );
    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNetInfoGPRS");
    MSmlDmDDFObject& netInfoGPRS = netInfo.AddChildObjectL(KNSmlDMStreamingAdapterNetInfoGPRS);
    FillNodeInfoL(  netInfoGPRS, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterNetInfo );
                    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNetInfoEGPRS");
    MSmlDmDDFObject& netInfoEGPRS = netInfo.AddChildObjectL(KNSmlDMStreamingAdapterNetInfoEGPRS);
    FillNodeInfoL(  netInfoEGPRS, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterNetInfo );
                    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNetInfoWCDMA");
    MSmlDmDDFObject& netInfoWCDMA = netInfo.AddChildObjectL(KNSmlDMStreamingAdapterNetInfoWCDMA);
    FillNodeInfoL(  netInfoWCDMA, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterNetInfo );
                    
                    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNetInfoCDMA2000");
    MSmlDmDDFObject& netInfoCDMA2000 = netInfo.AddChildObjectL(KNSmlDMStreamingAdapterNetInfoCDMA2000);
    FillNodeInfoL(  netInfoCDMA2000, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterNetInfo );
                    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNetInfoWLAN");
    MSmlDmDDFObject& netInfoWLAN = netInfo.AddChildObjectL(KNSmlDMStreamingAdapterNetInfoWLAN);
    FillNodeInfoL(  netInfoWLAN, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterNetInfo );
                    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNetInfoCDMA");
    MSmlDmDDFObject& netInfoCDMA = netInfo.AddChildObjectL(KNSmlDMStreamingAdapterNetInfoCDMA);
    FillNodeInfoL(  netInfoCDMA, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterNetInfo );
    
    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterNetInfoHSDPA");
    MSmlDmDDFObject& netInfoHSDPA = netInfo.AddChildObjectL(KNSmlDMStreamingAdapterNetInfoHSDPA);
    FillNodeInfoL(  netInfoHSDPA, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EChr,
                    KNSmlDMStreamingAdapterNetInfo );
    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterMinUdpPort");
    MSmlDmDDFObject& minUdpPort = dmStreamingAdap.AddChildObjectL(KNSmlDMStreamingAdapterMinUdpPort);
    FillNodeInfoL(  minUdpPort, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EInt,
                    KNSmlDMStreamingAdapterMinUdpPortDescription );
    
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): Creating Node KNSmlDMStreamingAdapterMaxUdpPort");
    MSmlDmDDFObject& maxUdpPort = dmStreamingAdap.AddChildObjectL(KNSmlDMStreamingAdapterMaxUdpPort);
    FillNodeInfoL(  maxUdpPort, 
                    accessNoDeleteTypes,
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent,
                    MSmlDmDDFObject::EInt,
                    KNSmlDMStreamingAdapterMaxUdpPortDescription );
    
    _DBG_FILE("CNSmlDmStreamingAdapter::DDFStructureL(): end");
    }
    
void CNSmlDmStreamingAdapter::SetNetInfoL(const TDesC8& aObject,const TDesC8 &aDes,TDataBearer aBearer,const TInt aStatusRef,const TInt aMnMaxBw, const TInt aMxMaxBw,const TInt aMnSusBw,const TInt aMxSusBw)
	{
	TInt aSusBw;
  TInt aMaxBw;
  TLex8 aConv;      
  TInt err;
  //aConv = aObject;
  TInt ret = aObject.Locate(KCommaSep()[0]);
  TInt ret1 = aObject.LocateReverse(KCommaSep()[0]);
  
  TInt len = aObject.Length() - (ret1 + 1);
  TPtrC8 segment1 = aObject.Right( len );
  aConv = segment1;
 
  TPtrC8 firstSeg = aObject.Left( ret );
  if(firstSeg.Compare(aDes))
      {
            DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::SetNetInfoL() - %d:  Error in SetMaxBandwidth"), aBearer);
      iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject);
      return;
      }
  
  
  err = aConv.Val(aMaxBw);
  if(err != KErrNone)
      {
            DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::SetNetInfoL() - %d: Error in Conversion"), aBearer);
      iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
      return;
      }

  if(aMaxBw > 0)
  	{
  	err = iModel->SetMaxBandwidth(aMaxBw,aBearer);
  	}
  else
  	{
  	_DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetMaxBandwidth");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
    return;
    }
    
  if(err != KErrNone)
      {
      DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::SetNetInfoL() - %d: Error in SetMaxBandwidth"), aBearer);
      iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
      return;
      //return retValue;
      }
  

  TBuf8<KMaxLengthTempInfo> aSusBf;
  for(TInt i = (ret+1); i < (ret1); i++)
      {
      aSusBf.Append(aObject[i]);
      }
  aConv = aSusBf;
  err = aConv.Val(aSusBw);
  if(err != KErrNone)
      {
            DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::SetNetInfoL() - %d: Error in Conversion"), aBearer);
      iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
      return;
      }
  
  if(aSusBw > 0)
  	{
  	err = iModel->SetSustainBandwidth(aSusBw,aBearer);
  	}
  else
  	{
  	_DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetSustainBandwidth");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
    return;
    }
  
    if(err != KErrNone)
		    {
		    DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::SetNetInfoL() - %d: Error in SetSustainBandwidth"), aBearer);
		    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
		    return;
		    }
    if(aMaxBw > aMxMaxBw || aMaxBw < aMnMaxBw)
  	    {
        DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::SetNetInfoL() - %d: Error Invalid aMaxBw"), aBearer);
        iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
        return;
        }
    if(aSusBw > aMxSusBw || aSusBw < aMnSusBw)
        {
        DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::SetNetInfoL() - %d: Error Invalid aSusBw"), aBearer);
        iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
        return;
        }

  iModel->StoreSettingsL();
  
  iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk);
	}

// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                               const TDesC8& aLUID, 
                                               const TDesC8& aObject, 
                                               const TDesC8& aType, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): begin");
    
		//CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    TInt ret = aURI.LocateReverse(KNSmlURISeparator()[0]);
    if ( ret == KErrNotFound ) 
        {
        //retValue = CSmlDmAdapter::EError;
        iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
        }
    else
        {
        //iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        TInt len = aURI.Length() - (ret + 1);
        TPtrC8 segment = aURI.Right( len );
        
        if(segment == KNSmlDMStreamingAdapterName)
            {
            	// Set for Streaming Configuration Name
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterName");
            TBuf<KMaxLengthStreamingName> aBuf;
            TInt aErr;
            CRepository* centrep( NULL);
            aBuf.Zero();
            if(aObject.Length() <= KMaxLengthStreamingName)
               {
            aBuf.Copy(aObject);
               }
            else
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in KMaxLengthStreamingName");
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ETooLargeObject);
                return;
                }
   					
            centrep = CRepository::NewLC( KCRUidStreamingAdapter );
            if ( centrep )
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Inside Cenrep Get");
                aErr = centrep->Set( KStreamingConfigurationName, aBuf );
                if(aErr != KErrNone)
                    {
                    _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in CenRep Get");
                    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                    CleanupStack::PopAndDestroy(centrep);	
                    return;
                    }
                CleanupStack::PopAndDestroy(centrep);
                iModel->StoreSettingsL();
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
                }
            else
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in Creating the cenrep");
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
                }
            }
        else if(segment == KNSmlDMStreamingAdapterToProxy)
            {
            	// Set for Streaming settings proxy address
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterToProxy");
            TBuf<KMaxLengthToProxy> aBuf;
            if(aObject.Length() <= KMaxLengthToProxy)
               {
            aBuf.Copy(aObject);
               }
           else
               {
               _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in KMaxLengthToProxy");
               iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ETooLargeObject);
               return;
               }
            ret = iModel->SetProxyHostNameL(aBuf);
            if(ret != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetProxyHostNameL %d"), ret);
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
                return;
                }
            iModel->StoreSettingsL();    
            iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );    
            }
        else if(segment == KNSmlDMStreamingAdapterToNapID)
            {
           	SetDefaultApL(aObject,aStatusRef);
           	/*
            TInt aNap;
            TUint32 aNapId;
            TLex8 aConv;
            aConv = aObject;
            CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
            CleanupStack::PushL(iapmatch);
            
            aNap = iapmatch->IAPIdFromURIL(aObject);
            
            if(aNap == KErrNotFound)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in IAPIdFromURIL %d"), aNap);
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
                return;
                }
                
            aNapId = aNap;
            CleanupStack::PopAndDestroy(); // iapMatch
            
            TInt ret;// = aConv.Val(aNapId,EDecimal);
            ret = iModel->SetDefaultAp(aNapId);
            if(ret == KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): SetDefaultAp success %d"), ret);
                iModel->StoreSettingsL();
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );    
                }
            else
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetDefaultAp %d"), ret);
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
                }*/
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoGPRS)
            {
                // Set for the netwrok information when Databearer is GPRS
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterNetInfoGPRS");
            SetNetInfoL(aObject,KNSmlDMStreamingAdapterNetInfoGPRS,EBearerGPRS,aStatusRef,KMnMaxBwGPRS,KMxMaxBwGPRS,KMnSusBwGPRS,KMxSusBwGPRS);
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoEGPRS)
            {
                // Set for the netwrok information when Databearer is EGPRS
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterNetInfoEGPRS");
            SetNetInfoL(aObject,KNSmlDMStreamingAdapterNetInfoEGPRS,EBearerEGPRS,aStatusRef,KMnMaxBwEGPRS,KMxMaxBwEGPRS,KMnSusBwEGPRS,KMxSusBwEGPRS);
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoWCDMA)
            {
                // Set for the netwrok information when Databearer is  WCDMA
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterNetInfoWCDMA");
            SetNetInfoL(aObject,KNSmlDMStreamingAdapterNetInfoWCDMA,EBearerWCDMA,aStatusRef,KMnMaxBwWCDMA,KMxMaxBwWCDMA,KMnSusBwWCDMA,KMxSusBwWCDMA);
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoCDMA2000)
            {
                // Set for the netwrok information when Databearer is CDMA2000
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterNetInfoCDMA2000");
            SetNetInfoL(aObject,KNSmlDMStreamingAdapterNetInfoCDMA2000,EBearerCDMA2000,aStatusRef,KMnMaxBwCDMA2000,KMxMaxBwCDMA2000,KMnSusBwCDMA2000,KMxSusBwCDMA2000);
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoWLAN)
            {
                // Set for the netwrok information when Databearer is WLAN
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterNetInfoWLAN");
            SetNetInfoL(aObject,KNSmlDMStreamingAdapterNetInfoWLAN,EBearerWLAN,aStatusRef,KMnMaxBwWLAN,KMxMaxBwWLAN,KMnSusBwWLAN,KMxSusBwWLAN);
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoCDMA)
            {
                // Set for the netwrok information when Databearer is CDMA
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterNetInfoCDMA");
            SetNetInfoL(aObject,KNSmlDMStreamingAdapterNetInfoCDMA,EBearerCDMA,aStatusRef,KMnMaxBwCDMA,KMxMaxBwCDMA,KMnSusBwCDMA,KMxSusBwCDMA);
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoHSDPA)
            {
                // Set for the netwrok information when Databearer is HSDPA
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterNetInfoHSDPA");
            SetNetInfoL(aObject,KNSmlDMStreamingAdapterNetInfoHSDPA,EBearerHSDPA,aStatusRef,KMnMaxBwHSDPA,KMxMaxBwHSDPA,KMnSusBwHSDPA,KMxSusBwHSDPA);
            }                
        else if(segment == KNSmlDMStreamingAdapterMinUdpPort)
            {
            	// Set for the min udp port for streaming settings
            	SetMinUDPPortL(aObject,aStatusRef);
            	/*
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterMinUdpPort");
            TInt aMinPort;
            TInt aMaxPort;
            TLex8 aConv;
            TInt ret = KErrNone;
            aConv = aObject;
            ret = iModel->GetMaxUDPPort(aMaxPort);
            if(ret != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in GetMaxUDPPort: %d"), ret);
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            ret = aConv.Val(aMinPort);
            if(ret != KErrNone)
            	{
      	      _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in conversion");
              iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
              return;
            	}
            
            
            if(aMinPort < (aMaxPort - KPortDiff) && aMinPort >= KMinPort && aMinPort < KMaxPort)
                {
                ret = iModel->SetMinUDPPort(aMinPort);
                if(ret != KErrNone)
                    {
                    DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetMinUDPPort: %d"), ret);
                    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                    return;
                    }
                iModel->StoreSettingsL();
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk);
                }
            else
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Invalid Port value");
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject);
                }
                */
            }
        else if(segment == KNSmlDMStreamingAdapterMaxUdpPort)
            {
            	SetMaxUDPPortL(aObject,aStatusRef);
            	// Set for the max udp port for streaming settings
            	/*
            _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): begin");
            TInt aMaxPort;
            TInt aMinPort;
            TLex8 aConv;
            TInt ret = KErrNone;
            aConv = aObject;
            ret = iModel->GetMinUDPPort(aMinPort);
            if(ret != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in GetMinUDPPort: %d"), ret);
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            ret = aConv.Val(aMaxPort);
            if(ret != KErrNone)
            	{
      	      _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in conversion");
              iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
              return;
            	}
            
            if(aMaxPort > (aMinPort + KPortDiff) && aMaxPort >= KMinPort && aMaxPort <= KMaxPort )
                {
                ret = iModel->SetMaxUDPPort(aMaxPort);
                if(ret != KErrNone)
                    {
                    DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetMaxUDPPort: %d"), ret);
                    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                    return;
                    }
                iModel->StoreSettingsL();
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk);
                }
            else
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Invalid Port value");
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject);
                } */    
            }
        }
    _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): end");
    }
    
    void CNSmlDmStreamingAdapter::SetDefaultApL(const TDesC8& aObject,const TInt aStatusRef)
    {
    TInt aNap;
    TUint32 aNapId;
    TLex8 aConv;
    aConv = aObject;
    CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
    CleanupStack::PushL(iapmatch);
    
    aNap = iapmatch->IAPIdFromURIL(aObject);
    
    if(aNap == KErrNotFound)
        {
        DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in IAPIdFromURIL %d"), aNap);
        iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
        CleanupStack::PopAndDestroy(); // iapMatch
        return;
        }
        
    aNapId = aNap;
    CleanupStack::PopAndDestroy(); // iapMatch
    
    TInt ret;// = aConv.Val(aNapId,EDecimal);
    ret = iModel->SetDefaultAp(aNapId);
    if(ret == KErrNone)
        {
        DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): SetDefaultAp success %d"), ret);
        iModel->StoreSettingsL();
        iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );    
        }
    else
        {
        DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetDefaultAp %d"), ret);
        iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
        }
    }
    
    void CNSmlDmStreamingAdapter::SetMinUDPPortL(const TDesC8& aObject,const TInt aStatusRef)
    {
    			_DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): KNSmlDMStreamingAdapterMinUdpPort");
            TInt aMinPort;
            TInt aMaxPort;
            TLex8 aConv;
            TInt ret = KErrNone;
            aConv = aObject;
            ret = iModel->GetMaxUDPPort(aMaxPort);
            if(ret != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in GetMaxUDPPort: %d"), ret);
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            ret = aConv.Val(aMinPort);
            if(ret != KErrNone)
            	{
      	      _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in conversion");
              iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
              return;
            	}
            
            
            if(aMinPort < (aMaxPort - KPortDiff) && aMinPort >= KMinPort && aMinPort < KMaxPort)
                {
                ret = iModel->SetMinUDPPort(aMinPort);
                if(ret != KErrNone)
                    {
                    DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetMinUDPPort: %d"), ret);
                    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                    return;
                    }
                iModel->StoreSettingsL();
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk);
                }
            else
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Invalid Port value");
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject);
                }
    }
    
    void CNSmlDmStreamingAdapter::SetMaxUDPPortL(const TDesC8& aObject,const TInt aStatusRef)
    {
    			_DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): begin");
            TInt aMaxPort;
            TInt aMinPort;
            TLex8 aConv;
            TInt ret = KErrNone;
            aConv = aObject;
            ret = iModel->GetMinUDPPort(aMinPort);
            if(ret != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in GetMinUDPPort: %d"), ret);
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            ret = aConv.Val(aMaxPort);
            if(ret != KErrNone)
            	{
      	      _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in conversion");
              iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
              return;
            	}
            
            if(aMaxPort > (aMinPort + KPortDiff) && aMaxPort >= KMinPort && aMaxPort <= KMaxPort )
                {
                ret = iModel->SetMaxUDPPort(aMaxPort);
                if(ret != KErrNone)
                    {
                    DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Error in SetMaxUDPPort: %d"), ret);
                    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                    return;
                    }
                iModel->StoreSettingsL();
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk);
                }
            else
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): Invalid Port value");
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject);
                }
      }


// -----------------------------------------------------------------------------
// CNSmlDmStreamingAdapter::DeleteObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::DeleteObjectL( const TDesC8& /*aURI*/, 
                                           const TDesC8& /*aLUID*/, 
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::DeleteLeafObjectL( ): begin");
    _DBG_FILE("CNSmlDmStreamingAdapter::DeleteLeafObjectL( ): end");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotAllowed );
    }


// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
// const TDesC8& aType, const TInt aResultsRef, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                              const TDesC8& /*aLUID*/, 
                                              const TDesC8& aType, 
                                              const TInt aResultsRef, 
                                              const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): begin");
    
    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    
    CSmlDmAdapter::TError retValue = FetchLeafObjectL( aURI, *object );
    
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType );
    CleanupStack::PopAndDestroy(); //object
    _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::ChildURIListL( const TDesC& aURI, 
// const TDesC& aParentLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
// const TInt aResultsRef, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::ChildURIListL( const TDesC8& aURI, 
                                           const TDesC8& /*aParentLUID*/, 
                                           const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, 
                                           const TInt aResultsRef, 
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::ChildURIListL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    CBufBase* currentURISegmentList = CBufFlat::NewL( 1 );
    CleanupStack::PushL( currentURISegmentList );
    TInt ret = aURI.LocateReverse(KNSmlURISeparator()[0]);
    if ( ret == KErrNotFound ) 
        {
        ret = -1;
        }
    TInt len = aURI.Length() - ( ret + 1 );
    TPtrC8 segment = aURI.Right( len );

    if ( segment == KNSmlDMStreamingAdapterNodeName )
        {
        _DBG_FILE("CNSmlDmStreamingAdapter::ChildURIListL(): begin");
      	currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMStreamingAdapterVal() );
        }
    else if ( segment == KNSmlDMStreamingAdapterNetInfo )
        {
        _DBG_FILE("CNSmlDmStreamingAdapter::ChildURIListL(): begin");
      	currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMStreamingAdapterNetInfoVal() );
        }
    
    else
        {
        retValue = CSmlDmAdapter::EError;
        }
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
    CleanupStack::PopAndDestroy(); //currentURISegmentList
    _DBG_FILE("CNSmlDmStreamingAdapter::ChildURIListL(): end");
    }


// -----------------------------------------------------------------------------
// CNSmlDmStreamingAdapter::AddNodeObjectL( const TDesC8& aURI, 
// const TDesC8& aParentLUID, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::AddNodeObjectL( const TDesC8& /*aURI*/, 
                                            const TDesC8& /*aParentLUID*/, 
                                            const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::AddNodeObjectL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotAllowed );
    _DBG_FILE("CNSmlDmStreamingAdapter::AddNodeObjectL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                               const TDesC8& /*aLUID*/, 
                                               RWriteStream*& /*aStream*/, 
                                               const TDesC8& /*aType*/, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): stream: begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmStreamingAdapter::UpdateLeafObjectL(): stream: end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
//  const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                                  const TDesC8& /*aLUID*/, 
                                                  const TDesC8& aType, 
                                                  const TInt aResultsRef, 
                                                  const TInt aStatusRef )
    {

    _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectSizeL(): begin");
    
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
    _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectSizeL(): end");
    }
    

// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::ExecuteCommandL( const TDesC8& aURI, 
//  const TDesC8& aParentLUID, TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::ExecuteCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmStreamingAdapter::ExecuteCommandL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::ExecuteCommandL( const TDesC8& aURI, 
//  const TDesC8& aParentLUID, RWriteStream*& aStream, const TDesC8& aType, 
//  TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                             const TDesC8& /*aParentLUID*/, 
                                             RWriteStream*& /*aStream*/, 
                                             const TDesC8& /*aType*/, 
                                             const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::ExecuteCommandL(): stream: begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmStreamingAdapter::ExecuteCommandL(): stream: end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmStreamingAdapter::CopyCommandL( const TDesC8& aTargetURI, const TDesC8& 
//  aSourceURI, const TDesC8& aSourceLUID, const TDesC8& /*aType*/, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmStreamingAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& 
                                          /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, 
                                          const TDesC8& /*aSourceLUID*/, 
                                          const TDesC8& /*aType*/, TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::CopyCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmStreamingAdapter::CopyCommandL(): end");
    }

// --------------------------------------
//  CNSmlDmStreamingAdapter::StartAtomicL()
// --------------------------------------
void CNSmlDmStreamingAdapter::StartAtomicL()
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::StartAtomicL(): begin");
    _DBG_FILE("CNSmlDmStreamingAdapter::StartAtomicL(): end");
    }
    
// ---------------------------------------
//  CNSmlDmStreamingAdapter::CommitAtomicL()
// ---------------------------------------
void CNSmlDmStreamingAdapter::CommitAtomicL()
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CNSmlDmStreamingAdapter::CommitAtomicL(): end");
    }

// -----------------------------------------
//  CNSmlDmStreamingAdapter::RollbackAtomicL()
// -----------------------------------------
void CNSmlDmStreamingAdapter::RollbackAtomicL()
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CNSmlDmStreamingAdapter::RollbackAtomicL(): end");
    }

// -----------------------------------------------------------
//  CNSmlDmStreamingAdapter::StreamingSupport( TInt& aItemSize )
// -----------------------------------------------------------  
TBool CNSmlDmStreamingAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::StreamingSupport(): begin");
    _DBG_FILE("CNSmlDmStreamingAdapter::StreamingSupport(): end");
    return EFalse;
    }

// ------------------------------------------
//  CNSmlDmStreamingAdapter::StreamCommittedL()
// ------------------------------------------
void CNSmlDmStreamingAdapter::StreamCommittedL()
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::StreamCommittedL(): begin");
    _DBG_FILE("CNSmlDmStreamingAdapter::StreamCommittedL(): end");
    }

// --------------------------------------------------
//  CNSmlDmStreamingAdapter::CompleteOutstandingCmdsL()
// --------------------------------------------------   
void CNSmlDmStreamingAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE("CNSmlDmStreamingAdapter::CompleteOutstandingCmdsL(): begin");
    _DBG_FILE("CNSmlDmStreamingAdapter::CompleteOutstandingCmdsL(): end");    
    }


CSmlDmAdapter::TError CNSmlDmStreamingAdapter::GetNetInfoL(CBufBase& aObject,TDataBearer aBearer,const TDesC8 &aDes)
    {
      TBuf8<KMaxLengthNetInfo> aNetInfoBuf;   
      CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
      TInt err;
      aNetInfoBuf.Append(aDes);
      aNetInfoBuf.Append(KCommaSep);
              
      TInt aSusBw = 0;
      err = iModel->GetSustainBandwidth(aSusBw,aBearer);
      if(err != KErrNone)
          {
          DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::GetNetInfoL() - %d: Error in GetSustainBandwidth: %d"),aBearer,err);
          retValue = CSmlDmAdapter::EError;
          return retValue;
          }
      TBuf8<KMaxLengthTempInfo> aSusBwBuf;
      aSusBwBuf.Num(aSusBw);
      aNetInfoBuf.Append(aSusBwBuf);
      aNetInfoBuf.Append(KCommaSep);
      //aObject.InsertL(0,aSusBwBuf);
      
      
      TInt aMaxBw;
      err = iModel->GetMaxBandwidth(aMaxBw,aBearer);
      if(err != KErrNone)
          {
          DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::GetNetInfoL() - %d: Error in GetMaxBandwidth: %d"),aBearer,err);
          retValue = CSmlDmAdapter::EError;
          return retValue;
          }
      TBuf8<KMaxLengthTempInfo> aMaxBwBuf;
      aMaxBwBuf.Num(aMaxBw);
      aNetInfoBuf.Append(aMaxBwBuf);
      
      aObject.InsertL(0,aNetInfoBuf);
      retValue= CSmlDmAdapter::EOk;
      return retValue;
    }
// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::FetchLeafObjectL()
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmStreamingAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                                               CBufBase& aObject )
    {
    _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    TInt ret = aURI.LocateReverse(KNSmlURISeparator()[0]);
    TInt err = KErrNone;
    if ( ret == KErrNotFound ) 
        {
        retValue = CSmlDmAdapter::EError;
        }
    else
        {
        TInt len = aURI.Length() - (ret + 1);
        TPtrC8 segment = aURI.Right( len );
        
        if(segment == KNSmlDMStreamingAdapterName)
            {
            	// Get for Streaming Configuration Setting name(It is not displayed for the user).
            _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterName");
            TBuf8<KMaxLengthStreamingName> aName;
            CRepository* centrep( NULL);
            aName.Zero();
            centrep = CRepository::NewLC( KCRUidStreamingAdapter );

            if ( centrep )
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): Error in CenRep");
                TFullName temp;
                
                if (centrep->Get( KStreamingConfigurationName, temp )==KErrNone && temp.Length() )
                    {
                    _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): Cenrep Get");
                    temp.Trim();
                    aName.Copy(temp);
                    }
                else
                		{
                		_DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): Error in Cenrep Get");
                		retValue = CSmlDmAdapter::EError;
                		CleanupStack::PopAndDestroy(centrep);	
                		return retValue;
                		}
                CleanupStack::PopAndDestroy(centrep);
                }
            aObject.InsertL(0,aName);
            retValue= CSmlDmAdapter::EOk;    
            }
        else if(segment == KNSmlDMStreamingAdapterToProxy)
            {
            	// Get for Proxy Address of the streaming settings.
            _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterToProxy");
            TBuf<KMaxLengthToProxy> uri;
            err = iModel->GetProxyHostName(uri);
            if(err != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::FetchLeafObjectL(): Error in GetProxyHostName: %d"), err);
                retValue = CSmlDmAdapter::EError;
                return retValue;
                }
            TPtr8 ptr = uri.Collapse(); 
            aObject.InsertL(0,ptr); 
            retValue= CSmlDmAdapter::EOk;
            }
        else if(segment == KNSmlDMStreamingAdapterToNapID)
            {
            	// Get for Access Point that is set in the streaming settings
            	retValue = GetDefaultApL(aObject);
            	retValue = CSmlDmAdapter::EOk;  // even if no AP set return with null value
            	return retValue;
            	/*
            _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterToNapID");
            TUint32 aNapId;
            err = iModel->GetDefaultAp(aNapId);
            if(err != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::FetchLeafObjectL(): Error in GetDefaultAp: %d"), err);
                retValue = CSmlDmAdapter::EError;
                return retValue;
                }
            CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
            CleanupStack::PushL(iapmatch);

            HBufC8* uri8 = iapmatch->URIFromIAPIdL( aNapId );
            
            if( uri8 )
                {
                CleanupStack::PushL(uri8);
                aObject.InsertL(0,uri8->Des());
                retValue= CSmlDmAdapter::EOk;
                CleanupStack::PopAndDestroy(); // uri8
                }
            else
                {
                _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): Error in URIFromIAPIdL:ENotFound");
                retValue= CSmlDmAdapter::ENotFound;
                }    
            CleanupStack::PopAndDestroy(); // iapMatch */
            
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoHSDPA)
            {
            	// Get for network information when data bearer is HSDPA
            _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterNetInfoHSDPA");
            retValue = GetNetInfoL(aObject,EBearerHSDPA,KNSmlDMStreamingAdapterNetInfoHSDPA);
             return retValue;
            
           
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoGPRS)
            {
            	// Get for network information when data bearer is GPRS
            _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterNetInfoGPRS");
            retValue = GetNetInfoL(aObject,EBearerGPRS,KNSmlDMStreamingAdapterNetInfoGPRS);
            return retValue;
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoEGPRS)
            {
            	// Get for network information when data bearer is EGPRS
            _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterNetInfoEGPRS");
            retValue = GetNetInfoL(aObject,EBearerEGPRS,KNSmlDMStreamingAdapterNetInfoEGPRS);
            return retValue;
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoWCDMA)
            {
            	// Get for network information when data bearer is WCDMA
            _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterNetInfoWCDMA");
            retValue = GetNetInfoL(aObject,EBearerWCDMA,KNSmlDMStreamingAdapterNetInfoWCDMA);
            return retValue;
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoCDMA)
            {
            	// Get for network information when data bearer is CDMA
            _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterNetInfoCDMA");
            retValue = GetNetInfoL(aObject,EBearerCDMA,KNSmlDMStreamingAdapterNetInfoCDMA);
            return retValue;
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoCDMA2000)
            {
            	// Get for network information when data bearer is CDMA2000
            _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterNetInfoCDMA2000");
            retValue = GetNetInfoL(aObject,EBearerCDMA2000,KNSmlDMStreamingAdapterNetInfoCDMA2000);
            return retValue;
            }
        else if(segment == KNSmlDMStreamingAdapterNetInfoWLAN)
            {
            	// Get for network information when data bearer is WLAN
            _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterNetInfoWLAN");
            retValue = GetNetInfoL(aObject,EBearerWLAN,KNSmlDMStreamingAdapterNetInfoWLAN);
            return retValue;
            }
        else if(segment == KNSmlDMStreamingAdapterMinUdpPort)
            {
            	// Get for Min UDP port of streaming settings
            _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterMinUdpPort");
            TInt aMinPort;
            err = iModel->GetMinUDPPort(aMinPort);
            if(err != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmInfoAdapter::FetchLeafObjectL(): Error in GetMinUDPPort: %d"), err);
                retValue = CSmlDmAdapter::EError;
                return retValue;
                }
            TBuf8<10> aMinPortBuf;
            aMinPortBuf.Num(aMinPort);
            aObject.InsertL(0,aMinPortBuf);
            retValue= CSmlDmAdapter::EOk;
            }
        else if(segment == KNSmlDMStreamingAdapterMaxUdpPort)
            {
            	// Get for Max UDP port of streaming settings
            _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterMaxUdpPort");
            TInt aMaxPort;
            err = iModel->GetMaxUDPPort(aMaxPort);
            if(err != KErrNone)
                {
                DBG_ARGS8(_S8("CNSmlDmInfoAdapter::FetchLeafObjectL(): Error in GetMaxUDPPort: %d"), err);
                retValue= CSmlDmAdapter::EError;
                return retValue;
                }
            TBuf8<10> aMaxPortBuf;
            aMaxPortBuf.Num(aMaxPort);
            aObject.InsertL(0,aMaxPortBuf);
            retValue= CSmlDmAdapter::EOk;    
            }
        }
    _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): end");
    return retValue;
    }
    
    
    CSmlDmAdapter::TError CNSmlDmStreamingAdapter::GetDefaultApL(CBufBase& aObject)
    {
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): KNSmlDMStreamingAdapterToNapID");
      TUint32 aNapId;
      TInt err;
      err = iModel->GetDefaultAp(aNapId);
      if(err != KErrNone)
          {
          DBG_ARGS8(_S8("CNSmlDmStreamingAdapter::FetchLeafObjectL(): Error in GetDefaultAp: %d"), err);
          retValue = CSmlDmAdapter::EError;
          return retValue;
          }
      CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
      CleanupStack::PushL(iapmatch);

      HBufC8* uri8 = iapmatch->URIFromIAPIdL( aNapId );
      
      if( uri8 )
          {
          CleanupStack::PushL(uri8);
          aObject.InsertL(0,uri8->Des());
          retValue= CSmlDmAdapter::EOk;
          CleanupStack::PopAndDestroy(); // uri8
          }
      else
          {
          _DBG_FILE("CNSmlDmStreamingAdapter::FetchLeafObjectL(): Error in URIFromIAPIdL:ENotFound");
          retValue= CSmlDmAdapter::ENotFound;
          }    
      CleanupStack::PopAndDestroy(); // iapMatch
      return retValue;
      }


// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::FillNodeInfoL()
// Fills the DDF node info.
// ---------------------------------------------------------------------------
//
void CNSmlDmStreamingAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode, 
                                        const TSmlDmAccessTypes& aAccTypes, 
                                        MSmlDmDDFObject::TOccurence aOccurrence, 
                                        MSmlDmDDFObject::TScope aScope, 
                                        MSmlDmDDFObject::TDFFormat aFormat, 
                                        const TDesC8& aDescription ) const
    {
    _DBG_FILE("CNSmlDmInfoAdapter::FillNodeInfoL(): end");
    aNode.SetAccessTypesL( aAccTypes );
    aNode.SetOccurenceL( aOccurrence );
    aNode.SetScopeL( aScope );
    aNode.SetDFFormatL( aFormat );
    
    if( aFormat != MSmlDmDDFObject::ENode )
        {
        aNode.AddDFTypeMimeTypeL( KNSmlDMStreamingAdapterTextPlain );
        }
    
    aNode.SetDescriptionL( aDescription );
   	_DBG_FILE("CNSmlDmInfoAdapter::FillNodeInfoL(): end");
    }

/*

TInt CNSmlDmStreamingAdapter::ThreadFunction(TAny* aStarted)
    {
    _DBG_FILE("CNSmlDmInfoAdapter::ThreadFunction(): Start");
    CTrapCleanup* cleanup = CTrapCleanup::New();
    _DBG_FILE("CNSmlDmInfoAdapter::CTrapCleanup New");
    RMsgQueueBase& aMsgQ= *(RMsgQueueBase*) aStarted;
    CNetworkInfo * aNetInfo;
    TRAPD(err,aNetInfo = CNetworkInfo::NewL());
    _DBG_FILE("CNSmlDmInfoAdapter::CNetworkInfo New");
    TRAP(err,aNetInfo->GetNetworkInfoL(aMsgQ));
    _DBG_FILE("CNSmlDmInfoAdapter::ThreadFunction(): End");
    delete aNetInfo;
    delete cleanup;
    }
*/

// -----------------------------------------------------------------------------
// CStreamingAdapter::Cleanup
// -----------------------------------------------------------------------------
//
void CNSmlDmStreamingAdapter::Cleanup( TAny* aAny )
    {
    RImplInfoPtrArray* implArray = 
        reinterpret_cast< RImplInfoPtrArray*> ( aAny );
    implArray->ResetAndDestroy();
    implArray->Close();
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
        IMPLEMENTATION_PROXY_ENTRY(KNSmlDMStreamingAdapterImplUid, CNSmlDmStreamingAdapter::NewL)
    };

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmStreamingAdapter: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmStreamingAdapter: end");
    return ImplementationTable;
    }

// End of file

