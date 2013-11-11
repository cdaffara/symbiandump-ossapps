// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimsmtpmobilitysettings.h"
#include <smtpset.h>

/**
Factory constructor
*/
EXPORT_C CImSmtpMobilitySettings* CImSmtpMobilitySettings::NewL()
	{
	CImSmtpMobilitySettings* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

/**
Factory constructor
*/
EXPORT_C CImSmtpMobilitySettings* CImSmtpMobilitySettings::NewLC()
	{
	CImSmtpMobilitySettings* self = new(ELeave) CImSmtpMobilitySettings();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Constructor
*/
CImSmtpMobilitySettings::CImSmtpMobilitySettings()
	{
	}

/**
Second phase constructor
*/
void CImSmtpMobilitySettings::ConstructL()
	{
	iSmtpSettings = new(ELeave) CImSmtpSettings();
	}

/**
Destructor
*/
CImSmtpMobilitySettings::~CImSmtpMobilitySettings()
	{
	delete iSmtpSettings;
	}

/**
Creates copy of class instance
*/
EXPORT_C CImSmtpMobilitySettings* CImSmtpMobilitySettings::CopyL()
	{
	CImSmtpMobilitySettings* dest = CopyLC();
	CleanupStack::Pop();
	return dest;
	}

/**
Creates copy of class instance
*/
EXPORT_C CImSmtpMobilitySettings* CImSmtpMobilitySettings::CopyLC()
	{
	CImSmtpMobilitySettings* dest = NewLC();

	dest->SetServerAddressL(ServerAddress());
	dest->SetPort(Port());
	dest->SetSecureSockets(SecureSockets());
	dest->SetSSLWrapper(SSLWrapper());
	dest->SetEmailAliasL(EmailAlias());
	dest->SetEmailAddressL(EmailAddress());
	dest->SetDefaultMsgCharSet(DefaultMsgCharSet());
	dest->SetLoginNameL(LoginName());
	dest->SetPasswordL(Password());
	dest->SetSMTPAuth(SMTPAuth());
	dest->SetTlsSslDomainL(TlsSslDomain());

	return dest;
	}

/**
Reset settings to default values
*/
EXPORT_C void CImSmtpMobilitySettings::Reset()
	{
	iSmtpSettings->Reset();
	}

/**
Sets the configured IP address or host name of the outbound SMTP email server

@param aServerAddress Outbound email server address to set
*/
EXPORT_C void CImSmtpMobilitySettings::SetServerAddressL(const TDesC& aServerAddress)
	{
	iSmtpSettings->SetServerAddressL(aServerAddress);
	}

/**
Gets the configured IP address or host name of the outbound SMTP email server

@return Outbound SMTP email server address
*/
EXPORT_C const TPtrC CImSmtpMobilitySettings::ServerAddress() const
	{
	return iSmtpSettings->ServerAddress();
	}

/**
Sets the port number of the TCP connection to the outbound SMTP email server

Defaults to port 25

@param aPortNumber Port number
*/
EXPORT_C void CImSmtpMobilitySettings::SetPort(const TUint aPortNumber)
	{
	iSmtpSettings->SetPort(aPortNumber);
	}

/**
Gets the port number to use for the TCP connection to the outbound SMTP email
server

@return Port number
*/
EXPORT_C TUint CImSmtpMobilitySettings::Port() const
	{
	return iSmtpSettings->Port();
	}

/**
Sets whether a secure TLS connection will be negotiated over a non-secure
socket when a connection to the outbound SMTP email server is being
established.

Refer to RFC 3207 - "SMTP Service Extension for Secure SMTP over Transport
Layer Security" for information on how a secure socket is established when
sending an email. 

Not all SMTP servers support this protocol

@param aFlag ETrue if a secure socket session will be negotiated after
       establishing a non-secure TCP connection with the SMTP server.
*/
EXPORT_C void CImSmtpMobilitySettings::SetSecureSockets(TBool aFlag)
	{
	iSmtpSettings->SetSecureSockets(aFlag);
	}

/**
Gets whether a secure TLS connection will be negotiated over a non-secure
TCP socket connection when a connection to the outbound SMTP email server
is being established.

Refer to RFC 3207 - "SMTP Service Extension for Secure SMTP over Transport
Layer Security" for information on how a secure socket is established when
sending an email. 

@return ETrue if a secure socket session will be negotiated after
        establishing a non-secure TCP connection with the SMTP server.
*/
EXPORT_C TBool CImSmtpMobilitySettings::SecureSockets() const
	{
	return iSmtpSettings->SecureSockets();
	}

/**
Sets whether a secure TLS connection will be established directly over a
TLS/SSL socket when connecting to the outbound SMTP email server

The port number upon which the connection is attempted is configured by
calling SetPort(). 
*/
EXPORT_C void CImSmtpMobilitySettings::SetSSLWrapper(TBool aFlag)
	{
	iSmtpSettings->SetSSLWrapper(aFlag);
	}

/**
Gets whether a secure TLS connection will be established directly over a
TLS/SSL socket when connecting to the outbound SMTP email server
*/
EXPORT_C TBool CImSmtpMobilitySettings::SSLWrapper() const
	{
	return iSmtpSettings->SSLWrapper();
	}

/**
Sets the email alias (display name) that will be sent with outbound email.

@param aEmailAlias Email alias
*/
EXPORT_C void CImSmtpMobilitySettings::SetEmailAliasL(const TDesC& aEmailAlias)
	{
	iSmtpSettings->SetEmailAliasL(aEmailAlias);
	}

/**
Gets the email alias (display name) that will be sent with outbound email.

@return Email alias
*/
EXPORT_C const TPtrC CImSmtpMobilitySettings::EmailAlias() const
	{
	return iSmtpSettings->EmailAlias();
	}

/**
Sets the SMTP email address of the user

@param aEmailAddress SMTP email address of the user.
*/
EXPORT_C void CImSmtpMobilitySettings::SetEmailAddressL(const TDesC& aEmailAddress)
	{
	iSmtpSettings->SetEmailAddressL(aEmailAddress);
	}

/**
Gets the SMTP email address of the user

@return SMTP email address of the user.
*/
EXPORT_C const TPtrC CImSmtpMobilitySettings::EmailAddress() const
	{
	return iSmtpSettings->EmailAddress();
	}

/**
Sets which character set is used to send text in MIME email messages.

The default character set is ISO-8859-1 (Latin 1). The UIDs used to identify 
all character sets are defined in the Character Conversion API. 

@param aDefaultMsgCharSet Character set identifier
*/
EXPORT_C void CImSmtpMobilitySettings::SetDefaultMsgCharSet(TUid aDefaultMsgCharSet)
	{
	iSmtpSettings->SetDefaultMsgCharSet(aDefaultMsgCharSet);
	}

/**
Gets which character set is used to send text in MIME email messages.

@return Character set identifier
*/
EXPORT_C const TUid CImSmtpMobilitySettings::DefaultMsgCharSet() const
	{
	return iSmtpSettings->DefaultMsgCharSet();
	}

/**
Sets the login user name to use for SMTP authentication

@param aLoginName Login user name
*/
EXPORT_C void CImSmtpMobilitySettings::SetLoginNameL(const TDesC8& aLoginName)
	{
	iSmtpSettings->SetLoginNameL(aLoginName);
	}

/**
Gets the login user name used for SMTP authentication

@return Login user name
*/
EXPORT_C const TPtrC8 CImSmtpMobilitySettings::LoginName() const
	{
	return iSmtpSettings->LoginName();
	}

/**
Sets the password to use for SMTP authentication

@param aPassword Password
*/
EXPORT_C void CImSmtpMobilitySettings::SetPasswordL(const TDesC8& aPassword)
	{
	iSmtpSettings->SetPasswordL(aPassword);
	}

/**
Gets the password used for SMTP authentication

@return Password
*/
EXPORT_C const TPtrC8 CImSmtpMobilitySettings::Password() const
	{
	return iSmtpSettings->Password();
	}

/**
Sets whether SMTP authentication is used when sending emails.

See RFC 2554 "SMTP Service Extension for Authentication" for more details.

@param aFlag ETrue to enable SMTP authentication, EFalse to disable it.
*/
EXPORT_C void CImSmtpMobilitySettings::SetSMTPAuth(TBool aFlag)
	{
	iSmtpSettings->SetSMTPAuth(aFlag);
	}

/**
Gets whether SMTP authentication is used when sending emails.

@return ETrue if SMTP authentication is enabled, EFalse if disabled
*/
EXPORT_C TBool CImSmtpMobilitySettings::SMTPAuth() const
	{
	return iSmtpSettings->SMTPAuth();
	}

/**
Sets the domain name to use during TLS/SSL certificate validation.

@param aDomainName Domain name
*/
EXPORT_C void CImSmtpMobilitySettings::SetTlsSslDomainL(const TDesC8& aDomainName)
	{
	iSmtpSettings->SetTlsSslDomainL(aDomainName);
	}

/**
Gets the domain name to use during TLS/SSL certificate validation.

@return Domain name
*/
EXPORT_C TPtrC8 CImSmtpMobilitySettings::TlsSslDomain() const
	{
	return iSmtpSettings->TlsSslDomain();
	}

/**
Sets the SMTP settings to a new value

@param aSettings New SMTP settings
*/
void CImSmtpMobilitySettings::SetSmtpSettings(CImSmtpSettings* aSettings)
	{
	delete iSmtpSettings;
	iSmtpSettings = aSettings;
	}

/**
Gets the SMTP settings

@return SMTP settings
*/
CImSmtpSettings& CImSmtpMobilitySettings::SmtpSettings() const
	{
	return *iSmtpSettings;
	}
