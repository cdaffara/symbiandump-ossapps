// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agsinstancesorter.h"
#include "agmsortcriteria.h"
#include "agssortinstance.h"
#include "agmsimpleentry.h"
#include "agmpanic.h"

const TInt  KGreater = 1;
const TInt 	KEqual = 0;
const TInt 	KLess = -1;
const TInt 	KAscending = 1;
const TInt 	KDescending = -1;

/**
Get comparing function pointer based on specified sort attribute
@param aSortAttr Sort attribute  
*/
InstanceCompareFptr TAgsInstanceSortUtil::GetCompareFptr(CalCommon::TCalSortAttribute aSortAttr)
	{
	InstanceCompareFptr ret(NULL);
	
	switch(aSortAttr)
		{
		case CalCommon::ESortAttrStartTime:
			ret = static_cast<InstanceCompareFptr>(CompareStartTime); 
			break;
		case CalCommon::ESortAttrEndTime:
			ret = static_cast<InstanceCompareFptr>(CompareEndTime); 
			break;
		case CalCommon::ESortAttrLastModified:
			ret = static_cast<InstanceCompareFptr>(CompareLastModifiedTime); 
			break;
		case CalCommon::ESortAttrPriority:
			ret = static_cast<InstanceCompareFptr>(ComparePriority); 
			break;
		case CalCommon::ESortAttrType:
			break;
		default:
			__ASSERT_DEBUG(0, Panic(EAgmErrInstanceSorterArgument));
			break;
		}
	return ret;
	}

/**
Comparing instances with instance start time
@param aFirst reference to first instance
@param aSecond reference to second instance
@return Positive if the start time of first instance is earlier than the second
		Zero if the start time of first instance is equal to the second
		Negative if the start time of first instance is later than the second
*/
TInt TAgsInstanceSortUtil::CompareStartTime(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond)
	{
	return CompareByTime(aFirst.StartTimeLocal(), aSecond.StartTimeLocal());
	}

/**
Comparing instances with instance's end time
@param aFirst reference to first instance
@param aSecond reference to second instance
@return Positive if the end time of first instance is earlier than the second
		Zero if the end time of first instance is equal to the second
		Negative if the end time of first instance is later than the second
*/
TInt TAgsInstanceSortUtil::CompareEndTime(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond)
	{
	return CompareByTime(aFirst.EndTimeLocal(), aSecond.EndTimeLocal());
	}

/**
Comparing instances with instance's last modified time
@param aFirst reference to first instance
@param aSecond reference to second instance
@return Positive if the last modified time of first instance is earlier than the second
		Zero if the last modified time of first instance is equal to the second
		Negative if the last modified time of first instance is later than the second
*/
TInt TAgsInstanceSortUtil::CompareLastModifiedTime(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond)
	{
	return CompareByTime(aFirst.SimpleEntry().LastModifiedDateUtc(), aSecond.SimpleEntry().LastModifiedDateUtc());
	}

/**
Comparing instances with instance's priority
@param aFirst reference to first instance
@param aSecond reference to second instance
@return Positive if the priority of first instance is lower than the second
		Zero if the priority of first instance is equal to the second
		Negative if the priority of first instance is higher than the second
*/
TInt TAgsInstanceSortUtil::ComparePriority(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond)
	{
	return (aFirst.SimpleEntry().Priority() - aSecond.SimpleEntry().Priority()); 
	}

TInt TAgsInstanceSortUtil::CompareByTime(const TTime& aFirst, const TTime& aSecond)
	{
	TInt ret(KEqual);
	if(aFirst > aSecond)
		{
		//aFirst is later than aSecond  
		ret = KGreater;
		}
	else if(aFirst < aSecond)
		{
		//aFirst is earlier than aSecond  
		ret = KLess;
		}
	return ret;
	}

/**
Constructor
*/
TAgsInstanceSortElement::TAgsInstanceSortElement(CalCommon::TCalSortAttribute aSortAttr, CalCommon::TCalSortDirection aSortDir)
	{
	iFptr = TAgsInstanceSortUtil::GetCompareFptr(aSortAttr);
	iSortDir = (aSortDir == CalCommon::EAscendingOrder) ? KAscending : KDescending;
	}

/**
Comparing function
*/
TInt TAgsInstanceSortElement::Compare(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond) const
	{
	TInt ret(KEqual);
	if(iFptr)
		{
		ret = (*iFptr)(aFirst, aSecond) * iSortDir;
		}
	return ret;
	}

/**
Constructor
*/
TAgsInstanceSortType::TAgsInstanceSortType(CalCommon::TCalSortAttribute aSortAttr, CalCommon::TCalSortDirection aSortDir, const RArray<CCalEntry::TType>& aEntryTypeOrder)
	: TAgsInstanceSortElement(aSortAttr, aSortDir), iEntryTypeOrder(aEntryTypeOrder) 
	{
	}

/**
Comparing function
*/
TInt TAgsInstanceSortType::Compare(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond) const
	{
	TUint firstIndex = iEntryTypeOrder.Find(aFirst.SimpleEntry().Type());
	TUint secondIndex = iEntryTypeOrder.Find(aSecond.SimpleEntry().Type());
	return (firstIndex - secondIndex) * iSortDir; 
	}

/**
Constructor
*/
TAgnInstanceSortKey::TAgnInstanceSortKey(const RPointerArray<TAgsInstanceSortElement>& aSortElements)
	: TKeyArrayFix(0,ECmpNormal,0), iSortElements(aSortElements)
	{
	}

TInt TAgnInstanceSortKey::Compare(TInt aLeft,TInt aRight) const
	{
	const TInt KCount(iSortElements.Count());
	TInt ret(0);
	TInt index(0);
	
	TAgnSortInstance* leftInst = static_cast<TAgnSortInstance*>(At(aLeft));
	TAgnSortInstance* rightInst = static_cast<TAgnSortInstance*>(At(aRight));
	
	//Loop through the sort elements and exit whenever a non-qual return occurs  
	while(index < KCount && ret == 0)
		{
		ret = iSortElements[index++]->Compare(*leftInst, *rightInst);
		}
	
	if (ret == 0)
		{
		ret = leftInst->SimpleEntry().CollectionId() - rightInst->SimpleEntry().CollectionId();
		}
	
	return ret;
	}
	

CAgsInstanceSorter* CAgsInstanceSorter::NewL(CAgnSortCriteria* aSortCriteria, const RArray<CCalEntry::TType>& aDefaultTypeOrder)
	{
	CAgsInstanceSorter* self = new (ELeave) CAgsInstanceSorter();
	CleanupStack::PushL(self);
	self->ConstructL(aDefaultTypeOrder, aSortCriteria);
	CleanupStack::Pop(self);
	return self;
	}

CAgsInstanceSorter::CAgsInstanceSorter()
	{
	}

void CAgsInstanceSorter::ConstructL(const RArray<CCalEntry::TType>& aDefaultTypeOrder, CAgnSortCriteria* aSortCriteria)
	{
	if(aSortCriteria)
		{
		const TInt KCount(aSortCriteria->Count());
		
		//constructing sort elements for each sort criterion in aSortCriteria
		for(TInt index = 0; index < KCount; index++)
			{
			const TAgnSortCriterion& sortCriterion = aSortCriteria->AtL(index);
			TAgsInstanceSortElement* sortElement(NULL);
			if(sortCriterion.iSortAttribute == CalCommon::ESortAttrType)
				{
				if(aSortCriteria->EntryTypeOrder().Count() == 0)
					{
					//Using the default sort order for entry type
			    	sortElement = new (ELeave) TAgsInstanceSortType(sortCriterion.iSortAttribute, sortCriterion.iSortDirection, aDefaultTypeOrder);
					}
				else
					{
					//Using the customerized sort order for entry type
			    	sortElement = new (ELeave) TAgsInstanceSortType(sortCriterion.iSortAttribute, sortCriterion.iSortDirection, aSortCriteria->EntryTypeOrder());
					}
				}
			else
				{
		    	sortElement = new (ELeave) TAgsInstanceSortElement(sortCriterion.iSortAttribute, sortCriterion.iSortDirection);
				}
			
			CleanupStack::PushL(sortElement);
			iSortElements.AppendL(sortElement);
			CleanupStack::Pop(sortElement);
			} //for
		} //if(aSortCriteria)

	//Take ownership of this as the last thing we do so we don't leave prematurely
	iSortCriteria = aSortCriteria;
	}
	
CAgsInstanceSorter::~CAgsInstanceSorter()
	{
	const TInt KCount(iSortElements.Count());
	for(TInt index = 0; index < KCount; index++)
		{
		delete iSortElements[index];
		}
	iSortElements.Close();
	
	delete iSortCriteria;
	}
	
void CAgsInstanceSorter::SortL(CArrayFix<TAgnSortInstance>& aInstances)
	{
	//Using CArrayFixBase::Sort to carry out the sorting
	TAgnInstanceSortKey sortKey(iSortElements);
	if(KErrNone != aInstances.Sort(sortKey))
		{
		//CArrayFix<TAny>::Sort return KErrGeneral when sorting operation cause stack overflow.
		User::Leave(KErrOverflow);
		}
	}

TInt CAgsInstanceSorter::FindL(CArrayFix<TAgnSortInstance>& aInstances, TAgnSortInstance& aSearchInstance)
	{
	TAgnInstanceSortKey sortKey(iSortElements);
	TInt index;
	User::LeaveIfError(aInstances.FindIsq(aSearchInstance, sortKey, index));
	
	return index;
	}
