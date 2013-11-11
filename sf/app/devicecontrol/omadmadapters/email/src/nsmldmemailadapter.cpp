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
* Description:    DM Email Settings Adapter
 *
*/




#include <implementationproxy.h> // For TImplementationProxy definition
#include <utf.h>
#include <iapprefs.h>
#include <smtpset.h>
#include <pop3set.h>
#include <imapset.h>
#include <sysutil.h>
#include <etelmm.h>

#include <cemailaccounts.h>

#include "nsmldmimpluids.h"
#include "nsmldmemailadapter.h"
#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldmconst.h"
#include "nsmldmiapmatcher.h"

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter* CNSmlDmEmailAdapter::NewL( )
// -----------------------------------------------------------------------------
CNSmlDmEmailAdapter* CNSmlDmEmailAdapter::NewL(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::NewL(): begin");
    CNSmlDmEmailAdapter* self = new (ELeave) CNSmlDmEmailAdapter(aDmCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    _DBG_FILE("CNSmlDmEmailAdapter::NewL(): end");
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::CNSmlDmEmailAdapter()
// -----------------------------------------------------------------------------

CNSmlDmEmailAdapter::CNSmlDmEmailAdapter( MSmlDmCallback* aDmCallback )
    : CSmlDmAdapter(aDmCallback)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::CNSmlDmEmailAdapter(): begin");
    iMsvSession = NULL;
    _DBG_FILE("CNSmlDmEmailAdapter::CNSmlDmEmailAdapter(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::ConstructL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::ConstructL()
    {
    iBuffer = new (ELeave) CArrayFixFlat <TNSmlEmailSettingsElement> 
        (KNSmlDmEmailGranularity);
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::~CNSmlDmEmailAdapter()
// -----------------------------------------------------------------------------
CNSmlDmEmailAdapter::~CNSmlDmEmailAdapter()
    {
    _DBG_FILE("CNSmlDmEmailAdapter::~CNSmlDmEmailAdapter(): begin");
    delete iMsvSession;
    if(iBuffer)
        {
        for(TInt i=0;i<iBuffer->Count();i++)
            {
            delete iBuffer->At(i).iMappingName;
            delete iBuffer->At(i).iName;
            delete iBuffer->At(i).iSmtpSettings;
            delete iBuffer->At(i).iPop3Settings;
            delete iBuffer->At(i).iImap4Settings;
            delete iBuffer->At(i).iIapPref;
            delete iBuffer->At(i).iSmtpIapPref;

            if(iBuffer->At(i).iNodeBuf)
                {
                for(TInt j=0;j<iBuffer->At(i).iNodeBuf->Count();j++)
                    {
                    delete iBuffer->At(i).iNodeBuf->At(j).iLastUriSeg;
                    delete iBuffer->At(i).iNodeBuf->At(j).iData;
                    }
                iBuffer->At(i).iNodeBuf->Reset();
                delete iBuffer->At(i).iNodeBuf;
                }
            }
        iBuffer->Reset();
        delete iBuffer;
        }

    _DBG_FILE("CNSmlDmEmailAdapter::~CNSmlDmEmailAdapter(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmEmailAdapter::DDFVersionL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::DDFVersionL(TDes& aDDFVersion): begin");
    aDDFVersion.InsertL(0,KNSmlDMEmailDDFVersion);
    _DBG_FILE("CNSmlDmEmailAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmEmailAdapter::DDFStructureL()
//
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::DDFStructureL(MSmlDmDDFObject& aDDF)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::DDFStructureL(): begin");

    TSmlDmAccessTypes accessTypesNode;

    accessTypesNode.SetGet();

    // Email
    MSmlDmDDFObject& email = aDDF.AddChildObjectL(KNSmlDMEmailNodeName);
    FillNodeInfoL(email,accessTypesNode,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EPermanent,
                  MSmlDmDDFObject::ENode,KNSmlDMEmailDescription);

    accessTypesNode.SetAdd();
    accessTypesNode.SetDelete();
    accessTypesNode.SetReplace();
    MSmlDmDDFObject& rtNode = email.AddChildObjectGroupL();


    FillNodeInfoL(rtNode,accessTypesNode,MSmlDmDDFObject::EZeroOrMore,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::ENode,
                  KNSmlDMDynamicNodeDescription);

    TSmlDmAccessTypes accessTypes;
    accessTypes.SetGet();
    accessTypes.SetAdd();
    accessTypes.SetReplace();
    //displayable account name
    MSmlDmDDFObject& name = rtNode.AddChildObjectL(KNSmlDMEmailName);
    FillNodeInfoL(name,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMNameDescription);

    //user id for email account
    MSmlDmDDFObject& uid = rtNode.AddChildObjectL(KNSmlDMEmailUID);
    FillNodeInfoL(uid,accessTypes,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMUIDDescription);

    //password for email account
    TSmlDmAccessTypes accessTypesPw;
    accessTypesPw.SetAdd();
    accessTypesPw.SetReplace();
    MSmlDmDDFObject& pw = rtNode.AddChildObjectL(KNSmlDMEmailPW);
    FillNodeInfoL(pw,accessTypesPw,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMPWDescription);

    //user's email address
    MSmlDmDDFObject& uaddr = rtNode.AddChildObjectL(KNSmlDMEmailUAddr);
    FillNodeInfoL(uaddr,accessTypes,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMUAddrDescription);

    //Displayable name for the user
    MSmlDmDDFObject& uname = rtNode.AddChildObjectL(KNSmlDMEmailUName);
    FillNodeInfoL(uname,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMUNameDescription);

    //Host name for the receiving server
    MSmlDmDDFObject& mrcv = rtNode.AddChildObjectL(KNSmlDMEmailMrcv);
    FillNodeInfoL(mrcv,accessTypes,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMrcvDescription);

    //Host name for the sending server
    MSmlDmDDFObject& msnd = rtNode.AddChildObjectL(KNSmlDMEmailMsnd);
    FillNodeInfoL(msnd,accessTypes,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMsndDescription);

    //Remote mailbox protocol
    MSmlDmDDFObject& mpro = rtNode.AddChildObjectL(KNSmlDMEmailMpro);
    FillNodeInfoL(mpro,accessTypes,MSmlDmDDFObject::EOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMMproDescription);

    //Use secure connection
    MSmlDmDDFObject& useSecCon = rtNode.AddChildObjectL(KNSmlDMEmailUseSecCon);
    FillNodeInfoL(useSecCon,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMUseSecConDescription);

    //Use APop
    MSmlDmDDFObject& useAPOP = rtNode.AddChildObjectL(KNSmlDMEmailUseAPOP);
    FillNodeInfoL(useAPOP, accessTypes, MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EBool,
                  KNSmlDMUseAPOPDescription );

    //Use smtp authentication
    MSmlDmDDFObject& useSauth = rtNode.AddChildObjectL(KNSmlDMEmailUseSauth);
    FillNodeInfoL(useSauth,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMUseSauthDescription);

    //smtp authentication user id
    MSmlDmDDFObject& sauthUid = rtNode.AddChildObjectL(KNSmlDMEmailSauthUID);
    FillNodeInfoL(sauthUid,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMSauthUIDDescription);

    //Use smtp authentication
    MSmlDmDDFObject& sauthPw = rtNode.AddChildObjectL(KNSmlDMEmailSauthPW);
    FillNodeInfoL(sauthPw,accessTypesPw,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMSauthPWDescription);

    //Use smtp authentication
    MSmlDmDDFObject& ptxtSAuth = rtNode.AddChildObjectL(KNSmlDMEmailPtxtSAuth);
    FillNodeInfoL(ptxtSAuth,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMPtxtSAuthDescription);

    //Link to IAP
    MSmlDmDDFObject& toNapId = rtNode.AddChildObjectL(KNSmlDMEmailToNapID);
    FillNodeInfoL(toNapId,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMToNapIDDescription);

    //Link to IAP for SMTP
    MSmlDmDDFObject& sToNapId = rtNode.AddChildObjectL(KNSmlDMEmailSToNapID);
    FillNodeInfoL(sToNapId,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMSToNapIDDescription);

    //SSL/TLS used in SMTP
    MSmlDmDDFObject& sUseSecCon = rtNode.
        AddChildObjectL(KNSmlDMEmailSUseSecCon);
    FillNodeInfoL(sUseSecCon,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMSUseSecConDescription);

    //StartTLS used in IMAP/POP
    MSmlDmDDFObject& useStartTls = rtNode.
        AddChildObjectL(KNSmlDMEmailUseStartTLS);
    FillNodeInfoL(useStartTls,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMUseStartTLSDescription);

    //StartTLS used in SMTP
    MSmlDmDDFObject& sUseStartTls = rtNode.
        AddChildObjectL(KNSmlDMEmailSUseStartTLS);
    FillNodeInfoL(sUseStartTls,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EBool,
                  KNSmlDMSUseStartTLSDescription);

    //port of IMAP/POP server
    MSmlDmDDFObject& mrcvPort = rtNode.AddChildObjectL(KNSmlDMEmailMrcvPort);
    FillNodeInfoL(mrcvPort,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EInt,
                  KNSmlDMMrcvPortDescription);

    //port of SMTP server
    MSmlDmDDFObject& msndPort = rtNode.AddChildObjectL(KNSmlDMEmailMsndPort);
    FillNodeInfoL(msndPort,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EInt,
                  KNSmlDMMsndPortDescription);

    //IMAP folder path
    MSmlDmDDFObject& folderPath = rtNode.
        AddChildObjectL(KNSmlDMEmailFolderPath);
    FillNodeInfoL(folderPath,accessTypes,MSmlDmDDFObject::EZeroOrOne,
                  MSmlDmDDFObject::EDynamic,MSmlDmDDFObject::EChr,
                  KNSmlDMFolderPathDescription);

    _DBG_FILE("CNSmlDmEmailAdapter::DDFStructureL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmEmailAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                             const TDesC8& aLUID,
                                             const TDesC8& aObject,
                                             const TDesC8& /*aType*/,
                                             const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(): begin");

    TInt index = -1;
    TBool firstAdd = EFalse;

    for(TInt i = 0; i<iBuffer->Count(); i++)
        {
        TPtrC8 parentUri = RemoveLastURISeg(aURI);
        //Check Node from buffer if match
        if(iBuffer->At(i).iMappingName->Compare(parentUri) == 0 )
            {
            firstAdd = ETrue;
            index = i;
            break;
            }
        }
 	
 	TInt luid = DesToInt(aLUID);
			
    if(aLUID.Length()==0 && index<0)
        {
        	luid = ConstructTreeL(aURI);
        	if(luid == -1)        	
        	{
        		Callback().SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
        		_DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(): end");
        		return;
        	}
        }

    if( OpenSessionL() != KErrNone )
        {
        Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
        _DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(): end");
        return;
        }

    if(!firstAdd)
        {

        if(LastURISeg(aURI).Compare(KNSmlDMEmailMpro)==0)
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(): end");
            return;
            }
        index = -1;
        for(TInt i=0;i<iBuffer->Count();i++)
            {
            if(iBuffer->At(i).iLuid == luid)
                {
                index = i;
                break;
                }
            }

        if(index<0)
            {
            TNSmlEmailSettingsElement newNode;
            newNode.iImap4Settings = NULL;
            newNode.iPop3Settings = NULL;
            newNode.iSmtpSettings = NULL;
            newNode.iIapPref = NULL;
            newNode.iSmtpIapPref = NULL;
            newNode.iLuid = luid;
            newNode.iMappingName = aURI.AllocLC();
            newNode.iName = HBufC::NewLC(0);
            newNode.iNodeBuf = new (ELeave) CArrayFixFlat 
                <TNSmlEmailCommandElement> (KNSmlDmEmailGranularity);
            newNode.iExecuted = ETrue;
            GetIdByLuid(newNode.iAccType, newNode.iAccId, luid); 
            iBuffer->AppendL(newNode);
            index = iBuffer->Count() - 1;
            TInt err = DoRestoreSettingsL(index);
            if(err!=KErrNone)
                {
                CleanupStack::PopAndDestroy(2); //newNode.iMappingName,newNode.iName
                newNode.iMappingName = NULL;
                newNode.iName = NULL;
                iBuffer->At(index).iNodeBuf->Reset();
                delete iBuffer->At(index).iNodeBuf;
                iBuffer->Delete(index);
                }
            if(err==KErrNotFound)
                {
                Callback().SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                _DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(): end");
                return;
                }
            else if(err!=KErrNone)
                {
                Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                _DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(): end");
                return;
                }
            CleanupStack::Pop(2); //newNode.iMappingName,newNode.iName
            }
        TNSmlEmailCommandElement newCommand;
        newCommand.iLastUriSeg = LastURISeg(aURI).AllocLC();
        newCommand.iData = NULL;
        newCommand.iStatusRef = aStatusRef;
        newCommand.iLeaf = ETrue;
        newCommand.iStatus = CSmlDmAdapter::EOk;
        iBuffer->At(index).iNodeBuf->AppendL(newCommand);
        CleanupStack::Pop(); //newCommand.iLastUriSeg
        iBuffer->At(index).iJustFetch=EFalse;
        DoAddLeafObjectL(newCommand.iLastUriSeg->Des(),aObject,index,
                         iBuffer->At(index).iNodeBuf->Count()-1);
        }
    else
        {
        FirstAddLeafL(aURI,aObject,aStatusRef,index);
        }

    _DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::DeleteObjectL( const TDesC8& aURI,
//                                         const TDesC8& aLUID,
//                                         const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::DeleteObjectL( const TDesC8& aURI,
                                         const TDesC8& aLUID,
                                         const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::DeleteLeafObjectL( ): begin");
    if(NumOfURISegs(aURI)==2)
        {
        if(aLUID.Length()==0)
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlDmEmailAdapter::DeleteLeafObjectL( ): ENotFound end");
            return;
            }

        TInt luid = DesToInt(aLUID);
        TInt err = OpenSessionL();
        if(err!=KErrNone)
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlDmEmailAdapter::DeleteLeafObjectL( ): EError end");
            return;
            }
        CEmailAccounts* emailAccs = CEmailAccounts::NewLC();
        TNSmlEmailAccountType accType;
        TInt id;
        GetIdByLuid(accType, id, luid );
        if( AccountExistsL( emailAccs, accType, id, -1 ) )
            {
            switch ( accType )
                {
            case EImap:
                {
                RArray<TImapAccount> imap4Accs;
                CleanupClosePushL( imap4Accs );
                emailAccs->GetImapAccountsL( imap4Accs );
                
                for (TInt i(0); i < imap4Accs.Count(); i++)
                    {
                    if ( imap4Accs[i].iImapAccountId == id  ) 
                        {
                        // Delete also related Smtp account
                        TSmtpAccount relatedSmtp;
                        emailAccs->GetSmtpAccountL( imap4Accs[i].iSmtpService, relatedSmtp );
                        emailAccs->DeleteImapAccountL( imap4Accs[i] );
                        emailAccs->DeleteSmtpAccountL( relatedSmtp );                       
                        //The only email a/c in phone gettiong deleted , so delete the default sntp a/c                     
                        if( imap4Accs.Count() == 1 )
	                        {
	                        TInt deferr = KErrNone;
                        	TRAP(deferr, emailAccs->DeleteDefaultSmtpAccountL());
                        	DBG_ARGS(_S16("CNSmlDmEmailAdapter::DeleteLeafObjectL( ): Delete default smtp account Error code %d"),deferr);
    	                    }
                        break;   
                        }
                    }
                
                CleanupStack::PopAndDestroy(&imap4Accs); //imap4Accs
                
                // If the default account was just deleted,
                // new one needs to be set.
                // 
	            TSmtpAccount defaultSmtp;
	            TInt retVal = emailAccs->DefaultSmtpAccountL( defaultSmtp );
	            if (  retVal != KErrNone )
	                {
                    RArray<TImapAccount> imapAccs;
                    CleanupClosePushL( imapAccs );
                    emailAccs->GetImapAccountsL( imapAccs );
                    if ( imapAccs.Count() > 0 )
                        {
                        TSmtpAccount relatedSmtp;
                        emailAccs->GetSmtpAccountL( imapAccs[0].iSmtpService, relatedSmtp );
	                    emailAccs->SetDefaultSmtpAccountL( relatedSmtp );
                        }
                    CleanupStack::PopAndDestroy( &imapAccs );
                    RArray<TPopAccount> popAccounts;
                    CleanupClosePushL( popAccounts );
                    emailAccs->GetPopAccountsL( popAccounts );
                    if ( popAccounts.Count() > 0 )
                        {
                        TSmtpAccount relatedSmtp;
                        emailAccs->GetSmtpAccountL( popAccounts[0].iSmtpService, relatedSmtp );
	                    emailAccs->SetDefaultSmtpAccountL( relatedSmtp );
                        }
                    CleanupStack::PopAndDestroy( &popAccounts );
	                }	                
                break;    
                }
                
            case EPop:
                {
                RArray<TPopAccount> popAccs;
                CleanupClosePushL( popAccs );
                emailAccs->GetPopAccountsL( popAccs );
                
                for (TInt i(0); i < popAccs.Count(); i++)
                    {
                    if ( popAccs[i].iPopAccountId == id  ) 
                        {
                        // Delete also related Smtp account
                        TSmtpAccount relatedSmtp;
                        emailAccs->GetSmtpAccountL( popAccs[i].iSmtpService, relatedSmtp );
                        emailAccs->DeletePopAccountL( popAccs[i] );    
                        emailAccs->DeleteSmtpAccountL( relatedSmtp );
                        if( popAccs.Count() == 1 )
                        	{
                        	TInt deferr = KErrNone;
                        	TRAP(deferr, emailAccs->DeleteDefaultSmtpAccountL());
                        	DBG_ARGS(_S16("CNSmlDmEmailAdapter::DeleteLeafObjectL( ): Delete default smtp account Error code %d "),deferr);
                        	}
                        break;                                             
                        }
                    }
                
                CleanupStack::PopAndDestroy(); //popAccs

                // If the default account was just deleted,
                // new one needs to be set.
                // 
	            TSmtpAccount defSmtp;
	            TInt retVal = emailAccs->DefaultSmtpAccountL( defSmtp );
	            if (  retVal != KErrNone )
	                {
                    RArray<TPopAccount> popAccounts;
                    CleanupClosePushL( popAccounts );
                    emailAccs->GetPopAccountsL( popAccounts );
                    if ( popAccounts.Count() > 0 )
                        {
                        TSmtpAccount relatedSmtp;
                        emailAccs->GetSmtpAccountL( popAccounts[0].iSmtpService, relatedSmtp );
	                    emailAccs->SetDefaultSmtpAccountL( relatedSmtp );
                        }
                    CleanupStack::PopAndDestroy( &popAccounts );
                    RArray<TImapAccount> imapAccs;
                    CleanupClosePushL( imapAccs );
                    emailAccs->GetImapAccountsL( imapAccs );
                    if ( imapAccs.Count() > 0 )
                        {
                        TSmtpAccount relatedSmtp;
                        emailAccs->GetSmtpAccountL( imapAccs[0].iSmtpService, relatedSmtp );
	                    emailAccs->SetDefaultSmtpAccountL( relatedSmtp );
                        }
                    CleanupStack::PopAndDestroy( &imapAccs );                    
	                }	                
                break;
                }                
                
            default:
                Callback().SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                break;
                }

            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            }
        else
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            }
        CleanupStack::PopAndDestroy(); //emailAccs
        }
    else
        {
        Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
        }

    _DBG_FILE("CNSmlDmEmailAdapter::DeleteLeafObjectL( ): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmEmailAdapter::FetchLeafObjectL( const TDesC8& aURI,
//                                            const TDesC8& aLUID,
//                                            const TDesC8& aType,
//                                            const TInt aResultsRef,
//                                            const TInt aStatusRef )
//  
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::FetchLeafObjectL( const TDesC8& aURI,
                                            const TDesC8& aLUID,
                                            const TDesC8& aType,
                                            const TInt aResultsRef,
                                            const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::FetchLeafObjectL(): begin");
		
	TInt luid = DesToInt(aLUID);	
			
    if(aLUID.Length()==0)
        {
        	luid = ConstructTreeL(aURI);
        	if(luid == -1)        	
        	{
        		Callback().SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
        		_DBG_FILE("CNSmlDmEmailAdapter::FetchLeafObjectL(): ENotFound end");
        		return;
        	}
        }
    if( OpenSessionL() != KErrNone )
        {
        Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
        _DBG_FILE("CNSmlDmEmailAdapter::FetchLeafObjectL(): EError end");
        return;
        }

    TInt index  = -1;
    for(TInt i=0;i<iBuffer->Count();i++)
        {
        if(iBuffer->At(i).iLuid == luid)
            {
            index = i;
            break;
            }
        }

    if(index<0)
        {
        TNSmlEmailSettingsElement newNode;
        newNode.iImap4Settings = NULL;
        newNode.iPop3Settings = NULL;
        newNode.iSmtpSettings = NULL;
        newNode.iIapPref = NULL;
        newNode.iSmtpIapPref = NULL;
        newNode.iLuid = luid;
        newNode.iMappingName = aURI.AllocLC();
        newNode.iName = HBufC::NewLC(0);
        newNode.iNodeBuf = new (ELeave)
            CArrayFixFlat <TNSmlEmailCommandElement> (KNSmlDmEmailGranularity);
        newNode.iExecuted = ETrue;
        newNode.iJustFetch = ETrue;
        GetIdByLuid(newNode.iAccType, newNode.iAccId, luid);
        iBuffer->AppendL(newNode);
        index = iBuffer->Count() - 1;
        if( DoRestoreSettingsL(index) != KErrNone )
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            CleanupStack::Pop(2); //newNode.iMappingName,newNode.iName
            _DBG_FILE("CNSmlDmEmailAdapter::FetchLeafObjectL(): EError end");
            return;
            }
        CleanupStack::Pop(2); //newNode.iMappingName,newNode.iName
        }

    TPtrC8 lastUriSeg = LastURISeg(aURI);
    CBufBase* object = CBufFlat::NewL(32);
    CleanupStack::PushL(object);
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    if(lastUriSeg.Compare(KNSmlDMEmailName)==0)
        {
        HBufC8 *data = HBufC8::NewLC(iBuffer->At(index).iName->Length());
        TPtr8 dataPtr = data->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( 
            dataPtr,iBuffer->At(index).iName->Des() );
        object->InsertL(0,dataPtr);
        CleanupStack::PopAndDestroy(); //data
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailUID)==0)
        {
        HBufC *data = NULL;
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            data = HBufC::NewLC(iBuffer->At(index).iPop3Settings->LoginName().Length());
            data->Des().Copy(iBuffer->At(index).iPop3Settings->LoginName());
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            data = HBufC::NewLC(iBuffer->At(index).iImap4Settings->LoginName().Length());
            data->Des().Copy(iBuffer->At(index).iImap4Settings->LoginName());
            }
        if(data)
            {
            HBufC8 *data8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*data);
            CleanupStack::PushL(data8);
            object->InsertL(0,*data8);
            CleanupStack::PopAndDestroy(2);//data, data8
            }
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailPW)==0)
        {
        HBufC *data = NULL;
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            data = HBufC::NewLC(iBuffer->At(index).iPop3Settings->Password().Length());
            data->Des().Copy(iBuffer->At(index).iPop3Settings->Password());
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            data = HBufC::NewLC(iBuffer->At(index).iImap4Settings->Password().Length());
            data->Des().Copy(iBuffer->At(index).iImap4Settings->Password());
            }
        if(data)
            {
            HBufC8 *data8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*data);
            CleanupStack::PushL(data8);
            object->InsertL(0,*data8);
            CleanupStack::PopAndDestroy(2);//data, data8
            }
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailUAddr)==0)
        {
        HBufC *data = NULL;
        data = iBuffer->At(index).iSmtpSettings->EmailAddress().AllocLC();
        HBufC8 *data8 = HBufC8::NewLC(data->Size());
        TPtr8 dataPtr8 = data8->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr8, data->Des() );

        object->InsertL(0,dataPtr8);
        CleanupStack::PopAndDestroy(2); //data, data8
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailUName)==0)
        {

        HBufC *data = NULL;
        data = iBuffer->At(index).iSmtpSettings->EmailAlias().AllocLC();
        HBufC8 *data8 = HBufC8::NewLC(data->Size());
        TPtr8 dataPtr8 = data8->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr8, data->Des() );

        object->InsertL(0,dataPtr8);
        CleanupStack::PopAndDestroy(2); //data, data8
        }

    else if(lastUriSeg.Compare( KNSmlDMEmailMrcv)==0)
        {
        HBufC *data = NULL;
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            data = iBuffer->At(index).iPop3Settings->ServerAddress().AllocLC();
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            data = iBuffer->At(index).iImap4Settings->ServerAddress().AllocLC();
            }
        else
        	{
          	status = CSmlDmAdapter::ENotFound;
        	}
        if(data)
        	{	    
        		HBufC8 *data8 = HBufC8::NewLC(data->Size());
        		TPtr8 dataPtr8 = data8->Des();
        		CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr8, data->Des() );

        		object->InsertL(0,dataPtr8);
        		CleanupStack::PopAndDestroy(2); //data, data8
        	}
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailMsnd)==0)
        {
        HBufC *data = NULL;
        data = iBuffer->At(index).iSmtpSettings->ServerAddress().AllocLC();
        HBufC8 *data8 = HBufC8::NewLC(data->Size());
        TPtr8 dataPtr8 = data8->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr8, data->Des() );

        object->InsertL(0,dataPtr8);
        CleanupStack::PopAndDestroy(2); //data, data8
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailMpro)==0)
        {
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            object->InsertL(0,KNSmlDMEmailMproPOP);
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            object->InsertL(0,KNSmlDMEmailMproIMAP);
            }
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailUseSecCon)==0)
        {
        FetchSecurityValueL(*object, &iBuffer->At(index), EUseSecCon );
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailUseSauth)==0)
        {
        if(iBuffer->At(index).iSmtpSettings->SMTPAuth())
            {
            object->InsertL(0,KNSmlDMEmailTrue);
            }
        else
            {
            object->InsertL(0,KNSmlDMEmailFalse);
            }
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailSauthUID)==0)
        {
        HBufC *data = HBufC::NewLC(iBuffer->At(index).iSmtpSettings->LoginName().Length());
        data->Des().Copy(iBuffer->At(index).iSmtpSettings->LoginName());
        HBufC8 *data8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*data);
        CleanupStack::PushL(data8);
        object->InsertL(0,*data8);
        CleanupStack::PopAndDestroy(2);//data, data8
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailSauthPW)==0)
        {
        HBufC *data = HBufC::NewLC(iBuffer->At(index).iSmtpSettings->Password().Length());
        data->Des().Copy(iBuffer->At(index).iSmtpSettings->Password());
        HBufC8 *data8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*data);
        CleanupStack::PushL(data8);
        object->InsertL(0,*data8);
        CleanupStack::PopAndDestroy(2);//data, data8
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailPtxtSAuth)==0)
        {
        if(iBuffer->At(index).iSmtpSettings->InboxLoginDetails())
            {
            object->InsertL(0,KNSmlDMEmailTrue);
            }
        else
            {
            object->InsertL(0,KNSmlDMEmailFalse);
            }
        }

    else if (lastUriSeg.Compare(KNSmlDMEmailSUseSecCon)==0)
        {
        FetchSecurityValueL(*object, &iBuffer->At(index), ESuseSecCon );
        }

    else if (lastUriSeg.Compare(KNSmlDMEmailUseStartTLS)==0)
        {
        FetchSecurityValueL(*object, &iBuffer->At(index), EUseStartTls );
        }

    else if (lastUriSeg.Compare(KNSmlDMEmailSUseStartTLS)==0)
        {
        FetchSecurityValueL(*object, &iBuffer->At(index), ESuseStartTls );
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailMrcvPort)==0)
        {
        TUint port;
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            port = iBuffer->At(index).iPop3Settings->Port();
            }
        else 
            {
            port = iBuffer->At(index).iImap4Settings->Port();
            }
        HBufC8 *data = IntToDes8LC(port);
        object->InsertL(0,data->Des());
        CleanupStack::PopAndDestroy(); //data
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailMsndPort)==0)
        {
        TUint port = iBuffer->At(index).iSmtpSettings->Port();
        HBufC8 *data = IntToDes8LC(port);
        object->InsertL(0,data->Des());
        CleanupStack::PopAndDestroy(); //data
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailUseAPOP)==0)
        {
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            if(iBuffer->At(index).iPop3Settings->Apop())
                {
                object->InsertL(0,KNSmlDMEmailTrue);
                }
            else
                {
                object->InsertL(0,KNSmlDMEmailFalse);
                }
            }
        else
            {
            status = CSmlDmAdapter::ENotFound;
            }
        }

    else if (lastUriSeg.Compare(KNSmlDMEmailFolderPath)==0)
        {
        if(iBuffer->At(index).iImap4Settings)
            {
            object->InsertL(0, iBuffer->At(index).iImap4Settings->FolderPath() );
            }
        else
            {
            status = CSmlDmAdapter::ENotFound;
            }
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailToNapID)==0)
        {

        CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
        CleanupStack::PushL(iapmatch);
        HBufC8* uri8 = NULL;
        TInt iapCount = iBuffer->At(index).iIapPref->NumberOfIAPs();
        if ( iapCount )
            {
            TImIAPChoice localIAP = iBuffer->At(index).iIapPref->IAPPreference(0);
            uri8 = iapmatch->URIFromIAPIdL( localIAP.iIAP );
            }

        if( uri8 )
            {
            CleanupStack::PushL(uri8);
            object->InsertL(0,uri8->Des());
            status = CSmlDmAdapter::EOk;
            CleanupStack::PopAndDestroy(); // uri8
            }        

        CleanupStack::PopAndDestroy(); // iapMatch
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailSToNapID)==0)
        {

        // SToNapID exists only for SMTP
        if (iBuffer->At(index).iSmtpSettings!= NULL)
            {
            CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
            CleanupStack::PushL(iapmatch);
            HBufC8* uri8 = NULL;
            TInt iapCount = iBuffer->At(index).iIapPref->NumberOfIAPs();
            if ( iapCount )
                {
                TImIAPChoice localIAP = iBuffer->At(index).iSmtpIapPref->IAPPreference(0);
                uri8 = iapmatch->URIFromIAPIdL( localIAP.iIAP );
                }

            if( uri8 )
                {
                CleanupStack::PushL(uri8);
                object->InsertL(0,uri8->Des());
                status = CSmlDmAdapter::EOk;
                CleanupStack::PopAndDestroy(); // uri8
                }           

            CleanupStack::PopAndDestroy(); // iapMatch
            }
        else
            {
            status = CSmlDmAdapter::ENotFound;
            }
        }

    if(status == CSmlDmAdapter::EOk)
        {
        Callback().SetResultsL(aResultsRef,*object,aType);
        }
    Callback().SetStatusL(aStatusRef, status);
    CleanupStack::PopAndDestroy(object); //object

    _DBG_FILE("CNSmlDmEmailAdapter::FetchLeafObjectL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmEmailAdapter::ChildURIListL( const TDesC8& aURI,
//                                          const TDesC8& aParentLUID,
//                                          const CArrayFix<TSmlDmMappingInfo>& 
//                                          aPreviousURISegmentList,
//                                          const TInt aResultsRef,
//                                          const TInt aStatusRef  )
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::ChildURIListL( const TDesC8& aURI,
                                         const TDesC8& aParentLUID,
                                         const CArrayFix<TSmlDmMappingInfo>& 
                                         aPreviousURISegmentList,
                                         const TInt aResultsRef,
                                         const TInt aStatusRef  )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::ChildURIListL(): begin");

    CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;

    if( OpenSessionL() != KErrNone )
        {
        Callback().SetStatusL(aStatusRef, ret);
        _DBG_FILE("CNSmlDmEmailAdapter::ChildURIListL(): end");
        return;
        }
    CBufBase *currentList = CBufFlat::NewL(128);
    CleanupStack::PushL(currentList);

    CEmailAccounts* emailAccs = CEmailAccounts::NewLC();

    if(aURI.Compare(KNSmlDMEmailNodeName)==0)
        {
        RArray<TImapAccount> imap4Accounts;
        CleanupClosePushL( imap4Accounts );
        emailAccs->GetImapAccountsL(imap4Accounts);

        // First, IMAP accounts
        //
        TInt luid;
        TPtrC8 name;
        // cycle through every found account
        for(TInt count = 0; count < imap4Accounts.Count(); count++)
            {
            SetLuidById( EImap, imap4Accounts[count].iImapAccountId, luid );

            TBool found = EFalse;
            // cycle through every known account
            for(TInt i = 0; i<aPreviousURISegmentList.Count();i++)
                {
                // compare if found is known
                if( luid == DesToInt(aPreviousURISegmentList.At(i).iURISegLUID))
                    {
                    found = ETrue;
                    name.Set(aPreviousURISegmentList.At(i).iURISeg);
                    break;
                    }
                }
            // if found != known add mapping to it
            if(!found)
                {
                HBufC8 *uri = HBufC8::NewLC(aURI.Length() + 
                                            KNSmlDMEmailMproIMAP().Length() + 6);
                                            
                TPtr8 uriPtr = uri->Des();
                uriPtr.Format(aURI);
                uriPtr.Append(KNSmlDMEmailSeparator8);
                uriPtr.Append(KNSmlDMEmailMproIMAP);
                TUint16 crc;
               	Mem::Crc ( crc, &imap4Accounts[count],sizeof(TImapAccount) );
                HBufC8 *crcBuf = IntToDes8LC(crc);
                uriPtr.Append(*crcBuf);
                HBufC8 *luidBuf = IntToDes8LC(luid);
                Callback().SetMappingL(uriPtr,luidBuf->Des());
                currentList->InsertL( currentList->Size(),LastURISeg(uriPtr));
                CleanupStack::PopAndDestroy(3); //uri,luidBuf, crcBuf
                }
            else
                {
                currentList->InsertL( currentList->Size(),name );
                }
            // in all cases: add it to results.
            if(count < imap4Accounts.Count()-1 )
                {
                currentList->InsertL(currentList->Size(),KNSmlDMEmailSeparator8);
                }
            }
        CleanupStack::PopAndDestroy(); //imap4Accounts

        // Then, POP accounts
        //
        RArray<TPopAccount> pop3Accounts;
        CleanupClosePushL( pop3Accounts );
        emailAccs->GetPopAccountsL( pop3Accounts );
        if (pop3Accounts.Count()&&currentList->Size()>0)
            {
            // there is at least one account; insert separator
            currentList->InsertL(currentList->Size(),KNSmlDMEmailSeparator8);
            }
        // cycle through every found account
        for(TInt count = 0; count < pop3Accounts.Count(); count++)
            {
            SetLuidById( EPop, pop3Accounts[count].iPopAccountId, luid );

            TBool found = EFalse;
            // cycle through every known account
            for(TInt i = 0; i<aPreviousURISegmentList.Count();i++)
                {
                // compare if found is known
                if( luid == DesToInt(aPreviousURISegmentList.At(i).iURISegLUID))

                    {
                    found = ETrue;
                    name.Set(aPreviousURISegmentList.At(i).iURISeg);
                    break;
                    }
                }
            // if found != known add mapping to it
            if(!found)
                {
                                         
                HBufC8 *uri = HBufC8::NewLC(aURI.Length() + 
                                            KNSmlDMEmailMproPOP().Length() + 5);
                TPtr8 uriPtr = uri->Des();
                uriPtr.Format(aURI);
                uriPtr.Append(KNSmlDMEmailSeparator8);
                uriPtr.Append(KNSmlDMEmailMproPOP);
                TUint16 crc;
               	Mem::Crc ( crc, &pop3Accounts[count],sizeof(TPopAccount) );
                HBufC8 *crcBuf = IntToDes8LC(crc);
                uriPtr.Append(*crcBuf);
                HBufC8 *luidBuf = IntToDes8LC(luid);
                Callback().SetMappingL(uriPtr,luidBuf->Des());
                currentList->InsertL( currentList->Size(),LastURISeg(uriPtr));
                CleanupStack::PopAndDestroy(3); //uri,luidBuf,crcBuf
                }
            else
                {
                currentList->InsertL( currentList->Size(),name );
                }
            // in all cases: add it to results.
            if(count < pop3Accounts.Count()-1)
                {
                currentList->InsertL(currentList->Size(),KNSmlDMEmailSeparator8);
                }
            }
        CleanupStack::PopAndDestroy(); //pop3Accounts
        ret = CSmlDmAdapter::EOk;
        }
    else
        {
        if(aParentLUID.Length() > 0)
            {
            TInt err = KErrNotFound;
            TInt luid = DesToInt(aParentLUID);
            TNSmlEmailAccountType accType;
            TInt id;
            GetIdByLuid( accType, id, luid );
            switch ( accType )
                {
            case EImap:
                {
                RArray<TImapAccount> imap4Accounts;
                CleanupClosePushL( imap4Accounts );
                emailAccs->GetImapAccountsL( imap4Accounts ); 
                for ( TInt i=0; i<imap4Accounts.Count(); i++ )
                    {
                    if ( imap4Accounts[i].iImapAccountId == id )
                        {
                        err = KErrNone;
                        break;
                        }
                    }
                CleanupStack::PopAndDestroy(); //imap4Accounts    
                break;                
                }
            case EPop:
                {
                RArray<TPopAccount> pop3Accounts;
                CleanupClosePushL( pop3Accounts );
                emailAccs->GetPopAccountsL(pop3Accounts); 
                for ( TInt i=0; i<pop3Accounts.Count(); i++ )
                    {
                    if ( pop3Accounts[i].iPopAccountId == id )
                        {
                        err = KErrNone;
                        break;
                        }
                    }
                CleanupStack::PopAndDestroy(); //pop3Accounts       
                break;                
                }
            default:
                ret = CSmlDmAdapter::EError;
                break;
                }
            if( err == KErrNotFound )
                {
                ret = CSmlDmAdapter::ENotFound;
                }
            
            if ( err == KErrNone )
                {
                ret = CSmlDmAdapter::EOk;
                currentList->InsertL(0,KNSmlDMEmailListOfLeafs);
                }
            }
        else
            {
            ret = CSmlDmAdapter::ENotFound;
            }
        }
    // Common part
    Callback().SetStatusL(aStatusRef, ret);
    if( ret == CSmlDmAdapter::EOk )
        {
        Callback().SetResultsL(aResultsRef,*currentList,KNullDesC8);
        }

    CleanupStack::PopAndDestroy(emailAccs); //emailAccs
    CleanupStack::PopAndDestroy(currentList); //currentList
    _DBG_FILE("CNSmlDmEmailAdapter::ChildURIListL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::AddNodeObjectL( const TDesC8& aURI, 
// const TDesC8& aParentLUID, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::AddNodeObjectL( const TDesC8& aURI,
                                          const TDesC8& aParentLUID,
                                          const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::AddNodeObjectL(): begin");

    if(aParentLUID.Length()>0)
        {
        TInt luid = DesToInt(aParentLUID);
        TInt accId;
        TNSmlEmailAccountType accType;
        GetIdByLuid( accType, accId, luid );
        TInt err = OpenSessionL();
        if(err!=KErrNone)
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlDmEmailAdapter::AddNodeObjectL(): EError end");
            return;
            }
        CEmailAccounts* emailAccs = CEmailAccounts::NewLC();
        TBool accEx = AccountExistsL( emailAccs, accType, accId, -1 );
        CleanupStack::PopAndDestroy( emailAccs );
        if( accEx )
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EAlreadyExists);
            _DBG_FILE("CNSmlDmEmailAdapter::AddNodeObjectL(): EAlreadyExists end");
            return;
            }
        else
            {
            TInt ret = Callback().RemoveMappingL( KNSmlDMEmailAdapterImplUid, 
						GetDynamicEmailNodeUri(aURI), ETrue );
			if(ret != KErrNone)
				{
            	Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
	            _DBG_FILE("CNSmlDmEmailAdapter::AddNodeObjectL(): EError end");
            	return;
				}
            }
        Callback().SetMappingL(aURI,KNullDesC8);
        }

    if( aURI.Find(KNSmlDMEmailNodeName)>=0 && NumOfURISegs(aURI)==2 )
        {
        if( OpenSessionL() != KErrNone )
            {
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlDmEmailAdapter::AddNodeObjectL(): EError end");
            return;
            }

        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL(fs);
        //2files and 2 folders are needed for settings
        if (SysUtil::FFSSpaceBelowCriticalLevelL(&fs,
                                                 KNSmlDmFatMinimumFileSize*4))
            {
            CleanupStack::PopAndDestroy(); //fs
            Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EDiskFull);
            _DBG_FILE("CNSmlDmEmailAdapter::AddNodeObjectL(): EDiskFull end");
            return;
            // do not write
            }
        CleanupStack::PopAndDestroy(); //fs


        TNSmlEmailSettingsElement newNode;
        newNode.iMappingName = aURI.AllocLC();
        newNode.iName = HBufC::NewLC(0);
        newNode.iNodeBuf = new (ELeave)
            CArrayFixFlat <TNSmlEmailCommandElement> (KNSmlDmEmailGranularity);
        newNode.iExecuted = EFalse;
        newNode.iImap4Settings = NULL;
        newNode.iPop3Settings = NULL;
        newNode.iSmtpSettings = NULL;
        newNode.iIapPref = NULL;
        newNode.iSmtpIapPref = NULL;
        newNode.iLuid = 0;
        newNode.iJustFetch=EFalse;
        iBuffer->AppendL(newNode);
        CleanupStack::Pop(2); //newNode.iMappingName,newNode.iName
        TNSmlEmailCommandElement newCommand;
        newCommand.iLastUriSeg = LastURISeg(aURI).AllocLC(); //aURI.AllocLC();
        newCommand.iData = NULL;
        newCommand.iStatusRef = aStatusRef;
        newCommand.iLeaf = EFalse;
        newCommand.iStatus = CSmlDmAdapter::EOk;
        TInt index = iBuffer->Count() - 1;
        iBuffer->At(index).iNodeBuf->AppendL(newCommand);
        CleanupStack::Pop(); //newCommand.iLastUriSeg


        }
    _DBG_FILE("CNSmlDmEmailAdapter::AddNodeObjectL(): end");
    }
// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::UpdateLeafObjectL( const TDesC8& aURI,
// const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType,
// const TInt aStatusRef )
// This should update a stream object, but in case of Email settings streaming 
// is not supported.
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/,
                                             const TDesC8& /*aLUID*/,
                                             RWriteStream*& /*aStream*/,
                                             const TDesC8& /*aType*/,
                                             const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(stream): begin");
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlDmEmailAdapter::UpdateLeafObjectL(stream): end");
    }
// -----------------------------------------------------------------------------
// void CNSmlDmEmailAdapter::FetchLeafObjectSizeL( const TDesC8& aURI,
// const TDesC8& aLUID, const TDesC8& aType, const TInt aResultsRef,
// const TInt aStatusRef )
// Fetches data object and returns its size.
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::FetchLeafObjectSizeL( const TDesC8& aURI,
                                                const TDesC8& aLUID,
                                                const TDesC8& aType,
                                                const TInt aResultsRef,
                                                const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::FetchLeafObjectSizeL(): begin");

    CBufBase *result = CBufFlat::NewL(32);
    CleanupStack::PushL(result);
    CSmlDmAdapter::TError status = FetchObjectL(aURI, aLUID, *result);
    if( status == CSmlDmAdapter::EOk )
        {
        TInt objSizeInBytes = result->Size();
        TBuf8<4> stringObjSizeInBytes;
        stringObjSizeInBytes.Num( objSizeInBytes );
        result->Reset();
        result->InsertL( 0, stringObjSizeInBytes );
        Callback().SetResultsL(aResultsRef, *result, aType);
        }
    Callback().SetStatusL(aStatusRef, status);
    CleanupStack::PopAndDestroy(); //result
    _DBG_FILE("CNSmlDmEmailAdapter::FetchLeafObjectSizeL(): end");
    }
// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::ExecuteCommandL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::ExecuteCommandL( const TDesC8& /*aURI*/,
                                           const TDesC8& /*aLUID*/,
                                           const TDesC8& /*aArgument*/,
                                           const TDesC8& /*aType*/,
                                           const TInt aStatusRef )
    {
    //Not supported
    _DBG_FILE("CNSmlDmEmailAdapter::ExecuteCommandL(): begin");
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlDmEmailAdapter::ExecuteCommandL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::ExecuteCommandL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::ExecuteCommandL( const TDesC8& /*aURI*/,
                                           const TDesC8& /*aLUID*/,
                                           RWriteStream*& /*aStream*/,
                                           const TDesC8& /*aType*/,
                                           const TInt aStatusRef )
    {
    //Not supported
    _DBG_FILE("CNSmlDmEmailAdapter::ExecuteCommandL(): begin");
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlDmEmailAdapter::ExecuteCommandL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::CopyCommandL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/,
                                        const TDesC8& /*aTargetLUID*/,
                                        const TDesC8& /*aSourceURI*/,
                                        const TDesC8& /*aSourceLUID*/,
                                        const TDesC8& /*aType*/,
                                        TInt aStatusRef )
    {
    //Not supported
    _DBG_FILE("CNSmlDmEmailAdapter::CopyCommandL(): begin");
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlDmEmailAdapter::CopyCommandL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::StartAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::StartAtomicL()
    {
    //Not supported
    _DBG_FILE("CNSmlDmEmailAdapter::StartAtomicL(): begin");
    _DBG_FILE("CNSmlDmEmailAdapter::StartAtomicL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::CommitAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::CommitAtomicL()
    {
    //Not supported
    _DBG_FILE("CNSmlDmEmailAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CNSmlDmEmailAdapter::CommitAtomicL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::RollbackAtomicL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::RollbackAtomicL()
    {
    //Not supported
    _DBG_FILE("CNSmlDmEmailAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CNSmlDmEmailAdapter::RollbackAtomicL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::StreamingSupport()
// -----------------------------------------------------------------------------
TBool CNSmlDmEmailAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::StreamingSupport(): begin");
    _DBG_FILE("CNSmlDmEmailAdapter::StreamingSupport(): end");
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::StreamCommittedL()
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::StreamCommittedL()
    {
    //Not supported
    _DBG_FILE("CNSmlDmEmailAdapter::StreamCommittedL(): begin");
    _DBG_FILE("CNSmlDmEmailAdapter::StreamCommittedL(): end");
    }
// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::CompleteOutstandingCmdsL()
// Indication of message ending. The buffered commands must be executed
// before return of this function.
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE("CNSmlDmEmailAdapter::CompleteOutstandingCmdsL(): begin");

    for(TInt i=0;i<iBuffer->Count();i++)
        {

        //Set buffered port values
        for(TInt cmd=0;cmd<iBuffer->At(i).iNodeBuf->Count();cmd++)
            {
            
            TNSmlEmailSettingsElement& bufElem = iBuffer->At(i);
            TNSmlEmailCommandElement& elem = bufElem.iNodeBuf->At(cmd);

            if(elem.iLastUriSeg->Compare(KNSmlDMEmailMrcvPort)==0)
                {
                TUint port = DesToInt(*elem.iData);
                
                if(port>0 && port <= KNSmlDMEmailMaxPortNbr)
                    {
                    if(bufElem.iPop3Settings!= NULL)
                        {
                        bufElem.iPop3Settings->SetPort(port);
                        }
                    else if(bufElem.iImap4Settings!= NULL)
                        {
                        bufElem.iImap4Settings->SetPort(port);
                        }
                    }
                else
                    {
                    elem.iStatus = CSmlDmAdapter::EInvalidObject;
                    }
                }
            else if(elem.iLastUriSeg->Compare(KNSmlDMEmailMsndPort)==0)
                {
                TUint port = DesToInt(*elem.iData);
                
                if(port>0 && port <= KNSmlDMEmailMaxPortNbr)
                    {
                    bufElem.iSmtpSettings->SetPort(port);
                    }
                else
                    {
                    elem.iStatus = CSmlDmAdapter::EInvalidObject;
                    }
                }
                if((iBuffer->At(i).iSmtpSettings->LoginName()==KNullDesC8) && elem.iLastUriSeg->Compare(KNSmlDMEmailUID)==0)
              	{
                	elem.iStatus = CSmlDmAdapter::EError;
        	      }
           }
        

        CSmlDmAdapter::TError stat = CSmlDmAdapter::EOk;
        TInt luid=-1;
        
        if(iBuffer->At(i).iExecuted)
            {
            TRAPD( leaveCode,luid = ValidateAndStoreSettingsL(i) );
            if(leaveCode!=KErrNone)
                {
                stat = CSmlDmAdapter::EError;
                }
            }
        else
            {
            stat = CSmlDmAdapter::EError;
            }

        for(TInt cmd=0;cmd<iBuffer->At(i).iNodeBuf->Count();cmd++)
            {

            if(iBuffer->At(i).iNodeBuf->At(cmd).iStatus==CSmlDmAdapter::EOk)
                {
                Callback().SetStatusL(iBuffer->At(i).iNodeBuf->At(cmd).iStatusRef,
                                      stat);
                if(!iBuffer->At(i).iNodeBuf->At(cmd).iLeaf)
                    {
                    if(stat==CSmlDmAdapter::EOk)
                        {
                        HBufC8* luidDes = IntToDes8LC(luid);
                        Callback().SetMappingL( iBuffer->At(i).iMappingName->Des(),
                                                luidDes->Des() );
                        CleanupStack::PopAndDestroy(); //luidDes
                        }
                    }
                }
            else
                {
                Callback().SetStatusL(iBuffer->At(i).iNodeBuf->At(cmd).iStatusRef,
                                      iBuffer->At(i).iNodeBuf->At(cmd).iStatus);
                }

            delete iBuffer->At(i).iNodeBuf->At(cmd).iLastUriSeg;
            iBuffer->At(i).iNodeBuf->At(cmd).iLastUriSeg= NULL;
            delete iBuffer->At(i).iNodeBuf->At(cmd).iData;
            iBuffer->At(i).iNodeBuf->At(cmd).iData= NULL;

            }
        iBuffer->At(i).iNodeBuf->Reset();
        delete iBuffer->At(i).iNodeBuf;
        iBuffer->At(i).iNodeBuf = NULL;
        delete iBuffer->At(i).iMappingName;
        iBuffer->At(i).iMappingName= NULL;
        delete iBuffer->At(i).iName;
        iBuffer->At(i).iName= NULL;
        delete iBuffer->At(i).iSmtpSettings;
        iBuffer->At(i).iSmtpSettings= NULL;
        delete iBuffer->At(i).iPop3Settings;
        iBuffer->At(i).iPop3Settings= NULL;
        delete iBuffer->At(i).iImap4Settings;
        iBuffer->At(i).iImap4Settings= NULL;
        delete iBuffer->At(i).iIapPref;
        iBuffer->At(i).iIapPref= NULL;
        delete iBuffer->At(i).iSmtpIapPref;
        iBuffer->At(i).iSmtpIapPref= NULL;

        } // end for
    iBuffer->Reset();

    _DBG_FILE("CNSmlDmEmailAdapter::CompleteOutstandingCmdsL(): end");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KNSmlDMEmailAdapterImplUid, 
                               CNSmlDmEmailAdapter::NewL)
    };

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmEmailAdapter: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmEmailAdapter: end");
    return ImplementationTable;
    }



// -----------------------------------------------------------------------------
// TPtrC8 CNSmlDmEmailAdapter::LastURISeg(const TDesC8& aURI)
// Returns only the last uri segemnt
// -----------------------------------------------------------------------------
TPtrC8 CNSmlDmEmailAdapter::LastURISeg(const TDesC8& aURI) const
    {
    _DBG_FILE("CNSmlDmEmailAdapter::LastURISeg() : begin");
    TInt i;
    for(i=aURI.Length()-1;i>=0;i--)
        {
        if(aURI[i]=='/')
            {
            break;
            }
        }
    _DBG_FILE("CNSmlDmEmailAdapter::LastURISeg() : end");
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
// TPtrC8 CNSmlDmEmailAdapter::RemoveLastURISeg(const TDesC8& aURI)
// returns parent uri, i.e. removes last uri segment
// -----------------------------------------------------------------------------
TPtrC8 CNSmlDmEmailAdapter::RemoveLastURISeg(const TDesC8& aURI) const
    {
    TInt i;
    for(i=aURI.Length()-1;i>=0;i--)
        {
        if(aURI[i]=='/')
            {
            break;
            }
        }
    return aURI.Left(i);
    }



// -----------------------------------------------------------------------------
// TInt CNSmlDmEmailAdapter::NumOfURISegs(const TDesC8& aURI)
// Returns the num of uri segs
// -----------------------------------------------------------------------------
TInt CNSmlDmEmailAdapter::NumOfURISegs(const TDesC8& aURI) const
    {
    TInt numOfURISegs = 1;
    for(TInt i=0;i<aURI.Length();i++)
        {
        if(aURI[i]=='/')
            {
            numOfURISegs++;
            }
        }
    return numOfURISegs;
    }



// -----------------------------------------------------------------------------
// void CNSmlDmEmailAdapter::HandleSessionEventL( TMsvSessionEvent /*aEvent*/,
//                                               TAny* /*aArg1*/,
//                                               TAny* /*aArg2*/,
//                                               TAny* /*aArg3*/ )
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::HandleSessionEventL( TMsvSessionEvent /*aEvent*/,
                                               TAny* /*aArg1*/,
                                               TAny* /*aArg2*/,
                                               TAny* /*aArg3*/ )
    {
    _DBG_FILE("CNSmlDmEmailAdapter::HandleSessionEventL(): begin");
    _DBG_FILE("CNSmlDmEmailAdapter::HandleSessionEventL(): end");
    }


// -----------------------------------------------------------------------------
// TInt CNSmlDmEmailAdapter::DesToInt(const TDesC& aLuid) const
// Converts 16bit descriptor to int
// -----------------------------------------------------------------------------
TInt CNSmlDmEmailAdapter::DesToInt(const TDesC& aLuid) const
    {
    TLex16 lex(aLuid);
    TInt value = 0;
    lex.Val(value);
    return value;
    }

// -----------------------------------------------------------------------------
// TInt CNSmlDmEmailAdapter::DesToInt(const TDesC8& aLuid) const
// Converts 8bit descriptor to int
// -----------------------------------------------------------------------------
TInt CNSmlDmEmailAdapter::DesToInt(const TDesC8& aLuid) const
    {
    TLex8 lex(aLuid);
    TInt value = 0;
    lex.Val(value);
    return value;
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::FirstAddLeafL()
// -----------------------------------------------------------------------------
TInt CNSmlDmEmailAdapter::FirstAddLeafL(const TDesC8& aURI, 
                                        const TDesC8& aObject,
                                        TInt aStatusRef,
                                        TInt aIndex)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::FirstAddLeafL() :: begin");

    TPtrC8 lastSeg = LastURISeg(aURI);

    TNSmlEmailCommandElement newCommand;
    newCommand.iLastUriSeg = lastSeg.AllocLC();
    newCommand.iData = aObject.AllocLC();
    newCommand.iStatusRef = aStatusRef;
    newCommand.iLeaf = ETrue;
    newCommand.iStatus = CSmlDmAdapter::EOk;
    iBuffer->At(aIndex).iNodeBuf->AppendL(newCommand);
    CleanupStack::Pop(2); //newCommand.iLastUriSeg, newCommand.iData
    if(!iBuffer->At(aIndex).iExecuted)
        {
        if(lastSeg.Compare(KNSmlDMEmailMpro)==0)
            {
            TBool ok = EFalse;
            HBufC8* object = aObject.AllocLC();
            TPtr8 objectPtr = object->Des();
            objectPtr.UpperCase();

            if(objectPtr.Find(KNSmlDMEmailMproPOP) >= 0)
                {
                iBuffer->At(aIndex).iPop3Settings = new(ELeave) CImPop3Settings;
                ok=ETrue;
                }

            else if(objectPtr.Find(KNSmlDMEmailMproIMAP) >= 0)
                {
                iBuffer->At(aIndex).iImap4Settings = new(ELeave) CImImap4Settings;
                ok=ETrue;
                }
            if(ok)
                {
                iBuffer->At(aIndex).iSmtpSettings = new(ELeave) CImSmtpSettings;
                iBuffer->At(aIndex).iIapPref = CImIAPPreferences::NewLC();
                iBuffer->At(aIndex).iSmtpIapPref = CImIAPPreferences::NewLC();
                CleanupStack::Pop(); //iBuffer->At(index).iIapPref
                CleanupStack::Pop(); //iBuffer->At(index).iSmtpIapPref
                SetDefaultSettingsL(aIndex);

                for(TInt i = 0; i<iBuffer->At(aIndex).iNodeBuf->Count();i++)
                    {
                    if(iBuffer->At(aIndex).iNodeBuf->At(i).iLeaf)
                        {
                        DoAddLeafObjectL(iBuffer->At(aIndex).iNodeBuf->At(i).iLastUriSeg->Des(),
                                         iBuffer->At(aIndex).iNodeBuf->At(i).iData->Des(),
                                         aIndex, i);
                        }
                    }

                iBuffer->At(aIndex).iExecuted = ETrue;
                }
            CleanupStack::PopAndDestroy(); //object
            }
        }
    else
        {
        DoAddLeafObjectL(lastSeg,aObject,aIndex,
                         iBuffer->At(aIndex).iNodeBuf->Count()-1);
        }

    _DBG_FILE("CNSmlDmEmailAdapter::FirstAddLeafL() :: end");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::DoAddLeafObjectL
// -----------------------------------------------------------------------------
TInt CNSmlDmEmailAdapter::DoAddLeafObjectL(const TDesC8& aLastUriSeg,
                                           const TDesC8& aObject,
                                           TInt aIndex,
                                           TInt aCommand)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::DoAddLeafObjectL() :: begin");
    if(aLastUriSeg.Compare(KNSmlDMEmailName)==0)
        {
        if ( aObject.Size() < KEmailAccountNameSize )
            {
            delete iBuffer->At(aIndex).iName;
            iBuffer->At(aIndex).iName = NULL;
            iBuffer->At(aIndex).iName = HBufC::NewL(aObject.Size());
            TPtr namePtr = iBuffer->At(aIndex).iName->Des();
            CnvUtfConverter::ConvertToUnicodeFromUtf8( namePtr, aObject );
            }
        else 
            {
            iBuffer->At(aIndex).iNodeBuf->At(aCommand).iStatus = 
                CSmlDmAdapter::ETooLargeObject;
            }
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailUID)==0)
        {
        if(iBuffer->At(aIndex).iPop3Settings!= NULL)
            {
            iBuffer->At(aIndex).iPop3Settings->SetLoginNameL(aObject);
            }
        else if(iBuffer->At(aIndex).iImap4Settings!= NULL)
            {
            iBuffer->At(aIndex).iImap4Settings->SetLoginNameL(aObject);
            }
        if(iBuffer->At(aIndex).iSmtpSettings->LoginName()==KNullDesC8)
            {
            iBuffer->At(aIndex).iSmtpSettings->SetLoginNameL(aObject);
            }
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailPW)==0)
        {
        if(iBuffer->At(aIndex).iPop3Settings!= NULL)
            {
            iBuffer->At(aIndex).iPop3Settings->SetPasswordL(aObject);
            }
        else if(iBuffer->At(aIndex).iImap4Settings!= NULL)
            {
            iBuffer->At(aIndex).iImap4Settings->SetPasswordL(aObject);
            }
        if(iBuffer->At(aIndex).iSmtpSettings->Password()==KNullDesC8)
            {
            iBuffer->At(aIndex).iSmtpSettings->SetPasswordL(aObject);
            }
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailUAddr)==0)
        {
        HBufC *object = HBufC::NewLC(aObject.Size());
        TPtr objectPtr = object->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( objectPtr, aObject );
        if(iBuffer->At(aIndex).iSmtpSettings!= NULL)
            {
            iBuffer->At(aIndex).iSmtpSettings->SetEmailAddressL(objectPtr);
            iBuffer->At(aIndex).iSmtpSettings->SetReplyToAddressL(objectPtr);
            iBuffer->At(aIndex).iSmtpSettings->SetReceiptAddressL(objectPtr);
            }

        CleanupStack::PopAndDestroy(); //object
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailUName)==0)
        {
        HBufC *object = HBufC::NewLC(aObject.Size());
        TPtr objectPtr = object->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( objectPtr, aObject );
        if(iBuffer->At(aIndex).iSmtpSettings!= NULL)
            {
            iBuffer->At(aIndex).iSmtpSettings->SetEmailAliasL(objectPtr);
            }
        CleanupStack::PopAndDestroy(); //object
        }

    else if(aLastUriSeg.Compare( KNSmlDMEmailMrcv)==0)
        {
        HBufC *object = HBufC::NewLC(aObject.Size());
        TPtr objectPtr = object->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( objectPtr, aObject );
        if(iBuffer->At(aIndex).iPop3Settings!= NULL)
            {
            iBuffer->At(aIndex).iPop3Settings->SetServerAddressL(objectPtr);
            }
        else if(iBuffer->At(aIndex).iImap4Settings!= NULL)
            {
            iBuffer->At(aIndex).iImap4Settings->SetServerAddressL(objectPtr);
            }
        CleanupStack::PopAndDestroy(); //object
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailMsnd)==0)
        {
        HBufC *object = HBufC::NewLC(aObject.Size());
        TPtr objectPtr = object->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( objectPtr, aObject );
        if(iBuffer->At(aIndex).iSmtpSettings!= NULL)
            {
            iBuffer->At(aIndex).iSmtpSettings->SetServerAddressL(objectPtr);
            }
        CleanupStack::PopAndDestroy(); //object
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailMpro)==0)
        {
        iBuffer->At(aIndex).iNodeBuf->At(aCommand).iStatus = CSmlDmAdapter::EOk;
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailUseSecCon)==0)
        {
        SetDefaultSecurityL(aObject, &iBuffer->At(aIndex), EUseSecCon );
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailUseSauth)==0)
        {
        HBufC8 *data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        TBool object=EFalse;
        if(dataPtr.Find(KNSmlDMEmailTrue)>=0)
            {
            object = ETrue;
            }
        if(iBuffer->At(aIndex).iSmtpSettings!= NULL)
            {
            iBuffer->At(aIndex).iSmtpSettings->SetSMTPAuth(object);
            }
        CleanupStack::PopAndDestroy(); //data
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailSauthUID)==0)
        {
        if( iBuffer->At(aIndex).iSmtpSettings )
            {
            iBuffer->At(aIndex).iSmtpSettings->SetLoginNameL(aObject);
            }
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailSauthPW)==0)
        {
        if( iBuffer->At(aIndex).iSmtpSettings )
            {
            iBuffer->At(aIndex).iSmtpSettings->SetPasswordL( aObject );
            }
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailPtxtSAuth)==0)
        {
        HBufC8 *data = aObject.AllocLC();
        TPtr8 dataPtr = data->Des();
        dataPtr.Capitalize();
        TBool object=EFalse;
        if(dataPtr.Find(KNSmlDMEmailTrue)>=0)
            {
            object = ETrue;
            }
        if(iBuffer->At(aIndex).iSmtpSettings!= NULL)
            {
            iBuffer->At(aIndex).iSmtpSettings->SetInboxLoginDetails(object);
            }
        CleanupStack::PopAndDestroy(); //data
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailSUseSecCon)==0)
        {
        SetDefaultSecurityL(aObject, &iBuffer->At(aIndex), ESuseSecCon );
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailUseStartTLS)==0)
        {
        SetDefaultSecurityL(aObject, &iBuffer->At(aIndex), EUseStartTls );
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailSUseStartTLS)==0)
        {
        SetDefaultSecurityL(aObject, &iBuffer->At(aIndex), ESuseStartTls );
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailMrcvPort)==0)
        {
        TNSmlEmailSettingsElement& bufElem = iBuffer->At(aIndex);
        TNSmlEmailCommandElement& elem = bufElem.iNodeBuf->At(aCommand);
        elem.iData = aObject.AllocL();
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailMsndPort)==0)
        {
        TNSmlEmailSettingsElement& bufElem = iBuffer->At(aIndex);
        TNSmlEmailCommandElement& elem = bufElem.iNodeBuf->At(aCommand);
        elem.iData = aObject.AllocL();
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailUseAPOP)==0)
        {
        TBool object=EFalse;
        if(aObject.Find(_L8("True"))>=0 || aObject.Find(_L8("true"))>=0)
            {
            object = ETrue;
            }
        if(iBuffer->At(aIndex).iPop3Settings!= NULL)
            {
            iBuffer->At(aIndex).iPop3Settings->SetApop(object);
            }
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailFolderPath)==0)
        {
        if(iBuffer->At(aIndex).iImap4Settings!= NULL)
            {
            iBuffer->At(aIndex).iImap4Settings->SetFolderPathL(aObject);
            }
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailToNapID)==0)
        {
        CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
        CleanupStack::PushL(iapmatch);

        TInt iap = iapmatch->IAPIdFromURIL( aObject );

        if ( iap != KErrNotFound )
            {
            TImIAPChoice localIAP;
            localIAP.iIAP = iap;
            DBG_ARGS(_S("IAP = %d"), localIAP.iIAP);
            localIAP.iDialogPref = ECommDbDialogPrefDoNotPrompt;

            if ( iBuffer->At(aIndex).iSmtpIapPref->NumberOfIAPs() )
                {
                iBuffer->At(aIndex).iIapPref->RemoveIAPL(0);
                }
            iBuffer->At(aIndex).iIapPref->AddIAPL(localIAP,0);
            if (iBuffer->At(aIndex).iSmtpSettings!= NULL)
                {
                if ( iBuffer->At(aIndex).iSmtpIapPref->NumberOfIAPs() )
                    {
                    iBuffer->At(aIndex).iSmtpIapPref->RemoveIAPL(0);
                    }
                iBuffer->At(aIndex).iSmtpIapPref->AddIAPL(localIAP,0);
                }
            }
        else
            {
            iBuffer->At(aIndex).iNodeBuf->At(aCommand).iStatus = CSmlDmAdapter::ENotFound;
            }

        CleanupStack::PopAndDestroy(); // iapmatch
        }

    else if(aLastUriSeg.Compare(KNSmlDMEmailSToNapID)==0)
        {
        CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
        CleanupStack::PushL(iapmatch);

        TInt iap = iapmatch->IAPIdFromURIL( aObject );

        if ( iap != KErrNotFound )
            {
            TImIAPChoice localIAP;
            localIAP.iIAP = iap;
            DBG_ARGS(_S("SMTP IAP = %d"), localIAP.iIAP);
            localIAP.iDialogPref = ECommDbDialogPrefDoNotPrompt;
            if ( iBuffer->At(aIndex).iSmtpSettings!= NULL )
                {
                if ( iBuffer->At(aIndex).iSmtpIapPref->NumberOfIAPs() )
                    {
                    iBuffer->At(aIndex).iSmtpIapPref->RemoveIAPL(0);
                    }
                iBuffer->At(aIndex).iSmtpIapPref->AddIAPL(localIAP,0);
                }
            else
                {
                iBuffer->At(aIndex).iNodeBuf->At(aCommand).iStatus =
                    CSmlDmAdapter::ENotFound;
                }
            }
        else
            {
            iBuffer->At(aIndex).iNodeBuf->At(aCommand).iStatus =
                CSmlDmAdapter::ENotFound;
            }

        CleanupStack::PopAndDestroy(); // iapmatch
        }
    _DBG_FILE("CNSmlDmEmailAdapter::DoAddLeafObjectL() :: end");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// void CNSmlDmEmailAdapter::SetDefaultSettingsL(TInt aIndex)
// Set defaults for used protocol
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::SetDefaultSettingsL(TInt aIndex)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::SetDefaultSettingsL() :: begin");
    TNSmlEmailSettingsElement& thisElem = iBuffer->At(aIndex);
    CEmailAccounts* emailAccs = CEmailAccounts::NewLC();
    if( thisElem.iPop3Settings )
        {
          
        emailAccs->PopulateDefaultPopSettingsL(*thisElem.iPop3Settings,
                                               *thisElem.iIapPref
                                               );
        emailAccs->PopulateDefaultSmtpSettingsL(*thisElem.iSmtpSettings,
                                                *thisElem.iSmtpIapPref 
                                               );
                                               
        thisElem.iPop3Settings->SetDeleteEmailsWhenDisconnecting( EFalse ); 
        thisElem.iPop3Settings->SetInboxSynchronisationLimit( KNSmlDMEmailDefaultHeaderCount );
        thisElem.iPop3Settings->SetSecureSockets( EFalse );
        thisElem.iPop3Settings->SetAutoSendOnConnect( ETrue );
        thisElem.iPop3Settings->SetDisconnectedUserMode( ETrue );
        thisElem.iPop3Settings->SetAcknowledgeReceipts( EFalse );                                
        }
    else if( thisElem.iImap4Settings )
        {
        
        emailAccs->PopulateDefaultImapSettingsL(*thisElem.iImap4Settings,
                                                *thisElem.iIapPref 
                                                );

        emailAccs->PopulateDefaultSmtpSettingsL(*thisElem.iSmtpSettings,
                                                *thisElem.iSmtpIapPref 
                                               );
                                                
        thisElem.iImap4Settings->SetDeleteEmailsWhenDisconnecting( EFalse );
        thisElem.iImap4Settings->SetInboxSynchronisationLimit( KNSmlDMEmailDefaultHeaderCount );
        thisElem.iImap4Settings->SetMailboxSynchronisationLimit( KNSmlDMEmailDefaultHeaderCount );
        thisElem.iImap4Settings->SetSecureSockets( EFalse );
        thisElem.iImap4Settings->SetAutoSendOnConnect( ETrue );
        thisElem.iImap4Settings->SetDisconnectedUserMode( ETrue );
        thisElem.iImap4Settings->SetImapIdle( ETrue );
        thisElem.iImap4Settings->SetUpdatingSeenFlags( ETrue );
        }
    thisElem.iSmtpSettings->SetLoginNameL(KNullDesC8);
    thisElem.iSmtpSettings->SetPasswordL(KNullDesC8);
    // Set IAP id to 0 and ask from user if ToNapID/SToNapID not sent
    TImIAPChoice tmpIap;
    tmpIap.iIAP = KNSmlDmEmailAlwaysAskIap;
    tmpIap.iDialogPref = ECommDbDialogPrefPrompt;
    thisElem.iIapPref->AddIAPL( tmpIap, 0 );
    thisElem.iSmtpIapPref->AddIAPL( tmpIap, 0 );
    CleanupStack::PopAndDestroy(); //emailAccs
    _DBG_FILE("CNSmlDmEmailAdapter::SetDefaultSettingsL() :: end");
    }

// -----------------------------------------------------------------------------
// HBufC* CNSmlDmEmailAdapter::IntToDesLC(const TInt aLuid) const
// Converts integer to 16bit descriptor
// -----------------------------------------------------------------------------
HBufC* CNSmlDmEmailAdapter::IntToDesLC(const TInt aLuid) const
    {
    HBufC* buf = HBufC::NewLC(10); //10 = max length of 32bit integer
    TPtr ptrBuf = buf->Des();
    ptrBuf.Num(aLuid);
    return buf;
    }

// -----------------------------------------------------------------------------
// HBufC8* CNSmlDmEmailAdapter::IntToDes8LC(const TInt aLuid) const
// Converts integer to 8bit descriptor
// -----------------------------------------------------------------------------
HBufC8* CNSmlDmEmailAdapter::IntToDes8LC(const TInt aLuid) const
    {
    HBufC8* buf = HBufC8::NewLC(10); //10 = max length of 32bit integer
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num(aLuid);
    return buf;
    }
   
// -----------------------------------------------------------------------------
//      CNSmlDmEmailAdapter::ConvertTo8LC()
//      Converts string value to 8-bit
//      
// -----------------------------------------------------------------------------
HBufC8* CNSmlDmEmailAdapter::ConvertTo8LC( const TDesC& aSource ) const
    {
    HBufC8* buf = HBufC8::NewLC( aSource.Length()*2);
    TPtr8 bufPtr = buf->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return buf;
    }

// -----------------------------------------------------------------------------
// TInt CNSmlDmEmailAdapter::ValidateAndStoreSettingsL(TInt aIndex
// Transfers the email settings from memory to phones permanent data store
// -----------------------------------------------------------------------------
TInt CNSmlDmEmailAdapter::ValidateAndStoreSettingsL(TInt aIndex)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::ValidateAndStoreSettingsL() : begin");
    TInt luid = 0;

    TNSmlEmailSettingsElement& thisElem = iBuffer->At(aIndex);

    if( iBuffer->At( aIndex ).iJustFetch )
        {
        _DBG_FILE("CNSmlDmEmailAdapter::ValidateAndStoreSettingsL() : end");
        return luid;
        }
    CEmailAccounts* emailAccs = CEmailAccounts::NewLC();
    if( iBuffer->At( aIndex ).iLuid != 0 )
        {
        switch ( thisElem.iAccType )
            {
        case EPop:
            {
            TPopAccount accId;
            emailAccs->GetPopAccountL( thisElem.iServiceId, accId ); 
            //Copy name field
            accId.iPopAccountName.Copy(thisElem.iName->Des());
            emailAccs->SavePopSettingsL( accId, *thisElem.iPop3Settings );
	        emailAccs->SavePopIapSettingsL( accId, *thisElem.iIapPref );            
            TSmtpAccount smtpAcc;
	        emailAccs->GetSmtpAccountL( accId.iSmtpService, smtpAcc ); 
	        //Copy name field
	        smtpAcc.iSmtpAccountName.Copy(thisElem.iName->Des());
	        emailAccs->SaveSmtpSettingsL( smtpAcc, *thisElem.iSmtpSettings );
	        emailAccs->SaveSmtpIapSettingsL( smtpAcc, *thisElem.iSmtpIapPref );
            break;            
            }
        case EImap:
            {
            TImapAccount accId2;
    	    emailAccs->GetImapAccountL( thisElem.iServiceId, accId2 );
    	    //Copy name field
    	    accId2.iImapAccountName.Copy(thisElem.iName->Des());
            emailAccs->SaveImapSettingsL( accId2, *thisElem.iImap4Settings );
	        emailAccs->SaveImapIapSettingsL( accId2, *thisElem.iIapPref );
            TSmtpAccount smtpAccForImap;
	        emailAccs->GetSmtpAccountL( accId2.iSmtpService, smtpAccForImap );
	        //Copy name field
	        smtpAccForImap.iSmtpAccountName.Copy(thisElem.iName->Des()); 
	        emailAccs->SaveSmtpSettingsL( smtpAccForImap, *thisElem.iSmtpSettings );
	        emailAccs->SaveSmtpIapSettingsL( smtpAccForImap, *thisElem.iSmtpIapPref );
            break;            
            }
        default:

            break;
            }
        }
    else // if(iBuffer->At(aIndex).iLuid != 0)
        {
        RTelServer telServer;
        User::LeaveIfError( telServer.Connect() );
        CleanupClosePushL(telServer);

        TInt numPhones = 0;
        User::LeaveIfError( telServer.EnumeratePhones( numPhones ) );
        if ( numPhones < 1 )
            {
            User::Leave( KErrNotFound );
            }

        RTelServer::TPhoneInfo info;
        User::LeaveIfError( telServer.GetPhoneInfo( 0, info ) );
        RMobilePhone mobilePhone;
        User::LeaveIfError( mobilePhone.Open( telServer, info.iName ) );
        CleanupClosePushL( mobilePhone );
        TRequestStatus status;
        RMobilePhone::TMobilePhoneIdentityV1 mobilePhoneIdentity;
        mobilePhone.GetPhoneId( status, mobilePhoneIdentity );
        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );
        TBuf<50> imei;            
        imei.Copy( mobilePhoneIdentity.iSerialNumber );
        
        CleanupStack::PopAndDestroy(&mobilePhone);
        CleanupStack::PopAndDestroy(&telServer);
        if ( thisElem.iPop3Settings )
            {
            TPopAccount accId;
            accId = emailAccs->CreatePopAccountL( *thisElem.iName, 
                                                  *thisElem.iPop3Settings, 
                                                  *thisElem.iIapPref, 
                                                  EFalse );

    	    TSmtpAccount smtpAcc = emailAccs->CreateSmtpAccountL( accId, 
    	                                                          *thisElem.iSmtpSettings, 
                                                                  *thisElem.iSmtpIapPref, 
                                                                  EFalse );
            TMsvEntry smtpEntry;
            TMsvEntry popEntry;
            CMsvEntry* entry = iMsvSession->GetEntryL(smtpAcc.iSmtpService);
            CleanupStack::PushL(entry);
            smtpEntry = entry->Entry();
            smtpEntry.iMtmData2 =  smtpAcc.iSmtpAccountId;
            smtpEntry.iDescription.Set( imei );
	        entry->ChangeL(smtpEntry);
            CleanupStack::PopAndDestroy(entry);

            entry = iMsvSession->GetEntryL(smtpAcc.iRelatedService);
            CleanupStack::PushL(entry);
            popEntry = entry->Entry();
            popEntry.iMtmData2 =  accId.iPopAccountId;          
            popEntry.iDescription.Set( imei );
	        entry->ChangeL(popEntry);
            CleanupStack::PopAndDestroy(entry);

            if ( accId.iSmtpService != smtpAcc.iSmtpService || smtpAcc.iRelatedService != accId.iPopService )
                {
                DBG_ARGS8(_S8("ERROR!!! PopAccount creation: iSmtpService=%d, smtpAcc.iSmtpService=%d, iRelatedService=%d, iPopService=%d"),
                                accId.iSmtpService, smtpAcc.iSmtpService, smtpAcc.iRelatedService, accId.iPopService );
                }                                                               
            
            thisElem.iAccType = EPop;
            thisElem.iAccId = accId.iPopAccountId;
            SetLuidById( thisElem.iAccType, thisElem.iAccId, thisElem.iLuid );

			  RArray<TPopAccount> popAccs;
              CleanupClosePushL( popAccs );
              emailAccs->GetPopAccountsL( popAccs );
              
			  RArray<TImapAccount> imapAccs;
              CleanupClosePushL( imapAccs );
              emailAccs->GetImapAccountsL( imapAccs );
              
              if ( popAccs.Count() == 1 && !imapAccs.Count())
                 {
                  emailAccs->SetDefaultSmtpAccountL( smtpAcc );
                 }
              
              CleanupStack::PopAndDestroy( &imapAccs );          
			  CleanupStack::PopAndDestroy( &popAccs );          
            // Check and set the default account
	        TSmtpAccount defSmtp;
	        TInt retVal = emailAccs->DefaultSmtpAccountL( defSmtp );
	        if (  retVal != KErrNone )
	            {
                RArray<TPopAccount> popAccounts;
                CleanupClosePushL( popAccounts );
                emailAccs->GetPopAccountsL( popAccounts );
                if ( popAccounts.Count() > 0 )
                    {
                    TSmtpAccount relatedSmtp;
                    emailAccs->GetSmtpAccountL( popAccounts[0].iSmtpService, relatedSmtp );
	                emailAccs->SetDefaultSmtpAccountL( relatedSmtp );
                    }
                CleanupStack::PopAndDestroy( &popAccounts );
	            }	                
            }
        else if ( thisElem.iImap4Settings )
            {
            TImapAccount accId2;
            accId2 = emailAccs->CreateImapAccountL(*thisElem.iName, 
                                                   *thisElem.iImap4Settings, 
                                                   *thisElem.iIapPref,
                                                   EFalse);

    	    TSmtpAccount smtpAcc = emailAccs->CreateSmtpAccountL( accId2, 
    	                                                          *thisElem.iSmtpSettings, 
                                                                  *thisElem.iSmtpIapPref, 
                                                                  EFalse );
            
            TMsvEntry smtpEntry;
            TMsvEntry imapEntry;
            CMsvEntry* entry = iMsvSession->GetEntryL(smtpAcc.iSmtpService);
            CleanupStack::PushL(entry);
            smtpEntry = entry->Entry();
            smtpEntry.iMtmData2 =  smtpAcc.iSmtpAccountId;          
            smtpEntry.iDescription.Set( imei );
	        entry->ChangeL(smtpEntry);
            CleanupStack::PopAndDestroy(entry);

            entry = iMsvSession->GetEntryL(smtpAcc.iRelatedService);
            CleanupStack::PushL(entry);
            imapEntry = entry->Entry();
            imapEntry.iMtmData2 =  accId2.iImapAccountId;          
            imapEntry.iDescription.Set( imei );
	        entry->ChangeL(imapEntry);
            CleanupStack::PopAndDestroy(entry);

            if ( accId2.iSmtpService != smtpAcc.iSmtpService || smtpAcc.iRelatedService != accId2.iImapService )
                {
                DBG_ARGS8(_S8("ERROR!!! ImapAccount creation: iSmtpService=%d, smtpAcc.iSmtpService=%d, iRelatedService=%d, iImapService=%d"),
                                accId2.iSmtpService, smtpAcc.iSmtpService, smtpAcc.iRelatedService, accId2.iImapService );
                }
                                                                  
                                                   
            thisElem.iAccType = EImap;
            thisElem.iAccId = accId2.iImapAccountId;
            SetLuidById( thisElem.iAccType, thisElem.iAccId, thisElem.iLuid );

            // Check and set the default account
            RArray<TPopAccount> popAccs;
            CleanupClosePushL( popAccs );
            emailAccs->GetPopAccountsL( popAccs );
              
			RArray<TImapAccount> imapAccs;
            CleanupClosePushL( imapAccs );
            emailAccs->GetImapAccountsL( imapAccs );
              
            if ( imapAccs.Count() == 1 && !popAccs.Count())
            	{
                emailAccs->SetDefaultSmtpAccountL( smtpAcc );
                }
              
            CleanupStack::PopAndDestroy( &imapAccs );          
			CleanupStack::PopAndDestroy( &popAccs );
	        TSmtpAccount defaultSmtp;
	        TInt retVal = emailAccs->DefaultSmtpAccountL( defaultSmtp );
	        if (  retVal != KErrNone )
	            {
                RArray<TImapAccount> imapAccs;
                CleanupClosePushL( imapAccs );
                emailAccs->GetImapAccountsL( imapAccs );
                if ( imapAccs.Count() > 0 )
                    {
                    TSmtpAccount relatedSmtp;
                    emailAccs->GetSmtpAccountL( imapAccs[0].iSmtpService, relatedSmtp );
	                emailAccs->SetDefaultSmtpAccountL( relatedSmtp );
                    }
                CleanupStack::PopAndDestroy( &imapAccs );
	            }	                
            }

        }
    luid = thisElem.iLuid;
    CleanupStack::PopAndDestroy(); //emailAccs
    _DBG_FILE("CNSmlDmEmailAdapter::ValidateAndStoreSettingsL() : end");
    return luid;
    }
// -----------------------------------------------------------------------------
// void CNSmlDmEmailAdapter::GetIdByLuid( TNSmlEmailAccountType& aAccType,
//                                           TInt& aAccId, 
//                                       const TInt aLuid ) const
// Calculates the account type and id on basis of luid
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::GetIdByLuid( TNSmlEmailAccountType& aAccType,
                                       TInt& aAccId, 
                                       const TInt aLuid ) const
    {
    if ( ( aLuid > KNSmlDmEmailImapLuidLow ) && 
         ( aLuid < KNSmlDmEmailImapLuidHigh ) )
        {
        aAccType = EImap;
        aAccId = aLuid - KNSmlDmEmailImapLuidLow;
        }
    else if (( aLuid > KNSmlDmEmailImapLuidHigh ) && 
             ( aLuid < KNSmlDmEmailPopLuidHigh ))
        {
        aAccType = EPop;
        aAccId = aLuid - KNSmlDmEmailImapLuidHigh;
        }
    else
        {
        aAccType = EUnknown;
        }

    }


// -----------------------------------------------------------------------------
// void CNSmlDmEmailAdapter::SetLuidById( const TNSmlEmailAccountType& aAccType,
// const TInt& aAccId, TInt& aLuid ) const
// Sets the luid on basis of account type and id
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::SetLuidById( const TNSmlEmailAccountType& aAccType,
                                       const TInt& aAccId, TInt& aLuid ) const
    {
    switch (aAccType)
        {
    case EImap :
        aLuid = KNSmlDmEmailImapLuidLow + aAccId;
        break;

    case EPop :
        aLuid = KNSmlDmEmailImapLuidHigh + aAccId;
        break;

    default :
        aLuid = -1;
        break;
        }
    }

// -----------------------------------------------------------------------------
// TBool CNSmlDmEmailAdapter::AccountExistsL( const CEmailAccounts* aAccs, 
//                                          const TNSmlEmailAccountType aAccType, 
//                                          const TInt aId,
//                                          TInt aIndex )
// Checks if the accout matching the given parameters is found. 
// returns True if found.
// -----------------------------------------------------------------------------
TBool CNSmlDmEmailAdapter::AccountExistsL( CEmailAccounts* aAccs, 
                                           const TNSmlEmailAccountType aAccType, 
                                           const TInt aId,
                                           TInt aIndex )
    {
    RArray<TImapAccount> imap4Accs;
    CleanupClosePushL( imap4Accs );
    RArray<TPopAccount> pop3Accs;
    CleanupClosePushL( pop3Accs );
    switch ( aAccType )
        {
    case EImap:
        aAccs->GetImapAccountsL(imap4Accs);
        for(TInt i=0;i<imap4Accs.Count();i++)
            {
            if ( imap4Accs[i].iImapAccountId == aId )
                {
                
                if ( aIndex >= 0 )
                    {
                    iBuffer->At(aIndex).iServiceId = imap4Accs[i].iImapService;
                    delete iBuffer->At(aIndex).iName;
                    iBuffer->At(aIndex).iName = NULL;
                    iBuffer->At(aIndex).iName = HBufC::NewL(imap4Accs[i].iImapAccountName.Size());
                    *iBuffer->At(aIndex).iName = imap4Accs[i].iImapAccountName;
                    }
                CleanupStack::PopAndDestroy(2); //imap4Accs, pop3Accs
                return ETrue;
                }
            }
        break;
        
    case EPop:
        aAccs->GetPopAccountsL(pop3Accs);
        for(TInt i=0;i<pop3Accs.Count();i++)
            {
            if ( pop3Accs[i].iPopAccountId == aId )
                {
                if ( aIndex >= 0 )
                    {
                    iBuffer->At(aIndex).iServiceId = pop3Accs[i].iPopService;                    
                    delete iBuffer->At(aIndex).iName;
                    iBuffer->At(aIndex).iName = NULL;
                    iBuffer->At(aIndex).iName = HBufC::NewL(pop3Accs[i].iPopAccountName.Size());
                    *iBuffer->At(aIndex).iName = pop3Accs[i].iPopAccountName;
                    }
                CleanupStack::PopAndDestroy(2); //imap4Accs, pop3Accs
                return ETrue;
                }
            }
        break;
    default:
        // Returns EFalse in this case.
        break;
        }
    CleanupStack::PopAndDestroy(2); //imap4Accs, pop3Accs
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TInt CNSmlDmEmailAdapter::DoRestoreSettingsL(TInt aIndex)
// Restores the settings to the adapter from settings store
// -----------------------------------------------------------------------------
TInt CNSmlDmEmailAdapter::DoRestoreSettingsL(TInt aIndex)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::DoRestoreSettingsL() : begin");
    TNSmlEmailSettingsElement& thisElem = iBuffer->At(aIndex);

    CEmailAccounts* emailAccs = CEmailAccounts::NewLC();
    TInt err = KErrNone;

    // These are destroyed latest at destructor
    thisElem.iSmtpSettings = new (ELeave) CImSmtpSettings;
    thisElem.iIapPref = CImIAPPreferences::NewLC();
    thisElem.iSmtpIapPref = CImIAPPreferences::NewLC();

    switch( thisElem.iAccType )
        {
    case EImap:
        if (AccountExistsL(emailAccs, thisElem.iAccType, thisElem.iAccId, aIndex ))
            {
            TImapAccount imapAccId;
            
            thisElem.iImap4Settings = new(ELeave) CImImap4Settings;
            
            //Get IMAP account by iAccId id
            emailAccs->GetImapAccountL(thisElem.iServiceId, imapAccId);
            //Get IMAP settings by account id
            emailAccs->LoadImapSettingsL( imapAccId, *thisElem.iImap4Settings );
            //Get IMAP settings IAP prefs
            emailAccs->LoadImapIapSettingsL( imapAccId, *thisElem.iIapPref );
            
            TSmtpAccount smtpAccount;
            //Get smtp account by related IMAP id
            emailAccs->GetSmtpAccountL( imapAccId.iSmtpService, smtpAccount );  
            //Get smtp settings by IMAP id                                        
            emailAccs->LoadSmtpSettingsL( smtpAccount, *thisElem.iSmtpSettings );
            //Get Smtp settings IAP prefs
            emailAccs->LoadSmtpIapSettingsL( smtpAccount, *thisElem.iSmtpIapPref );
            }
        else
            {
            err = KErrNotFound;
            }
        break;

    case EPop:
        if (AccountExistsL(emailAccs, thisElem.iAccType, thisElem.iAccId, aIndex ))
            {
            TPopAccount popAccId;
            
            thisElem.iPop3Settings = new(ELeave) CImPop3Settings;
            
            //Get POP3 account by iAccId id
            emailAccs->GetPopAccountL( thisElem.iServiceId, popAccId );
            //Get POP3 settings by account id
            emailAccs->LoadPopSettingsL( popAccId, *thisElem.iPop3Settings );
            //Get POP3 setting IAP prefs
            emailAccs->LoadPopIapSettingsL( popAccId, *thisElem.iIapPref );
                        
            TSmtpAccount smtpAccount;
            //Get smtp account by related POP id
            emailAccs->GetSmtpAccountL( popAccId.iSmtpService, smtpAccount );  
            //Get smtp settings                                        
            emailAccs->LoadSmtpSettingsL( smtpAccount, *thisElem.iSmtpSettings );
            //Get Smtp setting IAP prefs
            emailAccs->LoadSmtpIapSettingsL( smtpAccount, *thisElem.iSmtpIapPref );
            }
        else
            {
            err = KErrNotFound;
            }
        break;

    default:
        err = KErrNotFound;
        break;
        }

    if ( err )
        {
        _DBG_FILE("CNSmlDmEmailAdapter::DoRestoreSettingsL() : \
         KErrNotFound end");
        CleanupStack::PopAndDestroy(2); // iIapPref, iSmtpIapPref
        CleanupStack::PopAndDestroy(emailAccs); //emailAccs
        return KErrNotFound;
        }

    CleanupStack::Pop(2); // iIapPref, iSmtpIapPref
    CleanupStack::PopAndDestroy(emailAccs); //emailAccs
    _DBG_FILE("CNSmlDmEmailAdapter::DoRestoreSettingsL() : end");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::OpenSessionL()
// Opens the CMsvSession if it is not open already
// -----------------------------------------------------------------------------
TInt CNSmlDmEmailAdapter::OpenSessionL()
    {
    _DBG_FILE("CNSmlDmEmailAdapter::OpenSessionL() : begin");
    if(!iMsvSession)
        {
        TRAPD(err, iMsvSession = CMsvSession::OpenSyncL(*this));
        _DBG_FILE("CNSmlDmEmailAdapter::OpenSessionL() : end");
        return err;
        }

    _DBG_FILE("CNSmlDmEmailAdapter::OpenSessionL() : end");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlDmEmailAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,
                                         const TSmlDmAccessTypes& aAccTypes,
                                         MSmlDmDDFObject::TOccurence aOccurrence,
                                         MSmlDmDDFObject::TScope aScope,
                                         MSmlDmDDFObject::TDFFormat aFormat,
                                         const TDesC8& aDescription) const
    {
    aNode.SetAccessTypesL(aAccTypes);
    aNode.SetOccurenceL(aOccurrence);
    aNode.SetScopeL(aScope);
    aNode.SetDFFormatL(aFormat);
    if(aFormat!=MSmlDmDDFObject::ENode)
        {
        aNode.AddDFTypeMimeTypeL(KNSmlDMEmailTextPlain);
        }
    aNode.SetDescriptionL(aDescription);

    }

// -----------------------------------------------------------------------------
// TPtrC8 CNSmlDmEmailAdapter::RemoveDotSlash(const TDesC8& aURI)
// return uri without dot and slash in start
// -----------------------------------------------------------------------------
TPtrC8 CNSmlDmEmailAdapter::RemoveDotSlash(const TDesC8& aURI) const
    {
    if(aURI.Find(KNSmlEmailUriDotSlash)==0)
        {
        return aURI.Right(aURI.Length()-KNSmlEmailUriDotSlash().Length());
        }
    else
        {
        return aURI;
        }
    }
// -----------------------------------------------------------------------------
// CSmlDmAdapter::TError CNSmlDmEmailAdapter::FetchObjectL(const TDesC8& aURI,
// const TDesC8& aLUID, CBufBase& aObject)
// Fetches object acording to URI & LUID. Retur codes acording to 
// CSmlDmAdapter::TError.
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmEmailAdapter::FetchObjectL(const TDesC8& aURI,
                                                        const TDesC8& aLUID,
                                                        CBufBase& aObject)
    {
    _DBG_FILE("CNSmlDmEmailAdapter::FetchObjectL() : begin");
    CSmlDmAdapter::TError status = EOk;
    if(aLUID.Length()==0)
        {
        _DBG_FILE("CNSmlDmEmailAdapter::FetchObjectL() : ENotFound end");
        return CSmlDmAdapter::ENotFound;
        }
    if( OpenSessionL() != KErrNone )
        {
        _DBG_FILE("CNSmlDmEmailAdapter::FetchObjectL() : EError end");
        return CSmlDmAdapter::EError;
        }
    TInt luid = DesToInt(aLUID);
    TInt index  = -1;
    for(TInt i=0;i<iBuffer->Count();i++)
        {
        if(iBuffer->At(i).iLuid == luid)
            {
            index = i;
            break;
            }
        }

    if(index<0)
        {
        TNSmlEmailSettingsElement newNode;
        newNode.iImap4Settings = NULL;
        newNode.iPop3Settings = NULL;
        newNode.iSmtpSettings = NULL;
        newNode.iIapPref = NULL;
        newNode.iLuid = luid;
        newNode.iMappingName = aURI.AllocLC();
        newNode.iName = HBufC::NewLC(0);
        newNode.iNodeBuf = new (ELeave)
            CArrayFixFlat <TNSmlEmailCommandElement> (KNSmlDmEmailGranularity);
        newNode.iExecuted = ETrue;
        newNode.iJustFetch = ETrue;
        GetIdByLuid(newNode.iAccType, newNode.iAccId, luid);
        iBuffer->AppendL(newNode);
        index = iBuffer->Count() - 1;
        if(DoRestoreSettingsL(index)!=KErrNone)
            {
            CleanupStack::Pop(2); //newNode.iMappingName,newNode.iName
            _DBG_FILE("CNSmlDmEmailAdapter::FetchObjectL() : EError end");
            return CSmlDmAdapter::EError;
            }
        CleanupStack::Pop(2); //newNode.iMappingName,newNode.iName
        }

    TPtrC8 lastUriSeg = LastURISeg(aURI);

    if(lastUriSeg.Compare(KNSmlDMEmailName)==0)
        {
        HBufC8 *data = HBufC8::NewLC(iBuffer->At(index).iName->Length());
        TPtr8 dataPtr = data->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr, iBuffer->At(index).iName->Des() );
        aObject.InsertL(0,dataPtr);
        CleanupStack::PopAndDestroy(); //data
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailUID)==0)
        {
        HBufC *data = NULL;
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            data = HBufC::NewLC(iBuffer->At(index).iPop3Settings->LoginName().Length());
            data->Des().Copy(iBuffer->At(index).iPop3Settings->LoginName());
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            data = HBufC::NewLC(iBuffer->At(index).iImap4Settings->LoginName().Length());
            data->Des().Copy(iBuffer->At(index).iImap4Settings->LoginName());
            }
        if(data)
            {
            HBufC8 *data8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*data);
            CleanupStack::PushL(data8);
            aObject.InsertL(0,*data8);
            CleanupStack::PopAndDestroy(2);//data, data8
            }
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailPW)==0)
        {
        HBufC *data = NULL;
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            data = HBufC::NewLC(iBuffer->At(index).iPop3Settings->Password().Length());
            data->Des().Copy(iBuffer->At(index).iPop3Settings->Password());
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            data = HBufC::NewLC(iBuffer->At(index).iImap4Settings->Password().Length());
            data->Des().Copy(iBuffer->At(index).iImap4Settings->Password());
            }
        if(data)
            {
            HBufC8 *data8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*data);
            CleanupStack::PushL(data8);
            aObject.InsertL(0,*data8);
            CleanupStack::PopAndDestroy(2);//data, data8
            }
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailUAddr)==0)
        {
        HBufC *data = NULL;
        data = iBuffer->At(index).iSmtpSettings->EmailAddress().AllocLC();
        HBufC8 *data8 = HBufC8::NewLC(data->Size());
        TPtr8 dataPtr8 = data8->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr8, data->Des() );

        aObject.InsertL(0,dataPtr8);
        CleanupStack::PopAndDestroy(2); //data, data8
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailUName)==0)
        {

        HBufC *data = NULL;
        data = iBuffer->At(index).iSmtpSettings->EmailAlias().AllocLC();
        HBufC8 *data8 = HBufC8::NewLC(data->Size());
        TPtr8 dataPtr8 = data8->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr8, data->Des() );

        aObject.InsertL(0,dataPtr8);
        CleanupStack::PopAndDestroy(2); //data, data8
        }

    else if(lastUriSeg.Compare( KNSmlDMEmailMrcv)==0)
        {
        HBufC *data = NULL;
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            data = iBuffer->At(index).iPop3Settings->ServerAddress().AllocLC();
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            data = iBuffer->At(index).iImap4Settings->ServerAddress().AllocLC();
            }
        else
        	{
          	status = CSmlDmAdapter::ENotFound;
          	return status;            
        	}
        HBufC8 *data8 = HBufC8::NewLC(data->Size());
        TPtr8 dataPtr8 = data8->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr8, data->Des() );

        aObject.InsertL(0,dataPtr8);
        CleanupStack::PopAndDestroy(2); //data, data8
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailMsnd)==0)
        {
        HBufC *data = NULL;
        data = iBuffer->At(index).iSmtpSettings->ServerAddress().AllocLC();
        HBufC8 *data8 = HBufC8::NewLC(data->Size());
        TPtr8 dataPtr8 = data8->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( dataPtr8, data->Des() );

        aObject.InsertL(0,dataPtr8);
        CleanupStack::PopAndDestroy(2); //data, data8
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailMpro)==0)
        {
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            aObject.InsertL(0,KNSmlDMEmailMproPOP);
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            aObject.InsertL(0,KNSmlDMEmailMproIMAP);
            }
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailUseSecCon)==0)
        {
        FetchSecurityValueL(aObject, &iBuffer->At(index), EUseSecCon );
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailUseSauth)==0)
        {
        if(iBuffer->At(index).iSmtpSettings->SMTPAuth())
            {
            aObject.InsertL(0,KNSmlDMEmailTrue);
            }
        else
            {
            aObject.InsertL(0,KNSmlDMEmailFalse);
            }
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailSauthUID)==0)
        {
        HBufC *data = HBufC::NewLC(iBuffer->At(index).iSmtpSettings->LoginName().Length());
        data->Des().Copy(iBuffer->At(index).iSmtpSettings->LoginName());
        HBufC8 *data8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*data);
        CleanupStack::PushL(data8);
        aObject.InsertL(0,*data8);
        CleanupStack::PopAndDestroy(2);//data, data8
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailSauthPW)==0)
        {
        HBufC *data = HBufC::NewLC(iBuffer->At(index).iSmtpSettings->Password().Length());
        data->Des().Copy(iBuffer->At(index).iSmtpSettings->Password());
        HBufC8 *data8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*data);
        CleanupStack::PushL(data8);
        aObject.InsertL(0,*data8);
        CleanupStack::PopAndDestroy(2);//data, data8
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailPtxtSAuth)==0)
        {
        if(iBuffer->At(index).iSmtpSettings->InboxLoginDetails())
            {
            aObject.InsertL(0,KNSmlDMEmailTrue);
            }
        else
            {
            aObject.InsertL(0,KNSmlDMEmailFalse);
            }
        }

    else if (lastUriSeg.Compare(KNSmlDMEmailSUseSecCon)==0)
        {
        FetchSecurityValueL(aObject, &iBuffer->At(index), ESuseSecCon );
        }

    else if (lastUriSeg.Compare(KNSmlDMEmailUseStartTLS)==0)
        {
        FetchSecurityValueL(aObject, &iBuffer->At(index), EUseStartTls );
        }

    else if (lastUriSeg.Compare(KNSmlDMEmailSUseStartTLS)==0)
        {
        FetchSecurityValueL(aObject, &iBuffer->At(index), ESuseStartTls );
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailMrcvPort)==0)
        {
        TUint port;
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            port = iBuffer->At(index).iPop3Settings->Port();
            }
        else 
            {
            port = iBuffer->At(index).iImap4Settings->Port();
            }
        HBufC8 *data = IntToDes8LC(port);
        aObject.InsertL(0,data->Des());
        CleanupStack::PopAndDestroy(); //data
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailMsndPort)==0)
        {
        TUint port = iBuffer->At(index).iSmtpSettings->Port();
        HBufC8 *data = IntToDes8LC(port);
        aObject.InsertL(0,data->Des());
        CleanupStack::PopAndDestroy(); //data
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailUseAPOP)==0)
        {
        if(iBuffer->At(index).iPop3Settings!= NULL)
            {
            if(iBuffer->At(index).iPop3Settings->Apop())
                {
                aObject.InsertL(0,KNSmlDMEmailTrue);
                }
            else
                {
                aObject.InsertL(0,KNSmlDMEmailFalse);
                }
            }
        else if(iBuffer->At(index).iImap4Settings!= NULL)
            {
            status = CSmlDmAdapter::ENotFound;
            }
        }

    else if (lastUriSeg.Compare(KNSmlDMEmailFolderPath)==0)
        {
        if( iBuffer->At(index).iImap4Settings )
            {
            aObject.InsertL(0, iBuffer->At(index).iImap4Settings->FolderPath() );
            }
        else
            {
            status = CSmlDmAdapter::ENotFound;
            }
        }

    else if(lastUriSeg.Compare(KNSmlDMEmailToNapID)==0)
        {
        CNSmlDMIAPMatcher* iapmatch=CNSmlDMIAPMatcher::NewL( &Callback() );
        CleanupStack::PushL(iapmatch);
        HBufC8* uri8 = NULL;
        TInt iapCount = iBuffer->At(index).iIapPref->NumberOfIAPs();
        if ( iapCount )
            {
            TImIAPChoice localIAP = iBuffer->At(index).iIapPref->IAPPreference(0);
            uri8 = iapmatch->URIFromIAPIdL( localIAP.iIAP );
            }

        if( uri8 )
            {
            CleanupStack::PushL(uri8);
            aObject.InsertL(0,uri8->Des());
            status = CSmlDmAdapter::EOk;
            CleanupStack::PopAndDestroy(); // uri8
            }       

        CleanupStack::PopAndDestroy(); // iapMatch
        }
    else if(lastUriSeg.Compare(KNSmlDMEmailSToNapID)==0)
        {
        // SToNapID exists only for SMTP
        if (iBuffer->At(index).iSmtpSettings!= NULL)
            {
            CNSmlDMIAPMatcher* iapmatch=CNSmlDMIAPMatcher::NewL( &Callback() );
            CleanupStack::PushL(iapmatch);
            HBufC8* uri8 = NULL;
            TInt iapCount = iBuffer->At(index).iIapPref->NumberOfIAPs();
            if ( iapCount )
                {
                TImIAPChoice localIAP = iBuffer->At(index).iSmtpIapPref->IAPPreference(0);
                uri8 = iapmatch->URIFromIAPIdL( localIAP.iIAP );
                }

            if( uri8 )
                {
                CleanupStack::PushL(uri8);
                aObject.InsertL(0,uri8->Des());
                status = CSmlDmAdapter::EOk;
                CleanupStack::PopAndDestroy(); // uri8
                }          

            CleanupStack::PopAndDestroy(); // iapMatch
            }
        else
            {
            status = CSmlDmAdapter::ENotFound;
            }
        }
    _DBG_FILE("CNSmlDmEmailAdapter::FetchObjectL() : end");
    return status;
    }

// -----------------------------------------------------------------------------
// void CNSmlDmEmailAdapter::SetDefaultSecurityL(const TDesC8& aObject,
// const TNSmlEmailSettingsElement* thisElem,
// const TNSmlEmailSecuritySetting aSetting )
// Sets (on/off) the security modes: SSL/TLS, StartTLS
// -----------------------------------------------------------------------------
void CNSmlDmEmailAdapter::SetDefaultSecurityL(const TDesC8& aObject,
                                              const TNSmlEmailSettingsElement* aThisElem,
                                              const TNSmlEmailSecuritySetting aSetting
    ) const
    {
    _DBG_FILE("CNSmlDmEmailAdapter::SetDefaultSecurityL(): begin");
    HBufC8 *data = aObject.AllocLC();
    TPtr8 dataPtr = data->Des();
    dataPtr.Capitalize();
    TBool object=EFalse;
    if(dataPtr.Find(KNSmlDMEmailTrue)>=0)
        {
        object = ETrue;
        }
    switch ( aSetting )
        {
    case EUseSecCon :
        if ( object )
            {
            if ( aThisElem->iImap4Settings )
                {
                aThisElem->iImap4Settings->SetSecureSockets(EFalse);
                aThisElem->iImap4Settings->SetSSLWrapper(ETrue);
                aThisElem->iImap4Settings->SetPort( KNSmlDMEmailSslWrappedImap4Port );
                }
            else if ( aThisElem->iPop3Settings )
                {
                aThisElem->iPop3Settings->SetSecureSockets(EFalse);
                aThisElem->iPop3Settings->SetSSLWrapper(ETrue);
                aThisElem->iPop3Settings->SetPort( KNSmlDMEmailSslWrappedPop3Port );
                }
            }
        else
            {
            if ( aThisElem->iImap4Settings )
                {
                aThisElem->iImap4Settings->SetPort( KNSmlDMEmailNormalImap4Port );
                aThisElem->iImap4Settings->SetSSLWrapper(EFalse);
                }
            else if ( aThisElem->iPop3Settings )
                {
                aThisElem->iPop3Settings->SetPort( KNSmlDMEmailNormalPop3Port );
                aThisElem->iPop3Settings->SetSSLWrapper(EFalse);
                }
            }
        break;
    case EUseStartTls :
         if ( object )
            {
           if ( aThisElem->iImap4Settings )
                {
                aThisElem->iImap4Settings->SetSecureSockets(ETrue);
                aThisElem->iImap4Settings->SetSSLWrapper(EFalse);
                aThisElem->iImap4Settings->SetPort( KNSmlDMEmailNormalImap4Port );
                }
            else if ( aThisElem->iPop3Settings )
                {
                aThisElem->iPop3Settings->SetSecureSockets(ETrue);
                aThisElem->iPop3Settings->SetSSLWrapper(EFalse);
                aThisElem->iPop3Settings->SetPort( KNSmlDMEmailNormalPop3Port );
                }
            }
        else
            {
            if ( aThisElem->iImap4Settings )
                {
                aThisElem->iImap4Settings->SetSecureSockets(EFalse);
                }
            else if ( aThisElem->iPop3Settings )
                {
                aThisElem->iPop3Settings->SetSecureSockets(EFalse);
                }
            }
        break;
    case ESuseSecCon :
        if ( object )
            {
            // The port is same in all cases
            aThisElem->iSmtpSettings->SetSecureSockets(EFalse);
            aThisElem->iSmtpSettings->SetSSLWrapper(ETrue);
            aThisElem->iSmtpSettings->SetPort( KNSmlDMEmailSslSmtpPort );
            }
        else
            {
            aThisElem->iSmtpSettings->SetPort( KNSmlDMEmailNormalSmtpPort );
            aThisElem->iSmtpSettings->SetSSLWrapper( EFalse );
            }
        break;
    case ESuseStartTls :
        if ( object )
            {
            // The port is same in all cases
            aThisElem->iSmtpSettings->SetSecureSockets(ETrue);
            aThisElem->iSmtpSettings->SetSSLWrapper(EFalse);
            aThisElem->iSmtpSettings->SetPort( KNSmlDMEmailNormalSmtpPort );
            }
        else
            {
            aThisElem->iSmtpSettings->SetSecureSockets( EFalse );
            }

        break;
    default :
        break;
        }
    CleanupStack::PopAndDestroy(); // object
    _DBG_FILE("CNSmlDmEmailAdapter::SetDefaultSecurityL(): end");
    }

// -----------------------------------------------------------------------------
// CSmlDmAdapter::TError CNSmlDmEmailAdapter::FetchSecurityValueL(
//                               CBufBase& aObject,
//                               const TNSmlEmailSettingsElement* aElem,
//                               const TNSmlEmailSecuritySetting aSetting )
// Gets the status(on/off) of security modes  SSL/TLS or StartTLS
// -----------------------------------------------------------------------------

CSmlDmAdapter::TError CNSmlDmEmailAdapter::FetchSecurityValueL(
    CBufBase& aObject,
    const TNSmlEmailSettingsElement* aElem,
    const TNSmlEmailSecuritySetting aSetting ) const
    {
    _DBG_FILE("CNSmlDmEmailAdapter::FetchSecurityValueL(): start");
    TBool result=EFalse;
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    switch ( aSetting )
        {

        case EUseStartTls :
                if ( aElem->iImap4Settings)
                    {
                    result = aElem->iImap4Settings->SecureSockets();
                    }
                else if(aElem->iPop3Settings)
                    {
                    result = aElem->iPop3Settings->SecureSockets();
                    }
                else
                    {
                    status = CSmlDmAdapter::EError;
                    }
                break;
        case EUseSecCon :
                if ( aElem->iImap4Settings)
                    {
                    result = aElem->iImap4Settings->SSLWrapper();
                    }
                else if(aElem->iPop3Settings)
                    {
                    result = aElem->iPop3Settings->SSLWrapper();
                    }
                else
                    {
                    status = CSmlDmAdapter::EError;
                    }
                break;
        case ESuseStartTls :
                if(aElem->iSmtpSettings)
                    {
                    result = aElem->iSmtpSettings->SecureSockets();
                    }
                else
                    {
                    status = CSmlDmAdapter::EError;
                    }
                break;
        case ESuseSecCon :
                if(aElem->iSmtpSettings)
                    {
                    result = aElem->iSmtpSettings->SSLWrapper();
                    }
                else
                    {
                    status = CSmlDmAdapter::EError;
                    }
                break;
        default:
            status = CSmlDmAdapter::EError;
            break;
        }
        
    if ( result )
        {
        aObject.InsertL(0,KNSmlDMEmailTrue);
        }
    else 
        {
        aObject.InsertL(0,KNSmlDMEmailFalse);
        }
    _DBG_FILE("CNSmlDmEmailAdapter::FetchSecurityValueL(): end");
    return status;
    }

//--------------------------------------------------------------------
//TInt CNSmlDmEmailAdapter::ConstructTreeL(const TDesC8& aURI)
//
//-------------------------------------------------------------------

TInt CNSmlDmEmailAdapter::ConstructTreeL(const TDesC8& aURI)
{
	TInt ret = KErrNotFound;
	CEmailAccounts* emailAccs = CEmailAccounts::NewLC();
	
    if(aURI.Find(KNSmlDMEmailNodeName)!= KErrNotFound)
        {
        RArray<TImapAccount> imap4Accounts;
        CleanupClosePushL( imap4Accounts );
        emailAccs->GetImapAccountsL(imap4Accounts);

        // First, IMAP accounts
        //
        TInt luid;
        TPtrC8 name;
        // cycle through every found account
        for(TInt count = 0; count < imap4Accounts.Count(); count++)
            {
            
            	SetLuidById( EImap, imap4Accounts[count].iImapAccountId, luid );
            	HBufC8 *uri = HBufC8::NewLC(KNSmlDMEmailNodeName().Length() + 
                                            KNSmlDMEmailMproIMAP().Length() + 6);
                                            
                TPtr8 uriPtr = uri->Des();
                uriPtr.Append(KNSmlDMEmailNodeName);
                uriPtr.Append(KNSmlDMEmailSeparator8);
                uriPtr.Append(KNSmlDMEmailMproIMAP);
                uriPtr.AppendNumFixedWidth(count,EDecimal,3); 
                HBufC8 *luidBuf = IntToDes8LC(luid); 
                HBufC8* mapInfo = Callback().GetLuidAllocL(uriPtr);					
				if (mapInfo->Length() == 0)
				{                             
                	Callback().SetMappingL(uriPtr, luidBuf->Des());
                	if( aURI.Find(uriPtr) >= 0 )
        			{ 
         				ret = luid;
        			}  
				}
				else
				{
					delete mapInfo;
				}

                CleanupStack::PopAndDestroy(2); //uri,luidBuf   
            }
        CleanupStack::PopAndDestroy(); //imap4Accounts

        // Then, POP accounts
        //
        RArray<TPopAccount> pop3Accounts;
        CleanupClosePushL( pop3Accounts );
        emailAccs->GetPopAccountsL( pop3Accounts );        
        // cycle through every found account
        for(TInt count = 0; count < pop3Accounts.Count(); count++)
            {
            	SetLuidById( EPop, pop3Accounts[count].iPopAccountId, luid );
                                     
                HBufC8 *uri = HBufC8::NewLC(KNSmlDMEmailNodeName().Length() + 
                                            KNSmlDMEmailMproPOP().Length() + 5);
                TPtr8 uriPtr = uri->Des();
                uriPtr.Append(KNSmlDMEmailNodeName);
                uriPtr.Append(KNSmlDMEmailSeparator8);
                uriPtr.Append(KNSmlDMEmailMproPOP);
				uriPtr.AppendNumFixedWidth(count,EDecimal,3); 
                HBufC8 *luidBuf = IntToDes8LC(luid);
                HBufC8* mapInfo = Callback().GetLuidAllocL(uriPtr);					
				if (mapInfo->Length() == 0)
				{ 
                	Callback().SetMappingL(uriPtr,luidBuf->Des());
                	if( aURI.Find(uriPtr) >= 0 )
        			{ 
         				ret = luid;
        			}  
				}
				else
				{
					delete mapInfo;
				}

                CleanupStack::PopAndDestroy(2); //uri,luidBuf
            }
        CleanupStack::PopAndDestroy(); //pop3Accounts
        }
        CleanupStack::PopAndDestroy(); //emailAccs
		return ret;
}

//------------------------------------------------------------------------------
// TPtrC8 CNSmlDmEmailAdapter::GetDynamicEmailNodeUri( const TDesC8& aURI )
// returns Email/xxx URI
//------------------------------------------------------------------------------
TPtrC8 CNSmlDmEmailAdapter::GetDynamicEmailNodeUri( const TDesC8& aURI )
    {
    DBG_ARGS8(_S8("CNSmlDmEmailAdapter::GetDynamicEmailNodeUri() - <%S> "), &aURI);
    TInt i= 0;
	TBuf8<50> EmailAccRoot(KNSmlDMEmailNodeName);
    for ( i = aURI.Find( KNSmlDMEmailNodeName ) + EmailAccRoot.Length() + 1; 
						i < aURI.Length(); i++ )
        {
        if( aURI[i] == '/' )
            {
            break;
            }
        }        
    _DBG_FILE("CNSmlDmEmailAdapter::GetDynamicEmailNodeUri(): end");
    return aURI.Left( i );
    }


// End of File
