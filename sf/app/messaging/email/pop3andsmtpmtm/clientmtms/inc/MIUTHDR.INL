// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** Sets the field type.
@param aField Field type */
inline void TImHeaderEncodingInfo::SetField(const TFieldList aField)
	{
	iField = aField;
	}
	
/** Gets the field type.
@return Field type */
inline TImHeaderEncodingInfo::TFieldList TImHeaderEncodingInfo::Field() const
	{
	return iField;
	}

/** Sets the field offset.
@param aOffset Field offset */
inline void TImHeaderEncodingInfo::SetOffset(const TInt aOffset)
	{
	iOffset = (TUint16)aOffset;
	}

/** Gets the field offset.
@return Field offset */
inline TInt TImHeaderEncodingInfo::Offset() const
	{
	return iOffset;
	}

/** Sets the field length.
@param aLength Field length */
inline void TImHeaderEncodingInfo::SetLength(const TInt aLength)
	{
	iLength = (TUint16)aLength;
	}

/** Gets the field length.
@return Field length */
inline TInt TImHeaderEncodingInfo::Length()  const
	{
	return iLength;
	}

/** Sets the character set UID.
@param aUid Character set UID */
void TImHeaderEncodingInfo::SetCharsetUid(const TUint aUid)
	{
	iCharsetUid = aUid;
	}

/** Gets the character set UID.
@return Character set UID */
TUint TImHeaderEncodingInfo::CharsetUid() const
	{
	return iCharsetUid;
	}

/** Sets the encoding type (string).
@param aChar Encoding type */
inline void TImHeaderEncodingInfo::SetEncodingType(const TDesC8& aChar)
	{
	// If invalid, defaults to QPEncoding
	if (aChar[0] == 'b' || aChar[0] == 'B')
		iType = TImHeaderEncodingInfo::EBase64;
	else if (aChar[0] == 'q' || aChar[0] == 'Q')
		iType = TImHeaderEncodingInfo::EQP;
	else
		iType = TImHeaderEncodingInfo::ENoEncoding;
	}

/** Sets the encoding type (enumeration).
@param aType Encoding type */
inline void TImHeaderEncodingInfo::SetEncodingType(const TEncodingType aType)
	{
	iType = aType;
	}

/** Gets the encoding type (enumeration).
@return Encoding type */
inline TImHeaderEncodingInfo::TEncodingType TImHeaderEncodingInfo::EncodingType()  const
	{
	return iType;
	}

/** Sets the array value.
@param aValue Array value */
inline void TImHeaderEncodingInfo::SetArrayValue(const TInt aValue)
	{
	iArrayValue = (TUint16)aValue;
	}

/** Gets the array value.
@return Array value */
inline TInt TImHeaderEncodingInfo::ArrayValue()  const
	{
	return iArrayValue;
	}

/** Sets if a space character needs adding between two adjoining encoded-words.
@param atrueFalse True to add a space character */
inline void TImHeaderEncodingInfo::SetAddSpace(const TBool atrueFalse)
	{
	iAddSpace = atrueFalse;
	}

/** Tests if the add space character flag is set.
@return True for added space characters */
inline  TBool TImHeaderEncodingInfo::AddSpace() const
	{
	return iAddSpace;
	}

/** Sets the encoded length.
@param aLength Encoded length */
inline void TImHeaderEncodingInfo::SetEncodedLength(const TInt aLength)
	{
	iEncodedLength = (TInt8) aLength;
	}

/** Gets the encoded length.
@return Encoded length */
inline TInt TImHeaderEncodingInfo::EncodedLength() const
	{
	return iEncodedLength + (AddSpace() ? 1 : 0);
	}

//----------------------------------------------------------------------------------------

/** Gets a list of "To" recipients.


@return Recipient list */
inline CDesCArray& CImHeader::ToRecipients ()
	{
	return *iTo;
	}

/** Gets a list of "Cc" recipients.
@return Recipient list */
inline CDesCArray& CImHeader::CcRecipients ()
	{
	return *iCc;
	}
	
/** Gets a list of "Bcc" recipients.
@return Recipient list */
inline CDesCArray& CImHeader::BccRecipients()
	{
	return *iBcc;
	}

/** Gets a const list of "To" recipients.
@return Recipient list */
inline const CDesCArray& CImHeader::ToRecipients () const 
	{
	return *iTo;
	}

/** Gets a const list of "Cc" recipients.
@return Recipient list */
inline const CDesCArray& CImHeader::CcRecipients () const
	{
	return *iCc;
	}
	
/** Gets a const list of "Bcc" recipients.
@return Recipient list */
inline const CDesCArray& CImHeader::BccRecipients() const
	{
	return *iBcc;
	}

//-------------------------------------------------------------------------------------
//---------------------------- Used for forwarding an email ---------------------------

inline CDesCArray& CImHeader::ResentToRecipients ()
	{
	return *iResentTo;
	}

inline CDesCArray& CImHeader::ResentCcRecipients ()
	{
	return *iResentCc;
	}
	
inline CDesCArray& CImHeader::ResentBccRecipients()
	{
	return *iResentBcc;
	}

inline const CDesCArray& CImHeader::ResentToRecipients () const 
	{
	return *iResentTo;
	}

inline const CDesCArray& CImHeader::ResentCcRecipients () const
	{
	return *iResentCc;
	}
	
inline const CDesCArray& CImHeader::ResentBccRecipients() const
	{
	return *iResentBcc;
	}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

inline const TUint CImHeader::RemoteSize() const
	{
	return iRemoteSize;
	}

inline void CImHeader::SetRemoteSize( TUint aRemoteSize )
	{
	iRemoteSize = aRemoteSize;
	}


inline const TUint16 CImHeader::Version() const
	{
	return iVersion;
	}

inline void CImHeader::SetVersion( TUint16 aVersion )
	{
	iVersion = aVersion;
	}

/** Gets information relating to the encoding of header fields in received email. 

This includes the charset. This information can be used when forwarding the 
email, to re-encode the header fields.

@return Encoding information */
inline CArrayFix<TImHeaderEncodingInfo>& CImHeader::EncodingInfo()
	{
	return *iEncodingInfo;
	}

/** Gets const information relating to the encoding of header fields in received 
email. 

This includes the charset. This information can be used when forwarding the 
email, to re-encode the header fields.

@return Encoding information */
inline const CArrayFix<TImHeaderEncodingInfo>& CImHeader::EncodingInfo() const
	{
	return *iEncodingInfo;
	}

//
//															//
//

/** Tests whether encryption is used.

Not currently used.

@return True if encryption is used */
inline TBool TMsvEmailEntry::Encrypted() const
	{
    if(iMtmData1&KMsvEmailEntryEncryptedFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/** Sets whether encryption is used.

Not currently used.

@param aFlag True if encryption is used */
inline void TMsvEmailEntry::SetEncrypted(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryEncryptedFlag) | (aFlag?KMsvEmailEntryEncryptedFlag:KMsvEmailEntryClearFlag);
	}

/** Tests whether signing is used.

Not currently used.

@return True if signing is used */
inline TBool TMsvEmailEntry::Signed() const
	{
	if(iMtmData1&KMsvEmailEntrySignedFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/** Sets whether signing is used.

Not currently used.

@param aFlag True if signing is used */
inline void TMsvEmailEntry::SetSigned(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntrySignedFlag) | (aFlag?KMsvEmailEntrySignedFlag:KMsvEmailEntryClearFlag);
	}

//
/** Returns the flag indicating if the message has an HTML body part.

Note: the flag is for HTML and not only MHTML.

Use CImEmailMessage to retrieve the HTML body part.

@return True if the message has an HTML body part
@see CImEmailMessage
*/
inline TBool TMsvEmailEntry::MHTMLEmail() const
	{
	if(iMtmData1&KMsvEmailEntryMHTMLFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/** Sets the flag indicating if the message has an HTML body part.

Note: the flag is for HTML and not only MHTML.

Use CImEmailMessage to retrieve the HTML body part.

@param aFlag True if the message has an HTML body part
@see CImEmailMessage
*/
inline void TMsvEmailEntry::SetMHTMLEmail(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryMHTMLFlag) | (aFlag?KMsvEmailEntryMHTMLFlag:KMsvEmailEntryClearFlag);
	}

//
/** Tests if body text is complete.
@return True if body text is complete. */
inline TBool TMsvEmailEntry::BodyTextComplete() const
	{
	if(iMtmData1 & KMsvEmailEntryBodyTextCompleteFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets if body text is complete.
@param aFlag True if body text is complete. */
inline void TMsvEmailEntry::SetBodyTextComplete(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryBodyTextCompleteFlag) | (aFlag?KMsvEmailEntryBodyTextCompleteFlag:KMsvEmailEntryClearFlag);
	}

//
/** Tests if the message contains a VCard attachment.
@return True if the message contains a VCard attachment */
inline TBool TMsvEmailEntry::VCard() const
	{
	if(iMtmData1&KMsvEmailEntryVCardFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets a flag that the message contains a VCard attachment.
@param aFlag True if the message contains a VCard attachment */
inline void TMsvEmailEntry::SetVCard(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryVCardFlag) | (aFlag?KMsvEmailEntryVCardFlag:KMsvEmailEntryClearFlag);
	}

/** Tests if the message contains a VCal attachment.
@return True if the message contains a VCal attachment */
inline TBool TMsvEmailEntry::VCalendar() const
	{
	if(iMtmData1&KMsvEmailEntryVCalendarFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/** Sets a flag that the message contains a VCal attachment.
@param aFlag True if the message contains a VCal attachment */
inline void TMsvEmailEntry::SetVCalendar(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryVCalendarFlag) | (aFlag?KMsvEmailEntryVCalendarFlag:KMsvEmailEntryClearFlag);
	}

/** Tests if the message contains an iCalendar attachment.

@return True if the message contains an iCalendar attachment 
*/
inline TBool TMsvEmailEntry::ICalendar() const
	{
	if(iMtmData1&KMsvEmailEntryICalendarFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets a flag that the message contains an iCalendar attachment.

@param aFlag True if the message contains an iCalendar attachment 
*/
inline void TMsvEmailEntry::SetICalendar(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryICalendarFlag) | (aFlag?KMsvEmailEntryICalendarFlag:KMsvEmailEntryClearFlag);
	}	

//
/** Tests if the email message requests a receipt.
@return True if the email message requests a receipt */
inline TBool TMsvEmailEntry::Receipt() const
	{
	if(iMtmData1&KMsvEmailEntryReceiptFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets a flag that the email message requests a receipt.
@param aFlag True if the email message requests a receipt */
inline void TMsvEmailEntry::SetReceipt(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryReceiptFlag) | (aFlag?KMsvEmailEntryReceiptFlag:KMsvEmailEntryClearFlag);
	}


/** Gets the state of the unread messages IMAP4 flag.

This flag is set on a folder if the folder contains any
messages with the IMAP \\Seen flag set.

@return Unread messages IMAP4 flag 
@see TMsvEmailEntry::SeenIMAP4Flag()
*/
inline TBool TMsvEmailEntry::UnreadIMAP4Flag() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4UnreadFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the state of the unread messages IMAP4 flag.
@param aFlag Unread messages IMAP4 flag */
inline void TMsvEmailEntry::SetUnreadIMAP4Flag(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4UnreadFlag) | (aFlag?KMsvEmailEntryIMAP4UnreadFlag:KMsvEmailEntryClearFlag);
	}

/** Gets the state of the \\Seen IMAP4 flag.

This indicates if the message has been read. It is described in RFC3501, 2.3.2.
Flags Message Attribute.

Note that the server MTM creates messages with the TMsvEntry::Unread() flag set. If
on synchronisation a folder contains new messages, the Unread flag will also be
set on the folder.

@return \\Seen IMAP4 flag 
@see CImImap4Settings::UpdatingSeenFlags()
*/
inline TBool TMsvEmailEntry::SeenIMAP4Flag() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4SeenFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the state of the \\Seen IMAP4 flag.
@param aFlag \\Seen IMAP4 flag */
inline void TMsvEmailEntry::SetSeenIMAP4Flag(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4SeenFlag) | (aFlag?KMsvEmailEntryIMAP4SeenFlag:KMsvEmailEntryClearFlag);
	}

/** Gets the state of the \\Answered IMAP4 flag.

This indicates if the message has been answered. It is described in 
RFC3501, 2.3.2. Flags Message Attribute.

@return \\Answered IMAP4 flag */
inline TBool TMsvEmailEntry::AnsweredIMAP4Flag() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4AnsweredFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the state of the \\Answered IMAP4 flag.
@param aFlag \\Answered IMAP4 flag */
inline void TMsvEmailEntry::SetAnsweredIMAP4Flag(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4AnsweredFlag) | (aFlag?KMsvEmailEntryIMAP4AnsweredFlag:KMsvEmailEntryClearFlag);
	}


/** Gets the state of the \\Flagged IMAP4 flag.

This indicates if the message is flagged for urgent/special attention. 
It is described in RFC3501, 2.3.2. Flags Message Attribute.

@return \\Flagged IMAP4 flag */
inline TBool TMsvEmailEntry::FlaggedIMAP4Flag() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4FlaggedFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the state of the \\Flagged IMAP4 flag.
@param aFlag \\Flagged IMAP4 flag */
inline void TMsvEmailEntry::SetFlaggedIMAP4Flag(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4FlaggedFlag) | (aFlag?KMsvEmailEntryIMAP4FlaggedFlag:KMsvEmailEntryClearFlag);
	}

/** Gets the state of the \\Deleted IMAP4 flag.

This indicates if the message is marked as deleted for removal later. It is described in 
RFC3501, 2.3.2. Flags Message Attribute.

@return \\Deleted IMAP4 flag */
inline TBool TMsvEmailEntry::DeletedIMAP4Flag() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4DeletedFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the state of the \\Deleted IMAP4 flag.
@param aFlag \\Deleted IMAP4 flag */
inline void TMsvEmailEntry::SetDeletedIMAP4Flag(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4DeletedFlag) | (aFlag?KMsvEmailEntryIMAP4DeletedFlag:KMsvEmailEntryClearFlag);
	}

/** Gets the state of the \\Draft IMAP4 flag.

This indicates if the message has not completed composition. It is described in 
RFC3501, 2.3.2. Flags Message Attribute.

@return \\Draft IMAP4 flag */
inline TBool TMsvEmailEntry::DraftIMAP4Flag() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4DraftFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the state of the \\Draft IMAP4 flag.
@param aFlag \\Draft IMAP4 flag */
inline void TMsvEmailEntry::SetDraftIMAP4Flag(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4DraftFlag) | (aFlag?KMsvEmailEntryIMAP4DraftFlag:KMsvEmailEntryClearFlag);
	}

/** Gets the state of the \\Recent IMAP4 flag.

This indicates if the message is "recently" arrived in this mailbox.  This session
is the first session to have been notified about this message. For more details, see
see RFC3501, 2.3.2. Flags Message Attribute.

@return \\Recent IMAP4 flag */
inline TBool TMsvEmailEntry::RecentIMAP4Flag() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4RecentFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the state of the \\Recent IMAP4 flag.
@param aFlag \\Recent IMAP4 flag */
inline void TMsvEmailEntry::SetRecentIMAP4Flag(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4RecentFlag) | (aFlag?KMsvEmailEntryIMAP4RecentFlag:KMsvEmailEntryClearFlag);
	}


/** Tests if the specified folder is the IMAP4 Mailbox.
@return True if the specified folder is the Mailbox */
inline TBool TMsvEmailEntry::Mailbox() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4MailboxFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets if the specified folder is the IMAP4 Mailbox.
@param aFlag True if the specified folder is the Mailbox */
inline void TMsvEmailEntry::SetMailbox(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4MailboxFlag) | (aFlag?KMsvEmailEntryIMAP4MailboxFlag:KMsvEmailEntryClearFlag);
	}


/** Gets the IMAP4 orphaned state for the specified message/folder.

The concept of an orphan is no longer used, so this function should not be used.

@return True if the specified entry is orphaned */
inline TBool TMsvEmailEntry::Orphan() const
	{
	if(iMtmData1&KMsvEmailEntryOrphanFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the IMAP4 orphaned state for the specified message/folder.

The concept of an orphan is no longer used, so this function should not be used.

@param aFlag True if the specified entry is orphaned */
inline void TMsvEmailEntry::SetOrphan(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryOrphanFlag) | (aFlag?KMsvEmailEntryOrphanFlag:KMsvEmailEntryClearFlag);
	}


/** Gets the IMAP4 UID (unique identifier) validity for the specified folder.
@return True if IMAP4 UID validity is set */
inline TBool TMsvEmailEntry::ValidUID() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4ValidUIDFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the IMAP4 UID (unique identifier) validity for the specified folder.
@param aFlag True if IMAP4 UID validity is set */
inline void TMsvEmailEntry::SetValidUID(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4ValidUIDFlag) | (aFlag?KMsvEmailEntryIMAP4ValidUIDFlag:KMsvEmailEntryClearFlag);
	}


/** Gets the IMAP4 subscription state for the specified folder.
@return IMAP4 subscription state */
inline TBool TMsvEmailEntry::Subscribed() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4SubscribedFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the IMAP4 subscription state for the specified folder.
@param aFlag IMAP4 subscription state */
inline void TMsvEmailEntry::SetSubscribed(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4SubscribedFlag) | (aFlag?KMsvEmailEntryIMAP4SubscribedFlag:KMsvEmailEntryClearFlag);
	}


/** Gets the IMAP4 local subscription state for the specified folder.

@return IMAP4 local subscription state */
inline TBool TMsvEmailEntry::LocalSubscription() const
	{
	if(iMtmData1&KMsvEmailEntryIMAP4LocalSubFlag)
		{	
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}

/** Sets the IMAP4 local subscription state for the specified folder.

Depending on other settings, setting a folder to be locally subscribed can mean:

- that the folder is synchronised as part of synchronisation operations 
  (depending on the value of CImImap4Settings::Synchronise())
- that the folder will be set as subscribed on the remote server 
  (depending on the value of CImImap4Settings::SetSuscribe())

Note that clients can use the #KIMAP4MTMLocalSubscribe command to set
this flag and save the change to the entry.

@param aFlag IMAP4 local subscription state */
inline void TMsvEmailEntry::SetLocalSubscription(TBool aFlag)
	{
	iMtmData1 = (iMtmData1 & ~KMsvEmailEntryIMAP4LocalSubFlag) | (aFlag?KMsvEmailEntryIMAP4LocalSubFlag:KMsvEmailEntryClearFlag);
	}


/** Gets the IMAP4 UID (unique identifier) for the specified message/folder.
@return IMAP4 UID */
inline TUint32 TMsvEmailEntry::UID() const
	{
	return (TUint32)iMtmData2;	// hide the casting away from the caller
	}

/** Sets the IMAP4 UID (unique identifier) for the specified message/folder.
@param aUID IMAP4 UID */
inline void TMsvEmailEntry::SetUID(TUint32 aUID)
	{
	iMtmData2 = (TInt32) aUID;	// hide the casting away from the user
	}

/** Gets the number of messages stored in the remote folder.
@return Number of messages */
inline TInt TMsvEmailEntry::RemoteFolderEntries() const
	{
	return iMtmData3&KMsvRemoteFolderEntriesMask;
	}

/** Sets the number of messages stored in the remote folder.
@param aEntries Number of messages */
inline void TMsvEmailEntry::SetRemoteFolderEntries(TInt aEntries)
	{
	iMtmData3 = (TInt32) ((iMtmData3 & ~KMsvRemoteFolderEntriesMask) | (aEntries & KMsvRemoteFolderEntriesMask));
	}

//
//				CImMimeHeader inlines						//
//

/** Gets the const Content-Type parameters and their associated values.
@return Parameters and associated values */
inline const CDesC8Array& CImMimeHeader::ContentTypeParams() const 
	{
	return *iContentTypeParams;
	}


/** Gets the const Content-Disposition parameters and their associated values.
@return Parameters and associated values */
inline const CDesC8Array& CImMimeHeader::ContentDispositionParams() const
	{
	return *iContentDispositionParams;
	}


/** Gets the const MIME header parameters and their associated values.
This can be used by any MIME header fields.
@return Parameters and associated values */
inline const CDesC8Array& CImMimeHeader::XTypeParams() const
	{
	return *iXTypeParams;
	}


/** Gets the Content-Type parameters and their associated values.
@return Parameters and associated values */
inline CDesC8Array& CImMimeHeader::ContentTypeParams()
	{
	return *iContentTypeParams;
	}


/** Gets the Content-Disposition parameters and their associated values.
@return Parameters and associated values */
inline CDesC8Array& CImMimeHeader::ContentDispositionParams()
	{
	return *iContentDispositionParams;
	}


/** Gets the MIME header parameters and their associated values.
This can be used by any MIME header fields.
@return Parameters and associated values */
inline CDesC8Array& CImMimeHeader::XTypeParams()
	{
	return *iXTypeParams;
	}


inline const TUint16 CImMimeHeader::Version() const
	{
	return iVersion;
	}

inline void CImMimeHeader::SetVersion( TUint16 aVersion )
	{
	iVersion = aVersion;
	}
