// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CIMAPOPFETCHBODY_H__
#define __CIMAPOPFETCHBODY_H__

#include <e32std.h>
#include <msvstd.h>
#include "cimapoperation.h"
#include "cimapbodystructure.h" // for RAttributeValuePairList
#include "cimapenvelope.h"		// for RArrayTAddress
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <miuthdr.h>
#include <miutconv.h>
#endif

class CImapSession;
class CImapFolder;
class CMsgActive;
class CMsvServerEntry;
class TImImap4GetPartialMailInfo;
class CImapSettings;
class CImConvertHeader;
class CImapFetchResponse;
class CImapFetchBodyResponse;
class CImapRfc822HeaderFields;
class CFetchBodyInfo;
class CImapUtils;
class CCnvCharacterSetConverter;
class CImConvertCharconv;
class CImConvertHeader;
class CImCaf;
class CImapMailStore;
class CImapSyncManager;


/**
The CImapOpFetchBody class encapsulates fetching of body data from the remote
server to the local mirror. It is used when performing requested copy/move to
local or populate operations.

It is responsible for building the structure of the message in the messaging
store, according to the BODYSTRUCTURE returned by the remote server. The
BODYSTRUCTURE is requested, along with other message header parts, when first
populating a message.

From this structure (or during the construction of it), this class also
determines which parts of the message are to be fetched according to account
settings, or in the case of populate operations, the passed parameters.

For each body part to be fetched, a CFetchBodyInfo object is constructed and
populated with the information required by the CImapSession and CImapMailStore
to fetch and store the body part in the local mailstore. Once parsing of the
bodystructure is complete, each part is fetched in turn via the CImapSession.

Once a part has been fetched, this class is responsible for updating the 
appropriate message flags indicating local message completeness.

@internalTechnology
@prototype
*/
class CImapOpFetchBody : public CImapOperation
	{
public:
	typedef RArray<TPtrC8> RArrayTPtrC8;
	
	virtual ~CImapOpFetchBody();

	static CImapOpFetchBody* NewL(CImapSession*& aSession, CImapSyncManager& aSyncManager, CMsvServerEntry& aEntry, CImapSettings& aImapSettings, CImapMailStore& aMailStore);
	void FetchBodyL(TRequestStatus& aRequestStatus, const TMsvId aPart, const TImImap4GetPartialMailInfo& aGetPartialMailInfo);
	void Progress(TImap4GenericProgress& aGenericProgress);
	void CancelEnableResume();
	
private:
	CImapOpFetchBody(CImapSession*& aSession, CImapSyncManager& aSyncManager, CMsvServerEntry& aEntry, CImapSettings& aImapSettings, CImapMailStore& aMailStore);
	void ConstructL();
	
	// fetch initialisation
	void DoFetchL();
	void CheckForPartialPopulate();
	void FetchLargeHeaderL();

	// methods for constructing an array of items to fetch	
	void DoBuildFetchListL();
	void AddFetchItemL(CImMimeHeader& aMimeHeader, TMsvEmailEntry& aMessage, TBool aIsText, TInt32 aSizeToFetch);
	void AddFilterFetchItemL(CImMimeHeader& aMimeHeader, TMsvEmailEntry& aEntry, TImap4GetMailOptions& aPartTypes);
	void AddFilterFetchItemL(CImMimeHeader& aMimeHeader, TMsvEmailEntry& aEntry, TImImap4GetPartialMailInfo& aGetPartialMailInfo);
	void BuildFetchListL(TMsvId aPart);
	void RemoveHtmlPart(TMsvId aPart);

	TInt32 Minimum(TInt32 aThisPartTypeSizeLimit, TInt32 aTotalMailSizeLimit);

	// Requsts a specified item to be fetched.	
	TBool FetchPartL();
	
	// Registers an application part with the CAF framework
	void  RegisterWithCafL(CFetchBodyInfo& fetchInfo);
	
	// Header and bodystructure parsing
	TBool ProcessBodyStructureL();
	void PopulateImHeaderL(CImapRfc822HeaderFields& aHeaderFields, TMsvEmailEntry& aEntry, CImHeader& imHeader);
	void BuildTreeL(TMsvId aParent, CImapBodyStructure* aBodyStructure, const TDesC8& aPath, const TMsvId aThisMessage, TInt& aAttachments, TBool& aIsMHTML, TInt& aRelatedAttachments);
	void BuildContentEntryL(const TMsvId aParent, CImapBodyStructure* aBodyStructure, const TDesC8& aPath, const TMsvId aThisMessage, TInt& aAttachments, TBool& aIsMHTML, TInt& aRelatedAttachments);
	void ProcessTextSubtypeL(CImapBodyStructure* aBodyStructure, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, TBool& aIsMHTML);
	void ProcessParameterListL(const CImapBodyStructure::RAttributeValuePairList& aParamList, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, TFileName& aAttachmentFilename);
	void ProcessExtendedFieldsL(CImapBodyStructure* aBodyStructure, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, TFileName& aAttachmentFilename);
	void BuildEmbeddedMessageL(CImapBodyStructure* aBodyStructure, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, const TDesC8& aPath, TInt& aAttachments);
	void BuildNonMessageEntryL(const TMsvId& aParent, CImMimeHeader& aMime, TMsvEmailEntry& aMessage, TInt& aAttachments, TInt& aRelatedAttachments);

	void ProcessEnvelopeL(CImHeader* aHeader, TMsvEntry& aEntry, CImapEnvelope& aImapEnvelope);
	void GetDefaultFilenameL(TDes& aName, const TMsvEmailEntry& aMessage, const CImMimeHeader* mime);
	void ProcessAddressListL(CDesCArray& aWhere, const CImapEnvelope::RArrayTAddress& aAddressArray);
	void ProcessAddressListL(CDesCArray& aWhere, const TDesC8& aAddresses);
	TInt FindFilenameL(const CImMimeHeader& aMimeInfo, TPtrC8& aFilename);
	void FindFilenameDecodeL(const CImMimeHeader& aMimeInfo, TFileName& aFileName);
	void StripIllegalCharactersFromFileName(TDes16& aName);
	void CreateAttachmentInfoL(const TMsvEntry& aMsvEmailEntry);

	void StoreHeadersL(CImHeader* aImHeader, CImMimeHeader* aMimeHeader);
	void UpdateBodyTextRemainingSizeForHtml();
	
	static TPtrC8 StripAngledBrackets(const TDesC8& aString);
	
	// Post BodyPart-fetch processing
	void FetchPartCompleteL();
	void PropagateCompleteFlagL(TMsvId aId, TBool aDoBodyText, TBool aPartialFetched);
	
	// Server Error handling
	TInt ProcessServerError();

	// from CMsgActive
	virtual void DoRunL();
	void DoComplete(TInt& aErr);
	void DoCancel();
	
	void ClearFetchAttemptedL();

private:
	// These steps are those that need to be performed to fetch all, or part of a message.
	// The iNextStep data member is set when asynchronous calls are made to indicate the
	// next step that is to be performed on completion of the service request.
	enum TFetchSteps
		{
		EFinished,
		EGetBodyStructure,
		EProcessBodyStructure,
		EBuildFetchList,
		EFetchFirstPart,
		EFetchNext
		};

	// Referenced components
	CImapSyncManager& iSyncManager;	
	CImapSettings& iImapSettings;
	CImapMailStore& iMailStore;
	
	// State Machine Progress:
	TInt iNextStep;
	TInt iCurrentStep;
	
	// Stored fetch parameters.
	TImImap4GetPartialMailInfo iGetPartialMailInfo;
	
	// Requested message/message-part information
	TMsvId iRequestedPart;
	TUint  iMessageUid;
	TMsvId iMessageMsvId;

	// Header/bodystructure fetch response
	CImapFetchResponse* iFetchResponse;

	// Fetch body response information
	CImapFetchBodyResponse* iFetchBodyResponse;

	// Array of items to be fetched
	RPointerArray<CFetchBodyInfo> iFetchList;
	
	// Stats
	TInt  iPartsToDo;
	TInt  iPartsDone;
	TInt  iBytesToDo;
	TInt  iBytesDone;
	TBool iHasTextParts;

	// used for calculating parts to fetch when limits are being used
	TInt32 iBodyTextSize;
	TInt32 iHtmlEntrySize;
	TInt32 iSizeOfToBeFetchedAttachments;
	TInt32 iBodyPartRemainingSize;
	TMsvId iHtmlEntryPart; 
	
	// things to spot when processing the bodystructure
	TBool iFetchPartialMail;
	TBool iIsVCalendar;
	TBool iIsICalendar;
	TInt  iDecodedSizeOfAllParts;
	TFileName iAttachmentName;
	
	// converter objects
	CCnvCharacterSetConverter* iCharacterConverter;
	CImConvertCharconv* iCharConv;
	CImConvertHeader* iHeaderConverter;
	
	// CAF support
	CImCaf* iCaf;
	};

#endif	//__CIMAPOPFETCHBODY_H__
