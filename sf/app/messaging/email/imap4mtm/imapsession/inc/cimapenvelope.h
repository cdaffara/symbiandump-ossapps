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

#ifndef __CIMAPENVELOPE_H__
#define __CIMAPENVELOPE_H__

#include <e32base.h>

/**
Represents an "envelope" component of
a BODYSTRUCTURE.  See section 9 of RFC3501
@internalTechnology
@prototype
*/
class CImapEnvelope : public CBase
	{
public:
	static CImapEnvelope* NewL();
	~CImapEnvelope();
	
	/**
	Represents an IMAP address structure.
	The three main fields typically map to an email address of the form
		"iName" <iMailbox@iHost>
	See RFC2822 for the full meaning of each field.
	*/
	struct TAddress
		{
	public:		
		IMPORT_C HBufC16* CreateAddressStringL() const;
		
		void SetName(const TDesC8& aName);
		void SetAdl(const TDesC8& aAdl);
		void SetMailbox(const TDesC8& aMailbox);
		void SetHost(const TDesC8& aHost);

	private:		
		/**
		addr-name = nstring
		*/
		TPtrC8 iName;
		/** 
		addr-adl = nstring
		This contains routing information.
		*/
		TPtrC8 iAdl;
		/**
		addr-mailbox = nstring
		*/
		TPtrC8 iMailbox;
		/**
		addr-host = nstring
		*/
		TPtrC8 iHost;
		};
	
	/**
	An array of TAddress structures.
	@see CImapEnvelope::TAddress
	*/
	typedef RArray<const TAddress> RArrayTAddress;
	
	IMPORT_C const TDesC8& EnvDate();
	IMPORT_C const TDesC8& EnvSubject();
	IMPORT_C const RArrayTAddress& EnvFrom();
	IMPORT_C const RArrayTAddress& EnvSender();
	IMPORT_C const RArrayTAddress& EnvReplyTo();
	IMPORT_C const RArrayTAddress& EnvTo();
	IMPORT_C const RArrayTAddress& EnvCc();
	IMPORT_C const RArrayTAddress& EnvBcc();
	IMPORT_C const TDesC8& EnvInReplyTo();
	IMPORT_C const TDesC8& EnvMessageId();
	
	void SetEnvDate(const TDesC8& aEnvDate);
	void SetEnvSubject(const TDesC8& aEnvSubject);
	void AppendEnvFromL(const TAddress& aAddress);
	void AppendEnvSenderL(const TAddress& aAddress);
	void AppendEnvReplyToL(const TAddress& aAddress);
	void AppendEnvToL(const TAddress& aAddress);
	void AppendEnvCcL(const TAddress& aAddress);
	void AppendEnvBccL(const TAddress& aAddress);
	void SetEnvInReplyTo(const TDesC8& aEnvInReplyTo);
	void SetEnvMessageId(const TDesC8& aEnvMessageId);
	
protected:
	CImapEnvelope();
	
private:
		
	TPtrC8 iEnvDate;
	TPtrC8 iEnvSubject;
	RArrayTAddress iEnvFrom;
	RArrayTAddress iEnvSender;
	RArrayTAddress iEnvReplyTo;
	RArrayTAddress iEnvTo;
	RArrayTAddress iEnvCc;
	RArrayTAddress iEnvBcc;
	TPtrC8 iEnvInReplyTo;
	TPtrC8 iEnvMessageId;
	};

#endif // __CIMAPENVELOPE_H__
