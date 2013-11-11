/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef IRDATAPROVIDERCONSTANTS_H
#define IRDATAPROVIDERCONSTANTS_H

#include <e32def.h>

// Constants
const TInt KMaxHeaderNameLength = 32;
const TInt KMaxHeaderValueLength = 128;
const TInt KMaxStatusTextLength = 32;
const TInt KDPMaxContentTypeLength = 128;
const TInt KMaxCharSetLength = 128;

// This client accepts all content types.
_LIT8( KAccept, "*/*" );
// Used to specify the Accept Charset
_LIT8( KAcceptCharset, "iso-8859-1, utf-8, iso-10646-ucs-2; q=0.6" );
// Used to specify the Accept Language
_LIT8( KAcceptLanguage, "en" );
// Used to specify the Accept encoding
_LIT8( KAcceptEncoding, "gzip" );
// Some texts for header output
_LIT( KDPColon, ": " );
// Default Content-Type
_LIT8( KDefaultContentType, "application/xml" );
// Default CharSet
_LIT8( KDefaultCharSet, "UTF-8" );

 // HTTP POST ContentType
_LIT8( KPostContentType, "application/x-gzip" );
/*!
 * Http Success 2xx Status codes
 */

const TInt KOk = 200;
const TInt KCreated = 201;
const TInt KAccepted = 202;
const TInt KNonAuthoritativeInformation = 203;
const TInt KNoContent = 204;
const TInt KResetContent = 205;
const TInt KPartialContent = 206;
/*!
 * Redirection 3xx status codes
 */
const TInt KMultipleChoices = 300;
const TInt KMovedPermanently = 301;
const TInt KFound = 302;
const TInt KSeeOther = 303;
const TInt KNotModified = 304;
const TInt KUseProxy = 305;
/*
This status code is not used as per RFC 2616
const TInt KUnused = 306 
 */
const TInt KTemporaryRedirect = 307;
/*!
 * Client Error 4xx status codes
 */
const TInt KBadRequest = 400;
const TInt KUnauthorized = 401;
const TInt KPaymentRequired = 402;
const TInt KForbidden = 403;
const TInt KNotFound = 404;
const TInt KMethodNotAllowed = 405;
const TInt KNotAcceptable = 406;
const TInt KProxyAuthenticationRequired = 407;
const TInt KRequestTimeout = 408;
const TInt KConflict = 409;
const TInt KGone = 410;
const TInt KLengthRequired = 411;
const TInt KPreconditionFailed = 412;
const TInt KRequestEntityTooLarge = 413;
const TInt KRequestURITooLong = 414;
const TInt KUnsupportedMediaType = 415;
const TInt KRequestedRangeNotSatisfiable = 416;
const TInt KExpectationFailed = 417;
/*!
 * Server Error 5xx status codes
 */
const TInt KInternalServerError = 500;
const TInt KNotImplemented = 501;
const TInt KBadGateway = 502;
const TInt KServiceUnavailable = 503;
const TInt KGatewayTimeout = 504;
const TInt KHTTPVersionNotSupported = 505;
/*!
 * Timeout  code
 */
const TInt KDataProviderTimeout =  - 12345;
const TInt KDndTimedOut = -5120;

#endif // IRDATAPROVIDERCONSTANTS_H


