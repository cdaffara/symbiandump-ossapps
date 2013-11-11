/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CVMessageParser header
*
*/


#ifndef VMESSAGEPARSER_H
#define VMESSAGEPARSER_H

#include "omadsfolderobject.h"


// CONSTANTS
_LIT(KVMsgTagBegin, "BEGIN:");
_LIT(KVMsgTagEnd, "END:");
_LIT(KVMsgTagStatus, "X-IRMC-STATUS:");
_LIT(KVMsgTagBox, "X-IRMC-BOX:");
_LIT(KVMsgTagVersion, "VERSION:");
_LIT(KVMsgTagName, "N:");
_LIT(KVMsgTagTelephone, "TEL:");
_LIT(KVMsgTagDateTime, "X-NOK-DT:");
_LIT(KVMsgTagDate, "Date:");
_LIT(KVMsgSectionVMsg, "VMSG");
_LIT(KVMsgSectionVCard, "VCARD");
_LIT(KVMsgSectionVEnv, "VENV");
_LIT(KVMsgSectionVBody, "VBODY");
_LIT(KVMsgSectionEndVBody, "END:VBODY");
_LIT(KVMsgLineFeed, "\n");
_LIT(KDesNoData, "");
_LIT(KVMsgVersion, "1.1");
_LIT(KVMsgVCardVersion, "2.1");
_LIT(KVMsgStatusRead, "READ");
_LIT(KVMsgStatusUnread, "UNREAD");
_LIT(KVMsgStatusSent, "SENT");
_LIT(KVMsgStatusUnsent, "UNSENT");
_LIT(KFolderInbox, "INBOX");
_LIT(KFolderOutbox, "OUTBOX");
_LIT(KFolderDraft, "DRAFT");
_LIT(KFolderSent, "SENT");
_LIT(KFolderMyFolders, "MY FOLDERS");
_LIT(KFolderUnspesified, "");
const TInt KIndentStepSize = 1;


// FORWARD DECLARATIONS
class CRichText;

// CLASS DECLARATION

const TInt KNameMaxLength = 32;
const TInt KNumberMaxLength = 48;

/**
* Class CVMessageParser
* Helper class for converting SMS message information to/from text/x-VMessage.
*/
class CVMessageParser: public CBase 
	{
public: // Data types

	struct TTelephoneNumber
		{
		TBuf<KNameMaxLength> iName;
		TBuf<KNumberMaxLength> iNumber;
		};
		
public: // Constructors and destructor

	/**
    * Two-phased constructor.
    */
	static CVMessageParser* NewL();

	/**
    * Two-phased constructor leaving item to clenup stack
    */
	static CVMessageParser* NewLC();

    /**
    * Destructor.
    */
	~CVMessageParser();

private: // Private constructors

    /**
    * C++ default constructor.
    * C++ default constructor can NOT contain any code, that
    * might leave.
    */
	CVMessageParser();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
	void ConstructL();

public: // New functions

    /**
    * This function decodes received text/x-vMessage and stores the data
    * to member variables of this class.
    * 
    * @param aMessage  IN: buffer containing received text/x-vMessage
	* @return TInt Result, Length of SMS message body or negative error code.
    */
	TInt ParseMessageL( const TDesC8& aMessage );

    /**
    * This function constructs a text/x-vMessage based on the data stored
    * to member variables of this class.
    * 
    * @param aMessage  OUT: dynamic buffer where text/x-vMessage is constructed
    */
	void ConstructMessageL( CBufBase& aMessage );

    /**
    * Initialises all variables to with default values
    */
	void ResetAll();

    /**
    * Loads message body data to given rich text object
    * 
    * @param aMsgBody	where to load the data
    */
	void LoadMessageBodyL( CRichText& aMsgBody );

    /**
    * Stores message body data from given rich text object
    * 
    * @param aMsgBody	where from store the data
    */
	void StoreMessageBodyL( CRichText& aMsgBody );

    /**
    * Parses telephone number and name from the string containing number and possibly contact name
    * 
    * @param aNumberSring	IN: String from which to parse the number (like "Abc Pqr <+0123456789>")
	* @param aNumberStore	OUT: Variable where to store the output data
	* @return TBool		ETrue if operation was successful
    */
	TBool ParseTelephoneNumber(const TDesC& aNumberString, TTelephoneNumber& aNumberStore);

private: // Internal functions used by ReadMessage() 

    /**
    * Reads and decodes text/x-VMessage header. 
	*
	* @return TInt	KErrNone or negative error code
    */
	TInt ReadMessageHeader();

    /**
    * Reads and decodes envelope part of text/x-VMessage header. 
	* There can be multiple envelopes, which are handled recursively.
	*
	* @return TInt	KErrNone or negative error code
    */
	TInt ReadEnvelope();
	
    /**
    * Reads and parses time stamp field found from vMessage starting from the current read position.
    * Note that this field is optional. 
	*
	* @param aTimetstamp	IN: Timestamp field found from vMessage
	* @param aUniversalTime OUT: Parsed timestand as universal time
	* @return TInt	KErrNone if operation succeeded
    */	
    TInt ReadTaggedTimeStamp( TTime& aUniversalTime );

    /**
    * Reads and decodes text/x-VMessage body.
	*
	* @return TInt	Length of message body or negative value in error case
    */
	TInt ReadMessageBodyL();

    /**
    * Raeds VCARD section of text/x-vMessage - contains cotact information
    * 
    * @param aResult	telephone number and contact name found is stored here
	* @return TInt Result
    */
	TInt ReadVCard(TTelephoneNumber& aResult);

    /**
    * Searches next given message tag with given value from text/x-VMessage
    * 
    * @param aMsgTag  IN: Message tag to be found 
	* @param aValue	  IN: Expected value of the message tag to be found
	* @return TInt Result, Starting point of the message tag. KErrNotFound if not found.
    */
	TInt FindMessageTagWithValue( const TDesC& aMsgTag, const TDesC& aValue );

    /**
    * Searches next given message tag from text/x-VMessage and gets it's value
	* Value is expected to start from tag and end to newline.
    * 
    * @param aMsgTag  IN: Message tag to be found 
	* @param aValue	  OUT: Value of the message tag
	* @param aMoveReadPosition IN: whether moving start point of seach for the next field forward or not 
	* @return TInt Result, Starting point of the message tag. KErrNotFound if not found.
    */
	TInt GetMessageTagValue( const TDesC& aMsgTag, TDes& aValue, TBool aMoveReadBufPos = ETrue );

   /**
    * Stores a portion of message data from text/x-vMessage to local descriptor varíable.
	* Conversion from TDes8 to TDes16 is done and sufficient length of local descriptor
	* is checked - no more bytes than maximum length of the descriptor will be copied.
    * 
    * @param aStart		 IN: Start point of data to be copied in text/x-vMessage
	* @param aLength	 IN: Length of data to be copied
	* @param aLocalStore OUT: Place where the data will be copied
    */
	void StoreMessageDataField(TInt aStart, TInt aLength, TDes& aLocalStore);

private: // Internal functions used by WriteMessageL() 

    /**
    * Writes message properties to text/x-VMessage header. 
    */
	void WriteMessagePropertiesL();

    /**
    * Writes message VCARD information to text/x-VMessage header. 
    */
	void WriteMessageVCARDL(const TDesC& aName, const TDesC& aNumber);

    /**
    * Writes message envelope information to text/x-VMessage header. 
    */
	void WriteMessageEnvelopeL();
	
    /**
    * Writes tagged timestamp field to vMessage in format
    * X-NOK-DT:yyyymmddThhmmssZ (universal time format)
    *
    * @param aUniversalTime 	IN: Time value to be used
    */	
	void WriteTaggedTimestampL( TTime aUniversalTime );

    /**
    * Writes message body to text/x-VMessage. 
    */
	void WriteMessageBodyL();

    /**
    * Writes one line to text/x-vMessage
    * 
    * @param aMsgTag        IN: Message tag to use
	* @param aMsgTagData	IN: Message tag value to use
	* @param aIndent		IN: Whether indentication is used or not
    */
	void WriteMessageLineL( const TDesC& aMsgTag, const TDesC& aMsgTagData, TBool aIndent = EFalse );

    /**
    * Writes given data to text/x-vMessage.
	* Size of pre-allocated dynamic buffer is checked and new size allocated if necessary.
    * 
    * @param aDate			IN: Data to be written
    */
	void WriteToMessageL( const TDesC& aData );

    /**
    * Gets currect indentication level for writing message line.
    * 
    * @param aMsgTag		IN: Message tag used in the line
    */
	TInt GetIndentLevel( const TDesC& aMsgTag );

    /**
    * Fills fixed size buffer with given data in safe way so that max size will not be exeeded.
    * 
    * @param aData			IN: Input data
	* @param aBuffer		OUT: Buffer to fill
    */
	void AssignDataToBuffer( const TDesC& aData, TDes& aBuffer );
	




public: // Public member variables. Contain text/x-vMessage data.
		
	
	/**
    * Actual message body is stored here
    */
	HBufC*	iMessageBody;
	
    /**
    * Message time stamp in universal time
    */
	TTime		iUniversalTime;	

    /**
    * Message time stamp in home time
    */
	TTime		iHomeTime;

    /**
    * Message folder name 
    */
	TBuf<KMaxFolderNameLength>	iFolder;
	
   /**
    * Message status
    */
	TBuf<8>	iStatus;

   /**
    * Sender phone number
    */
	TTelephoneNumber	iSender;

   /**
    * Recipient phone numbers
    */	
	RArray<TTelephoneNumber> iRecipients;

private: // Internal member variables
		
	/**
    * Internal variables used for parsing vMessage
    */
	TInt		iReadBufPosition;
	TPtrC		iReadBuf;

	/**
    * Internal variables used for constructing vMessage
    */
	TInt		iWriteBufPosition;
	TInt		iWriteBufSize;
	TInt		iWriteIndentLevel;
	CBufBase*	iWriteBuf;

	};

#endif // VMESSAGEPARSER_H
