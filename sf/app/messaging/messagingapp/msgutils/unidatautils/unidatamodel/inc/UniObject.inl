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
*       CUniObject, Storage for single attachment in presentation. 
*
*
*/


// ---------------------------------------------------------
// CUniObject::AttachmentId
//
// Accessor.
// ---------------------------------------------------------
//
inline TMsvAttachmentId CUniObject::AttachmentId() const
    {
    return iAttachmentId;
    }

// ---------------------------------------------------------
// CUniObject::MimeInfo
//
// Accessor.
// ---------------------------------------------------------
//
inline CUniMimeInfo* CUniObject::MimeInfo() const
    {
    return iMimeInfo;
    }

// ---------------------------------------------------------
// CUniObject::MimeInfo
//
// Accessor.
// ---------------------------------------------------------
//
inline CUniDrmInfo* CUniObject::DrmInfo() const
    {
    return iDrmInfo;
    }

// ---------------------------------------------------------
// CUniObject::MediaInfo
//
// Accessor.
// ---------------------------------------------------------
//
inline CMsgMediaInfo* CUniObject::MediaInfo() const
    {
    return iMediaInfo;
    }

// ---------------------------------------------------------
// Corrupted
//
// Accessor
// ---------------------------------------------------------
inline TBool CUniObject::Corrupted() const
    {
    return iObjectFlags & EObjectCorrupted;
    }

// ---------------------------------------------------------
// SetCorrupted
//
// Mutator
// ---------------------------------------------------------
inline void CUniObject::SetCorrupted( TBool aCorrupted )
    {
    if ( aCorrupted )
        {
        iObjectFlags |= EObjectCorrupted;
        }
    else
        {
        iObjectFlags &= ~EObjectCorrupted;
        }
    }

// ---------------------------------------------------------
// CUniObject::StoreState
//
// Accessor.
// ---------------------------------------------------------
//
//inline TMsgObjectStoreState CUniObject::StoreState() const
//    {
//    return iStoreState;
//    }

// ---------------------------------------------------------
// CUniObject::SetReferenceCount
//
// Mutator.
// ---------------------------------------------------------
//
inline void CUniObject::SetReferenceCount( TInt aCount )
    {
    iReferenceCount = aCount;
    }

// ---------------------------------------------------------
// CUniObject::ReferenceCount
//
// Accessor.
// ---------------------------------------------------------
//
inline TInt CUniObject::ReferenceCount() const
    {
    return iReferenceCount;
    }

// ---------------------------------------------------------
// CUniObject::SetRegion
//
// Mutator.
// ---------------------------------------------------------
//
inline void CUniObject::SetRegion( TUniRegion aRegion )
    {
    iRegion = aRegion;
    }

// ---------------------------------------------------------
// CUniObject::Region
//
// Accessor.
// ---------------------------------------------------------
//
inline TUniRegion CUniObject::Region() const
    {
    return iRegion;
    }

// ---------------------------------------------------------
// CUniObject::SetMediaTypeBySmilTag
//
// Mutator.
// ---------------------------------------------------------
//
inline void CUniObject::SetMediaTypeBySmilTag( TMsgMediaType aMediaType )
    {
    iMediaTypeBySmilTag = aMediaType;
    }

// ---------------------------------------------------------
// CUniObject::MediaTypeBySmilTag
//
// Accessor.
// ---------------------------------------------------------
//
inline TMsgMediaType CUniObject::MediaTypeBySmilTag() const
    {
    return iMediaTypeBySmilTag;
    }

// ---------------------------------------------------------
// CUniObject::SetUniqueControlId
//
// Mutator.
// ---------------------------------------------------------
//
inline void CUniObject::SetUniqueControlHandle( TInt aUniqueControlHandle )
    {
    iUniqueControlHandle = aUniqueControlHandle;
    }

// ---------------------------------------------------------
// CUniObject::UniqueControlHandle
//
// Accessor.
// ---------------------------------------------------------
//
inline TInt CUniObject::UniqueControlHandle() const
    {
    return iUniqueControlHandle;
    }

// ---------------------------------------------------------
// CUniObject::SetAttachment
//
// Mutator.
// ---------------------------------------------------------
//
inline void CUniObject::SetAttachment( TBool aObjectAttachment )
    {
    if ( aObjectAttachment )
        {
        iObjectFlags |= EObjectAttachment;
        }
    else
        {
        iObjectFlags &= ~EObjectAttachment;
        }
    }

// ---------------------------------------------------------
// CUniObject::Attachment
//
// Accessor.
// ---------------------------------------------------------
//
inline TBool CUniObject::Attachment() const
    {
    return iObjectFlags & EObjectAttachment;
    }

// ---------------------------------------------------------
// CUniObject::Attachment
//
// Accessor.
// ---------------------------------------------------------
//
inline TBool CUniObject::IsSaved( )
    {
    return iSavedToFs;
    }

// ---------------------------------------------------------
// CUniObject::SetAttachment
//
// Mutator.
// ---------------------------------------------------------
//
inline void CUniObject::SetSaved( TBool aSaved )
    {
    iSavedToFs = aSaved;
    }


// ---------------------------------------------------------
// IsDrmRightsWhenLoaded
//
// Accessor.
// ---------------------------------------------------------
//
inline TBool CUniObject::IsDrmRightsWhenLoaded( ) const
    {
    return iObjectFlags & EObjectDrmRightsWhenLoaded;    
    }

// ---------------------------------------------------------
// IsDrmRightsWhenLoadedObjects
//
// Accessor.
// ---------------------------------------------------------
//
inline TBool CUniObject::IsDrmRightsWhenLoadedObjects( ) const
    {
    return iObjectFlags & EObjectDrmRightsWhenLoadedObjects;    
    }

