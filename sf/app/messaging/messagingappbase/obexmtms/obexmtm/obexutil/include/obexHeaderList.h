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

#ifndef __OBEXHEADERLIST_H__
#define __OBEXHEADERLIST_H__



#include <e32std.h> 
#include <e32base.h>    //CBase 

//forward declaration
class RReadStream;
class RWriteStream;
class CObexHeader;
class CObexBaseObject;
class CObexServerSendOperation;


class CMsvAttachment;

const TUid KUidObexHeaders = {0x10204282};

class CObexHeaderList : public CBase
/**
Encapsulates and owns a list of CObexHeader objects.
Public users of this class can add CObexHeader objects 
                        
@publishedAll
@released
*/
	{
public:

	/*
    It provides functionalities for its friend classes to internalize and 
    externalize itself from a CMsvStore stream, and add all its contained 
    CObexHeader objects to a CObexBaseObject, as well as two general list
    functions to return the number of objects it contained and a contained
    object at a specified index into the list.
    */  
	friend class CObexServerSendOperation;		// requires the use of 'AddHeadersToBaseObjectL'

	/**
	 * Factory function to return a new CObexHeaderList.
	 @return New CObexHeaderList object 
	 */
	IMPORT_C static CObexHeaderList* NewL();
	
	
	/**
	 * Factory function to return a new CObexHeaderList and leave it on the 
	 * cleanup stack.
	 @return New CObexHeaderList object 
	 */
	IMPORT_C static CObexHeaderList* NewLC();


	/**
	 * Destructor which frees all allocated memory with the list.
	 */
    IMPORT_C ~CObexHeaderList();
    
    /**
	 * Add a CObexHeader object to the list. The CObexHeader object added 
	 * should have its attribute, header ID and header value set before
	 * it is added to the list. CObexHeaderlist will not change these
	 * values of its contained CObexHeader objects
	 * @param  aHeader Pointer to a constant CObexHeader object
	 *  
	 * @return KErrNone, if the operation is successful, otherwise
	 *         one of the system wide error codes  
	 */
	IMPORT_C TInt AddHeader(const CObexHeader*  aHeader);
	
	IMPORT_C void ExportToAttachmentL(CMsvAttachment& aAttachment) const;
	
	IMPORT_C void ImportFromAttachmentL(CMsvAttachment& aAttachment);

	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const;
	
	IMPORT_C void InternalizeL(RReadStream& aReadStream);

protected:



    /**
	 * Add all the CObexHeader objects contained in a list to a 
	 * CObexBaseObject.
	 *
	 *
	 * @param aObexBaseObj CObexBaseObject to add the objects to
	 *  
	 * @leave Error System wide error code 
	 * 
	 */
    IMPORT_C void AddHeadersToBaseObjectL(CObexBaseObject& aObexBaseObj);

	
	//General list processing functions

    /**
	 * Return the number of CObexHeader object pointers contained in the list.
	 *
	 * 
	 * @return The number of CObexHeader object pointers in the list
	 */
	IMPORT_C TInt Count() const;

	/**
	 * Get a non constant reference to the CObexHeader object pointer at 
	 * position aPos into the list.
	 *
	 *
	 * @param aPos A list index specifying the CObexHeader object pointer to 
	 *        be returned
	 *  
	 * @leave KErrArgument aPos is out of range
	 * @leave KErrNotFound The list is empty
	 *        
	 * @return A non constant reference to the CObexHeader object 
	 *         pointer
	 */
	IMPORT_C CObexHeader*& ObexHeaderL(TInt aPos); 

private:
	/**
	 * Leaving constructor function used in 2 phase construction in NewLC()
	 */
	void ConstructL();

    /**
	 *  Constructor function used in 2 phase construction in NewLC()
	 */
	CObexHeaderList();

private:
    //<pointer to the CObexHeader object pointer array
	RPointerArray<CObexHeader>*	iObexHeaders;
	};
	
#endif	// __OBEXHEADERLIST_H__
