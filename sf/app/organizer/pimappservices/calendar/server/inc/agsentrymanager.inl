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

inline CAgnEntryManager::TBufItem::TBufItem(CAgnEntry* aEntry, TBool aToBeDeleted)
	: iEntry(aEntry),iToBeDeleted(aToBeDeleted)
	{}

inline TBool CAgnEntryManager::TBufItem::ToBeDeleted() const
	{ return ( iToBeDeleted ); }

inline CAgnEntry* CAgnEntryManager::TBufItem::Entry() const
	{ return ( iEntry ); }

inline void CAgnEntryManager::TBufItem::SetToBeDeleted(TBool aSetting)
	{ iToBeDeleted = aSetting; }

inline void CAgnEntryManager::TBufItem::SetEntry(CAgnEntry* aEntry)
	{ iEntry = aEntry; }

inline TBool CAgnEntryManager::BufferHasBeenStored() const
	{ return ( iBufferHasBeenStored ); }

inline TBool CAgnEntryManager::BufferedDeleting() const
	{ return ( iBufferedDeleting ); }

inline void CAgnEntryManager::SetStore(CStreamStore& aStore)
	{ iStore = &aStore; }

inline CStreamStore& CAgnEntryManager::StreamStore() const
	{ return *iStore; }
