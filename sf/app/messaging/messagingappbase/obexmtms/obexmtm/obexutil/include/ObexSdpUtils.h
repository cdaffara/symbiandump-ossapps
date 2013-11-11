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

#ifndef __OBEXSDPUTILS_H__
#define __OBEXSDPUTILS_H__

#include <e32base.h>
#include <bttypes.h>
#include <btsdp.h>


const TInt KMaxObexSupportedFormats = 7; //SDP Query will complete with KErrTooBig if remote device returns more than KMaxObexSupportedFormats format codes
typedef TBuf8<KMaxObexSupportedFormats> TObexSupportedFormatsList;

/**
* Observer class for CObexSdpUtils. Contains one method which indicates the results of 
* an SDP query.
*/
class MObexSdpUtilsObserver
/**
@internalTechnology
@released
*/
	{
public:
/**
 * SDP Query result
 *
 * @param aError If this is anything but KErrNone then the query has not completed correctly and all other parameters are meaningless.
 * @param aPortNumber The RFCOMM port number that the remote device supports OBEX on
 * @param aObexObjectPushProfileSupported ETrue if the remote device supports the Bluetooth OBEX Push Profile
 * @param aObexObjectPushProfileVersion Version of Bluetooth OBEX Push Profile supported (only valid if aObexObjectPushProfileSupported == ETrue)
 * @param aSupportedFormats Structure containing a list of the OBEX Object formats supported by the remote device
 */
	virtual void RemoteBtObexQueryResult(TInt aError, 
										 TInt aPortNumber, 
										 TBool aObexObjectPushProfileSupported, 
										 TInt aObexObjectPushProfileVersion, 
										 TObexSupportedFormatsList aSupportedFormats) = 0;
	};


/**
 * This class provides a single exported method to perform an SDP Query on a remote Bluetooth 
 * device, the results of which are returned via the MObexSdpUtilsObserver observer.
 *
 * This class uses a CSdpAgent object (and it's various observers) to process SDP records 
 * received from the remote Bluetooth device. Each SDP record is made up of attributes which 
 * in turn contain attribute components. CObexSdpUtils simply goes through these attribute 
 * components checking that we receive what we expect to and logging important information 
 * (e.g. port number OBEX is supported on).
 */
class CObexSdpUtils : public CBase, public MSdpAgentNotifier, public MSdpElementBuilder
/**
@internalTechnology
@released
*/
	{
public:

	/**
	 * Leave safe constructor
	 *
	 * @param aObserver Used to indicate results of SDP query
	 */

	IMPORT_C static CObexSdpUtils* NewL(MObexSdpUtilsObserver& aObserver);

	/**
	 * Leave safe constructor (which adds the newly created object to the cleanup stack)
	 * 
	 * @param aObserver Used to indicate results of SDP query
	 */

	IMPORT_C static CObexSdpUtils* NewLC(MObexSdpUtilsObserver& aObserver);
	
	/**
	 * Destructor. This can be called before the SDP Query completes to safely cancel the 
	 * query - N.B. this is the only way to cancel a pending query
	 */
	
	~CObexSdpUtils();

	/**
	 * Perform SDP query on a remote bluetooth device. Result is returned through the MObexSdpUtilsObserver 
	 * observer class. The query can be cancelled at any time by destroying this CObexSdpUtils 
	 * object.
	 *
	 * @param aBtDevAddr The address of the bluetooth device
	 */
	
	IMPORT_C void RemoteBtObexQueryL(TBTDevAddr aBtDevAddr);

public: //from MSdpAgentNotifier
	
	/** 
	 * Got an SDP record from the remote device, so check for errors and then request the 
	 * first attribute.
	 *
	 * @param aError Error code 
	 * @param aHandle Handle to the service recorde
	 * @param aTotalRecordsCount The total number of records
	 */
	
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
	
	/**
	 * The overload of AttributeRequestL() that we are using in NextRecordRequestComplete()
	 * should NOT result in this function being called, if it does then halt the query with an 
	 * error.
	 *
	 * @param aHandle The handle to the service record
	 * @param aAttrID The SDP Attribute ID
	 * @param aAttrValue The SDP Attribute Value
	 */

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
	
	/**
	 * Called after we have got all the attribute components for current attribute being 
	 * processed. If everything is OK get the next attribute or record.
	 * 
	 * @param aHandle The handle to the service record
	 * @param aError The error code
	 */	
	
	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError);

public: //from MSdpElementBuilder
	
	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */
	
	virtual MSdpElementBuilder* BuildUnknownL(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData);
	
	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */
	
	virtual MSdpElementBuilder* BuildNilL();
	
	/**
	 * Got a Uint attribute componet. Check it is what we expected to receive. Get the next attribute component.
	 *
	 * @param aUint Attribute as Uint contained in a descriptor
	 */

	virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	
	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */
	
	virtual MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
	
	/**
	 * Got a UUID attribute component. Check it is what we expected to receive. Get the next attribute component.
	 *
	 * @param aUUID The Bluetooth UUID of attribute
	 */
	
	virtual MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
 	
	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */

	virtual MSdpElementBuilder* BuildBooleanL(TBool aBool);
	
	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */

	virtual MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	
	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */

	virtual MSdpElementBuilder* BuildDESL();
	
	/**
	* Not required for our SDP query, so we provide an empty implementation.
	*/
	
	virtual MSdpElementBuilder* BuildDEAL();

	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */

	virtual MSdpElementBuilder* StartListL();
	
	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */

	virtual MSdpElementBuilder* EndListL();
	
	/**
	 * Not required for our SDP query, so we provide an empty implementation.
	 */
	
	virtual MSdpElementBuilder* BuildURLL(const TDesC8& aURL);

private:
	
	/**
	 * Simple constructor
	 *
	 * @param aObserver Used to indicate results of SDP query
	 */

	CObexSdpUtils(MObexSdpUtilsObserver& aObserver);
	
	/**
	* Necessary initial construction - Creates various structures used to check 
	* results obtained through CSdpAgent
	*/

	void ConstructL();

	/**
	 * Halt the current SDP query by destroying the CSdpAgent.
	 */

	void HaltQuery();
	
	/**
	 * Halt the current SDP query by destroying the CSdpAgent. Inform MObexSdpUtilsObserver 
	 * observer that query has completed with an error.
	 * 
	 * @param aError The error code
	 */

	void HaltQueryWithError(TInt aError);
	
	/**
	 * Convert descriptor containing Uint to a numeric Uint.
	 *
	 * The Uints can be 1, 2, 4, 8 or 16 bytes long in theory. It is doubtful 
	 * that the 8 or 16 byte variety would actually be used in our Obex Object 
	 * Push Service case. The attribute Id will always be a 2 byte Uint but it 
	 * is unclear from the spec what size the the port number should be. It is 
	 * implied that it should only be 1 byte but in the EPOC implementation, it 
	 * is returned as whatever size is it registered as (there is no type/size 
	 * checking carried out in the SDP server).
	 *
	 * @param aUint The Uint as a descriptor
	 */
	
	TUint32 UInt32(const TDesC8& aUint);
	
	/**
	 * Get next attribute if we are expecting another attribute.
	 * 
	 * @param aHandle The handle to the service records
	 */

	void RequestNextAttribute(TSdpServRecordHandle aHandle);
	
	/**
	 * Get next attribute component if we are expecting another attribute component. Set 
	 * internal expectations for the next attribute component we receive.
	 */

	void NextAttributeComponent();

public:
	enum TType
		{
		ENoComponent,
		EAnythingUntilNextExpectedValue,
		EUUID,
		EUint,
		EUintPossible,
		EUintListUntilEnd, //expected to get a list Uints for the rest of this attribute
		EAnythingUntilEnd  //not interested in what we get for this attribute anymore
		};

private:
	class TAttributeComponent
		{
	public:
		TType iType;
		TBool iSpecificValue;
		TUint32 iValue;
		};

	class CAttribute
		{
	public:
		
		/**
		 * Simple constructor
		 */

		CAttribute();
		
		/**
		 * Destructor deletes array of attributes
		 */

		~CAttribute();
		
		TUint16 iAttributeId;
		CArrayFixFlat<TAttributeComponent>* iAttributeComponentArray;
		};

private:
	MObexSdpUtilsObserver& iObserver; //< Observer used to indicate result of SDP query
	CSdpAgent* iSdpAgent; //< Makes SDP requests from remote SDP server
	TBool iIsActiveSDPQuery; //< Indicates an SDP query has yet to complete
	TInt iSdpRecordCount; //< Index of current record in the SDP query
	TBool iNotThisSdpRecord; //< Indicates in remote device complies fully with obex object push profile

	CArrayPtrFlat<CAttribute>* iAttributeArray; //<
	TInt iAttribute; //<
	TInt iAttributeComponent; //<
	TUUID iCurrentUUID; //<
	TUint32 iCurrentAttributeId; //<
	TType iExpectedType; //<
	TBool iExpectingSpecific; //<
	TUint32 iExpectedValue; //<
	TType iNextExpectedType; //<
	TUint32 iNextExpectedValue; //<

	TInt iRemoteObexPort; //< Port for sending obex message
	TBool iFullComplianceWithObexObjectPush; //<
	TInt iObexObjectPushProfileVersion; //<
	TObexSupportedFormatsList iSupportedFormatsList; //<
	};

#endif // __OBEXSDPUTILS_H__
