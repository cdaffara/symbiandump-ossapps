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

#include "MIUTSET.H"
#include "MIUT_ERR.H"


GLREF_C TPtrC8 LimitStringSize(const TPtrC8& aString, TInt aMaxSize);
GLREF_C TPtrC16 LimitStringSize(const TPtrC16& aString, TInt aMaxSize);


EXPORT_C CImBaseEmailSettings::CImBaseEmailSettings()
/** 
Allocates and creates a new CImBaseEmailSettings object initialised to
default values.

@internalComponent
*/
	{
	Reset();
	}


EXPORT_C CImBaseEmailSettings::~CImBaseEmailSettings()
/**
Destructor.

@internalComponent
*/
	{
	Reset();
	}


EXPORT_C void CImBaseEmailSettings::Reset()
/**
Resets the settings to their default values.

@internalComponent
*/
	{
	delete iServerAddress;
	iServerAddress = NULL;
	

	iFlags=0;
	iPortNumber=0;
	}


EXPORT_C const TPtrC CImBaseEmailSettings::ServerAddress() const
/**
Retrieves the configured IP address or host name of the email server.
The IP address or hostname is configured by calling SetServerAddressL().

If an instance of this class is of type CImPop3Settings or CImImap4Settings,
the address should be the incoming POP3 or IMAP4 email server.
If an instance of this class is of type CImSmtpSettings, the address should
be the outbound SMTP email server.

The default setting is empty. 

@see CImPop3Settings
@see CImImap4Settings
@see CImSmtpSettings

@return
Email server IP address if it is configured, or empty if not configured.
*/
	{
	return iServerAddress ? TPtrC(*iServerAddress) : TPtrC();
	}


EXPORT_C TUint CImBaseEmailSettings::Port() const
/**
Retrieves the port number of an email server when establishing
a TCP connection. The port number is configured by calling
SetPort().

If an instance of this class is of type CImPop3Settings, 
the default port number is 110.
If an instance of this class is of type CImImap4Settings,
the default port number is 143.
If an instance of this class is of type CImSmtpSettings,
the default port number is 25.

@see CImPop3Settings
@see CImImap4Settings
@see CImSmtpSettings

@return
Email server TCP port number.
*/
	{
	return iPortNumber;
	}


EXPORT_C void CImBaseEmailSettings::SetServerAddressL(const TDesC& aServerAddress)
/**
If an instance of this class is of type CImPop3Settings or CImImap4Settings,
the address should be the incoming POP3 or IMAP4 email server.
If an instance of this class is of type CImSmtpSettings, the address should
be the outbound SMTP email server.

The default setting is empty. 

@see CImPop3Settings
@see CImImap4Settings
@see CImSmtpSettings

@param aServerAddress
Email server IP address.
*/
	{
	HBufC* newServerAddress = aServerAddress.AllocL();
	delete iServerAddress;
	iServerAddress = newServerAddress;
	}


EXPORT_C void CImBaseEmailSettings::SetPort(const TUint aPortNumber)
/**
Configures the port number of an email server when establishing
a TCP connection.

If an instance of this class is of type CImPop3Settings, 
the default port number is 110.
If an instance of this class is of type CImImap4Settings,
the default port number is 143.
If an instance of this class is of type CImSmtpSettings,
the default port number is 25.

@see CImPop3Settings
@see CImImap4Settings
@see CImSmtpSettings

@param aPortNumber
Email server TCP port number.
*/
	{
	iPortNumber = aPortNumber;
	}


EXPORT_C TBool CImBaseEmailSettings::SecureSockets() const
/** 
Retrieves whether a secure TLS connection will be negotiated over an
unsecure TCP socket connection when a connection is being established.
This setting is configured by calling SetSecureSockets().

Refer to RFC 2595 - "Using TLS with IMAP, POP3 and ACAP" for information
on how a secure socket is established when retrieving email from a POP3
or IMAP4 server.

Refer to RFC 3207 - "SMTP Service Extension for Secure SMTP over Transport
Layer Security" for information on how a secure socket is established when
sending an email. 

@return
ETrue if a secure sockets is enabled.
*/
	{
	if(iFlags & EBaseEmailSettingsSecureSockets)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}		
	}


EXPORT_C void CImBaseEmailSettings::SetSecureSockets(TBool aFlag)
/** 
Sets whether a secure TLS connection will be negotiated over an
unsecure socket when a connection is being established. This setting
is configured by calling SetSecureSockets().

Refer to RFC 2595 - "Using TLS with IMAP, POP3 and ACAP" for information
on how a secure socket is established when retrieving email from a POP3
or IMAP4 server.

Refer to RFC 3207 - "SMTP Service Extension for Secure SMTP over Transport
Layer Security" for information on how a secure socket is established when
sending an email. 

Not all POP3, IMAP4, or SMTP servers support this protocol.

@post
If aFlag is ETrue, secure TLS connection will be disabled if it had been
previously been enabled by calling SetSSLWrapper().

@param aFlag
ETrue if a secure socket session will be negotiated after establishing
a non-secure TCP connection with the POP3, IMAP4, or SMTP server.
*/
	{
	iFlags = (iFlags & ~EBaseEmailSettingsSecureSockets) | (aFlag?EBaseEmailSettingsSecureSockets : EBaseEmailSettingsClearFlag);

	// Ensure that SSLWrapper AND SecureSockets are not BOTH set
	if (aFlag && SSLWrapper())
		{
		SetSSLWrapper(EFalse);
		}
	}


EXPORT_C TBool CImBaseEmailSettings::SSLWrapper() const
/**
Whether secure sockets will be used. 

Retrieves whether a secure TLS connection will be established 
directly over a TLS/SSL socket when connecting to the email server.

The well known port number for a secure TLS POP3 connection is 995.
The well known port number for a secure TLS IMAP4 connection is 993.
There is no well known port number for a secure SMTP TLS connection.
The TLS/SSL port number that the email server is listening on may not
necessarily be configured to the well known port number.
The port number upon which the connection is attempted is configured
by calling SetPort(). 

@return
ETrue if a secure TLS connection will be attempted when establishing
a connection.
*/
	{
	if(iFlags & EBaseEmailSettingsSSLWrappedSockets)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}		
	}


EXPORT_C void CImBaseEmailSettings::SetSSLWrapper(TBool aFlag)
/**
Specifies whether a secure TLS connection will be established 
directly over a TLS socket when connecting to the email server.

The well known port number for a secure TLS POP3 connection is 995.
The well known port number for a secure TLS IMAP4 connection is 993.
There is no well known port number for a secure SMTP TLS connection.
The TLS/SSL port number that the email server is listening on may not
necessarily be configured to the well known port number.
The port number upon which the connection is attempted is configured
by calling SetPort(). 

@post
If aFlag is ETrue, secure socket negotiation over an unsecure connection
will be disabled if it had been previously enabled by calling
SetSecureSockets().

@param aFlag
If ETrue, a secure TLS connection will be established 
when connecting to the email server.
*/
	{
	iFlags = (iFlags & ~EBaseEmailSettingsSSLWrappedSockets) | (aFlag?EBaseEmailSettingsSSLWrappedSockets : EBaseEmailSettingsClearFlag);

	// Ensure that SSLWrapper AND SecureSockets are not BOTH set
	if (aFlag && SecureSockets())
		{
		SetSecureSockets(EFalse);
		}
	}


EXPORT_C CImBaseEmailSettings& CImBaseEmailSettings::CopyL(const CImBaseEmailSettings& aCImBaseEmailSettings)
/**
Copies the configuration settings from another service settings object
into this object.

@param aCImBaseEmailSettings
Specifies the object to copy.

@return
This object after copying.

@internalComponent
*/	
	{
	Reset();
	iPortNumber		=	aCImBaseEmailSettings.iPortNumber;
	iFlags			=	aCImBaseEmailSettings.iFlags;
#ifdef __SUPPORT_MESSAGING_API_V1	
	iData			=	aCImBaseEmailSettings.iData;
#endif	
	iServerAddress	=	aCImBaseEmailSettings.ServerAddress().AllocL();
	return (*this);
	}


EXPORT_C TBool CImBaseEmailSettings::operator==(const CImBaseEmailSettings& aCImBaseEmailSettings) const
/**
Equality operator.

@param aCImBaseEmailSettings
Specifies the settings object to compare with.

@return
ETrue if the settings are the same.

@internalComponent
*/
	{
	return (
			(Port()			==	aCImBaseEmailSettings.iPortNumber) &&
			(iFlags			==	aCImBaseEmailSettings.iFlags) &&
			!(ServerAddress().Compare(aCImBaseEmailSettings.ServerAddress())));
	}

TUint32 CImBaseEmailSettings::SettingsFlags() const
	{
	return iFlags;
	}

void CImBaseEmailSettings::SetSettingsFlags(TUint32 aSetFlags)
	{
	iFlags = aSetFlags;
	}

/**
Sets flag to indicate bearer mobility will be supported by the service.
default 0 shall indicate that bearer mobility is not supported.
 

@param aFlag
Specifies the new value of the Flag to be set.

@return
void

@internalComponent
*/	
EXPORT_C void CImBaseEmailSettings::SetBearerMobility(TBool aFlag)
	{
	iFlags = (iFlags & ~EBaseEmailSettingsBearerMobFlag) | (aFlag?EBaseEmailSettingsBearerMobFlag : EBaseEmailSettingsClearFlag);
	}

/**
Indicates whether bearer mobility is supported by the service.
 
@param
None

@return
ETrue if bearer mobility is supported by the service, EFalse otherwise.

@internalComponent
*/	
EXPORT_C TBool CImBaseEmailSettings::BearerMobility() const
	{
	if(iFlags & EBaseEmailSettingsBearerMobFlag)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}		
	}
