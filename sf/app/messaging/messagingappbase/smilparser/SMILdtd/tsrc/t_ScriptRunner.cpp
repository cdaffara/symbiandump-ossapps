// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Parsing and Processing of script commands, to test the
// SMIL DOM classes.
// 
//


#include "t_ScriptRunner.h"


_LIT(KLogFileName, "c:\\MsgTest\\SMIL_DOM\\SmilDomLog.txt");


// Script Commands
_LIT(KCreateDOMtree, "createdomtree");
_LIT(KDeleteDOMtree, "deletedomtree");
_LIT(KCreate,        "create");
_LIT(KDeleteChild,   "deletechild");
_LIT(KParent,        "parent");
_LIT(KRootSmil,      "rootsmil");
_LIT(KFirstChild,    "firstchild");
_LIT(KLastChild,     "lastchild");
_LIT(KNextSibling,   "nextsibling");
_LIT(KPrevSibling,   "prevsibling");
_LIT(KFirst,         "first");
_LIT(KLast,          "last");
_LIT(KNext,          "next");
_LIT(KPrev,          "prev");
_LIT(KSetAtt,        "setatt");
_LIT(KGetAtt,        "getatt");
_LIT(KUnsetAtt,      "unsetatt");
_LIT(KLegalChildren, "legalchildren");
_LIT(KSetId,         "setid");
_LIT(KGetId,         "getid");
_LIT(KRemoveId,      "removeid");


// Script Command Arguments: SMIL Elements (excluding 'smil')
_LIT(KA,           "a");
_LIT(KAnchor,      "anchor");
_LIT(KAnimation,   "animation");
_LIT(KArea,        "area");
_LIT(KAudio,       "audio");
_LIT(KBody,        "body");
_LIT(KHead,        "head");
_LIT(KImg,         "img");
_LIT(KLayout,      "layout");
_LIT(KMeta,        "meta");
_LIT(KPar,         "par");
_LIT(KRef,         "ref");
_LIT(KRegion,      "region");
_LIT(KRoot_layout, "root-layout");
_LIT(KSeq,         "seq");
_LIT(KSwitch,      "switch");
_LIT(KText,        "text");
_LIT(KTextstream,  "textstream");
_LIT(KVideo,       "video");


// Other Script Command Arguments
_LIT(KAppend,      "append");
_LIT(KInsert,      "insert");
_LIT(KReplace,     "replace");


// Errors reported via the log file
_LIT(KNotSupported,                      "Not supported.");
_LIT(KInvalidChild,                      "Invalid child.");
_LIT(KCannotFindSpecifiedChild,          "Cannot find specified child.");
_LIT(KReachedEnd_ElementInsertedAtStart, "Reached end of child list, element inserted at start of list.");
_LIT(KReachedEnd_NoChildReplaced,        "Reached end of child list, no child has been replaced.");
_LIT(KReachedEnd_NoChildRemoved,         "Reached end of child list, no child has been removed.");
_LIT(KAlreadyAtEndOfList,                "Already at end of list.");
_LIT(KInvalidArgument,                   "Invalid argument.");
_LIT(KAttributeNotSet,                   "Attribute not set.");
_LIT(KElementHasLegalChildren,           "Element has legal children.");
_LIT(KElementHasIllegalChildren,         "Element has illegal children.");
_LIT(KUnrecognisedCommand,               "Unrecognised command.");
_LIT(KUnrecognisedArgument,              "Unrecognised argument.");
_LIT(KUnrecognisedElementType,           "Unrecognised element type.");
_LIT(KNotABasicElt,                      "Error: Current element is not a BasicElt.");
_LIT(KNotOfSpecifiedType,                "Error: Current element is not of the specified type.");

//
//
// CScriptRunner
//

CScriptRunner* CScriptRunner::NewL()
	{
	CScriptRunner* self = new (ELeave) CScriptRunner();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CScriptRunner::~CScriptRunner()
	{
	delete iArgumentList;
	delete iCurrentCommand;
	delete iWriter;
	delete iDOMtreeRoot; // This will delete the entire DOM tree
	}


CScriptRunner::CScriptRunner()
: iCurrentCommand(NULL), iCurrentElement(NULL), iDOMtreeRoot(NULL)
	{
	}


void CScriptRunner::ConstructL()
	{
	iArgumentList = new (ELeave) CDesCArrayFlat(3);
	iWriter = CTestDataWriter::NewL(KLogFileName);
	}


// Extract the command, and any arguments, from the given string
void CScriptRunner::ParseL(const TDesC& aCommand)
	{
	// First, echo the command line to the log file
	iWriter->WriteTest(aCommand);

	iArgumentList->Reset();
	delete iCurrentCommand;
	iCurrentCommand = NULL;

	// Determine whether aCommand is a command, or a comment
	if (aCommand.Length() >= 2  &&  aCommand[0] == '/'  &&  aCommand[1] == '/')
		{
		// This is a comment
		iCurrentCommand = NULL;
		}
	else // Extract the command, and any arguments
		{
		TInt commandIter = 0;
		TInt commandStart = 0;
		TBool inQuotes = EFalse;
		while (commandIter <= aCommand.Length())
			{
			TBool commandFound = EFalse;

			if (commandIter < aCommand.Length())
				{
				if (aCommand[commandIter] == '"')
					{
					// We assume that quotes only occur at the beginning and end
					// of arguments. Hence, if we've just found a new quote, we should
					// not make it part of the argument.
					if (!inQuotes)
						{
						inQuotes = ETrue;
						commandStart++;
						}
					else
						{
						// Assume we've got to the end of the argument, and skip the
						// following space.
						commandFound = ETrue;
						commandIter++;
						}
					}
				else if ((aCommand[commandIter] == ' ') && (!inQuotes))
					{
					commandFound = ETrue;
					}
				}
			else
				{
				commandFound = ETrue;
				}

			if (commandFound)
				{
				if (commandIter > commandStart)
					{
					// Command or argument found
					TInt commandLength = commandIter - commandStart;

					// If the argument ended in quotes, modify the length to exclude them
					if (inQuotes)
						{
						inQuotes = EFalse;
						commandLength--;
						}

					if (iCurrentCommand == NULL)
						{
						// This is a command
						iCurrentCommand = HBufC::NewL(commandLength);
						(*iCurrentCommand) = aCommand.Mid(commandStart, commandLength);
						}
					else
						{
						// This is a command argument
						iArgumentList->AppendL(aCommand.Mid(commandStart, commandLength));
						}

					// Set the start of the next argument
					commandStart = commandIter + 1;
					}
				}

			commandIter++;
			}
		}

	if (iCurrentCommand != NULL)
		{
		// Process the command
		ProcessL();
		}
	}


// Process the script command
void CScriptRunner::ProcessL()
	{
	if (iCurrentCommand->Compare(KCreateDOMtree) == 0)
		{
		// Create a new DOM tree (document + smil) & make the smil the current element.
		// Command: createdomtree

		iDOMtreeRoot = CMDXMLSMILDocument::NewL();
		CMDXMLSMILsmil* smilElement = CMDXMLSMILsmil::NewL(iDOMtreeRoot);

		// The 'document' takes ownership of the 'smil' element
		CMDXMLElement* documentElement = iDOMtreeRoot->DocumentElement();
		documentElement->AppendChild(smilElement);
		iCurrentElement = smilElement;
		}
	else if (iCurrentCommand->Compare(KDeleteDOMtree) == 0)
		{
		// Delete the DOM tree
		// Command: deletedomtree

		delete iDOMtreeRoot; // This will delete the entire DOM tree
		iDOMtreeRoot = NULL;
		iCurrentElement = NULL;
		}
	else if (iCurrentCommand->Compare(KCreate) == 0)
		{
		CMDXMLElement* newElement = NULL;

		// Determine element type to create
		if ((*iArgumentList)[0].Compare(KA) == 0)
			{
			newElement = CMDXMLSMILa::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KAnchor) == 0)
			{
			newElement = CMDXMLSMILanchor::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KAnimation) == 0)
			{
			newElement = CMDXMLSMILanimation::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KArea) == 0)
			{
			newElement = CMDXMLSMILarea::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KAudio) == 0)
			{
			newElement = CMDXMLSMILaudio::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KBody) == 0)
			{
			newElement = CMDXMLSMILbody::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KHead) == 0)
			{
			newElement = CMDXMLSMILhead::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KImg) == 0)
			{
			newElement = CMDXMLSMILimg::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KLayout) == 0)
			{
			newElement = CMDXMLSMILlayout::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KMeta) == 0)
			{
			newElement = CMDXMLSMILmeta::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KPar) == 0)
			{
			newElement = CMDXMLSMILpar::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KRef) == 0)
			{
			newElement = CMDXMLSMILref::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KRegion) == 0)
			{
			newElement = CMDXMLSMILregion::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KRoot_layout) == 0)
			{
			newElement = CMDXMLSMILroot_layout::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KSeq) == 0)
			{
			newElement = CMDXMLSMILseq::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KSwitch) == 0)
			{
			newElement = CMDXMLSMILswitch::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KText) == 0)
			{
			newElement = CMDXMLSMILtext::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KTextstream) == 0)
			{
			newElement = CMDXMLSMILtextstream::NewL(iDOMtreeRoot);
			}
		else if ((*iArgumentList)[0].Compare(KVideo) == 0)
			{
			newElement = CMDXMLSMILvideo::NewL(iDOMtreeRoot);
			}
		else  // Unrecognised element type
			{
			iWriter->WriteTest(KUnrecognisedElementType);
			}


		if ((*iArgumentList)[1].Compare(KAppend) == 0)
			{
			// Create an element of a specified type, and add it to the end of the current
			// element’s child list.
			// Command: create <element type> append

			TInt err = iCurrentElement->AppendChild(newElement);
			if (err != KErrNone)
				{
				delete newElement; // Ownership was not taken
				if (err == KErrNotSupported)
					{
					iWriter->WriteTest(KNotSupported);
					}
				else if (err == KErrXMLInvalidChild)
					{
					iWriter->WriteTest(KInvalidChild);
					}
				}
			}
		else if ((*iArgumentList)[1].Compare(KInsert) == 0)
			{
			// Create an element of a specified type, and insert it at a numerically defined
			// position in the current element’s child list.
			// Command: create <element type> insert n
			// Use n = -3 for start of list (specified with NULL parameter),
			// and no element to insert.
			// Use n = -2 for start of list (specified with pointer to first child),
			// and no element to insert.
			// Use n = -1 for invalid pointer specifying place to insert.
			// Use n = 0 for start of list (specified with NULL parameter)
			// Use n = 1 for start of list (specified with pointer to first child)
			// Then n = 2 for second child etc.

			// Convert the final argument to an integer
			TLex lex = (*iArgumentList)[2];
			TInt position;
			lex.Val(position);

			CMDXMLNode* insertBeforeChild = NULL;

			if (position == -3)
				{
				insertBeforeChild = NULL; // One way of specifying start of list
				delete newElement;
				newElement = NULL;
				}
			else if (position == -2)
				{
				insertBeforeChild = iCurrentElement->FirstChild();
				delete newElement;
				newElement = NULL;
				}
			else if (position == -1)
				{
				insertBeforeChild = iCurrentElement; // Invalid pointer - not a child
				}
			else if (position == 0)
				{
				insertBeforeChild = NULL; // One way of specifying start of list
				}
			else if (position > 0)
				{
				insertBeforeChild = iCurrentElement->FirstChild();

				while (position-- > 1  &&  insertBeforeChild != NULL)
					{
					insertBeforeChild = insertBeforeChild->NextSibling();
					}

				if (insertBeforeChild == NULL)
					{
					iWriter->WriteTest(KReachedEnd_ElementInsertedAtStart);
					}
				}

			TInt err = iCurrentElement->InsertBefore(insertBeforeChild, newElement);
			if (err != KErrNone)
				{
				delete newElement; // Ownership was not taken
				if (err == KErrNotSupported)
					{
					iWriter->WriteTest(KNotSupported);
					}
				else if (err == KErrXMLInvalidChild)
					{
					iWriter->WriteTest(KInvalidChild);
					}
				else if (err == KErrNotFound)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				}
			}
		else if ((*iArgumentList)[1].Compare(KReplace) == 0)
			{
			// Create an element of a specified type and use it to replace a numerically defined
			// child element in the current element’s child list. Delete the old child.
			// Command: create <element type> replace n
			// Use n = -2 for start of list, and no element to insert.
			// Use n = -1 for invalid pointer specifying child to replace.
			// Use n = 0 for NULL, in place of pointer to child to replace.
			// Use n = 1 for start of list
			// Then n = 2 for second child etc.

			// Convert the final argument to an integer
			TLex lex = (*iArgumentList)[2];
			TInt position;
			lex.Val(position);

			CMDXMLNode* childToReplace = NULL;

			if (position == -2)
				{
				childToReplace = iCurrentElement->FirstChild();
				delete newElement;
				newElement = NULL;
				}
			else if (position == -1)
				{
				childToReplace = iCurrentElement; // Invalid pointer - not a child
				}
			else if (position == 0)
				{
				childToReplace = NULL; // Invalid
				}
			else if (position > 0)
				{
				childToReplace = iCurrentElement->FirstChild();

				while (position-- > 1  &&  childToReplace != NULL)
					{
					childToReplace = childToReplace->NextSibling();
					}

				if (childToReplace == NULL)
					{
					iWriter->WriteTest(KReachedEnd_NoChildReplaced);
					}
				}

			TInt err = iCurrentElement->ReplaceChild(newElement, childToReplace);
			if (err != KErrNone)
				{
				delete newElement; // Ownership was not taken
				if (err == KErrNotSupported)
					{
					iWriter->WriteTest(KNotSupported);
					}
				else if (err == KErrXMLInvalidChild)
					{
					iWriter->WriteTest(KInvalidChild);
					}
				else if (err == KErrNotFound)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				}
			else // Replace operation was successful
				{
				delete childToReplace;
				}
			}
		else  // Unrecognised argument
			{
			iWriter->WriteTest(KUnrecognisedArgument);
			}
		}
	else if (iCurrentCommand->Compare(KDeleteChild) == 0)
		{
		// Remove a child element, defined by a numeric position, in the current element’s
		// list of children. Then delete it.
		// Command: deletechild n
		// Use n = -1 for invalid pointer specifying child to remove.
		// Use n = 0 for NULL, in place of pointer to child to remove.
		// Use n = 1 for start of list
		// Then n = 2 for second child etc.

		// Convert the argument to an integer
		TLex lex = (*iArgumentList)[0];
		TInt position;
		lex.Val(position);

		CMDXMLNode* childToRemove = NULL;

		if (position == -1)
			{
			childToRemove = iCurrentElement; // Invalid pointer - not a child
			}
		else if (position == 0)
			{
			childToRemove = NULL; // Invalid
			}
		else if (position > 0)
			{
			childToRemove = iCurrentElement->FirstChild();

			while (position-- > 1  &&  childToRemove != NULL)
				{
				childToRemove = childToRemove->NextSibling();
				}

			if (childToRemove == NULL)
				{
				iWriter->WriteTest(KReachedEnd_NoChildRemoved);
				}
			}

		TInt err = iCurrentElement->RemoveChild(childToRemove);
		if (err != KErrNone)
			{
			if (err == KErrNotSupported)
				{
				iWriter->WriteTest(KNotSupported);
				}
			else if (err == KErrXMLInvalidChild)
				{
				iWriter->WriteTest(KInvalidChild);
				}
			else if (err == KErrNotFound)
				{
				iWriter->WriteTest(KCannotFindSpecifiedChild);
				}
			}
		else // Remove operation was successful
			{
			delete childToRemove;
			}
		}
	else if (iCurrentCommand->Compare(KParent) == 0)
		{
		// Navigate up the object tree to the current element’s parent.
		// Command: parent

		CMDXMLNode* parentNode = iCurrentElement->ParentNode();

		if (parentNode == NULL || parentNode->NodeName() == KXMLDocumentElementNodeName)
			{
			iWriter->WriteTest(KAlreadyAtEndOfList);
			}
		else
			{
			iCurrentElement = (CMDXMLElement*)parentNode;
			}
		}
	else if (iCurrentCommand->Compare(KRootSmil) == 0)
		{
		// Navigate to the element at the root of the DOM tree (smil).
		// Command: rootsmil

		iCurrentElement = (CMDXMLElement*)iDOMtreeRoot->DocumentElement()->FirstChild();
		}
	else if (iCurrentCommand->Compare(KFirstChild) == 0)
		{
		// Navigate to the first child of the current element.
		// Command: firstchild

		CMDXMLNode* firstChild = iCurrentElement->FirstChild();

		if (firstChild == NULL)
			{
			iWriter->WriteTest(KCannotFindSpecifiedChild);
			}
		else
			{
			iCurrentElement = (CMDXMLElement*)firstChild;
			}
		}
	else if (iCurrentCommand->Compare(KLastChild) == 0)
		{
		// Navigate to the last child of the current element.
		// Command: lastchild

		CMDXMLNode* lastChild = iCurrentElement->LastChild();

		if (lastChild == NULL)
			{
			iWriter->WriteTest(KCannotFindSpecifiedChild);
			}
		else
			{
			iCurrentElement = (CMDXMLElement*)lastChild;
			}
		}
	else if (iCurrentCommand->Compare(KNextSibling) == 0)
		{
		// Navigate to the next sibling of the current element.
		// Command: nextsibling

		CMDXMLNode* nextSibling = iCurrentElement->NextSibling();

		if (nextSibling == NULL)
			{
			iWriter->WriteTest(KAlreadyAtEndOfList);
			}
		else
			{
			iCurrentElement = (CMDXMLElement*)nextSibling;
			}
		}
	else if (iCurrentCommand->Compare(KPrevSibling) == 0)
		{
		// Navigate to the previous sibling of the current element.
		// Command: prevsibling

		CMDXMLNode* prevSibling = iCurrentElement->PreviousSibling();

		if (prevSibling == NULL)
			{
			iWriter->WriteTest(KAlreadyAtEndOfList);
			}
		else
			{
			iCurrentElement = (CMDXMLElement*)prevSibling;
			}
		}
	else if (iCurrentCommand->Compare(KFirst) == 0)
		{
		// Navigate to the first child, of specified element type, belonging to the current
		// element.
		// Command: first <element type>

		// If current element is a BasicElt, cast the pointer, so that we can call BasicElt
		// members.
		if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
		&&  iCurrentElement->NodeName() != KXMLSMILEltmeta)
			{
			CMDXMLSMILBasicElt* basicElt = (CMDXMLSMILBasicElt*)iCurrentElement;

			if ((*iArgumentList)[0].Compare(KAnimation) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirstanimationChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KAudio) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirstaudioChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KImg) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirstimgChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KPar) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirstparChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KRef) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirstrefChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KRegion) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirstregionChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KSeq) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirstseqChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KText) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirsttextChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KTextstream) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirsttextstreamChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KVideo) == 0)
				{
				CMDXMLElement* firstChild = basicElt->FirstvideoChild();

				if (firstChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = firstChild;
					}
				}
			else  // Invalid element type
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
		else // Error: Current element is not a BasicElt
			{
			iWriter->WriteTest(KNotABasicElt);
			}
		}
	else if (iCurrentCommand->Compare(KLast) == 0)
		{
		// Navigate to the last child, of specified element type, belonging to the current
		// element.
		// Command: last <element type>

		// If current element is a BasicElt, cast the pointer, so that we can call BasicElt
		// members.
		if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
		&&  iCurrentElement->NodeName() != KXMLSMILEltmeta)
			{
			CMDXMLSMILBasicElt* basicElt = (CMDXMLSMILBasicElt*)iCurrentElement;

			if ((*iArgumentList)[0].Compare(KAnimation) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LastanimationChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KAudio) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LastaudioChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KImg) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LastimgChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KPar) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LastparChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KRef) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LastrefChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KRegion) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LastregionChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KSeq) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LastseqChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KText) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LasttextChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KTextstream) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LasttextstreamChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else if ((*iArgumentList)[0].Compare(KVideo) == 0)
				{
				CMDXMLElement* lastChild = basicElt->LastvideoChild();

				if (lastChild == NULL)
					{
					iWriter->WriteTest(KCannotFindSpecifiedChild);
					}
				else
					{
					iCurrentElement = lastChild;
					}
				}
			else  // Invalid element type
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
		else // Error: Current element is not a BasicElt
			{
			iWriter->WriteTest(KNotABasicElt);
			}
		}
	else if (iCurrentCommand->Compare(KNext) == 0)
		{
		// Navigate to the next sibling, of specified element type, from a current
		// element of the same type.
		// Command: next <element type>

		if ((*iArgumentList)[0].Compare(KAnimation) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltanimation)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILanimation* eltOfSpecificType = (CMDXMLSMILanimation*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NextanimationSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KAudio) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltaudio)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILaudio* eltOfSpecificType = (CMDXMLSMILaudio*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NextaudioSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KImg) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltimg)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILimg* eltOfSpecificType = (CMDXMLSMILimg*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NextimgSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KPar) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltpar)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILpar* eltOfSpecificType = (CMDXMLSMILpar*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NextparSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KRef) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltref)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILref* eltOfSpecificType = (CMDXMLSMILref*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NextrefSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KRegion) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltregion)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILregion* eltOfSpecificType = (CMDXMLSMILregion*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NextregionSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KSeq) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltseq)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILseq* eltOfSpecificType = (CMDXMLSMILseq*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NextseqSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KText) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILElttext)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILtext* eltOfSpecificType = (CMDXMLSMILtext*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NexttextSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KTextstream) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILElttextstream)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILtextstream* eltOfSpecificType = (CMDXMLSMILtextstream*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NexttextstreamSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KVideo) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltvideo)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILvideo* eltOfSpecificType = (CMDXMLSMILvideo*)iCurrentElement;

				CMDXMLElement* nextSibling = eltOfSpecificType->NextvideoSibling();

				if (nextSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = nextSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else  // Invalid element type
			{
			iWriter->WriteTest(KInvalidArgument);
			}
		}
	else if (iCurrentCommand->Compare(KPrev) == 0)
		{
		// Navigate to the previous sibling, of specified element type, from a current
		// element of the same type.
		// Command: prev <element type>

		if ((*iArgumentList)[0].Compare(KAnimation) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltanimation)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILanimation* eltOfSpecificType = (CMDXMLSMILanimation*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PreviousanimationSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KAudio) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltaudio)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILaudio* eltOfSpecificType = (CMDXMLSMILaudio*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PreviousaudioSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KImg) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltimg)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILimg* eltOfSpecificType = (CMDXMLSMILimg*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PreviousimgSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KPar) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltpar)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILpar* eltOfSpecificType = (CMDXMLSMILpar*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PreviousparSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KRef) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltref)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILref* eltOfSpecificType = (CMDXMLSMILref*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PreviousrefSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KRegion) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltregion)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILregion* eltOfSpecificType = (CMDXMLSMILregion*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PreviousregionSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KSeq) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltseq)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILseq* eltOfSpecificType = (CMDXMLSMILseq*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PreviousseqSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KText) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILElttext)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILtext* eltOfSpecificType = (CMDXMLSMILtext*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PrevioustextSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KTextstream) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILElttextstream)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILtextstream* eltOfSpecificType = (CMDXMLSMILtextstream*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PrevioustextstreamSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else if ((*iArgumentList)[0].Compare(KVideo) == 0)
			{
			// If current element is of the specified type...
			if (iCurrentElement->NodeType() == CMDXMLNode::EElementNode
			&&  iCurrentElement->NodeName() == KXMLSMILEltvideo)
				{
				// ...cast the pointer, so that we can call the members of that type.
				CMDXMLSMILvideo* eltOfSpecificType = (CMDXMLSMILvideo*)iCurrentElement;

				CMDXMLElement* prevSibling = eltOfSpecificType->PreviousvideoSibling();

				if (prevSibling == NULL)
					{
					iWriter->WriteTest(KAlreadyAtEndOfList);
					}
				else
					{
					iCurrentElement = prevSibling;
					}
				}
			else // Error: Current element is not of the specified type
				{
				iWriter->WriteTest(KNotOfSpecifiedType);
				}
			}
		else  // Invalid element type
			{
			iWriter->WriteTest(KInvalidArgument);
			}
		}
	else if (iCurrentCommand->Compare(KSetAtt) == 0)
		{
		// Set an attribute value of the current element (referring to the attribute by name).
		// Command: setatt <attribute name> "<attribute value>"

		TInt err = iCurrentElement->SetAttributeL((*iArgumentList)[0],  // aAttributeName
												  (*iArgumentList)[1]); // aAttributeValue
		if (err != KErrNone)
			{
			if (err == KErrNotSupported)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
		}
	else if (iCurrentCommand->Compare(KGetAtt) == 0)
		{
		// Get an attribute value of the current element if it is set (referring to the attribute
		// by name).
		// Command: getatt <attribute name>

		TPtrC attributeValue; // Used to point to an existing HBufC, which could be any length.
							  // We don't want to copy it into a buffer.

		TInt err = iCurrentElement->GetAttribute((*iArgumentList)[0],  // aAttributeName
												 attributeValue);
		if (err != KErrNone)
			{
			if (err == KErrNotSupported)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			else if (err == KErrNotFound)
				{
				iWriter->WriteTest(KAttributeNotSet);
				}
			}
		else  // successful get
			{
			iWriter->WriteTest(attributeValue);
			}
		}
	else if (iCurrentCommand->Compare(KUnsetAtt) == 0)
		{
		// Unset an attribute for the current element (referring to the attribute by name).
		// Command: unsetatt <attribute name>

		TInt err = iCurrentElement->RemoveAttribute((*iArgumentList)[0]); // aAttributeName

		if (err != KErrNone)
			{
			if (err == KErrNotSupported)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
		}
	else if (iCurrentCommand->Compare(KLegalChildren) == 0)
		{
		// Check the child elements of the current element.
		// Command: legalchildren

		if (iCurrentElement->CheckChildren())
			{
			iWriter->WriteTest(KElementHasLegalChildren);
			}
		else
			{
			iWriter->WriteTest(KElementHasIllegalChildren);
			}
		}
	else if (iCurrentCommand->Compare(KSetId) == 0)
		{
		IdAttTestL( KIdSetOp );
		}
	else if (iCurrentCommand->Compare(KGetId) == 0)
		{
		IdAttTestL( KIdGetOp );
		}
	else if (iCurrentCommand->Compare(KRemoveId) == 0)
		{
		IdAttTestL( KIdRemoveOp );
		}
	else  // Unrecognised command
		{
		iWriter->WriteTest(KUnrecognisedCommand);
		}

	iArgumentList->Reset();
	delete iCurrentCommand;
	iCurrentCommand = NULL;
	}

void CScriptRunner::IdAttTestL( TIdOp aOp )
	{
	// We can set the Id by means of SetAttribute but we have a separate test that attempts
	// to use the direct member functioin to test that.  Id is only one of a range of attributes
	// but it has been selected as a sample (because it is almost universal)
	TInt err = KErrNone;

	switch (iCurrentElement->ElementType())
		{
		case (KSMILElementa):
			{
			CMDXMLSMILa *ptr = CMDXMLSMILa::CastToSMILa(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementanchor):
			{
			CMDXMLSMILanchor *ptr = CMDXMLSMILanchor::CastToSMILanchor(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementanimation):
			{
			CMDXMLSMILanimation *ptr = CMDXMLSMILanimation::CastToSMILanimation(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementarea):
			{
			CMDXMLSMILarea *ptr = CMDXMLSMILarea::CastToSMILarea(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementaudio):
			{
			CMDXMLSMILaudio *ptr = CMDXMLSMILaudio::CastToSMILaudio(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementbody):
			{
			CMDXMLSMILbody *ptr = CMDXMLSMILbody::CastToSMILbody(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementhead):
			{
			CMDXMLSMILhead *ptr = CMDXMLSMILhead::CastToSMILhead(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementimg):
			{
			CMDXMLSMILimg *ptr = CMDXMLSMILimg::CastToSMILimg(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementlayout):
			{
			CMDXMLSMILlayout *ptr = CMDXMLSMILlayout::CastToSMILlayout(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementpar):
			{
			CMDXMLSMILpar *ptr = CMDXMLSMILpar::CastToSMILpar(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementref):
			{
			CMDXMLSMILref *ptr = CMDXMLSMILref::CastToSMILref(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementregion):
			{
			CMDXMLSMILregion *ptr = CMDXMLSMILregion::CastToSMILregion(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementroot_layout):
			{
			CMDXMLSMILroot_layout *ptr = CMDXMLSMILroot_layout::CastToSMILroot_layout(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementseq):
			{
			CMDXMLSMILseq *ptr = CMDXMLSMILseq::CastToSMILseq(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementsmil):
			{
			CMDXMLSMILsmil *ptr = CMDXMLSMILsmil::CastToSMILsmil(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementswitch):
			{
			CMDXMLSMILswitch *ptr = CMDXMLSMILswitch::CastToSMILswitch(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementtext):
			{
			CMDXMLSMILtext *ptr = CMDXMLSMILtext::CastToSMILtext(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementtextstream):
			{
			CMDXMLSMILtextstream *ptr = CMDXMLSMILtextstream::CastToSMILtextstream(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		case (KSMILElementvideo):
			{
			CMDXMLSMILvideo *ptr = CMDXMLSMILvideo::CastToSMILvideo(iCurrentElement);
			switch(aOp)
				{
				case KIdSetOp:
					err = ptr->SetAttIdL((*iArgumentList)[0]);
					break;
				case KIdGetOp:
					if( ptr->IsAttIdSpecified() )
						{
						TPtrC attributeValue = ptr->AttId();
						iWriter->WriteTest(attributeValue);
						}
					else
						{
						iWriter->WriteTest(KAttributeNotSet);
						}
					break;
				case KIdRemoveOp:
					ptr->RemoveAttId();
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (err != KErrNone)
				{
				iWriter->WriteTest(KInvalidArgument);
				}
			}
			break;
		
		default:
			iWriter->WriteTest(KUnrecognisedElementType);
			break;
		} //end switch
	}
