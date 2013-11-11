// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ctestimapatombuilder.h"

#include <utf.h>

#include "cimapatom.h"
#include "cimapatomparser.h"
#include "cimapatomwalker.h"
#include "cimaplogger.h"

CTestImapAtomBuilder::CTestImapAtomBuilder()
	: iAtomParser(NULL)
	{}

CTestImapAtomBuilder::~CTestImapAtomBuilder()
	{
	delete iAtomParser;
	}

void CTestImapAtomBuilder::SetupL()
	{
	iAtomParser = CImapAtomParser::NewL(EFalse, KDefaultLog);
	}
	
void CTestImapAtomBuilder::TearDownL()
	{
	delete iAtomParser;
	iAtomParser = NULL;
	}

// Tests
void CTestImapAtomBuilder::TestAtomParserL()
	{
	INFO_PRINTF1(_L("TestL"));
	
	_LIT8(KLine, "sugar (one (two \"33 () three\" )) apple wibble");
	TPtrC8 ptrLine = KLine();
	TBool wantMore = iAtomParser->ProcessLineL(ptrLine);
	
	ASSERT_FALSE(wantMore);
	
	CImapAtom* root = iAtomParser->RootAtom();
	RPointerArray<CImapAtom> stack;
	stack.AppendL(root);
	
	CImapAtom* current = root;
	
	while(current!=NULL)
		{
		TPtrC8 ptrAtom = current->Atom(EFalse);
		TInt topOfStack = stack.Count() - 1;
				
		// Log the data (SendDataReq is non-leaving, so avoid leaving.  Hence no cleanup stack)
		HBufC16* unicodeAtom = NULL;
		TRAPD(err,
			unicodeAtom = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptrAtom);
		);
		
		if (err == KErrNone)
			{
			INFO_PRINTF3(_L("%d [%S]"), topOfStack, unicodeAtom);
			}
			
		delete unicodeAtom;	
		unicodeAtom=NULL;
		
		// Try going down
		CImapAtom* candidateAtom = current->Child();
		if (candidateAtom != NULL)
			{
			stack.AppendL(candidateAtom);
			++topOfStack;
			}
		// Try going along
		else
			{
			candidateAtom = current->Next();
			// Try going back-up-and-along
			while (candidateAtom == NULL && topOfStack > 0)
				{
				stack.Remove(topOfStack);
				--topOfStack;
				candidateAtom = stack[topOfStack];
				candidateAtom = candidateAtom->Next();
				}
			
			stack[topOfStack] = candidateAtom;
			}
			
		// Could use a table (or at least tables - need repeats) to parse different BODYSTRUCTURE's !!!
		current = candidateAtom;
	}
		
	stack.Reset();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapAtomBuilder::TestWalkerL()
	{
	INFO_PRINTF1(_L("TestWalkerL"));
	
//	_LIT8(KLine, "spandex (one (two \"33 () three\" )) arse wibble (apple ((pear) banana) cherry) dog");
//	TPtrC8 ptrLine = KLine();
//	TBool wantMore = iAtomParser->ProcessLineL(ptrLine);
//	ASSERT_FALSE(wantMore);

	_LIT8(KLine1, "(TEXT PLAIN (name1 value1 name2 value2) ID {25}");
	_LIT8(KLit2, "first line\r\nsecond line\r\n");
//	_LIT8(KLine3, "encoding size lines");
//	_LIT8(KLine3, "encoding size \"missing quote"); // and missing end bracket!
	_LIT8(KLine3, "encoding size lines) Stuff at the end that we're not interested in");
//	_LIT8(KLine3, "encoding size lines)");
	
	TBool wantMore = iAtomParser->ProcessLineL(KLine1);
	ASSERT_TRUE(wantMore);
	iAtomParser->ProcessLiteralBlockL(KLit2);
	wantMore = iAtomParser->ProcessLineL(KLine3);
	ASSERT_FALSE(wantMore);
	
	DumpAtomTreeL(iAtomParser->RootAtom());

	TPtrC8 unparsed = iAtomParser->UnparsedData();
	TInt len = unparsed.Length();
	}

void CTestImapAtomBuilder::DumpAtomTreeL(CImapAtom* aRoot)
	{
	CImapAtomWalker* atomWalker = CImapAtomWalker::NewL(KDefaultLog);
	CleanupStack::PushL(atomWalker);
	atomWalker->SetRootL(aRoot);
		
	TBool bFound = ETrue;
		
	while(bFound)
		{
		TPtrC8 ptrAtom = atomWalker->CurrentDes(EFalse);
						
		// Log the data (SendDataReq is non-leaving, so avoid leaving.  Hence no cleanup stack)
		HBufC16* unicodeAtom = NULL;
		TRAPD(err,
			unicodeAtom = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptrAtom);
		);
		
		if (err == KErrNone)
			{
			INFO_PRINTF3(_L("%d [%S]"), atomWalker->StackCount(), unicodeAtom);
			}
			
		delete unicodeAtom;	
		unicodeAtom=NULL;
		
		// Try going down
		if (atomWalker->PeekDown() == NULL)
			{
			// Try going across
			if (atomWalker->WalkAcrossL(EFalse) == NULL)
				{
				// Might not find something on the way up
				bFound = EFalse;
				
				// Try going up
				while (atomWalker->StackCount() > 1)
					{
					atomWalker->WalkUpL();
					CImapAtom* upAtom = atomWalker->Current();
					
					if (upAtom && atomWalker->WalkAcrossL(EFalse) != NULL)
						{
						// found something!
						bFound = ETrue;
						break;
						}
					}
				}
			}
		else
			{
			atomWalker->WalkDownL();
			}
		}
		
	CleanupStack::PopAndDestroy(atomWalker);
	
	INFO_PRINTF1(_L("Complete"));
	}
CTestSuite* CTestImapAtomBuilder::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_TEST_STEP(TestWalkerL);
	END_SUITE;
	}
