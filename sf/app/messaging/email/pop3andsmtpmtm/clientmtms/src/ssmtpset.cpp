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

#include "ssmtpset.h" 

GLREF_C TPtrC8 LimitStringSize(const TPtrC8& aString, TInt aMaxSize);
GLREF_C TPtrC16 LimitStringSize(const TPtrC16& aString, TInt aMaxSize);
//
// CImSSmtpSettings stores and restores SMTP Server settings
// 
EXPORT_C CImSSmtpSettings::CImSSmtpSettings()
	{
	__DECLARE_NAME(_S("CImSSmtpSettings"));
	Reset();
	} 

EXPORT_C CImSSmtpSettings::~CImSSmtpSettings()
	{
	Reset();
	}

EXPORT_C void CImSSmtpSettings::Reset()
	{	
	CImBaseEmailSettings::Reset();

	iVersion = KImSmtpServerSettingsVersion;
	iPortNumber= KSSMTPDefaultPortNumber;

	delete iEmailAddress;
	iEmailAddress = NULL;
	
	delete iPhoneNumber;
	iPhoneNumber = NULL;
	}

EXPORT_C void CImSSmtpSettings::StoreL(CMsvStore& aMsvStore) const
	{
	RMsvWriteStream out;
	out.AssignLC( aMsvStore, KUidMsvServiceEntry ); // pushes 'out' to the stack
	TRAPD(error, ExternalizeL(out));
	if (error==KErrNone)
		out.CommitL();
	out.Close(); // make sure we close the file
	User::LeaveIfError(error);
	aMsvStore.CommitL();
	CleanupStack::PopAndDestroy();
	}

EXPORT_C void CImSSmtpSettings::RestoreL( CMsvStore& aMessageStore )
	{
	RMsvReadStream in;
	in.OpenLC( aMessageStore, KUidMsvServiceEntry ); // pushes 'in' to the stack
	InternalizeL(in);
	CleanupStack::PopAndDestroy();
	}

EXPORT_C void CImSSmtpSettings::ExternalizeL(RMsvWriteStream& aWriteStream) const
	{
	CImBaseEmailSettings::ExternalizeL(aWriteStream);

	aWriteStream << LimitStringSize(EmailAddress(), KMaxSettingStringLength);
    aWriteStream << LimitStringSize(PhoneNumber(), KMaxSettingStringLength);
	}

EXPORT_C void CImSSmtpSettings::InternalizeL( RMsvReadStream& aReadStream )
	{
	Reset();
	CImBaseEmailSettings::InternalizeL(aReadStream);

	iEmailAddress = HBufC::NewL( aReadStream, KMaxSettingStringLength );
	iPhoneNumber = HBufC::NewL( aReadStream, KMaxSettingStringLength );
	}

EXPORT_C const TPtrC CImSSmtpSettings::EmailAddress() const
	{
	return iEmailAddress ? TPtrC(*iEmailAddress) : TPtrC();
	}

EXPORT_C const TPtrC CImSSmtpSettings::PhoneNumber() const
	{
	return iPhoneNumber ? TPtrC(*iPhoneNumber) : TPtrC();
	}

EXPORT_C void CImSSmtpSettings::SetEmailAddressL( const TDesC& aEmailAddress )
	{
	HBufC* newEmailAddress = aEmailAddress.AllocL();
	delete iEmailAddress;
	iEmailAddress = newEmailAddress;
	}

EXPORT_C void CImSSmtpSettings::SetPhoneNumberL( const TDesC& aPhoneNumber )
	{
	HBufC* newPhoneNumber = aPhoneNumber.AllocL();
	delete iPhoneNumber;
	iPhoneNumber = newPhoneNumber;
	}

EXPORT_C CImSSmtpSettings& CImSSmtpSettings::CopyL(const CImSSmtpSettings& aCImSSmtpSettings)
	{
	Reset();
	CImBaseEmailSettings::CopyL(aCImSSmtpSettings);
	iEmailAddress			=	aCImSSmtpSettings.EmailAddress().AllocL();
	iPhoneNumber			=	aCImSSmtpSettings.PhoneNumber().AllocL();
	return (*this);
	}

EXPORT_C TBool CImSSmtpSettings::operator==(const CImSSmtpSettings& aCImSSmtpSettings) const
	{
	return ((CImBaseEmailSettings::operator==(aCImSSmtpSettings)) &&
			!((EmailAddress().Compare(aCImSSmtpSettings.EmailAddress())) &&
			 (PhoneNumber().Compare(aCImSSmtpSettings.PhoneNumber()))));
	}
