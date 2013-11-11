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



#include <iapprefs.h>
#include <msvids.h>
#include <msvuids.h>
#include <utf.h>
#include <implementationproxy.h> // For TImplementationProxy definition
#include <commdb.h>
#include <cdbcols.h>             // CommsDB columname defs
#include <mmsclient.h>
#include <MmsConformance.h>
#include <protectdb.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>

#include "nsmldmimpluids.h"
#include "nsmldmmmsadapter.h"
#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldmconst.h"
#include "nsmldmiapmatcher.h"
#include "nsmldmtreedbclient.h"

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter* CNSmlDmMMSAdapter::NewL( )
// -----------------------------------------------------------------------------
CNSmlDmMMSAdapter* CNSmlDmMMSAdapter::NewL(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::NewL(): begin");
    CNSmlDmMMSAdapter* self = new (ELeave) CNSmlDmMMSAdapter(aDmCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    _DBG_FILE("CNSmlDmMMSAdapter::NewL(): end");
    return self;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::CNSmlDmMMSAdapter()
// -----------------------------------------------------------------------------
CNSmlDmMMSAdapter::CNSmlDmMMSAdapter(MSmlDmCallback* aDmCallback) 
    : CSmlDmAdapter(aDmCallback)
    {
    _DBG_FILE("CNSmlDmMMSAdapter::CNSmlDmMMSAdapter(): begin");
    _DBG_FILE("CNSmlDmMMSAdapter::CNSmlDmMMSAdapter(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::ConstructL()
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::ConstructL()
    {
    _DBG_FILE("CNSmlDmMMSAdapter::ConstructL(): begin");
    iStatusRef = KNSmlMMSUndefinedStatusRef;
    iImgSizeStatusRef = KNSmlMMSUndefinedStatusRef;
    iDatabase = CCommsDatabase::NewL(EDatabaseTypeIAP);
    _DBG_FILE("CNSmlDmMMSAdapter::ConstructL(): end");
    }


// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::~CNSmlDmMMSAdapter()
// -----------------------------------------------------------------------------
CNSmlDmMMSAdapter::~CNSmlDmMMSAdapter()
    {
    _DBG_FILE("CNSmlDmMMSAdapter::~CNSmlDmMMSAdapter(): begin");
    delete iMmsClient;
    delete iClientMtmRegistry;
    delete iMsvSession;
    delete iMMSSAddr;
    delete iMmsSettings;
	if(iDatabase)
    delete iDatabase;
    _DBG_FILE("CNSmlDmMMSAdapter::~CNSmlDmMMSAdapter(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmMMSAdapter::DDFVersionL()
//  Returns the DDF version of the adapter
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    _DBG_FILE("CNSmlDmMMSAdapter::DDFVersionL(TDes& aDDFVersion): begin");
    aDDFVersion.InsertL(0,KNSmlDMMMSDDFVersion);
    _DBG_FILE("CNSmlDmMMSAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmMMSAdapter::DDFStructureL()
//  Builds the DDF structure of adapter
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::DDFStructureL(): begin");

    TSmlDmAccessTypes accessTypesGetAdd;
    accessTypesGetAdd.SetGet();
    accessTypesGetAdd.SetAdd();
    
    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesNoDelete;
    accessTypesNoDelete.SetGet();
    accessTypesNoDelete.SetAdd();
    accessTypesNoDelete.SetReplace();

    // MMS
    MSmlDmDDFObject& MMS = aDDF.AddChildObjectL(KNSmlDMMMSNodeName);
    FillNodeInfoL(MMS,accessTypesGet,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EPermanent,MSmlDmDDFObject::ENode,
                  KNSmlDMMMSNodeNameDescription);

    //run time node 
    MSmlDmDDFObject& rtAcc = MMS.AddChildObjectGroupL();
    FillNodeInfoL(rtAcc,accessTypesGet,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::ENode,
                  KNSmlDMMMSDynamicNodeDescription);

    //level of mms protocol operations
    MSmlDmDDFObject& mmRecep = rtAcc.AddChildObjectL(KNSmlDMMMSMMRecep);
    FillNodeInfoL(mmRecep,accessTypesNoDelete,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMMSMMRecepDescription);

    //Creation mode
    MSmlDmDDFObject& creMode = rtAcc.AddChildObjectL(KNSmlDMMMSCreMode);
    FillNodeInfoL(creMode,accessTypesNoDelete,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMMSCreModeDescription);

    //anonymous messages allowed
    MSmlDmDDFObject& amsg = rtAcc.AddChildObjectL(KNSmlDMMMSAMsg);
    FillNodeInfoL(amsg,accessTypesNoDelete,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMMMSAMsgDescription);

    //adverts messages allowed
    MSmlDmDDFObject& rcvAds = rtAcc.AddChildObjectL(KNSmlDMMMSRcvAds);
    FillNodeInfoL(rcvAds,accessTypesNoDelete,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMMMSRcvAdsDescription);

    //image size in sending
    MSmlDmDDFObject& imgSize = rtAcc.AddChildObjectL(KNSmlDMMMSImgSize);
    FillNodeInfoL(imgSize,accessTypesNoDelete,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMMSImgSizeDescription);

    //deny delivery report sending
    MSmlDmDDFObject& dRepSnd = rtAcc.AddChildObjectL(KNSmlDMMMSDRepSnd);
    FillNodeInfoL(dRepSnd,accessTypesNoDelete,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMMMSDRepSndDescription);

    //delivery report requested by default
    MSmlDmDDFObject& rcvRep = rtAcc.AddChildObjectL(KNSmlDMMMSRcvRep);
    FillNodeInfoL(rcvRep,accessTypesNoDelete,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMMMSRcvRepDescription);

    //message validity time
    MSmlDmDDFObject& msgVal = rtAcc.AddChildObjectL(KNSmlDMMMSMsgVal);
    FillNodeInfoL(msgVal,accessTypesNoDelete,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMMSMsgValDescription);

    // MMS/<X>/Con
    MSmlDmDDFObject& con = rtAcc.AddChildObjectL(KNSmlDMMMSCon);
    FillNodeInfoL(con,accessTypesGet,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::ENode,
                  KNSmlDMMMSConNodeDescription);


    // MMS/<X>/Con/<X>
    MSmlDmDDFObject& rtCon = con.AddChildObjectL(KNSmlDMMMSConDynamicName16);
    FillNodeInfoL(rtCon,accessTypesGet,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::ENode,
                  KNSmlDMMMSConRtNodeDescription);


    // MMS/<X>/Con/<X>/MMSSAddr
    MSmlDmDDFObject& mmsAddr = rtCon.AddChildObjectL(KNSmlDMMMSMMSSAddr);
    FillNodeInfoL(mmsAddr,accessTypesNoDelete,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMMSMMSSAddrDescription);

    // MMS/<X>/Con/<X>/MToNapID
    MSmlDmDDFObject& mToNapID = rtCon.AddChildObjectL(KNSmlDMMMSMToNapID);
    FillNodeInfoL(mToNapID,accessTypesGet,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::ENode,
                  KNSmlDMMMSConRtNodeDescription);

    // MMS/<X>/Con/<X>/MToNapID/<X>
    MSmlDmDDFObject& rtNapIds = mToNapID.AddChildObjectL(KNullDesC8);
    FillNodeInfoL(rtNapIds,accessTypesGet,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::ENode,
                  KNSmlDMMMSConRtNodeDescription);

    // MMS/<X>/Con/<X>/MToNapID/<X>/MToNapIdL
    MSmlDmDDFObject& mToNapIDL = rtNapIds.AddChildObjectL(KNSmlDMMMSMToNapIDL);
    FillNodeInfoL(mToNapIDL,accessTypesNoDelete,MSmlDmDDFObject::EOneOrMore,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMMSMToNapIDLDescription);

    RNSmlDMCallbackSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL(session);
    
    session.UpdateAclL(KNSmlDMMMSLevel3Parents,KNSmlDmMMSAclGet);
    session.UpdateAclL(KNSmlDMMMSAclName,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclMMRecep,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclCreMode,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclAMsg,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclRcvAds,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclImgSize,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclDRepSnd,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclRcvRep,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclMsgVal,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclMMSSAddr,KNSmlDmMMSAclDefault);
    session.UpdateAclL(KNSmlDMMMSAclPrimaryMToNapIDL,KNSmlDmMMSAclDefault);
    CleanupStack::PopAndDestroy(); //session
    
    _DBG_FILE("CNSmlDmMMSAdapter::DDFStructureL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmMMSAdapter::UpdateLeafObjectL()
// Updates value of leaf object
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                           const TDesC8& aLUID, 
                                           const TDesC8& aObject, 
                                           const TDesC8& /*aType*/, 
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::UpdateLeafObjectL(): begin");
    CSmlDmAdapter::TError status = ValidateUriL( aURI );
    if ( status == EOk )
        {
        _DBG_FILE("CNSmlDmMMSAdapter::UpdateLeafObjectL(): OK update");
        status = UpdateDataL(aURI, aLUID, aObject, aStatusRef);
        }
    // ImgSize and MMSAddr are always stored last, from the 
    // CompleteOutstandingCmdsL() function
    if( ( aStatusRef != iStatusRef ) && (aStatusRef != iImgSizeStatusRef ))
        {
        StoreSettingsL();
        Callback().SetStatusL(aStatusRef, status);
        }
    _DBG_FILE("CNSmlDmMMSAdapter::UpdateLeafObjectL(): end");
    }
// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID )
// This should delete values or accounts, but in case of MMS adapter the delete 
// is not allowed
// and the adapters returns allways the EError.
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::DeleteObjectL( const TDesC8& /*aURI*/, 
                                       const TDesC8& /*aLUID*/, 
                                       const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::DeleteLeafObjectL( ): begin");

    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlDmMMSAdapter::DeleteLeafObjectL( ): end");
    }
// -----------------------------------------------------------------------------
//  CNSmlDmMMSAdapter::FetchLeafObjectL( const TDesC8& aURI, 
//  const TDesC8& aLUID,
//  const TDesC8& aType, 
//  const TInt aResultsRef, 
//  const TInt aStatusRef )
//  Fetches the values of leaf objects.
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                          const TDesC8& /*aLUID*/, 
                                          const TDesC8& aType, 
                                          const TInt aResultsRef, 
                                          const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::FetchLeafObjectL(): begin");
    CBufBase* result = CBufFlat::NewL(64);
    CleanupStack::PushL(result);
    CSmlDmAdapter::TError status = FetchObjectL(aURI, *result);
    if(status == CSmlDmAdapter::EOk)
        {
        Callback().SetResultsL(aResultsRef,*result,aType);
        }
    Callback().SetStatusL(aStatusRef, status);

    CleanupStack::PopAndDestroy(); //result
    _DBG_FILE("CNSmlDmMMSAdapter::FetchLeafObjectL(): end");
    }

//----------------------------------------------------------------------
// TUint32 GetIapIdL( TUint32 aUid )
// Gets the IAP ID 
//----------------------------------------------------------------------
TUint32 CNSmlDmMMSAdapter::GetIapIdL( TUint32 aUid )
        {
        TUint32 retval( 0 );
        RCmManagerExt  cmmanagerExt;
        cmmanagerExt.OpenL();
        CleanupClosePushL(cmmanagerExt);
        RCmConnectionMethodExt cm;
        cm = cmmanagerExt.ConnectionMethodL( aUid );
        CleanupClosePushL( cm );
        retval = cm.GetIntAttributeL(CMManager::ECmIapId);
        CleanupStack::PopAndDestroy( 2 ); //cmmanagerext,cm
        return retval;

        }


// -----------------------------------------------------------------------------
//  CNSmlDmMMSAdapter::ChildURIListL( const TDesC8& aURI, 
//                                       const TDesC8& /*aLUID*/, 
//                                       const CArrayFix<TSmlDmMappingInfo>& 
//                                       /*aPreviousURISegmentList*/, 
//                                       const TInt aResultsRef, 
//                                       const TInt aStatusRef )
//  Retuns the list of children of the node
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::ChildURIListL( const TDesC8& aURI, 
                                       const TDesC8& /*aLUID*/, 
                                       const CArrayFix<TSmlDmMappingInfo>& 
                                       /*aPreviousURISegmentList*/, 
                                       const TInt aResultsRef, 
                                       const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::ChildURIListL(): begin");
    TRAPD(err, CreateMMSClientL());
    if(err!=KErrNone)
        {
        Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
        _DBG_FILE("CNSmlDmMMSAdapter::ChildURIListL(): EError end");
        return;
        }

    CBufBase* currentList = CBufFlat::NewL(1);
    CleanupStack::PushL(currentList);

    TMsvId id = iMmsSettings->Service();
    if(id==KMsvNullIndexEntryId)
        {
        if(aURI.Compare(KNSmlDMMMSNodeName)==0)
            {
            Callback().SetResultsL(aResultsRef,*currentList,KNullDesC8);
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlDmMMSAdapter::ChildURIListL(): end");
            }
        else
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlDmMMSAdapter::ChildURIListL(): ENotFound end");
            }
        CleanupStack::PopAndDestroy(); //currentList
        return;
        }

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    TPtrC8 lastSeg = LastURISeg(aURI);
    TInt num = NumOfURISegs(aURI);

    if(aURI.Compare(KNSmlDMMMSNodeName)==0)
        {
        currentList->InsertL(0,KNSmlDMMMSAccount8);
        }
    else if(num==2&&aURI.Find(KNSmlDMMMSAccount8)>=0)
        {
        currentList->InsertL(0,KNSmlDMMMSListOfLeafsLo);
        }
    else if(num==3&&lastSeg.Compare(KNSmlDMMMSCon)==0)
        {
        currentList->InsertL(0,KNSmlDMMMSConDynamicName8);
        }
    else if(num==4&&lastSeg.Compare(KNSmlDMMMSConDynamicName16)==0)
        {
        currentList->InsertL(0,KNSmlDMMMSListOfLeafsMid);
        }
    else if(num==5&&lastSeg.Compare(KNSmlDMMMSMToNapID)==0)
        {
        currentList->InsertL(0,KNSmlDMMMSValuePrimary);
        }
    else if(num==6&&(lastSeg.Compare(KNSmlDMMMSDynamicPrimary)==0))
        {
        currentList->InsertL(0,KNSmlDMMMSListOfLeafsHi);
        }
    else
        {
        status=CSmlDmAdapter::ENotFound;
        }
    Callback().SetStatusL(aStatusRef, status);
    if(status==CSmlDmAdapter::EOk)
        {
        Callback().SetResultsL(aResultsRef,*currentList,KNullDesC8);
        }
    CleanupStack::PopAndDestroy(); //currentList

    _DBG_FILE("CNSmlDmMMSAdapter::ChildURIListL(): end");
    }
// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::AddNodeObjectL( const TDesC8& aURI, 
// const TDesC8& aParentLUID )
// This should add a new account, but in case of MMS settings only one 
// account exist,
// and so the add is not possible. Returns allways CSmlDmAdapter::EError.
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::AddNodeObjectL( const TDesC8& /*aURI*/, 
                                        const TDesC8& /*aParentLUID*/, 
                                        const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::AddNodeObjectL(): begin");
    //If add is needed in future then add call to AddObjectL function here
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlDmMMSAdapter::AddNodeObjectL(): end");
    }
// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
// const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType,
//  const TInt aStatusRef )
// This should update a stream object, but in case of MMS settings streaming is 
// not supported.
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                           const TDesC8& /*aLUID*/, 
                                           RWriteStream*& /*aStream*/, 
                                           const TDesC8& /*aType*/, 
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::UpdateLeafObjectL(stream): begin");
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlDmMMSAdapter::UpdateLeafObjectL(stream): end");
    }
// -----------------------------------------------------------------------------
// void CNSmlDmMMSAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
// const TDesC8& aLUID, const TDesC8& aType, const TInt aResultsRef, 
// const TInt aStatusRef )
// Fetches data object and returns its size.
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                              const TDesC8& /*aLUID*/, 
                                              const TDesC8& aType, 
                                              const TInt aResultsRef, 
                                              const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::FetchLeafObjectSizeL(): begin");

    CBufBase* result = CBufFlat::NewL(64);
    CleanupStack::PushL(result);
    CSmlDmAdapter::TError status = FetchObjectL(aURI, *result);
    if( status == CSmlDmAdapter::EOk )
        {
        TInt objSizeInBytes = result->Size();
        TBuf8<KNSmlDmMmsObjectSizeWidth> stringObjSizeInBytes;
        stringObjSizeInBytes.Num( objSizeInBytes );
        result->Reset();
        result->InsertL( 0, stringObjSizeInBytes );
        Callback().SetResultsL(aResultsRef, *result, aType);
        }
    Callback().SetStatusL(aStatusRef, status);
    CleanupStack::PopAndDestroy(); //result
    _DBG_FILE("CNSmlDmMMSAdapter::FetchLeafObjectSizeL(): end");
    }
//==============================================================================
// CNSmlDmMMSAdapter::ExecuteCommandL()
//==============================================================================
void CNSmlDmMMSAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                         const TDesC8& /*aLUID*/, 
                                         const TDesC8& /*aArgument*/, 
                                         const TDesC8& /*aType*/, 
                                         const TInt aStatusRef )
    {
    //Not supported 
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    }

//==============================================================================
// CNSmlDmMMSAdapter::ExecuteCommandL()
//==============================================================================
void CNSmlDmMMSAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                         const TDesC8& /*aLUID*/, 
                                         RWriteStream*& /*aStream*/, 
                                         const TDesC8& /*aType*/, 
                                         const TInt aStatusRef )
    {
    //Not supported
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    }

//==============================================================================
// CNSmlDmMMSAdapter::CopyCommandL()
//==============================================================================
void CNSmlDmMMSAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                                      const TDesC8& /*aTargetLUID*/, 
                                      const TDesC8& /*aSourceURI*/, 
                                      const TDesC8& /*aSourceLUID*/, 
                                      const TDesC8& /*aType*/, 
                                      TInt aStatusRef )
    {
    //Not supported
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);

    }

//==============================================================================
// CNSmlDmMMSAdapter::StartAtomicL()
//==============================================================================
void CNSmlDmMMSAdapter::StartAtomicL()
    {
    //Not supported
    }

//==============================================================================
// CNSmlDmMMSAdapter::CommitAtomicL()
//==============================================================================
void CNSmlDmMMSAdapter::CommitAtomicL()
    {
    //Not supported
    }

//==============================================================================
// CNSmlDmMMSAdapter::RollbackAtomicL()
//==============================================================================
void CNSmlDmMMSAdapter::RollbackAtomicL()
    {
    //Not supported
    }

//==============================================================================
// CNSmlDmMMSAdapter::StreamingSupport()
//==============================================================================
TBool CNSmlDmMMSAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    return EFalse;
    }

//==============================================================================
// CNSmlDmMMSAdapter::StreamCommittedL()
//==============================================================================
void CNSmlDmMMSAdapter::StreamCommittedL()
    {
    //Not supported
    }
// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::CompleteOutstandingCmdsL()
// Indication of message ending. The buffered commands must be executed before 
// return of this function.
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE("CNSmlDmMMSAdapter::CompleteOutstandingCmdsL(): begin");
    if(iStatusRef!=KNSmlMMSUndefinedStatusRef)
        {
        Callback().SetStatusL(iStatusRef,UpdateMMSAddrL());
        delete iMMSSAddr;
        iMMSSAddr = 0;
        iStatusRef = KNSmlMMSUndefinedStatusRef;
        }
    StoreSettingsL();
    _DBG_FILE("CNSmlDmMMSAdapter::CompleteOutstandingCmdsL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::HandleSessionEventL()
// Inherited from MMsvSessionObserver, and this is needs to implemented for 
// opening the CMsvSession
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::HandleSessionEventL( TMsvSessionEvent /*aEvent*/, 
                                             TAny* /*aArg1*/, 
                                             TAny* /*aArg2*/, 
                                             TAny* /*aArg3*/ )
    {
    }


// -----------------------------------------------------------------------------
// CSmlDmAdapter::TError CNSmlDmMMSAdapter::UpdateDataL()
// The function is used for updating the MMS settings. The function is used in 
// AddLeafObjectL() and UpdateLeafObjectL()
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmMMSAdapter::UpdateDataL(const TDesC8& aURI, 
                                                     const TDesC8& /*aParentLUID*/,
                                                     const TDesC8& aObject, 
                                                     TInt aStatusRef)
    {
    _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): begin");
    HBufC8* data = NULL;
    TRAPD(err, CreateMMSClientL());
    if(err!=KErrNone)
        {
        _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): EError end");
        return CSmlDmAdapter::EError;
        }

    TMsvId id = iMmsSettings->Service();

    if( id == KMsvNullIndexEntryId || aURI.Find( KNSmlDMMMSAccount8 )
        ==KErrNotFound )
        {
        _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): ENotFound end");
        return CSmlDmAdapter::ENotFound;
        }

    TPtrC8 uriSeg = LastURISeg(aURI);
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    if(uriSeg.Compare(KNSmlDMMMSName)==0)
        {
        // Not supported.
        _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): ENotFound end");
        status = CSmlDmAdapter::ENotFound;
        }
    else if(uriSeg.Compare(KNSmlDMMMSMMRecep)==0)
        {
        data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        if(dataPtr.Find(KNSmlDMMMSValueAlwaysOn)==0)
            {
            iMmsSettings->SetReceivingModeHome(EMmsReceivingAutomatic);
            iMmsSettings->SetReceivingModeForeign(EMmsReceivingAutomatic);
            }
        else if(dataPtr.Compare(KNSmlDMMMSValueOff)==0)
            {
            iMmsSettings->SetReceivingModeHome(EMmsReceivingReject);
            iMmsSettings->SetReceivingModeForeign(EMmsReceivingReject);
            }
        else if(dataPtr.Find(KNSmlDMMMSValueAutomaticAtHome)==0)
            {
            iMmsSettings->SetReceivingModeHome(EMmsReceivingAutomatic);
            iMmsSettings->SetReceivingModeForeign(EMmsReceivingReject);
            }
        else if (dataPtr.Find(KNSmlDMMMSValueAlwaysManual2)==0 ||
                 dataPtr.Find(KNSmlDMMMSValueAlwaysManual)==0 )
            {
            iMmsSettings->SetReceivingModeHome(EMmsReceivingManual);
            iMmsSettings->SetReceivingModeForeign(EMmsReceivingManual);
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        CleanupStack::PopAndDestroy(); //data
        }
    else if(uriSeg.Compare(KNSmlDMMMSCreMode)==0)
        {
        data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        if(dataPtr.Find(KNSmlDMMMSValueFree)==0)
            {
            iMmsSettings->SetCreationMode(EMmsCreationModeFree);
            }
        else if(dataPtr.Find(KNSmlDMMMSValueRestricted)==0)
            {
               iMmsSettings->SetImageWidth(KImageRichWidth);
               iMmsSettings->SetImageHeight(KImageRichHeight);
               iMmsSettings->SetCreationMode(EMmsCreationModeRestricted);
            }
        else if(dataPtr.Find(KNSmlDMMMSValueWarning)==0)
            {
            iMmsSettings->SetCreationMode(EMmsCreationModeWarning);
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        CleanupStack::PopAndDestroy(); //data
        }
    else if( uriSeg.Compare(KNSmlDMMMSAMsg) == 0 )
        {
        data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        TBool object=EFalse;
        if(dataPtr.Find(KNSmlDMMMSValueTrue)>=0)
            {
            object = ETrue;
            }
        iMmsSettings->SetAcceptAnonymousMessages(object);
        CleanupStack::PopAndDestroy(); //data
        }
    else if( uriSeg.Compare(KNSmlDMMMSRcvAds) == 0 )
        {
        data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        TBool object=EFalse;
        if(dataPtr.Find(KNSmlDMMMSValueTrue)>=0)
            {
            object = ETrue;
            }
        iMmsSettings->SetAcceptAdvertisementMessages(object);
        CleanupStack::PopAndDestroy(); //data
        }
    else if(uriSeg.Compare(KNSmlDMMMSImgSize)==0)
        {
        data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        if(dataPtr.Compare(KNSmlDMMMSValueLarge)==0)
            {
             switch(iMmsSettings->CreationMode())
            {
                case EMmsCreationModeRestricted:
                status = CSmlDmAdapter::EInvalidObject;
                break;
                default : 
                iMmsSettings->SetImageWidth(KImageMegapixelWidth);
                iMmsSettings->SetImageHeight(KImageMegapixelHeight);
                break;
            }
            iImgSizeStatusRef = KNSmlMMSUndefinedStatusRef;
            }
        else if(dataPtr.Compare(KNSmlDMMMSValueSmall)==0)
            {
            iMmsSettings->SetImageWidth(KImageRichWidth);
            iMmsSettings->SetImageHeight(KImageRichHeight);
            iImgSizeStatusRef = KNSmlMMSUndefinedStatusRef;
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        CleanupStack::PopAndDestroy(); //data
        }
    else if(uriSeg.Compare(KNSmlDMMMSDRepSnd)==0)
        {
        data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        if(dataPtr.Compare(KNSmlDMMMSValueFalse)==0)
            {
            iMmsSettings->SetDeliveryReportSendingAllowed(KMmsYes);
            }
        else
            {
            iMmsSettings->SetDeliveryReportSendingAllowed(KMmsNo);
            }
        CleanupStack::PopAndDestroy(); //data
        }
    else if(uriSeg.Compare(KNSmlDMMMSRcvRep)==0)
        {
        data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        if(dataPtr.Compare(KNSmlDMMMSValueTrue)==0)
            {
            iMmsSettings->SetDeliveryReportWanted(KMmsYes);
            }
        else
            {
            iMmsSettings->SetDeliveryReportWanted(KMmsNo);
            }
        CleanupStack::PopAndDestroy(); //data
        }
    else if(uriSeg.Compare(KNSmlDMMMSMsgVal)==0)
        {
        data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.LowerCase();
        if(dataPtr.Compare(KNSmlDMMMSValueHour)==0 ||
           dataPtr.Compare(KNSmlDMMMSValueHourShort)==0)
            {
            iMmsSettings->SetExpiryInterval(KNSmlDMMMSExpiryHour);
            }
        else if(dataPtr.Find(KNSmlDMMMSValueSixHoursShort)==0 ||
                dataPtr.Find(KNSmlDMMMSValueSixHoursShortNum)==0)
            {
            iMmsSettings->SetExpiryInterval(KNSmlDMMMSExpirySixHours);
            }
        else if(dataPtr.Find(KNSmlDMMMSValueDayShort)==0)
            {
            iMmsSettings->SetExpiryInterval(KNSmlDMMMSExpiryDay);
            }
        else if(dataPtr.Find(KNSmlDMMMSValueThreeDaysShort)==0 ||
                dataPtr.Find(KNSmlDMMMSValueThreeDaysShortNum)==0)
            {
            iMmsSettings->SetExpiryInterval(KNSmlDMMMSExpiryThreeDays);
            }
        else if(dataPtr.Find(KNSmlDMMMSValueWeekShort)>=0)
            {
            iMmsSettings->SetExpiryInterval(KNSmlDMMMSExpiryWeek);
            }
        else if(dataPtr.Find(KNSmlDMMMSValueMaximumTimeShort)==0)
            {
            iMmsSettings->SetExpiryInterval(KNSmlDMMMSExpiryMaximum);
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        CleanupStack::PopAndDestroy(); //data
        }
    else if(uriSeg.Compare(KNSmlDMMMSMToNapIDL)==0)
        {
        TPtrC8 seg=LastURISeg(RemoveLastURISeg(aURI));
        if( seg.Compare(KNSmlDMMMSDynamicPrimary)==0 )
            {
            CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
            CleanupStack::PushL(iapmatch);
            
            TInt iap = iapmatch->IAPIdFromURIL( aObject );

            CleanupStack::PopAndDestroy(); // iapmatch
            
            if ( iap != KErrNotFound )
                {
                TRAP(err, iap = GetIapIdL( iap ) );
                DBG_ARGS(_S("IAP = %d"), iap);

                if( err == KErrNone)
                    {
                    ReplaceAPL( iap, KNSmlDMMMSFirstAccessPointId );
                    }
                else
                    {
                    _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): \
                    err!=KErrNone");
                    iap = KNSmlDMMMSIapNotDefined;
                    status = CSmlDmAdapter::EInvalidObject;
                    }
                }
            else
                {
                _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): status = \
                CSmlDmAdapter::EInvalidObject");
                status = CSmlDmAdapter::EInvalidObject;
                }
            }
        else
            {
            if( status == CSmlDmAdapter::EOk )
                {
                status = CSmlDmAdapter::ENotFound;
                }
            }
        }
    else if(uriSeg.Compare(KNSmlDMMMSMMSSAddr)==0)
        {
        if(iMMSSAddr)
            {
            delete iMMSSAddr;
            iMMSSAddr = 0;
            }
        if(iStatusRef != KNSmlMMSUndefinedStatusRef)
            {
            Callback().SetStatusL(iStatusRef,CSmlDmAdapter::EError);
            }
        iMMSSAddr = aObject.AllocL();
        iStatusRef = aStatusRef;
        }

    _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): end");
    return status;
    }

// -----------------------------------------------------------------------------
// void CNSmlDmMMSAdapter::ReplaceAP( TInt aIapId, TInt aPos )
// replace AP, if no AP exitst in aPos then just add it.
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::ReplaceAPL( TInt aIapId, TInt aPos )
    {
    if( iMmsSettings->AccessPoint( aPos ) != KErrNotFound )
        {
        iMmsSettings->DeleteAccessPointL( aPos );
        }
    iMmsSettings->AddAccessPointL( aIapId, aPos );
    }

// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::UpdateMMSAddrL()
// Executes the buffered MMSSAddr update command. 
// This command is called from CompleteOutstandingCmdsL() 
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmMMSAdapter::UpdateMMSAddrL()
    {
    _DBG_FILE("CNSmlDmMMSAdapter::UpdateMMSAddrL(): begin");
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    TBool ok = EFalse;
    TUint32 wapid = iMmsSettings->AccessPoint(KNSmlDMMMSFirstAccessPointId);
    
    //Check if locked , if locked unlock else execute normal sequence.
        
        TBool reclock = EFalse;
        TBool lockerror =KErrNone;
        reclock = IsWAPAccessPointRecordLockedL(wapid);
        if(reclock)
        {
        	_DBG_FILE("CNSmlDmMMSAdapter::UpdateMMSAddrL():making record unlock");
        	//unlock the record
        	lockerror = DoProtectWAPAccessRecordL(wapid,EFalse);
        	_DBG_FILE("CNSmlDmMMSAdapter::UpdateMMSAddrL():record unlocked with error as ");
        	DBG_ARGS(_S("err = %d"), lockerror);
        }
        
    
    CCommsDbTableView* tableView=0;
    tableView = iDatabase->OpenViewMatchingUintLC(TPtrC(WAP_ACCESS_POINT),
                                               TPtrC(COMMDB_ID), wapid );        
    if(tableView->GotoFirstRecord()==KErrNone)
        {
        _DBG_FILE("CNSmlDmMMSAdapter::UpdateMMSAddrL(): GotoFirstRecord");
        
        
        //If it was unlocked, lock it again
        
        if(lockerror == KErrNone)
        {
        	TInt err = tableView->UpdateRecord();
        	if(err == KErrNone)
        	{
        		HBufC* object = HBufC::NewLC(iMMSSAddr->Length());
        TPtr obPtr = object->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( obPtr, *iMMSSAddr);

        TRAPD(err,tableView->WriteLongTextL(TPtrC(WAP_START_PAGE),obPtr));
        DBG_ARGS(_S("err = %d"), err);
        if(err==KErrNone)
            {
            _DBG_FILE("CNSmlDmMMSAdapter::UpdateMMSAddrL(): Write OK");
            tableView->PutRecordChanges();
            ok = ETrue;
            }
        CleanupStack::PopAndDestroy(); //object
        
        	}
        	
        }
        
        }
    CleanupStack::PopAndDestroy(1); //tableview  //, commDb--iDatabase
if(reclock)
        	{
        		TInt err = DoProtectWAPAccessRecordL(wapid,ETrue);
        	_DBG_FILE("CNSmlDmMMSAdapter::UpdateMMSAddrL():record again locked with error as ");
        	 DBG_ARGS(_S("err = %d"), err);
        	 reclock = EFalse;
        	}
    if(!ok)
        {
        status = CSmlDmAdapter::EError;
        }

    _DBG_FILE("CNSmlDmMMSAdapter::UpdateMMSAddrL(): end");
    return status;
    }

// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::DesToInt()
// Converts a 16 bit descriptor to TInt
// -----------------------------------------------------------------------------
TInt CNSmlDmMMSAdapter::DesToInt(const TDesC& aLuid)
    {
    TLex16 lex(aLuid);
    TInt value = 0;
    lex.Val(value);
    return value;
    }


// -----------------------------------------------------------------------------
// TPtrC8 CNSmlDmMMSAdapter::RemoveLastURISeg(const TDesC8& aURI)
// returns parent uri, i.e. removes last uri segment
// -----------------------------------------------------------------------------
TPtrC8 CNSmlDmMMSAdapter::RemoveLastURISeg(const TDesC8& aURI)
    {
    TInt i;
    for(i=aURI.Length()-1;i>=0;i--)
        {
        if(aURI[i]==KNSmlDMMMSSeparatorDef)
            {
            break;
            }
        }
    return aURI.Left(i);
    }



// -----------------------------------------------------------------------------
// TInt CNSmlDmMMSAdapter::NumOfURISegs(const TDesC8& aURI)
// Returns the num of uri segs
// -----------------------------------------------------------------------------
TInt CNSmlDmMMSAdapter::NumOfURISegs(const TDesC8& aURI)
    {
    TInt numOfURISegs = 1;
    for(TInt i=0;i<aURI.Length();i++)
        {
        if(aURI[i]==KNSmlDMMMSSeparatorDef)
            {
            numOfURISegs++;
            }
        }
    return numOfURISegs;
    }


// -----------------------------------------------------------------------------
// TPtrC8 CNSmlDmMMSAdapter::LastURISeg(const TDesC8& aURI)
// Returns only the last uri segemnt
// -----------------------------------------------------------------------------
TPtrC8 CNSmlDmMMSAdapter::LastURISeg(const TDesC8& aURI)
    {
    _DBG_FILE("CNSmlDmMMSAdapter::LastURISeg() : begin");
    TInt i;
    for(i=aURI.Length()-1;i>=0;i--)
        {
        if(aURI[i]==KNSmlDMMMSSeparatorDef)
            {
            break;
            }
        }
    _DBG_FILE("CNSmlDmMMSAdapter::LastURISeg() : end");
    if(i==0)
        {
        return aURI;
        }
    else
        {
        return aURI.Mid(i+1);
        }
    }


// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::CreateMMSClientL()
// Creates MMS client mtm
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::CreateMMSClientL()
    {
    if(iMmsClient ==0)
        {
        iMsvSession = CMsvSession::OpenSyncL( *this );
        iClientMtmRegistry = CClientMtmRegistry::NewL(* iMsvSession );
        iMmsClient = (CMmsClientMtm *) iClientMtmRegistry->
            NewMtmL( KUidMsgTypeMultimedia );
        iMmsClient->RestoreSettingsL();
        iMmsSettings = CMmsSettings::NewL();
        iMmsSettings->CopyL( iMmsClient->MmsSettings() );
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDmMMSAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,
                                       TSmlDmAccessTypes aAccTypes,
                                       MSmlDmDDFObject::TOccurence aOccurrence, 
                                       MSmlDmDDFObject::TScope aScope, 
                                       MSmlDmDDFObject::TDFFormat aFormat,
                                       const TDesC8& aDescription)
    {
    aNode.SetAccessTypesL(aAccTypes);
    aNode.SetOccurenceL(aOccurrence);
    aNode.SetScopeL(aScope);
    aNode.SetDFFormatL(aFormat);
    if(aFormat!=MSmlDmDDFObject::ENode)
        {
        aNode.AddDFTypeMimeTypeL(KNSmlDMMMSTextPlain);
        }
    aNode.SetDescriptionL(aDescription);

    }

// -----------------------------------------------------------------------------
// CSmlDmAdapter::TError FetchObjectL(const TDesC8& aURI, const TDesC8& aObject)
// Fetches data object and returns it in parameter. 
// Return values according to CSmlDmAdapter::TError.
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmMMSAdapter::FetchObjectL(const TDesC8& aURI, 
                                                      CBufBase& aObject)
    {
    _DBG_FILE("CNSmlDmMMSAdapter::FetchObjectL(): begin");
    TRAPD(err, CreateMMSClientL());
    if(err!=KErrNone)
        {
        _DBG_FILE("CNSmlDmMMSAdapter::FetchObjectL(): EError end");
        return CSmlDmAdapter::EError;
        }

    TMsvId id = iMmsSettings->Service();

    if(id == KMsvNullIndexEntryId||aURI.Find(KNSmlDMMMSAccount8)==KErrNotFound)
        {
        _DBG_FILE("CNSmlDmMMSAdapter::FetchObjectL(): ENotFound end");
        return CSmlDmAdapter::ENotFound;
        }

    TPtrC8 uriSeg = LastURISeg(aURI);
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    if(uriSeg.Compare(KNSmlDMMMSName)==0)
        {
        status = CSmlDmAdapter::ENotFound;
        }
    else if(uriSeg.Compare(KNSmlDMMMSMMRecep)==0)
        {
        TMmsReceivingMode rcvHome = iMmsSettings->ReceivingModeHome();
        TMmsReceivingMode rcvForeign = iMmsSettings->ReceivingModeForeign();
        if ( rcvHome == EMmsReceivingAutomatic )
            {
            if ( rcvForeign == EMmsReceivingAutomatic )
                {
                aObject.InsertL( 0, KNSmlDMMMSValueAlwaysOn );
                }
            else
                {
                aObject.InsertL( 0, KNSmlDMMMSValueAutomaticAtHome );
                }
            }
        else if ( rcvHome == EMmsReceivingReject )
            {
            aObject.InsertL( 0, KNSmlDMMMSValueOff );
            }
        else if ( rcvHome == EMmsReceivingManual )
            {
            aObject.InsertL( 0, KNSmlDMMMSValueAlwaysManual );
            }
        else
            {
            aObject.InsertL( 0, KNSmlDMMMSValueAutomaticAtHome );
            }

        }
    else if(uriSeg.Compare(KNSmlDMMMSCreMode)==0)
        {
        switch(iMmsSettings->CreationMode())
            {
        case EMmsCreationModeWarning:
            aObject.InsertL(0,KNSmlDMMMSValueWarning);
            break;
            
        case EMmsCreationModeFree:
            aObject.InsertL(0,KNSmlDMMMSValueFree);
            break;

        case EMmsCreationModeRestricted:
            aObject.InsertL(0,KNSmlDMMMSValueRestricted);
            break;

        default:
            aObject.InsertL(0,KNSmlDMMMSValueWarning);
            break;
            }
        }
    else if(uriSeg.Compare(KNSmlDMMMSAMsg)==0)
        {
        if(iMmsSettings->AcceptAnonymousMessages())
            {
            aObject.InsertL(0,KNSmlDMMMSValueTrue);
            }
        else
            {
            aObject.InsertL(0,KNSmlDMMMSValueFalse);
            }
        }
    else if(uriSeg.Compare(KNSmlDMMMSRcvAds)==0)
        {
        if(iMmsSettings->AcceptAdvertisementMessages())
            {
            aObject.InsertL(0,KNSmlDMMMSValueTrue);
            }
        else
            {
            aObject.InsertL(0,KNSmlDMMMSValueFalse);
            }
        }
    else if(uriSeg.Compare(KNSmlDMMMSImgSize)==0)
        {
        if( iMmsSettings->ImageWidth() == KImageRichWidth && 
            iMmsSettings->ImageHeight() == KImageRichHeight )
            {
            aObject.InsertL(0,KNSmlDMMMSValueSmall);
            }
        else if ( iMmsSettings->ImageWidth() == KImageMegapixelWidth && 
                  iMmsSettings->ImageHeight() == KImageMegapixelHeight )
            {
            aObject.InsertL(0,KNSmlDMMMSValueLarge);
            }
        }
    else if(uriSeg.Compare(KNSmlDMMMSDRepSnd)==0)
        {
        if( iMmsSettings->DeliveryReportSendingAllowed() == KMmsYes)
            {
            aObject.InsertL(0,KNSmlDMMMSValueFalse);
            }
        else
            {
            aObject.InsertL(0,KNSmlDMMMSValueTrue);
            }
        }
    else if(uriSeg.Compare(KNSmlDMMMSRcvRep)==0)
        {
        if( iMmsSettings->DeliveryReportWanted() == KMmsYes)
            {
            aObject.InsertL(0,KNSmlDMMMSValueTrue);
            }
        else
            {
            aObject.InsertL(0,KNSmlDMMMSValueFalse);
            }
        }
    else if( uriSeg.Compare( KNSmlDMMMSMsgVal ) == 0 )
        {
        switch(iMmsSettings->ExpiryInterval())
            {
        case KNSmlDMMMSExpiryHour:
            aObject.InsertL(0,KNSmlDMMMSValueHourShort);
            break;
            
        case KNSmlDMMMSExpirySixHours:
            aObject.InsertL(0,KNSmlDMMMSValueSixHours);
            break;

        case KNSmlDMMMSExpiryDay:
            aObject.InsertL(0,KNSmlDMMMSValueDay);
            break;

        case KNSmlDMMMSExpiryThreeDays:
            aObject.InsertL(0,KNSmlDMMMSValueThreeDays);
            break;

        case KNSmlDMMMSExpiryWeek:
            aObject.InsertL(0,KNSmlDMMMSValueWeek);
            break;

        case KNSmlDMMMSExpiryMaximum:
            aObject.InsertL(0,KNSmlDMMMSValueMaximumTime);
            break;

        default:
            aObject.InsertL(0,KNSmlDMMMSValueThreeDays);
            break;
            }
        }
    else if(uriSeg.Compare(KNSmlDMMMSMToNapIDL)==0)
        {
        TUint32 iap = KNSmlDMMMSIapNotDefined;
        TInt32 mmsIap = -1;
        
        
        if(LastURISeg(RemoveLastURISeg(aURI)).Compare(
               KNSmlDMMMSDynamicPrimary)==0)
            {
            mmsIap = iMmsSettings->AccessPoint( KNSmlDMMMSFirstAccessPointId );
            }
        
        if(mmsIap != KErrNotFound )
        	{
        	iap = mmsIap;
        	TRAPD(err, iap =GetIapIdL(iap) ); 
        
        	if( iap != KNSmlDMMMSIapNotDefined && err == KErrNone )
            	{
            	CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
            	CleanupStack::PushL(iapmatch);
    
            	HBufC8* uri8 = iapmatch->URIFromIAPIdL( iap );
                
            if( uri8 )
                {
                CleanupStack::PushL(uri8);
                aObject.InsertL(0,uri8->Des());
                status = CSmlDmAdapter::EOk;
                CleanupStack::PopAndDestroy(); // uri8
                }                
            		CleanupStack::PopAndDestroy(); // iapMatch
            }        	
        } 
        }
    else if(uriSeg.Compare(KNSmlDMMMSMMSSAddr)==0)
        {
        CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        CleanupStack::PushL(commDb);

        TBool ok = EFalse;

        CCommsDbTableView* tableView=0;
        
        TInt apId = iMmsSettings->AccessPoint(KNSmlDMMMSFirstAccessPointId);
        
        if ( apId != KErrNotFound )
            {
            
            tableView = commDb->OpenViewMatchingUintLC(TPtrC(WAP_ACCESS_POINT),
                                                       TPtrC(COMMDB_ID),
                                                       apId);

            if(tableView->GotoFirstRecord()==KErrNone)
                {
                TBuf<KCommsDbSvrMaxFieldLength> columnValue;
                HBufC* tmpPage;
                tmpPage = tableView->ReadLongTextLC(TPtrC(WAP_START_PAGE));
            
                if(err==KErrNone)
                    {
                    HBufC8* val8 = HBufC8::NewLC(tmpPage->Size());
                    TPtr8 valPtr = val8->Des();
                    CnvUtfConverter::ConvertFromUnicodeToUtf8( valPtr, 
                                                               *tmpPage);
                    aObject.InsertL(0,valPtr);
                    ok = ETrue;
                    CleanupStack::PopAndDestroy(); //val8
                    }
                CleanupStack::PopAndDestroy(tmpPage);
                }
            CleanupStack::PopAndDestroy(); //tableview
            }
        CleanupStack::PopAndDestroy(); //commDb
        if(!ok)
            {
            status = CSmlDmAdapter::EError;
            }
        }
    else 
        {
        status = CSmlDmAdapter::ENotFound;
        }
    _DBG_FILE("CNSmlDmMMSAdapter::FetchObjectL(): end");
    return status;
    }

// -----------------------------------------------------------------------------
// void StoreSettingsL()
// Stores the MMS settings to the phone.
// 
// -----------------------------------------------------------------------------
void CNSmlDmMMSAdapter::StoreSettingsL()
    {
    _DBG_FILE("CNSmlDmMMSAdapter::StoreSettingsL(): begin");
    if ( iMmsSettings && iMmsClient )
        {
        iMmsSettings->SaveSettingsL();
        iMmsClient->SetSettingsL( *iMmsSettings );
        }
    _DBG_FILE("CNSmlDmMMSAdapter::StoreSettingsL(): end");
    }

// -----------------------------------------------------------------------------
// CSmlDmAdapter::TError ValidateUriL( const TDesC8& aURI )
// Validate URI, return error if not valid.
// 
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmMMSAdapter::ValidateUriL( const TDesC8& aURI )
    {
    _DBG_FILE("CNSmlDmMMSAdapter::ValidateUriL(): begin");
    CSmlDmAdapter::TError status = EOk;
    TPtrC8 parentUri = RemoveLastURISeg(aURI);
    switch ( NumOfURISegs( aURI ) )
        {
    case 3:
        if ( parentUri.Match( KNSmlDMMMSLevel3Parents ) != 0 )
            {
            _DBG_FILE("CNSmlDmMMSAdapter::ValidateUriL(): 3");
            status = ENotFound;
            }
        break;
    case 5:
        if ( parentUri.Match( KNSmlDMMMSLevel5Parents ) != 0 )
            {
            _DBG_FILE("CNSmlDmMMSAdapter::ValidateUriL(): 5");
            status = ENotFound;
            }
        break;
    case 7:
        if ( parentUri.Match( KNSmlDMMMSLevel7Parents1 ) != 0 && 
             parentUri.Match( KNSmlDMMMSLevel7Parents2 ) != 0)
            {
            _DBG_FILE("CNSmlDmMMSAdapter::ValidateUriL(): 7");
            status = ENotFound;
            }
        break;
    default:
        status = ENotFound;
        break;

        }
    _DBG_FILE("CNSmlDmMMSAdapter::ValidateUriL(): end");
    return status;
    }

// -----------------------------------------------------------------------------
// TImplementationProxy ImplementationTable[]
// Needed because of ECOM architecture
// -----------------------------------------------------------------------------

#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KNSmlDMMMSAdapterImplUid, 
                               CNSmlDmMMSAdapter::NewL)
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy()
// Needed because of ECOM architecture
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& 
                                                               aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmMMSAdapter: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmMMSAdapter: end");
    return ImplementationTable;
    }


//----------------------------------------------------------------------
//TBool CNSmlDmMMSAdapter::IsWAPAccessPointRecordLockedL(TUint32 aRecord)
//check for wap access point record  lock status 
//----------------------------------------------------------------------
    
TBool CNSmlDmMMSAdapter::IsWAPAccessPointRecordLockedL(TUint32 aRecord)
	{
	_DBG_FILE("CNSmlDmMMSAdapter::IsWAPAccessPointRecordLockedL():  BEGIN");	
	
	TBool ret = EFalse;
    CCommsDbTableView* wapView;
    DBG_ARGS8(_S8("CNSmlDmMMSAdapter::IsWAPAccessPointRecordLockedL record being checked = %d"),aRecord);
    wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(WAP_ACCESS_POINT),
                                                            TPtrC(COMMDB_ID),
                                                            aRecord);
      	                                                      
    TInt aAccess( 0 );
    if(wapView!=NULL)
    	{
   		_DBG_FILE("CNSmlDmMMSAdapter::IsWAPAccessPointRecordLockedL():wap record exists, check for access type");	
    	TInt err = (((CCommsDbProtectTableView*) wapView )->GetRecordAccess( aAccess ) );
    	DBG_ARGS8(_S8("CNSmlDmMMSAdapter::IsWAPAccessPointRecordLockedL , GetrecordAccess code is =%d and acess type of record = %d"),err, aAccess);
    	CleanupStack::PopAndDestroy( wapView);                                            
    	if(aAccess == RDbRowSet::EReadOnly)                                                    
       		ret = ETrue;
    	}
    
    _DBG_FILE("CNSmlDmMMSAdapter::IsWAPAccessPointRecordLockedL():  END");	
    return ret;
        		
	}
//----------------------------------------------------------------------
//TBool CNSmlDmMMSAdapter::DoProtectWAPAccessRecordL(TUint32 aRecord,TBool aProtect)
// protect or unprotect wapaccesspoint record of access point being altered 
//----------------------------------------------------------------------


TInt CNSmlDmMMSAdapter::DoProtectWAPAccessRecordL(TUint32 aRecord,TBool aProtect)
	{
	_DBG_FILE("CNSmlDmMMSAdapter::DoProtectWAPAccessRecordL():  BEGIN");	
	DBG_ARGS8(_S8("CNSmlDmMMSAdapter::DoProtectWAPAccessRecordL , record and protect value  =%d  = %d"),aRecord, aProtect);
	TInt err = KErrNone;
	
	CCommsDbTableView* wapView;
    wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(WAP_ACCESS_POINT),
                                                            TPtrC(COMMDB_ID),
                                                            aRecord);
	if( wapView != NULL )
		{
		_DBG_FILE("CNSmlDmMMSAdapter::DoProtectWAPAccessRecordL():  wap record exists");	
		if( !aProtect )
    		{
    		err = ( (CCommsDbProtectTableView*) wapView )->UnprotectRecord();
    		DBG_ARGS8(_S8("CNSmlDmMMSAdapter::DoProtectWAPAccessRecordL ,error code of Unprotectrecord = %d"),err);
        	}
     			
     	else
     		{
     		err = ( (CCommsDbProtectTableView*) wapView )->ProtectRecord();
     		DBG_ARGS8(_S8("CNSmlDmMMSAdapter::DoProtectWAPAccessRecordL ,error code of ProtectRecord = %d"),err);	
     		}
  
  		CleanupStack::PopAndDestroy( wapView);
		}
   	
	_DBG_FILE("CNSmlDmMMSAdapter::DoProtectWAPAccessRecordL():  END");	
	return err;
	}


	

// End of File
