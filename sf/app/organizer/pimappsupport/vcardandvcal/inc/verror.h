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
//

#ifndef __VERROR_H__
#define __VERROR_H__

const TInt KVersitErrCodeBase = -5750;
const TInt KVersitErrBadDateTime			= KVersitErrCodeBase-1;
const TInt KVersitErrRepeatTypeNotHandled			= KVersitErrCodeBase-2;
const TInt KVersitErrBadRepeatValue				= KVersitErrCodeBase-3;
const TInt KVersitErrBadTimeZone			= KVersitErrCodeBase-4;
const TInt KVersitErrBadTimePeriod				= KVersitErrCodeBase-5;
const TInt KVersitErrNumberExpected				= KVersitErrCodeBase-6;
const TInt KVersitErrNestedVcardExpected	= KVersitErrCodeBase-7;
const TInt KVersitErrNoPropertyName			= KVersitErrCodeBase-8;



const TInt KVersitErrCodeLimit = -5999;
#endif
