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
// $Workfile: obexHeader.cpp $
// $Author: Stevep $
// $Revision: 11 $
// $Date: 25/03/02 10:57 $
// 
//

//class include
#include <obexheader.h>
//system includes
#include <msvstore.h>   //CMsvStore, RMsvWriteStream, RMsvReadStream

//user includes
#include <obexinternalutils.h>

EXPORT_C CObexMtmHeader::CObexMtmHeader(TUid aStreamUid, TInt32 aStreamVersion) :
	iStreamUid (aStreamUid),
	iStreamVersion(aStreamVersion)	

/**
Default constructor, which assigns the TUid for the stream and stream version

  @param aStreamUid Uid of the header stream--specified in the derived class
  @param aStreamVersion Version number for the stream, must be specified in the derived classes 
  and incremented each time a new version of the stream is implemented.
*/
	{
	}
EXPORT_C void CObexMtmHeader::BaseConstructL()
/**
 * Leaving constructor, which must be called by the base classes to initialise the HBufC members
 */
	{
	iName = HBufC::NewL(0);
	iSubject = HBufC::NewL(0);
	iType = HBufC8::NewL(0);
	iTarget = HBufC8::NewL(0);
	iConnectionID = HBufC8::NewL(0);
	}

EXPORT_C CObexMtmHeader::~CObexMtmHeader()
/**
* Destructor which deletes all of the HBufC members
*/
	{
	delete iName;
	delete iSubject;
	delete iType;
	delete iTarget;
	delete iConnectionID;
	}


EXPORT_C void CObexMtmHeader::SetTime(const TTime& aTime)
/**
 * Sets the Time header value
 * @param aTime The new Time header value
 */
	{
	iTime = aTime;
	}

EXPORT_C const TTime& CObexMtmHeader::Time() const
/**
 * Returns the Time header value
 * @return Time header value
 */
	{
	return iTime;
	}


EXPORT_C void CObexMtmHeader::SetLength(TInt aLength)
/**
 *Sets the Length header value
 * @param aLength The new Length header value
 */
	{
	iLength = aLength;
	}

EXPORT_C TInt CObexMtmHeader::Length() const
/**
* Returns the Length header value
* @return Length header value
*/
	{
	return iLength;
	}


EXPORT_C TPtrC CObexMtmHeader::Name() const
/**
* Returns the Name header value.
* @return The Name header value. This will be a zero-length descriptor if this Name has not been set.
*/
    {
    return TPtrC(*iName);
    }

EXPORT_C void CObexMtmHeader::SetNameL(const TDesC& aName)
/**
* Sets the Name header value.
* @param aName The new Name header value
* @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the Name header fails.
*/
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    }


EXPORT_C TPtrC CObexMtmHeader::Subject() const
/**
* Returns the Subject header value.
* @return The Subject header value. This will be a zero-length descriptor if this Subject has not been set.
*/
    {
    return TPtrC(*iSubject);
    }

EXPORT_C void CObexMtmHeader::SetSubjectL(const TDesC& aSubject)
/**
* Sets the Subject header value.
*
* @param aName The new Subject header value
* @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the Subject header fails.
*/
    {
    delete iSubject;
    iSubject = NULL;
    iSubject = aSubject.AllocL();
    }


EXPORT_C TPtrC8 CObexMtmHeader::Type() const
/**
* Returns the Type header value.
* @return The Type header value. This will be a zero-length descriptor if this Type has not been set.
*/
    {
    return TPtrC8(*iType);
    }

EXPORT_C void CObexMtmHeader::SetTypeL(const TDesC8& aType)
/**
* Sets the Type header value.
*
* @param aType The new Type header value
* @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the Type header fails.
*/
    {
    delete iType;
    iType = NULL;
    iType = aType.AllocL();
    }


EXPORT_C TPtrC8 CObexMtmHeader::ConnectionID() const
/**
* Returns the ConnectionID header value.
* @return The ConnectionID header value. This will be a zero-length descriptor if this ConnectionID has not been set.
*/
    {
    return TPtrC8(*iConnectionID);
    }

EXPORT_C void CObexMtmHeader::SetConnectionIDL(const TDesC8& aConnectionID)
/**
* Sets the ConnectionID header value.
* 
* @param ConnectionID The new ConnectionID header value
* @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the ConnectionID header fails.
*/
    {
    delete iConnectionID;
    iConnectionID = NULL;
    iConnectionID = aConnectionID.AllocL();
    }


EXPORT_C TPtrC8 CObexMtmHeader::Target() const
/**
* Returns the Target header value.
* @return The Target header value. This will be a zero-length descriptor if this Target has not been set.
*/
    {
    return TPtrC8(*iTarget);
    }

EXPORT_C void CObexMtmHeader::SetTargetL(const TDesC8& aTarget)
/**
* Sets the ConnectionID header value.
* 
* @param ConnectionID The new Target header value
* @leave KErrXXX Standard EPOC error codes if allocation of a buffer to hold the Target header fails.
*/
    {
    delete iTarget;
    iTarget = NULL;
    iTarget = aTarget.AllocL();
    }



EXPORT_C void CObexMtmHeader::StoreL(CMsvStore& aStore) const
/**
 * Creates a stream in aStore and externalises this object to it.
 *
 *
 * @param aStore The store to which this object is to be stored. Note this must be open with write access or 
 *  this function will leave.
 * @leave KErrXXX Standard EPOC stream leave codes.
 * @leave KErrAccessDenied aStore is not opened with write access.
 */
    {
    RMsvWriteStream out;
    out.AssignLC(aStore, iStreamUid); 
    
    ExternalizeL(out);

    out.CommitL();//Commit the stream, but not the store.  The store will be committed by
                  //the owning class that's passed the store into this method.
    out.Close(); // make sure we close the stream
    
    CleanupStack::PopAndDestroy();
    }

EXPORT_C void CObexMtmHeader::RestoreL(const CMsvStore& aStore)
/**
 * Opens a stream in aStore and internalises this object from it.
 * @param aStore The store from which this object is to be restored. Note this must be open with read or 
 *  read/write access or this function will leave.
 * @leave KErrXXX Standard EPOC stream leave codes. 
 * @leave KErrAccessDenied aStore is not opened with read access.
 */
    {
    RMsvReadStream in;
    in.OpenLC(aStore, iStreamUid); 

    InternalizeL(in);

    in.Close();// make sure we close the stream
    CleanupStack::PopAndDestroy();
    }

EXPORT_C void CObexMtmHeader::BaseExternalizeL(RMsvWriteStream& aWriteStream) const
/**
 * Externalizes CObexMtmHeader object to a stream. Must be called by the derived class BEFORE 
 * that class externalizes
 *
 * @param aWriteStream The stream to be written to
 * @leave KErrXXX Standard EPOC stream leave codes
 */
    {
    // N.B. THE STREAM VERSION NUMBER MUST BE WRITTEN FIRST
    aWriteStream.WriteInt32L(KObexMessageStreamVersion);

    //TTime
    aWriteStream << iTime.Int64();

    //TInt
    aWriteStream.WriteInt32L(iLength);

    //HBufC
    ObexInternalUtils::ExternalizeL(iName, aWriteStream);
    ObexInternalUtils::ExternalizeL(iSubject, aWriteStream);

    //HBufC8
    ObexInternalUtils::ExternalizeL(iConnectionID, aWriteStream);
    ObexInternalUtils::ExternalizeL(iType, aWriteStream);
    ObexInternalUtils::ExternalizeL(iTarget, aWriteStream);
    }

EXPORT_C void CObexMtmHeader::BaseInternalizeL( RMsvReadStream& aReadStream)
/**
 * Internalizes the CObexMtmHeader object from a stream.Must be called by the derived class BEFORE 
 * that class internalizes
 *
 * @param aReadStream The stream to be read from
 * @leave KErrXXX Standard EPOC stream leave codes
 */
    {

    //CDesCArrays get reset by ::InternalizeL and are passed by reference, so they shouldn't be deleted


    const TInt32 STREAM_VERSION = aReadStream.ReadInt32L();

    switch (STREAM_VERSION)
        {
        case 1:
            {
            //TTime
            TInt64 time; 
            aReadStream >> time;
            iTime = time; 

            //TInt
            iLength = aReadStream.ReadInt32L();

            //HBufC 
            ObexInternalUtils::InternalizeL(iName, aReadStream);
            ObexInternalUtils::InternalizeL(iSubject, aReadStream);

			//HBufC8
            ObexInternalUtils::InternalizeL(iConnectionID, aReadStream);
            ObexInternalUtils::InternalizeL(iType, aReadStream);
            ObexInternalUtils::InternalizeL(iTarget, aReadStream);

            break;
            }

        default:
			ObexInternalUtils::Panic(ObexInternalUtils::EObexBadStreamVersion);
            break;
        }
    }



