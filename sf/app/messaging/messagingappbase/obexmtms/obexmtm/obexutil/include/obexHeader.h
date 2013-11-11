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
// $Workfile: obexHeader.h $
// $Author: Stevep $
// $Revision: 13 $
// $Date: 25/03/02 10:57 $
// 
//

#if !defined (__OBEXHEADER_H__)
#define __OBEXHEADER_H__


#include <e32std.h> 
#include <e32base.h>    //CBase 

class CMsvStore;
class RMsvReadStream;
class RMsvWriteStream;

// Stream version -- increment by one each time the externalized data format changes
// for a significant release.
const TInt32 KObexMessageStreamVersion = 1;//<Stream version
const TInt KObexBadStreamVersion = 1;


class CObexMtmHeader : public CBase
/**
Base class for OBEX object headers. Allows them to be stored and restored to and from the
CMsvStore.
 
@internalTechnology
@released
*/
	{
public:

	/**
	* Destructor which deletes all of the HBufC members
	*/

    IMPORT_C ~CObexMtmHeader();

	/**
	 * Creates a stream in aStore and externalises this object to it.
	 *
	 *
	 * @param aStore The store to which this object is to be stored. Note this must be open with write access or 
	 *  this function will leave.
	 * @leave KErrXXX Standard EPOC stream leave codes.
	 * @leave KErrAccessDenied aStore is not opened with write access.
	 */
    
	IMPORT_C void StoreL(CMsvStore& aStore) const;
    
	/**
	 * Opens a stream in aStore and internalises this object from it.
	 * @param aStore The store from which this object is to be restored. Note this must be open with read or 
	 *  read/write access or this function will leave.
	 * @leave KErrXXX Standard EPOC stream leave codes. 
	 * @leave KErrAccessDenied aStore is not opened with read access.
	 */

	IMPORT_C void RestoreL(const CMsvStore& aStore);

	//Setters/Getters
    
	/**
	 * Returns the Name header value.
	 * @return The Name header value. This will be a zero-length descriptor if this Name has not been set.
	 */
	
	IMPORT_C TPtrC Name() const;

	/**
	 * Sets the Name header value.
	 * @param aName The new Name header value
	 * @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the Name header fails.
	 */

    IMPORT_C void SetNameL(const TDesC& aSubject);

	/**
	 * Returns the Subject header value.
	 * @return The Subject header value. This will be a zero-length descriptor if this Subject has not been set.
	 */

    IMPORT_C TPtrC Subject() const;
    
	/**
	 * Sets the Subject header value.
	 *
	 * @param aName The new Subject header value
	 * @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the Subject header fails.
	 */

	IMPORT_C void SetSubjectL(const TDesC& aSubject);

    /**
	 * Returns the Type header value.
	 * @return The Type header value. This will be a zero-length descriptor if this Type has not been set.
	 */
	
	IMPORT_C TPtrC8 Type() const;
    
	/**
	 * Sets the Type header value.
	 *
	 * @param aType The new Type header value
	 * @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the Type header fails.
	 */

	IMPORT_C void SetTypeL(const TDesC8& aSubject);

    /**
	 * Returns the Target header value.
	 * @return The Target header value. This will be a zero-length descriptor if this Target has not been set.
	 */

	IMPORT_C TPtrC8 Target() const;
    
	/**
	 * Sets the ConnectionID header value.
	 * 
	 * @param ConnectionID The new Target header value
	 * @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the Target header fails.
	 */
	
	IMPORT_C void SetTargetL(const TDesC8& aSubject);

    /**
	 * Returns the ConnectionID header value.
	 * @return The ConnectionID header value. This will be a zero-length descriptor if this ConnectionID has not been set.
	 */
	
	IMPORT_C TPtrC8 ConnectionID() const;
    
	/**
	 * Sets the ConnectionID header value.
	 * 
	 * @param ConnectionID The new ConnectionID header value
	 * @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the ConnectionID header fails.
	 */
	
	IMPORT_C void SetConnectionIDL(const TDesC8& aSubject);

	/**
	 * Returns the Length header value
	 * @return Length header value
	 */
	
	IMPORT_C TInt Length() const;
	
	/**
	 * Sets the Length header value
	 * @param aLength The new Length header value
	 */

	IMPORT_C void SetLength(TInt aLength);

	/**
	 * Returns the Time header value
	 * @return Time header value
	 */

	IMPORT_C const TTime& Time() const;
	
	/**
	 * Sets the Time header value
	 * @param aTime The new Time header value
	 */

	IMPORT_C void SetTime(const TTime& aTime);

	//Transport specific address getter, therefore implemented in the derived classes
    IMPORT_C virtual TPtrC8 Addr() const = 0;
	//Transport specific address setter, therefore implemented in the derived classes
    IMPORT_C virtual void SetAddrL(const TDesC8& aSubject) = 0;

	//Called by Store/Restore -- must be implemented in the derived class and call BaseInternaliseL first
    IMPORT_C virtual void InternalizeL(RMsvReadStream& aReadStream) = 0;
	//Called by Store/Restore -- must be implemented in the derived class and call BaseExternaliseL first
    IMPORT_C virtual void ExternalizeL(RMsvWriteStream& aWriteStream) const = 0;

protected:
	//Base class functions that MUST be called by the derived classes
    
	/**
	 * Internalizes the CObexMtmHeader object from a stream.Must be called by the derived class BEFORE 
	 * that class internalizes
	 *
	 * @param aReadStream The stream to be read from
	 * @leave KErrXXX Standard EPOC stream leave codes
	 */
	
	IMPORT_C void BaseInternalizeL(RMsvReadStream& aReadStream);

	/**
	 * Externalizes CObexMtmHeader object to a stream. Must be called by the derived class BEFORE 
	 * that class externalizes
	 * @param aWriteStream The stream to be written to
	 * @leave KErrXXX Standard EPOC stream leave codes
	 */

    IMPORT_C void BaseExternalizeL(RMsvWriteStream& aWriteStream) const;
	
	/**
	 * Leaving constructor, which must be called by the base classes to initialise the HBufC members
	 */
	
	IMPORT_C void BaseConstructL();

protected:
	
	/**
	 * Default constructor, which assigns the TUid for the stream and stream version
	 *
	 * @param aStreamUid Uid of the header stream--specified in the derived class
	 * @param aStreamVersion Version number for the stream, must be specified in the derived classes 
	 * and incremented each time a new version of the stream is implemented.
	 */

	IMPORT_C CObexMtmHeader(TUid aStreamUid, TInt32 aStreamVersion);

protected:
	HBufC*			iName; //< The name of the Obex object
	HBufC*			iSubject; //< The subject of the Obex object
	HBufC8*			iType; //< The type of the Obex object
	TInt			iLength; //< The size of the Obex object
	TTime			iTime; //< Date and time of last modification of Obex object
	HBufC8*			iTarget; //< Byte sequence identifying intended target
	HBufC8*			iConnectionID; //< The transport type

	const TUid		iStreamUid; //< Uid of the header stream
	const TInt32	iStreamVersion; //< Version number for the streams
	};
#endif	//!defined (__OBEXHEADER_H__)
