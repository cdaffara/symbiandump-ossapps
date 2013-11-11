// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include <e32base.h>
#include <obex.h>


class CBaseContainer : public CBase
/**
Class CBaseContainer

A base class for derivation of the concrete OBEX container classes. Provides public accessor functions for data,
and allows data to be moved to a specified file independent of acutal storage method. 

@internalTechnology
@removed
*/
	{
public:

	
	IMPORT_C void DummySetHeaderDetailsL(CObexBaseObject&);

	IMPORT_C void DummyGetHeaderDetailsL(CObexBaseObject&) const;
	};

class CObexFileContainer;
/**
 * Class CContainer
 *
 * The purpose of this class is to encapsulate both the CBufBase and the CObexBufObject
 * as the latter does not export the the former both would need to be passed around between 
 * methods where used. 
 */
class CContainer : public CBaseContainer
/**
@internalTechnology
@removed
*/
	{
public:
	IMPORT_C static void DummyNewL();

	
	IMPORT_C static void DummyNewL(CBufBase*);

	IMPORT_C void DummyDtorCContainer();
	
	IMPORT_C void DummyCopyToFileContainerLC(RFs&, const TDesC&);

	
	
	IMPORT_C  void DummyMoveToFile(RFs& , const TDesC&);

	
	IMPORT_C  void DummyCopyOfDataLC(TInt);
	
	IMPORT_C  void DummyCopyOfDataLC();
	
	IMPORT_C  void DummyObexBaseObject() const;

	};

/**
 * Class CObexFileContainer
 *
 * The purpose of this class is to encapsulate both the TFileName and the CObexFileObject
 * as the latter does not export the the former both would need to be passed around between 
 * methods where used. 
 */
class CObexFileContainer : public CBaseContainer
/**
@internalTechnology
@removed
*/
	{
public:
	IMPORT_C static void DummyNewL(const TDesC&);

	IMPORT_C void DummyDtorCObexFileContainer();

	IMPORT_C  void DummyMoveToFile(RFs&, const TDesC&);

	IMPORT_C  void DummyCopyOfDataLC(TInt);
	
	IMPORT_C void DummyCopyOfDataLC();
	
	IMPORT_C void DummyObexBaseObject() const;
	
	};

#endif  // __CONTAINER_H__
