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
// CSMILDTD.cpp
// @file
// This file contains the definition of the SMILDTD class
// which describes the SMIL DTD and is responsible for validation
// of SMIL documents
// 
//

#include "SmilDtd.h"
#include "SmilData.h"
#include <stringpool.h>
#include "smilelements.h"
#include "smilattributes.h"
#include "smilgenericelements.h"
#include <gmxmlconstants.h>

//
// Global functions					//
//


EXPORT_C CSMILDtd* CSMILDtd::NewL()
//
// Two phase static factory function constructor
// @return Created CSMILDtd
// @leave can Leave due to OOM
//
	{
	CSMILDtd* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CSMILDtd* CSMILDtd::NewLC()
//
// Two phase static factory function constructor
// @return Created CSMILDtd
// @leave can Leave due to OOM
//
	{
	CSMILDtd* self = new (ELeave) CSMILDtd();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSMILDtd::ConstructL()
//
// Second stage constructor
// @leave can Leave due to OOM
//
	{
	// nothing to do
	}


CSMILDtd::CSMILDtd()
	{
	}


EXPORT_C CSMILDtd::~CSMILDtd()
	{
	}

TBool CSMILDtd::IsValidElementL(const TDesC& aElement) const
	{
//
// Checks to see if the element name passed in has been specified in the string table
// @param aElementName the element name to be checked
// @return ETrue if defined in the string table else EFalse
// @leave Leave due to OOM
//

	// Use the string table SMILElements to validate the element name
	TBool validElement = EFalse;

	RStringPool pool;
	CleanupClosePushL(pool);
	pool.OpenL(SMILElements::Table);

	int numTableEntries = SMILElements::Table.iCount;
	if(numTableEntries != 0)
		{
	    // Using a binary search since the table is always sorted alphabetically by element.

		// Set us search indices to outer bounds of array
		TInt left = 0;
		TInt right = numTableEntries - 1;
		TInt compareResult = 0;
		TInt pos;
		while (right >= left)
			{
			pos = (left + right) / 2;

			RStringF tableEntry = pool.StringF(pos, SMILElements::Table);
			HBufC* buf = HBufC::NewLC(tableEntry.DesC().Length());
			buf->Des().Copy(tableEntry.DesC());

			TLex string(*buf);

			TPtrC token = string.NextToken();
			compareResult = aElement.Compare(token);
			if(compareResult == 0)
				validElement = ETrue;

			CleanupStack::PopAndDestroy(buf);


			if(compareResult == 0)
				break;
			else if (compareResult > 0)
				left = pos + 1;
			else
				right = pos - 1;
  			}

		}
	CleanupStack::PopAndDestroy();  // close pool
	return validElement;

	}

TInt CSMILDtd::IsValidAttributeForElementL(const TDesC& aElement, const TDesC& aAttribute, const TDesC& aAttributeValue) const
	{
//
// Checks that both the Attribute Name & Attribute Value are valid using string tables stored in the document
// @param aAttributeName - name of attribute to be checked
// @param aAttributeValue - value of attribute to be checked
// @return KErrNone if attribute & value are valid, otherwise KErrXMLBadAttributeName or KErrXMLBadAttributeValue
// @leave Leave due to OOM
//


	// To reduce the size of the element/attribute/value generic element names are used
	// So for example 'img' and 'video' are both represented by the genericelement 'media'
	// A table exists (SMILGenericElements) detailing these relationships

	// So first we see if there is a generic element name for this element
	// If not then genericElementName will just be the element name passed in
	HBufC* genericElementName = NULL;


	RStringPool pool;
	CleanupClosePushL(pool);
	pool.OpenL(SMILGenericElements::Table);

	TInt numTableEntries = SMILGenericElements::Table.iCount;
	if(numTableEntries != 0)
		{
		// Using a binary search since the table is always sorted alphabetically by element.

		// Set us search indices to outer bounds of array
		TInt left = 0;
		TInt right = numTableEntries - 1;
		TInt pos;
		while (right >= left)
			{
			pos = (left + right) / 2;

			RStringF elementAndGeneric = pool.StringF(pos, SMILGenericElements::Table);

			HBufC* buf = HBufC::NewLC(elementAndGeneric.DesC().Length());
			buf->Des().Copy(elementAndGeneric.DesC());
			TLex string(*buf);

			TPtrC token = string.NextToken();
			TInt compare = aElement.Compare(token);

			if(compare == 0)
				{
				// We've got a match so use the generic name
				// This isn't pushed on the CleanupStack here as we need to be able to pop
				// buf and pool whilst genericElementName is still in scope.  It will be
				// pushed once these aren't necessary.  Because of this THERE MUST BE
				// NOTHING THAT LEAVES UNTIL genericElementName IS PUT ONTO THE CLEANUPSTACK
				genericElementName =(string.NextToken()).AllocL();
				}

			CleanupStack::PopAndDestroy(buf); //buf


			if(compare == 0)
				break;
			else if (compare > 0)
				left = pos + 1;
			else
				right = pos - 1;
			}
		}

	CleanupStack::PopAndDestroy();	// Close pool

	if(genericElementName != NULL)
		{
		CleanupStack::PushL(genericElementName);
		}
	else 
		{
		// We didn't find a generic name so use the element name passed in
		genericElementName = HBufC::NewLC(aElement.Length());
		genericElementName->Des().Copy(aElement);
		}

	// Using the generic element name test to see if we have a valid attribute and value

	// assume the attribute name is invalid
	TInt error = KErrXMLBadAttributeName;

	// retrieve the attributeValue string table
	// this is of the form elementName attributeName attribValue1 attribValue2 ...

	CleanupClosePushL(pool);
	pool.OpenL(SMILAttributes::Table);

	numTableEntries = SMILAttributes::Table.iCount;
	if(numTableEntries != 0)
		{
	    // Using a binary search since the table is always sorted alphabetically by element+attribute.

		// Set us search indices to outer bounds of array
		TInt left = 0;
		TInt right = numTableEntries - 1;
		TInt compareResult = 0;
		TInt pos;
		while (right >= left)
			{
			pos = (left + right) / 2;

			RStringF tableEntry = pool.StringF(pos, SMILAttributes::Table);
			HBufC* buf = HBufC::NewLC(tableEntry.DesC().Length());
			buf->Des().Copy(tableEntry.DesC());

			TLex string(*buf);

			// Get the element name from the string table (the first token)
			TPtrC token = string.NextToken();
			// Is this the element name we are interested in
			compareResult = genericElementName->Compare(token);
			if(compareResult == 0)
				{
				// we're looking at an entry in the string table for this element
				// so test to see if it's the correct attribute too.
				token.Set(string.NextToken());
				compareResult =  aAttribute.Compare(token);
				if(compareResult == 0)
					{
					// we've got the correct entry in the table (both element & attribute match)
					// so now assume the error is an incorrect attribute value
					error = KErrXMLBadAttributeValue;

					// get hold of the first valid attribure value
					token.Set(string.NextToken());
					// if we don't have a list of attribute values then we can assume whatever we've got is valid
					// so set the error to KErrNone
					if (token.Length() == 0)
						error = KErrNone;
					else
						{
						// Cycle through all listed attribute values to see if we have a valid one
						while (token.Length() != 0)
							{
							if (aAttributeValue.Compare(token) == 0)
								{
								// value of attribute is valid
								error = KErrNone;
								break;
								}
							else
								token.Set(string.NextToken());

							}
						}

					}

				}
			CleanupStack::PopAndDestroy(buf);	// buf
			if (compareResult == 0)         // Matching item found
				break;
			else if (compareResult > 0)
				left = pos + 1;
			else
				right = pos - 1;
  			}

		}
	CleanupStack::PopAndDestroy(2);  // pool, genericElementName

	return error;
	}

TBool CSMILDtd::AreValidChildElementsL(const TDesC& aParentElement, const CDesCArray& aChildElements) const
// Function to determine whether the parent/child relationship is valid in DTD
// @return ETrue if parent/child relationship is valid
// @param aParentElement the name of the parent element to be tested
// @param aChildElements an array of child element name to be tested
// @leave leave due to OOM
//
	{
	TBool retVal = EFalse;



	if(aParentElement == KSMILDTDElta)
		{
		retVal = CheckValidChildren(SMILDTDAChildStates,KSMILDTDAChildStateTranCount, aChildElements);
		}

	else if( aParentElement == KSMILDTDEltDoc)
		{
		retVal = ETrue;
		}


	else if(aParentElement == KSMILDTDEltanimation || aParentElement == KSMILDTDEltaudio || aParentElement == KSMILDTDEltimg || aParentElement == KSMILDTDEltref
		    || aParentElement == KSMILDTDElttext || aParentElement == KSMILDTDElttextstream || aParentElement == KSMILDTDEltvideo)
		{
		retVal = CheckValidChildren(SMILDTDMediaChildStates, KSMILDTDMediaChildStateTranCount, aChildElements);
		}

	else if(aParentElement == KSMILDTDEltbody)
		{
		retVal = CheckValidChildren(SMILDTDBodyChildStates, KSMILDTDBodyChildStateTranCount, aChildElements);
		}

	else if(aParentElement == KSMILDTDElthead)
		{
		retVal = CheckValidChildren(SMILDTDHeadChildStates, KSMILDTDHeadChildStateTranCount, aChildElements);
		}

	else if(aParentElement == KSMILDTDEltlayout)
		{
		retVal = CheckValidChildren(SMILDTDLayoutChildStates, KSMILDTDLayoutChildStateTranCount, aChildElements);
		}

	else if(aParentElement == KSMILDTDEltpar || aParentElement == KSMILDTDEltseq)
		{
		retVal = CheckValidChildren(SMILDTDTimingChildStates, KSMILDTDTimingChildStateTranCount, aChildElements);
		}

	else if(aParentElement == KSMILDTDEltsmil)
		{
		retVal = CheckValidChildren(SMILDTDSmilChildStates, KSMILDTDSmilChildStateTranCount, aChildElements);
		}

	else if(aParentElement == KSMILDTDEltswitch)
		{
		retVal = CheckValidChildren(SMILDTDSwitchChildStates, KSMILDTDSwitchChildStateTranCount, aChildElements);
		}

	return retVal;
	}


TBool CSMILDtd::CheckValidChildren(const TSMILDTDChildStateType aStateTrans[],TInt aStateCount, const CDesCArray& aChildElements) const
//
// Checks child element ownership based on a Finite State Machine
// @param aFirstChild - pointer to first child element
// @param aStateTrans - Array of state transition elements.
//        The elements must be ordered by tag name first and then starting state
//        as this routine uses the ordering to drive an efficient search.
// @param aStateCount - the number of state transitions in the array
// @return true if the list of children matches the defined state machine
//
	{
	// This routine works by considering the allowed set of child elements as a Finite State
	// Machine.  When tracing through the list of children, each child encountered causes
	// a state transition.  The actual states are 'between' elements.  The states are
	// simply referred to by numbers, 0 is the starting state, the legal final state is
	// state -1, other states are positive integers (the actual values have no significance,
	// only the transitions and the start and end are of importance.
	// When the list of children ends, a special 'empty tag' element is considered to be
	// found.  If this empty tag element causes a transition to the final state then the list
	// has been successfully traversed.
	// If, at any point, a child element is encountered which does not lead to a valid
	// transition from the current state then the list is invalid.  By considering the
	// empty tag element to be on the end of the list we handle the requirements for valid
	// completion.
	// This routine is general - it just needs to be fed a set of state transitions for a specific
	// element type.

	TBool returnValue = true; // We are successful until proved otherwise
	if( aStateCount < 1 )
		{
		returnValue = false; // Just check for a duff count
		}
	TInt fromState=KSMILDTDStartState; // Current state - the one we are looking for a transition from
	TInt toState=KSMILDTDEndState;   // State to which this tag leads us - initialised to avoid warning
	TInt midPoint= aStateCount / 2; // Middle of the state array, used for binary search
	TInt initJump = midPoint / 2; // Size of initial jump for binary search
	TInt tranArrInd; // Index into the state transition array

	// Prime the search with the initial state and the tag for the first element
	// We skip nodes which are not elements (e.g. text, comments or processing instructions)


	for (TInt i = 0; i<aChildElements.Count(); i++)
	  {
		// Check for the 'empty tag'.  We could take a local copy but that would
		// not be most efficient so we have slightly more complex code.

			{
			// Given a current state and tag, find a valid transition.
			// Use a binary search - the array is sorted on tag name first and then
			// from state.  We binary chop to a suitable level and then do a linear
			// search until we hit pay-dirt or find an element which proves.  We
			// only bother to binary search based on the tag name.
			// This is a home baked binary chop and could potentially be made more elegant.

			tranArrInd = midPoint;
			TInt jump = initJump;
			TBool keepChopping = true;
			TInt compVal;
			TPtrC tranTag;
			while(keepChopping)
				{
				tranTag.Set( aStateTrans[tranArrInd].TagName, aStateTrans[tranArrInd].TagLength);

				compVal = aChildElements[i].Compare(tranTag);
				if(compVal < 0)
					{
					tranArrInd -= jump;
					}
				else if(compVal > 0)
					{
					tranArrInd += jump;
					}
				jump = jump / 2;
				if((compVal == 0) || (jump < KSMILDTDMinJump))
					{
					keepChopping = false;
					}
				}// endwhile
			// We have now finished binary chopping, either because we matched the tag or because
			// We got to a small jump size.  Now do a linear scan, up or down, to fimd a match.

			TBool up = true; // Direction of scan
			tranTag.Set( aStateTrans[tranArrInd].TagName, aStateTrans[tranArrInd].TagLength);
			compVal = aChildElements[i].Compare(tranTag);
			if((compVal < 0) ||
			   ((compVal == 0) && (fromState < aStateTrans[tranArrInd].FromState)))
				{
				up = false;
				}
			if( up )
				{
				while((tranArrInd < aStateCount) &&
					  ((compVal > 0) ||
					   ((compVal == 0) && (fromState > aStateTrans[tranArrInd].FromState))))
					{
					tranArrInd ++;
					tranTag.Set( aStateTrans[tranArrInd].TagName, aStateTrans[tranArrInd].TagLength);
					if(tranArrInd < aStateCount)
						{
						compVal = aChildElements[i].Compare(tranTag);
						}
					}// endwhile stepping up
				}
			else
				{
				while((tranArrInd >= 0) &&
					  ((compVal < 0) ||
					   ((compVal == 0) && (fromState < aStateTrans[tranArrInd].FromState))))
					{
					tranArrInd --;
					tranTag.Set( aStateTrans[tranArrInd].TagName, aStateTrans[tranArrInd].TagLength);
					if(tranArrInd >= 0)
						{
						compVal = aChildElements[i].Compare(tranTag);
						}
					}// endwhile stepping down
				}
			// If we have a match, fine, else this is an illegal transition
			if((tranArrInd >= 0) && (tranArrInd < aStateCount) &&
			   (compVal == 0) && (fromState == aStateTrans[tranArrInd].FromState))
				{
				toState = aStateTrans[tranArrInd].ToState;
				}
			else
				{
				returnValue = false;
				break;
				}
			}//end else not reached end of list of children

		fromState = toState;
		}// endfor

		if(returnValue)
			{
			tranArrInd = 0;
			while((tranArrInd < aStateCount) &&
				  (aStateTrans[tranArrInd].FromState != fromState) &&
				  (aStateTrans[tranArrInd].TagLength == 0))
				{
				tranArrInd++;
				}
			if((tranArrInd < aStateCount) &&
			   (aStateTrans[tranArrInd].FromState == fromState) &&
			   (aStateTrans[tranArrInd].TagLength == 0))
				{
				toState = aStateTrans[tranArrInd].ToState ; // Better be the final state!
				}
			else
				{
				returnValue = false ; // No legal transition
				}
			}


	return returnValue;
	}




TBool CSMILDtd::CanElementHaveChildren(const TDesC& aElement) const
//
// Function to determine whether it is valid for a particular element to
// have children
// @param aElement the name of the element to be tested
// @return ETrue if it is valid for element to have children
// 
	{
	TBool retVal = ETrue;
	if(aElement == KSMILDTDEltanchor || aElement == KSMILDTDEltmeta || aElement == KSMILDTDEltroot_layout
		|| aElement == KSMILDTDEltregion || aElement == KSMILDTDEltarea || aElement == KSMILDTDEltmetadata
		|| aElement == KSMILDTDEltprefetch || aElement == KSMILDTDEltTrans)
		retVal = EFalse;

	return retVal;

	}
