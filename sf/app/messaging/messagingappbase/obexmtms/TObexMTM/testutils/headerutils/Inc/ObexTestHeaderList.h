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

#ifndef __OBEXTESTHEADERLIST_H__
#define __OBEXTESTHEADERLIST_H__



#ifndef __OBEXHEADERLIST_H__
#define __OBEXHEADERLIST_H__

#include <e32std.h> 
#include <e32base.h>	// CBase

//forward declaration
class CObexHeader;
class CObexBaseObject;
class CObexServerSendOperation;


const TUid KUidObexHeaders = {0x10204282};

class CMsvAttachment;
class RWriteStream;
class RReadStream;

class CObexHeaderList : public CBase
/**
Encapsulates and owns a list of CObexHeader objects.
Public users of this class can add CObexHeader objects 
                        
@publishedAll
@prototype
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
	friend class CObexTestHeaderList;			// use all functionality

	/**
	 * Factory function to return a new CObexHeaderList.
	 @return New CObexHeaderList object 
	 */
	static CObexHeaderList* NewL();
	
	
	/**
	 * Factory function to return a new CObexHeaderList and leave it on the 
	 * cleanup stack.
	 @return New CObexHeaderList object 
	 */
	static CObexHeaderList* NewLC();


	/**
	 * Destructor which frees all allocated memory with the list.
	 */
    ~CObexHeaderList();
    
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
	TInt AddHeader(const CObexHeader*  aHeader);
	
	void ExportToAttachmentL(CMsvAttachment& aAttachment) const;
	
	void ImportFromAttachmentL(CMsvAttachment& aAttachment);

	void ExternalizeL(RWriteStream& aWriteStream) const;
	
	void InternalizeL(RReadStream& aReadStream);

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
    void AddHeadersToBaseObjectL(CObexBaseObject& aObexBaseObj);

	
	//General list processing functions

    /**
	 * Return the number of CObexHeader object pointers contained in the list.
	 *
	 * 
	 * @return The number of CObexHeader object pointers in the list
	 */
	TInt Count() const;

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
	CObexHeader*& ObexHeaderL(TInt aPos); 

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

#endif // __OBEXHEADERLIST_H__

class CObexTestHeaderList : public CObexHeaderList
/**
 * CObexTestHeaderList class is derived from CObexHeaderList
 * to allow us to externalise and internalise header list so we can check it later on
 * and also to allow us to add a descriptor for the expected result.
 */
	{
	public:
		/**
		 * Factory function for class initialisation.
		 */
		static CObexTestHeaderList* NewL();

		/**
		 * Utility function for test harness use
		 * creates a write stream in the file passed in and externalises the
		 * header list and expected result descriptor
		 * the file will be created if not present, and if it's present
		 * it's contents will be removed.
		 */
		void AutoExternaliseL(const TDesC& aFileName, const TDesC& aDesiredResult);

		/**
		 * Utility function for test harness use
		 * opens a read stream in the file passed in and internalises the
		 * header list and expected result descriptor.  
		 */
		void AutoInternaliseL(const TDesC& aFileName);

		/**
		 * returns a reference to the expected result descriptor created during an internalise operation.
		 */
		const TDesC& ExpectedResultL();

		/**
		 * Default destructor
		 */
		~CObexTestHeaderList();

		/**
		 * Returns the number of headers stored in the list belonging to the base class.
		 */
		TInt GetCount();

		/**
		 * Return a reference to a pointer to a particular obex header from the list, 
		 * identified by array position.
		 */
		CObexHeader*& GetHeaderByIndexL(TInt aIndex);

	private:
		/**
		 * Second phase constructor.
		 */
		void ConstructL();				
		CObexTestHeaderList::CObexTestHeaderList();

	private:
		HBufC* iExpectedResult;
	};
#endif // __OBEXTESTHEADERLIST_H__

