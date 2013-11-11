/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Harvester message plugin
 *
*/


#ifndef CMESSAGEDATAHANDLER_H
#define CMESSAGEDATAHANDLER_H

//  INCLUDES 

#include <e32base.h>
#include <msvapi.h>
#include "cmessageplugin.h"

// FORWARD DECLARATIONS
class CMmsClientMtm;
class CSmsClientMtm;
class CClientMtmRegistry;
class CSmtpClientMtm;
class CPop3ClientMtm;
class CImap4ClientMtm;

class CMessageDataHandler : public CActive
	{
public:    
	/**
	 * Construction
	 * @return Harvester image plugin
	 */
	static CMessageDataHandler* NewL(CMessagePlugin& aMessagePlugin,
			CMsvSession& aSession);

	/**
	 * Destruction
	 */
	virtual ~CMessageDataHandler();

	/**
	 * Set a item to be indexed.
	 * @param TMsvId& aMessageId Id of message
	 * @param TCPixActionType aActionType Action type of item
	 * @param TMsvId& aFolderId Id of the folder where message is located
	 */
	void GatherDataL(TMsvId& aMessageId, TCPixActionType aActionType, TMsvId& aFolderId);
	
	void ResumeL();

private:
	//From CActive

	/**
	 * From CACtive.
	 */
	void DoCancel();

	/**
	 * From CACtive.
	 */
	void RunL();

	/**
	 * From CActive.
	 * @param aError Leave code from RunL()
	 * @return Error code, KErrNone if error was handled successfully.
	 */
	TInt RunError(TInt aError);

	/**
	 * Check if the message is valid and what type it is.
	 * @param TMsvId aIdToCheck Id of the message
	 * @param TMsgInfo& aMessageType Reference to put message type
	 * @return TInt error, if any
	 */
	TInt IsValidMessageType(TMsvId aIdToCheck, TMsgType& aMessageType);
	
	/**
	 * Launch the next request. 
	 */
	void HandleNextRequest();

private:

	/**
	 * Private constructor
	 */
	CMessageDataHandler(CMessagePlugin& aMessagePlugin, CMsvSession& aSession);

	/**
	 * 2nd phase construction
	 */
	void ConstructL();

	/**
	 * Create a message item to be indexed based on the params.
	 * @param TMsvId& aMsvId Id of the item to be indexed
	 * @param TCPixActionType aAction type of the indexing event
	 * @param TFolderId& aFolderId Id of the folder where indexed message is located
	 */
	 void CreateMessageIndexItemL(const TMsvId& aMsvId, TCPixActionType aActionType, const TMsvId& aFolderId);

	 /**
	  * Creates general message excerpt
	  */
	 HBufC* CreateExcerptLC(const TDesC& aFromAddress, 
							const CDesCArray& aRecipientArray,
							const TDesC& aSubject,
							const TDesC& aBodyText,
							const TMsvId& aFolderId);
	 
	 CSearchDocument* CreateSmsDocumentL(const TMsvId& aMsvId, const TMsvId& aFolderId);
	 CSearchDocument* CreateMmsDocumentL(const TMsvId& aMsvId, const TMsvId& aFolderId);
	 CSearchDocument* CreateEmailDocumentL(const TMsvId& aMsvId, const TMsvId& aFolderId);
	 TUint TextFileCharsetL(RFs aFs, RFile aFile);
	 void ConvertToUcs2FromUtfL(RFs& aFs,
								  TDes& aUcs2Text,
								  const TDesC8& aUtfText,
								  TUint aMibCharset);
	 TBool IsTextUcs2(const TDesC8& aText);
	 TBool TextContainsUtf8Bom(const TDesC8& aText);

private:
	class TMessageItem
		{
		public:
			TMessageItem(TMsvId aMessageId, TCPixActionType aActionType, TMsvId aFolderId)
				{
				iMessageId = aMessageId;
				iActionType = aActionType;
				iFolderId = aFolderId;
				}
			;

		public:
			TMsvId iMessageId;
			TCPixActionType iActionType;
			TMsvId iFolderId;
		};

private:
	// Message plugin handle
	CMessagePlugin& iMessagePlugin;
	// Session Message server session, own
	CMsvSession& iMsvSession;
	// Messages to be handled
	RArray<TMessageItem> iMessageArray;

	// Data sources
	CClientMtmRegistry*         iMtmReg;
	CMmsClientMtm*              iMmsMtm;
	CSmsClientMtm*              iSmsMtm;
	CSmtpClientMtm*             iSmtpMtm;                 // client for opening smtp
	CPop3ClientMtm*				iPop3Mtm;                   // client for opening pop3
	CImap4ClientMtm*			iImap4Mtm;                  // client for opening imap4
	RFs							iFs;
#ifdef HARVESTERPLUGINTESTER_FRIEND
    friend class CHarvesterPluginTester;
#endif
	};

#endif // CMESSAGEDATAHANDLER_H
// End of File
