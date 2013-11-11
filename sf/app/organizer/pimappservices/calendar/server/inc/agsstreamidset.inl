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

// ------------------------------------ CAgnStreamIdSet ----------------------------------
inline TInt CAgnStreamIdSet::Count() const
	{ return (iArray->Count()); }

// --------------------------- CAgnModelStreamIdSet------------------------------
inline const CAgnStreamIdSet& CAgnModelStreamIdSet::EntryStreamIdSet() const
	{ return (*iEntryStreamIdSet); }

inline CAgnStreamIdSet& CAgnModelStreamIdSet::EntryStreamIdSet()
	{ return (*iEntryStreamIdSet); }

inline const TStreamId& CAgnModelStreamIdSet::EntrySetStreamId() const
	{ return (iEntrySetStreamId); }

inline const TStreamId& CAgnModelStreamIdSet::NextLocalUidValueStreamId() const
	{ return (iNextLocalUidValueStreamId); }
	
inline const TStreamId& CAgnModelStreamIdSet::EntryManagerStreamId() const
	{ return (iEntryManagerStreamId); }

inline const TStreamId& CAgnModelStreamIdSet::FileInformationStreamId() const
	{ return (iFileInformationStreamId); }

inline TAgnVersion CAgnModelStreamIdSet::Version() const
	{ return (iVersion); }


