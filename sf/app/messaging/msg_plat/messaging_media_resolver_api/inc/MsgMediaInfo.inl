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
* Description: MsgMediaInfo.inl*
*/



// -----------------------------------------------------------------------------
// CMsgMediaInfo::FullFilePath
// ACCESSOR
// -----------------------------------------------------------------------------
//
inline TPtrC CMsgMediaInfo::FullFilePath() const
    {
    
    if(iFullFilePath)
        {
        return iFullFilePath->Des();
        }
    else
        {
        return KNullDesC();
        }
    
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::FileSize
// ACCESSOR
// -----------------------------------------------------------------------------
//
inline TInt CMsgMediaInfo::FileSize() const
    {
    return iFileSize;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::FileTime
// ACCESSOR
// -----------------------------------------------------------------------------
//
inline void CMsgMediaInfo::ModificationTime( TTime& aTime ) const
    {
    aTime = iModificationTime;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::MimeType
// ACCESSOR
// -----------------------------------------------------------------------------
//
inline TPtrC8 CMsgMediaInfo::MimeType() const
    {
    return iMimeType.Des8();
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::MediaType
// ACCESSOR
// -----------------------------------------------------------------------------
//
inline TMsgMediaType CMsgMediaInfo::MediaType() const
    {
    return iMediaType;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::ProtectionStatus
// ACCESSOR
// -----------------------------------------------------------------------------
//
inline TUint32 CMsgMediaInfo::Protection() const
    {
    return iProtection;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::SetProtectionStatus
// MUTATOR
// -----------------------------------------------------------------------------
//
inline void CMsgMediaInfo::SetProtection( TUint32 aProtection )
    {
    iProtection = aProtection;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::DRMContentURI
// ACCESSOR
// -----------------------------------------------------------------------------
//
inline HBufC8* CMsgMediaInfo::DRMContentURI() const
    {
    return iContentURI;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::SetDRMContentURI
// MUTATOR
// -----------------------------------------------------------------------------
//
inline void CMsgMediaInfo::SetDRMContentURI( HBufC8* aContentURI )
    {
    iContentURI = aContentURI;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::Duration
// ACCESSOR
// -----------------------------------------------------------------------------
TInt CMsgMediaInfo::Duration() const
    {
    return iDuration;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::Parsed
// ACCESSOR
// -----------------------------------------------------------------------------
TBool CMsgMediaInfo::Parsed() const
    {
    return iParsed;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::ParseError
// ACCESSOR
// -----------------------------------------------------------------------------
TBool CMsgMediaInfo::ParseError() const
    {
    return iParseError;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::HandlerApplicationUid
// ACCESSOR
// -----------------------------------------------------------------------------
TUid CMsgMediaInfo::HandlerApplicationUid() const
    {
    return iHandlerAppUid;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::HandlerApplicationUid
// MUTATOR
// -----------------------------------------------------------------------------
void CMsgMediaInfo::SetHandlerApplicationUid( TUid aHandlerAppUid )
    {
    iHandlerAppUid = aHandlerAppUid;
    }

