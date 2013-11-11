// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include "ObexParsers.h"
// System includes
#include <obexheaders.h>
// User includes
#include "harness.h"                    // CMainTestHarness
#include "ObexHarness.h"                // CObexClientTest
#include "ObexTestUtils.h"              // CObexTestUtils
#include "testframeutils.h"             // CTestScript
#include "SelectObexMtmTypeState.h"     // CSelectObexMtmTypeState
#include "SetObexMessageRecipientStates.h"	// CSetObexMessageRecipientState & BT Version
#include "CreateServiceEntryState.h"    // CCreateServiceEntryState
#include "CreateObexMessageState.h"     // CCreateObexMessageState
#include "SendMessageState.h"           // CSendMessageState
#include "AddObexAttachmentState.h"     // CAddObexAttachmentState
#include "ObexFileDebugReceiveState.h"  // CObexFileDebugReceiveState
#include "ObexIgnoreSendErrorsState.h"  // CObexIgnoreSendErrorsState
#include "sendasstates.h"               // CObexSendAsCreateState, CObexSendAsAddRecipientState, 
                                        // CObexSendAsSetSubjectState, CObexSendAsAddAttachmentState, 
                                        // CObexSendAsAbandonState, CObexSendAsSaveState, CObexSendAsValidateState
#include "obexsetpasswordstate.h"		// CObexSetPasswordState
#include "obexresetpasswordstate.h"		// CObexResetPasswordState
#include "obexgetpasswordstate.h"		// CObexGetPasswordState

#include "RegisterObexMtmTypeState.h"   // CRegisterObexMtmTypeState
#include "UnRegisterObexMtmTypeState.h"	// CRegisterObexMtmTypeState

#include "AddHeadersState.h"			// CAddHeadersState


//
//                       SCRIPT TOKENS                             //
//

// Test tokens
_LIT(KCommandObexClientTest,            "obex_client_test");
_LIT(KCommandObexSelectMtm,             "obex_select_mtm");
_LIT(KCommandObexCreateMessage,         "create_message");
_LIT(KCommandObexSetMessageAddressee,   "set_recipient");
_LIT(KCommandObexSetBTMessageAddressee, "set_bt_recipient");
_LIT(KCommandObexSendMessage,           "send_message");
_LIT(KCommandObexSendMessageAndCancel,  "send_message_and_cancel");
_LIT(KCommandObexCreateService,         "create_service");
_LIT(KCommandObexAddAttachmentAsEntry,  "add_attachment_as_entry");
_LIT(KCommandObexAddAttachmentByName,   "add_attachment_by_name");
_LIT(KCommandObexSimulateObjectReceipt, "simulate_object_receipt");
_LIT(KCommandObexIgnoreSendErrors,      "ignore_send_errors");


// Send As Commands
_LIT(KCommandObexCreateSendAs,          "sendas_create");                       // 0 parameters, creates a SendAs object and message
_LIT(KCommandObexAddSendAsRecipient,    "sendas_add_recipient");                // 1 parameter, adds the specified recipient to the SendAs message
_LIT(KCommandObexAddSendAsBTRecipient,  "sendas_add_bt_recipient");             // 1 parameter, adds the specified recipient to the SendAs message
_LIT(KCommandObexSetSendAsSubject,      "sendas_set_subject");                  // 1 parameter, sets the subject of a SendAs Message
_LIT(KCommandObexAddSendAsAttachment,   "sendas_add_attachment");               // 1 parameter, adds the specified attachment to a SendAs Message
_LIT(KCommandObexAbandonSendAsMessage,  "sendas_abandon_message");              // 0 parameters, abandons the creation of a SendAs message
_LIT(KCommandObexSaveSendAsMessage,     "sendas_save_message");                 // 0 parameters, saves the current SendAs message to the outbox
_LIT(KCommandObexValidateSendAsMessage, "sendas_validate_message");             // 0 parameters, validates the current SendAs message

//password commands

//register/unregistering commands
_LIT(KCommandObexRegisterMtm, "register_mtm");									// 1 paramter, registers either an IR or BT Mtm
_LIT(KCommandObexUnRegisterMtm, "unregister_mtm");								// 1 paramter, unregister either an IR or BT Mtm

// commands for obex header support
_LIT(KCommandObexAddHeader, "add_header");
_LIT(KCommandObexAddAttachmentByNameWithHeaders, "add_attachment_by_name_with_headers");


//
// CObexMainSectionParser
//
CObexMainSectionParser* CObexMainSectionParser::NewL(CMainTestHarness& aMainTest, CTestScript& aScript)
    {
    CObexMainSectionParser* self = new (ELeave) CObexMainSectionParser(aMainTest, aScript);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

void CObexMainSectionParser::ConstructL()
    {
    CMainSectionParser::ConstructL();
    iCommandParsers->AppendL(CNewObexTestCommandParser::NewL(iScript, iMainTest));
    }

CObexMainSectionParser::CObexMainSectionParser(CMainTestHarness& aMainTest, CTestScript& aScript)
: CMainSectionParser(aMainTest, aScript),
  iMainTest(aMainTest)
    {
    }


//
// CNewObexTestCommandParser
//
CNewObexTestCommandParser* CNewObexTestCommandParser::NewL(CTestScript& aScript, CMainTestHarness& aMainTest)
    {
    CNewObexTestCommandParser* self = new (ELeave) CNewObexTestCommandParser(aScript, aMainTest);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

void CNewObexTestCommandParser::ConstructL()
    {
	CBaseCommandParser::ConstructL();
	AddCommandL(KCommandObexClientTest);
    }

CNewObexTestCommandParser::CNewObexTestCommandParser(CTestScript& aScript, CMainTestHarness& aMainTest)
: iMainTest(aMainTest),
  iScript(aScript)
    {
    }

void CNewObexTestCommandParser::ProcessL()
    {
	// Create an obex test utilities
	CObexTestUtils* obexTestUtils = CObexTestUtils::NewL(iMainTest.TestConsole());
	CleanupStack::PushL(obexTestUtils);                                                                                 // PUSH

	// Create an obex test, give it the test utilities
	CObexClientTest* obexTest = CObexClientTest::NewL(obexTestUtils, iMainTest.TestConsole()); // obexTest takes ownership of obexTestUtils
	CleanupStack::Pop(); // obexTestUtils                                                                               // POP(1)
	CleanupStack::PushL(obexTest);                                                                                      // PUSH

	// Attach the obex test to the main test
	iMainTest.AddStateL(obexTest, iDebugInfo); // iMainTest takes ownership of obexTest
	CleanupStack::Pop(); // obexTest                                                                                    // POP(1)

	// Check that there is one argument, the obex test section name
	CheckNumberOfArgumentsL(1);

	// Create an obex client section parser
	CObexClientSectionParser* sectionParser = CObexClientSectionParser::NewL(*obexTest, iScript, (*iArgumentList)[0]);
	CleanupStack::PushL(sectionParser);                                                                                 // PUSH
	sectionParser->ParseL();
	CleanupStack::PopAndDestroy(); // sectionParser                                                                     // POP(1)
    }



//
// CObexClientSectionParser
//
CObexClientSectionParser* CObexClientSectionParser::NewL(CObexClientTest& aObexClientTest, CTestScript& aScript, const TDesC& aNewSectionName)
    {
	CObexClientSectionParser* self = new (ELeave) CObexClientSectionParser(aObexClientTest, aScript);
	CleanupStack::PushL(self);
	self->ConstructL(aNewSectionName);
	CleanupStack::Pop(self);
	return self;
    }

void CObexClientSectionParser::ConstructL(const TDesC& aNewSectionName)
    {
    CBaseSectionParser::ConstructL(aNewSectionName);
    
    TTestDebugInfo debugInfo = iObexTest.GetDebugInfo();
	debugInfo.SetTestHarnessName(iSection->SectionPosition());
	iObexTest.SetDebugInfo(debugInfo);

    iCommandParsers->AppendL(CMsvSelectParser::NewL(iObexTest));
	iCommandParsers->AppendL(CMsvUtilitiesParser::NewL(iObexTest));
	iCommandParsers->AppendL(CMsvOperationParser::NewL(iObexTest));
    iCommandParsers->AppendL(CObexClientTestCommandParser::NewL(iScript, iObexTest));
    }

CObexClientSectionParser::CObexClientSectionParser(CObexClientTest& aObexTest, CTestScript& aScript)
: CBaseSectionParser(aScript),
  iObexTest(aObexTest),
  iScript(aScript)
    {
    }


void CObexClientSectionParser::LogCommentL(const TDesC& aComment)
    {
	iObexTest.ObexTestUtils().WriteComment(aComment);
    }

//
// CObexClientTestCommandParser
//
void CObexClientTestCommandParser::ProcessL()
    {
    if ((*iCurrentCommand) == KCommandObexSelectMtm)
        {
        CheckNumberOfArgumentsL(1);     //"BT" or "IR"
        iObexTest.AddStateL(new (ELeave)CSelectObexMtmTypeState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if ((*iCurrentCommand) == KCommandObexCreateService)
        {
        CheckNumberOfArgumentsL(0);
        iObexTest.AddStateL(new (ELeave)CCreateServiceEntryState(iObexTest), iDebugInfo);
        }
    else if ((*iCurrentCommand) == KCommandObexCreateMessage)
        {
        CheckNumberOfArgumentsL(0);     
        iObexTest.AddStateL(new (ELeave)CCreateObexMessageState(iObexTest), iDebugInfo);
        }
    else if ((*iCurrentCommand) == KCommandObexSendMessage)
        {
		if (iArgumentList->Count() == 0)
			iObexTest.AddStateL(new (ELeave)CSendMessageState(_L("succeed"), iObexTest), iDebugInfo);
		else
			iObexTest.AddStateL(new (ELeave)CSendMessageState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if ((*iCurrentCommand) == KCommandObexSendMessageAndCancel)
        {
		iObexTest.SetCancelGranularity(1000);
        iObexTest.AddStateL(new (ELeave)CSendMessageState(_L("succeed"), iObexTest), iDebugInfo);
        }
    else if ((*iCurrentCommand) == KCommandObexSetMessageAddressee)
        {
        CheckNumberOfArgumentsL(1);     
        iObexTest.AddStateL(new (ELeave)CSetObexMessageRecipientState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if ((*iCurrentCommand) == KCommandObexSetBTMessageAddressee)
        {
        CheckNumberOfArgumentsL(1);     
        iObexTest.AddStateL(new (ELeave)CSetObexMessageBTRecipientState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if ((*iCurrentCommand) == KCommandObexAddAttachmentByName) 
        {
        CheckNumberOfArgumentsL(1);     
        iObexTest.AddStateL(new (ELeave)CAddObexAttachmentByNameState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if ((*iCurrentCommand) == KCommandObexAddAttachmentAsEntry) 
        {
        CheckNumberOfArgumentsL(1);     
        iObexTest.AddStateL(new (ELeave)CAddObexAttachmentAsEntryState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexSimulateObjectReceipt)
        {
        CheckNumberOfArgumentsL(2);      
        iObexTest.AddStateL(new (ELeave)CObexFileDebugReceiveState((*iArgumentList)[0], (*iArgumentList)[1], iObexTest), iDebugInfo);
        }
//sendas
    else if((*iCurrentCommand) == KCommandObexCreateSendAs)
        {
        CheckNumberOfArgumentsL(0);
        iObexTest.AddStateL(new (ELeave) CObexSendAsCreateState(iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexAddSendAsRecipient)
        {
        CheckNumberOfArgumentsL(1);
        iObexTest.AddStateL(new (ELeave) CObexSendAsAddRecipientState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexAddSendAsBTRecipient)
        {
        CheckNumberOfArgumentsL(1);
        iObexTest.AddStateL(new (ELeave) CObexSendAsAddBTRecipientState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexSetSendAsSubject)
        {
        CheckNumberOfArgumentsL(1);
        iObexTest.AddStateL(new (ELeave) CObexSendAsSetSubjectState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexAddSendAsAttachment)
        {
        CheckNumberOfArgumentsL(1);
        iObexTest.AddStateL(new (ELeave) CObexSendAsAddAttachmentState((*iArgumentList)[0], iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexAbandonSendAsMessage)
        {
        CheckNumberOfArgumentsL(0);
        iObexTest.AddStateL(new (ELeave) CObexSendAsAbandonState(iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexSaveSendAsMessage)
        {
        CheckNumberOfArgumentsL(0);
        iObexTest.AddStateL(new (ELeave) CObexSendAsSaveState(iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexValidateSendAsMessage)
        {
        CheckNumberOfArgumentsL(0);
        iObexTest.AddStateL(new (ELeave) CObexSendAsValidateState(iObexTest), iDebugInfo);
        }
    else if((*iCurrentCommand) == KCommandObexIgnoreSendErrors)
        {
        CheckNumberOfArgumentsL(0);
        iObexTest.AddStateL(new (ELeave) CObexIgnoreSendErrorsState(iObexTest), iDebugInfo);
        }
//registering mtms
	else if ((*iCurrentCommand) == KCommandObexRegisterMtm)
		{
		CheckNumberOfArgumentsL(1);
		iObexTest.AddStateL(new (ELeave) CRegisterObexMtmTypeState((*iArgumentList)[0], iObexTest), iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandObexUnRegisterMtm)
		{
		CheckNumberOfArgumentsL(1);
		iObexTest.AddStateL(new (ELeave) CUnRegisterObexMtmTypeState((*iArgumentList)[0], iObexTest), iDebugInfo);
		}
// obex header
	else if ((*iCurrentCommand) == KCommandObexAddHeader)
		{
		CheckNumberOfArgumentsL(2);

		// Determine the obex header HI value.
		TUint8 headerHI = 0;
		TLex lex((*iArgumentList)[0]);

		lex.Inc(2); // Jump '0x'.
		User::LeaveIfError(lex.Val(headerHI, EHex));

		// Determine the obex header type from the HI value.
		CObexHeader::THeaderType headerType;

		// Mask the top 2 bits, and shift the result down.
		headerType = (CObexHeader::THeaderType)((0xC0 & headerHI) >> 6);

		iObexTest.AddStateL(new (ELeave) CAddHeadersState(headerType, headerHI, (*iArgumentList)[1], iObexTest), iDebugInfo);
		}

	else if ((*iCurrentCommand) == KCommandObexAddAttachmentByNameWithHeaders)
		{
        CheckNumberOfArgumentsL(1);     
		iObexTest.AddStateL(new (ELeave) CAddObexAttachmentByNameWithHeadersState((*iArgumentList)[0], iObexTest), iDebugInfo);
		}

    else
        User::Leave(KErrNotSupported);
    }

CObexClientTestCommandParser* CObexClientTestCommandParser::NewL(CTestScript& aScript, CObexClientTest& aObexTest)
    {
    CObexClientTestCommandParser* self = new (ELeave) CObexClientTestCommandParser(aScript, aObexTest);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CObexClientTestCommandParser::ConstructL()
    {
    CBaseCommandParser::ConstructL();
	AddCommandL(KCommandObexSelectMtm);
	AddCommandL(KCommandObexCreateMessage); 
	AddCommandL(KCommandObexSendMessage);
	AddCommandL(KCommandObexSendMessageAndCancel);
	AddCommandL(KCommandObexSetMessageAddressee);
	AddCommandL(KCommandObexSetBTMessageAddressee);
    AddCommandL(KCommandObexCreateService);
    AddCommandL(KCommandObexAddAttachmentByName);
    AddCommandL(KCommandObexAddAttachmentAsEntry);
    AddCommandL(KCommandObexSimulateObjectReceipt);
    AddCommandL(KCommandObexIgnoreSendErrors);

//sendas
    AddCommandL(KCommandObexCreateSendAs);
    AddCommandL(KCommandObexAddSendAsRecipient);
    AddCommandL(KCommandObexAddSendAsBTRecipient);
    AddCommandL(KCommandObexSetSendAsSubject);
    AddCommandL(KCommandObexAddSendAsAttachment);
    AddCommandL(KCommandObexAbandonSendAsMessage);
    AddCommandL(KCommandObexSaveSendAsMessage);
    AddCommandL(KCommandObexValidateSendAsMessage);

#ifdef __SUPPORT_MESSAGING_API_V2__
//password
	AddCommandL(KCommandObexSetPassword);
	AddCommandL(KCommandObexResetPassword);
	AddCommandL(KCommandObexGetPassword);
#endif //__SUPPORT_MESSAGING_API_V2__

//registering mtm types
	AddCommandL(KCommandObexRegisterMtm);
	AddCommandL(KCommandObexUnRegisterMtm);

// obex header
	AddCommandL(KCommandObexAddHeader);
	AddCommandL(KCommandObexAddAttachmentByNameWithHeaders);
    }

CObexClientTestCommandParser::CObexClientTestCommandParser(CTestScript& aScript, CObexClientTest& aObexTest)
: iScript(aScript),
  iObexTest(aObexTest)
    {

    }


