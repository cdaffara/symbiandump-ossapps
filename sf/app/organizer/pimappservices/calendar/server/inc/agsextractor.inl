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

inline void CAgnInstanceExtractor::SetIndex(CAgnIndex& aSimpleEntryIndex)
/** Sets a reference to a CAgnIndex in the extractor.

@param aSimpleEntryIndex The Index to iterate over. */
	{
    iIterator->SetIndex(aSimpleEntryIndex);
	}
	
inline	void CAgnInstanceExtractor::SetFilter(const TAgnFilter& aFilter)
/** Sets a Filter in the Extractor's Iterator

@param aFilter The filter that is set. */
	{
	iIterator->SetFilter(aFilter);
	}

inline	void CAgnInstanceExtractor::SetUndatedTodoTime(const TTime& aUndatedTodoTime)
/** Sets today's date in the extractor and also in the iterator the 
    extractor owns

@param aUndatedTodoTime The time to use for undated todos. */
	{
	iUndatedTodoTime = aUndatedTodoTime;
	iIterator->SetUndatedTodoTime(aUndatedTodoTime);
	}

inline	const TTime& CAgnInstanceExtractor::UndatedTodoTime() const
/** Fetch today's date from the extractor.

@return The time to use for undated todos. */
	{
	return iUndatedTodoTime;
	}

inline const TAgnFilter* CAgnInstanceExtractor::Filter() const
/** Get the filter from the extractor's iterator

@return a reference to the filter kept in the extractor's iterator */
    {
    return iIterator->Filter();
    }

    