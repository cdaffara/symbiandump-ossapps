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

// --------------------------TAgnIterator-------------------
inline void TAgnIterator::SetIndex(CAgnIndex& aSimpleEntryIndex)
	{
	iArray = &aSimpleEntryIndex;	
	iCursor = 0;
	}
	
inline void TAgnIterator::SetFilter(const TAgnFilter& aFilter)
	{
	iFilter = aFilter;
	}
	
inline const TAgnFilter* TAgnIterator::Filter()
	{
	return &iFilter;
	}
	
inline const void TAgnIterator::SetUndatedTodoTime(const TTime& aUndatedTodoTime)
	{
	iUndatedTodoTime = aUndatedTodoTime;
	}
