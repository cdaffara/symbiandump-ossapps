/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       CUniMimeInfo, Storage for objects mime headers.
*
*
*/


// ---------------------------------------------------------
// ContentType
// NOTE: 8 bit.
// ---------------------------------------------------------
inline const TPtrC8 CUniMimeInfo::ContentType() const
    {
    return iContentType ? TPtrC8( *iContentType ) : TPtrC8();
    }

// ---------------------------------------------------------
// Charset
// ---------------------------------------------------------
inline TUint CUniMimeInfo::Charset() const
    {
    return iContentTypeCharset;
    }

// ---------------------------------------------------------
// SetCharset
// ---------------------------------------------------------
inline void CUniMimeInfo::SetCharset( TUint aCharset )
    {
    iContentTypeCharset = aCharset;
    }

// ---------------------------------------------------------
// ContentLocation
// NOTE: 16 bit.
// ---------------------------------------------------------
inline const TPtrC CUniMimeInfo::ContentLocation() const
    {
    return iContentLocation ? TPtrC( *iContentLocation ) : TPtrC();
    }
        
// ---------------------------------------------------------
// ContentId
// NOTE: 8 bit.
// ---------------------------------------------------------
inline const TPtrC8 CUniMimeInfo::ContentId() const
    {
    return iContentId ? TPtrC8( *iContentId ) : TPtrC8();
    }

// ---------------------------------------------------------
// ContentBaseL
// NOTE: 8 bit.
// ---------------------------------------------------------
inline const TPtrC8 CUniMimeInfo::ContentBase() const
    {
    return iContentBase ? TPtrC8( *iContentBase ) : TPtrC8();
    }   

// ---------------------------------------------------------
// ContentDescription
// NOTE: 8 bit.
// ---------------------------------------------------------
inline const TPtrC8 CUniMimeInfo::ContentDescription() const
    {
    return iContentDescription ? TPtrC8( *iContentDescription ) : TPtrC8();
    }

// ---------------------------------------------------------
// ContentDisposition
// NOTE: 8 bit.
// ---------------------------------------------------------
inline const TPtrC8 CUniMimeInfo::ContentDisposition() const
    {
    return iContentDisposition ? TPtrC8( *iContentDisposition ) : TPtrC8();
    }

