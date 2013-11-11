// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "parsers.h"
#include "commands.h"

#ifndef _NO_IAP_PREFS
#include <iapprefs.h>
#endif


// Msv client commands
_LIT(KCommandStartClientSession,	"start_client_session");		// 0 parameters, initialises CMsvTestUtils, and frees resources. Should be called at the start of every section
_LIT(KCommandResetClient,			"reset_client");				// 0 parameters, resets CMsvTestUtils, and frees resources. Should be called at the end of every section

// Msv utility commands
_LIT(KCommandCleanMessageFolder,	"clean_message_folder");		// 0 parameters, deletes message store
_LIT(KCommandCheckMessageStore,		"check_message_store");
_LIT(KCommandAddComment,			"add_comment");					// n parameters, adds a comment to the log, concatenates parameters
_LIT(KCommandWait,					"wait");						// 1 parameter, waits for n seconds...
_LIT(KCommandTestEntry,				"test_entry");					// 1 parameter, tests the attributes of the currently selected message
_LIT(KCommandDumpMessageStore,		"dump_message_store");			// 0 parameters, outputs the contents of the message store to a file

// Msv selection commands
_LIT(KCommandSelectFolder,			"select_folder");				// 1 parameter, selects a folder
_LIT(KCommandSelectEntry,			"select_entry");				// 1 parameter, select an entry by looking in iDetails and iDescription (Note: can only take 1 parameter)
_LIT(KCommandSelectRootFolder,		"select_root_folder");			// 0 parameters, selects root
_LIT(KCommandSelectAllMessages,		"select_all_messages");			// 0 parameters, selects all entries in current folder
_LIT(KCommandSelectDestination,		"select_destination_folder");	// 1 parameter, selects a destination folder used for copying and moving
_LIT(KCommandCheckSelectionCount,	"check_selection_count");		// 1 parameter, tests the current selection count == parameter
_LIT(KCommandSelectLastInSelection,	"select_last_in_selection");	// 1 parameter, reduces the current selection to its last N entries
_LIT(KCommandSelectFirstInSelection,"select_first_in_selection");	// 1 parameter, reduces the current selection to its first N entries
_LIT(KCommandCheckRemoteFolderSize,	"check_remote_folder_size");	// 1 parameter, checks the remote folder size is what is expected
_LIT(KCommandSelectPopulated,		"select_populated");			// 0 parameter, selects populated messages in the current selection
_LIT(KCommandCheckNewFlag,			"check_new_flag");				// 2 parameters, tests if the selected entry(with the index Parameter1) has its New flag == parameter2
_LIT(KCommandCheckPriority,			"check_priority");				// 2 parameters, tests if the selected entry(with the index Parameter1) has its Priority flag == parameter2
_LIT(KCommandCheckAttachment,		"check_attachment");			// 2 parameters, tests if the selected entry(with the index Parameter1) has its Attachment flag == parameter2
_LIT(KCommandCheckBodyText,			"check_body_text");				// 2 parameters, tests if the selected entry(with the index Parameter1) has Body Text == parameter2

// The main section
_LIT(KMainSectionName, "main");


// Msv client operations
_LIT(KCommandMsvCopySelection,		"copy");						// 0 parameters, copies a 'selection' to a 'destination folder'
_LIT(KCommandMsvMoveSelection,		"move");						// 0 parameters, moves a 'selection' to a 'destination folder'
_LIT(KCommandMsvDeleteSelection,	"delete");

// Test entry commands
_LIT(KCommandTestEntryVisible,		"visible");						// 0 parameters, copies a 'selection' to a 'destination folder'
_LIT(KCommandTestEntryComplete,		"complete");					// 0 parameters, moves a 'selection' to a 'destination folder'

// True or False ?
_LIT(KParameterETrue,				"true");	// !EFalse
_LIT(KParameterEFalse,				"false");	// EFalse

//
//
// CMainSectionParser
//

EXPORT_C CMainSectionParser* CMainSectionParser::NewL(CMainTestHarness& aMainTest, CTestScript& aScript)
	{
	CMainSectionParser* self = new (ELeave) CMainSectionParser(aMainTest, aScript);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CMainSectionParser::LogCommentL(const TDesC& aComment)
	{
	iMainTest.TestConsole().Printf(aComment);
	}

EXPORT_C void CMainSectionParser::ConstructL()
	{
	CBaseSectionParser::ConstructL(KMainSectionName);
	//iCommandParsers->AppendL(CNewEmailTestCommandParser::NewL(iScript, iMainTest));
	}

EXPORT_C CMainSectionParser::CMainSectionParser(CMainTestHarness& aMainTest, CTestScript& aScript) : CBaseSectionParser(aScript), iMainTest(aMainTest)
	{
	}




//
//
// CMsvUtilitiesParser
//

EXPORT_C CMsvUtilitiesParser* CMsvUtilitiesParser::NewL(CMsvClientTest& aMsvClientTest)
	{
	CMsvUtilitiesParser* self = new (ELeave) CMsvUtilitiesParser(aMsvClientTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CMsvUtilitiesParser::ProcessL()
	{
	if ((*iCurrentCommand) == KCommandCleanMessageFolder)
		{
		iMsvClientTest.AddStateL(new (ELeave) CCleanMessageFolder(iMsvClientTest.MsvTestUtils()), iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandCheckMessageStore)
		{
		CheckNumberOfArgumentsL(2);
		TParse fileName;
		// MMS Test Harness: Changed this line to use the MsvTestUtils version of
		// ResolveFile() rather than the one declared globally in t_email.cpp.
		iMsvClientTest.MsvTestUtils().ResolveFile((*iArgumentList)[0], (*iArgumentList)[1], fileName);
		iMsvClientTest.AddStateL(new (ELeave) CCheckMessageFolder(fileName.FullName(), iMsvClientTest.MsvTestUtils()), iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandStartClientSession)
		{
		iMsvClientTest.AddStateL(new (ELeave) CGoClientSide(iMsvClientTest.MsvTestUtils()), iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandAddComment)
		{
		iMsvClientTest.AddStateL(CAddComment::NewL(*iArgumentList, iMsvClientTest.MsvTestUtils()), iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandResetClient)
		{
		// MMS Test Harness: this wait appears to be useless!
		//iMsvClientTest.AddStateL(new (ELeave) CLongWait(), iDebugInfo);
		iMsvClientTest.AddStateL(new (ELeave) CResetMessageServer(iMsvClientTest), iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandTestEntry)
		{
		iMsvClientTest.AddStateL(new (ELeave) CMsvTestEntry(iMsvClientTest), iDebugInfo);

		// eh xxxx, set up section parser for the details
		}
	// MMS Test Harness: added this command to output the contents
	// of the message store
	else if ((*iCurrentCommand) == KCommandDumpMessageStore)
		{
		iMsvClientTest.AddStateL(new (ELeave) CDumpMessageStore(iMsvClientTest.MsvTestUtils()), iDebugInfo);
		}
	// MMS Test Harness: moved the "wait" command here from
	// CEmailUtilitiesParser.
	else if ((*iCurrentCommand) == KCommandWait)
		{
		CheckNumberOfArgumentsL(1);

		// Get the Wait Time
		TLex lex((*iArgumentList)[0]);
		TInt waitTime;
		User::LeaveIfError(lex.Val(waitTime));

		iMsvClientTest.AddStateL(new (ELeave) CLongWait(waitTime), iDebugInfo);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CMsvUtilitiesParser::ConstructL()
	{
	CBaseCommandParser::ConstructL();
	AddCommandL(KCommandCleanMessageFolder);
	AddCommandL(KCommandCheckMessageStore);
	AddCommandL(KCommandAddComment);
	AddCommandL(KCommandStartClientSession);
	AddCommandL(KCommandResetClient);
	AddCommandL(KCommandTestEntry);
	AddCommandL(KCommandDumpMessageStore);
	// MMS Test Harness: moved the "wait" command here from
	// CEmailUtilitiesParser.
	AddCommandL(KCommandWait);
	}

CMsvUtilitiesParser::CMsvUtilitiesParser(CMsvClientTest& aMsvClientTest) : iMsvClientTest(aMsvClientTest)
	{
	}








//
//
// CMsvSelectParser
//

EXPORT_C CMsvSelectParser* CMsvSelectParser::NewL(CMsvClientTest& aMsvClientTest)
	{
	CMsvSelectParser* self = new (ELeave) CMsvSelectParser(aMsvClientTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C void CMsvSelectParser::ProcessL()
	{
	if ((*iCurrentCommand) == KCommandSelectFolder)
		{
		CheckNumberOfArgumentsL(1);
//		iMsvClientTest.AddStateL(CSelectEntry::NewL((*iArgumentList)[0], iMsvClientTest.iSelectedFolder, iMsvClientTest.iSelectedFolder, iMsvClientTest.MsvTestUtils())
//									, iDebugInfo);
		iMsvClientTest.AddStateL(CSelectEntry::NewL((*iArgumentList)[0], iMsvClientTest.iSelectedFolder, &(iMsvClientTest.iSelectedFolder), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandSelectEntry)
		{
		CheckNumberOfArgumentsL(1);
//		iMsvClientTest.AddStateL(CSelectEntry::NewL((*iArgumentList)[0], iMsvClientTest.iSelectedFolder, iMsvClientTest.iSelectedFolder, iMsvClientTest.MsvTestUtils())
//									, iDebugInfo);
		iMsvClientTest.AddStateL(CSelectEntry::NewL((*iArgumentList)[0], iMsvClientTest.iSelectedFolder, iMsvClientTest.iCurrentSelection, iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandSelectRootFolder)
		{
		CheckNumberOfArgumentsL(0);
		iMsvClientTest.AddStateL(new (ELeave) CSelectEntryById(KMsvRootIndexEntryId, iMsvClientTest.iSelectedFolder, iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandSelectAllMessages)
		{
		CheckNumberOfArgumentsL(0);
		iMsvClientTest.AddStateL(new (ELeave) CSelectAllMessages(iMsvClientTest.iSelectedFolder, *(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandSelectDestination)
		{
		CheckNumberOfArgumentsL(1);
//		iMsvClientTest.AddStateL(CSelectEntry::NewL((*iArgumentList)[0], iMsvClientTest.iSelectedFolder, iMsvClientTest.iDestinationFolder, iMsvClientTest.MsvTestUtils() )
//									, iDebugInfo);
		iMsvClientTest.AddStateL(CSelectEntry::NewL((*iArgumentList)[0], iMsvClientTest.iSelectedFolder, &(iMsvClientTest.iDestinationFolder), iMsvClientTest.MsvTestUtils() )
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandCheckSelectionCount)
		{
		CheckNumberOfArgumentsL(1);
		TLex lex((*iArgumentList)[0]);
		TInt expectedCount;
		User::LeaveIfError(lex.Val(expectedCount));
		iMsvClientTest.AddStateL(new (ELeave) CCheckSelectionCount(expectedCount, *(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandCheckNewFlag)
		{
		CheckNumberOfArgumentsL(2);

		// Get the index of the Selected Entry to Test
		TLex lex((*iArgumentList)[0]);
		TInt selectedIndex;
		User::LeaveIfError(lex.Val(selectedIndex));

		// Get the expected New Status for this entry
		lex = ((*iArgumentList)[1]);
		TInt argumentValue;
		User::LeaveIfError(lex.Val(argumentValue));
		TBool expectedNewStatus = STATIC_CAST(TBool, argumentValue);
		iMsvClientTest.AddStateL(new (ELeave) CCheckNewFlag(selectedIndex, expectedNewStatus, *(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandCheckPriority)
		{
		CheckNumberOfArgumentsL(2);

		// Get the index of the Selected Entry to Test
		TLex lex((*iArgumentList)[0]);
		TInt selectedIndex;
		User::LeaveIfError(lex.Val(selectedIndex));

		// Get the expected New Status for this entry
		lex = ((*iArgumentList)[1]);
		TInt priority;
		User::LeaveIfError(lex.Val(priority));
		iMsvClientTest.AddStateL(new (ELeave) CCheckPriority(selectedIndex, priority, *(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandCheckAttachment)
		{
		CheckNumberOfArgumentsL(2);

		// Get the index of the Selected Entry to Test
		TLex lex((*iArgumentList)[0]);
		TInt selectedIndex;
		User::LeaveIfError(lex.Val(selectedIndex));

		// Get the expected Attachment Status for this entry
		lex = ((*iArgumentList)[1]);
		TInt argumentValue;
		User::LeaveIfError(lex.Val(argumentValue));
		TBool testAttachment = STATIC_CAST(TBool, argumentValue);
		iMsvClientTest.AddStateL(new (ELeave) CCheckAttachment(selectedIndex, testAttachment, *(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandCheckBodyText)
		{
		CheckNumberOfArgumentsL(2);

		// Get the index of the Selected Entry to Test
		TLex lex((*iArgumentList)[0]);
		TInt selectedIndex;
		User::LeaveIfError(lex.Val(selectedIndex));

		// Get the expected Body Text
		TBuf<KMaxTestBodyTxt> bodyText = (*iArgumentList)[1];
		iMsvClientTest.AddStateL(CCheckBodyText::NewL(selectedIndex, bodyText, *(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandSelectLastInSelection)
		{
		CheckNumberOfArgumentsL(1);
		TLex lex((*iArgumentList)[0]);
		TInt expectedCount;
		User::LeaveIfError(lex.Val(expectedCount));
		iMsvClientTest.AddStateL(new (ELeave) CSelectLastInSelection(expectedCount, *(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandSelectFirstInSelection)
		{
		CheckNumberOfArgumentsL(1);
		TLex lex((*iArgumentList)[0]);
		TInt expectedCount;
		User::LeaveIfError(lex.Val(expectedCount));
		iMsvClientTest.AddStateL(new (ELeave) CSelectFirstInSelection(expectedCount, *(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandCheckRemoteFolderSize)
		{
		CheckNumberOfArgumentsL(1);
		TLex lex((*iArgumentList)[0]);
		TInt expectedCount;
		User::LeaveIfError(lex.Val(expectedCount));
		iMsvClientTest.AddStateL(new (ELeave) CCheckRemoteFolderSize(expectedCount, iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandSelectPopulated)
		{
		CheckNumberOfArgumentsL(0);
		iMsvClientTest.AddStateL(new (ELeave) CSelectPopulatedInSelection(*(iMsvClientTest.iCurrentSelection), iMsvClientTest.MsvTestUtils())
									, iDebugInfo);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CMsvSelectParser::ConstructL()
	{
	CBaseCommandParser::ConstructL();
	AddCommandL(KCommandSelectRootFolder);
	AddCommandL(KCommandSelectFolder);
	AddCommandL(KCommandSelectEntry);
	AddCommandL(KCommandSelectAllMessages);
	AddCommandL(KCommandSelectDestination);
	AddCommandL(KCommandCheckSelectionCount);
	AddCommandL(KCommandSelectLastInSelection);
	AddCommandL(KCommandSelectFirstInSelection);
	AddCommandL(KCommandCheckRemoteFolderSize);
	AddCommandL(KCommandSelectPopulated);
	AddCommandL(KCommandCheckNewFlag);
	AddCommandL(KCommandCheckPriority);
	AddCommandL(KCommandCheckAttachment);
	AddCommandL(KCommandCheckBodyText);
	}

CMsvSelectParser::CMsvSelectParser(CMsvClientTest& aMsvClientTest) : iMsvClientTest(aMsvClientTest)
	{
	}



//
//
// CMsvOperationParser
//

EXPORT_C CMsvOperationParser* CMsvOperationParser::NewL(CMsvClientTest& aMsvClientTest)
	{
	CMsvOperationParser* self = new (ELeave) CMsvOperationParser(aMsvClientTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CMsvOperationParser::ProcessL()
	{
	if ((*iCurrentCommand) == KCommandMsvCopySelection)
		{
		CheckNumberOfArgumentsL(0);
		iMsvClientTest.AddStateL(new (ELeave) CCopySelection(iMsvClientTest), iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandMsvMoveSelection)
		{
		CheckNumberOfArgumentsL(0);
		iMsvClientTest.AddStateL(new (ELeave) CMoveSelection(iMsvClientTest), iDebugInfo);
		}
	else if ((*iCurrentCommand) == KCommandMsvDeleteSelection)
		{
		CheckNumberOfArgumentsL(0);
		iMsvClientTest.AddStateL(new (ELeave) CDeleteSelection(iMsvClientTest), iDebugInfo);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CMsvOperationParser::ConstructL()
	{
	CBaseCommandParser::ConstructL();
	AddCommandL(KCommandMsvCopySelection);
	AddCommandL(KCommandMsvMoveSelection);
	AddCommandL(KCommandMsvDeleteSelection);
	}

CMsvOperationParser::CMsvOperationParser(CMsvClientTest& aMsvClientTest) : iMsvClientTest(aMsvClientTest)
	{
	}




//
//
// CMsvTestEntryCommandParser
//

EXPORT_C void CMsvTestEntryCommandParser::ProcessL()
	{
	if ((*iCurrentCommand) == KCommandTestEntryVisible)
		{
		iTestEntry.TestVisible(GetBooleanArgumentL());
		}
	else if ((*iCurrentCommand) == KCommandTestEntryComplete)
		{
		iTestEntry.TestComplete(GetBooleanArgumentL());
		}
	}

EXPORT_C CMsvTestEntryCommandParser* CMsvTestEntryCommandParser::NewL(CMsvTestEntry& aTestEntry)
	{
	CMsvTestEntryCommandParser* self = CMsvTestEntryCommandParser::NewL(aTestEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMsvTestEntryCommandParser::ConstructL()
	{
	CBaseCommandParser::ConstructL();
	AddCommandL(KCommandTestEntryVisible);
	AddCommandL(KCommandTestEntryComplete);
	}

CMsvTestEntryCommandParser::CMsvTestEntryCommandParser(CMsvTestEntry& aTestEntry) : iTestEntry(aTestEntry)
	{
	}

TBool CMsvTestEntryCommandParser::GetBooleanArgumentL()
	{
	TBool result = ETrue;

	CheckNumberOfArgumentsL(1);
	
	if ((*iArgumentList)[0] == KParameterETrue)
		{
		result = ETrue;
		}
	else if ((*iArgumentList)[0] == KParameterEFalse)
		{
		result = EFalse;
		}
	else
		{
		User::Leave(KErrArgument);
		}

	return result;
	}



//
//
// CMsvTestEntrySectionParser
//

EXPORT_C CMsvTestEntrySectionParser* CMsvTestEntrySectionParser::NewL(const TDesC& aSectionName, CTestScript& aScript, CMsvTestEntry& aTestState)
	{
	CMsvTestEntrySectionParser* self = new (ELeave) CMsvTestEntrySectionParser(aScript, aTestState);
	CleanupStack::PushL(self);
	self->ConstructL(aSectionName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CMsvTestEntrySectionParser::~CMsvTestEntrySectionParser()
	{
	}

EXPORT_C void CMsvTestEntrySectionParser::ConstructL(const TDesC& aSectionName)
	{
	CBaseSectionParser::ConstructL(aSectionName);
	AddCommandParserL(CMsvTestEntryCommandParser::NewL(iTestState));
	}

EXPORT_C CMsvTestEntrySectionParser::CMsvTestEntrySectionParser(CTestScript& aScript, CMsvTestEntry& aTestState) 
: CBaseSectionParser(aScript),
  iTestState(aTestState)
	{
	}
	
EXPORT_C void CMsvTestEntrySectionParser::LogCommentL(const TDesC& /*aComment*/)
	{
	// Log comment
	}
