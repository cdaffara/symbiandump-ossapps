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
* Description: MsgMediaResolver.inl*
*/



// -----------------------------------------------------------------------------
// CMsgMediaResolver::FileProtectionResolver
// ACCESSOR
// -----------------------------------------------------------------------------
inline CDRMHelper* CMsgMediaResolver::DRMHelper() const
    {
    return iDRMHelper;
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::FileProtectionResolver
// ACCESSOR
// -----------------------------------------------------------------------------
inline CFileProtectionResolver* CMsgMediaResolver::FileProtectionResolver() const
    {
    return iFileProt;
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::SetCharacterSetRecognition
// MUTATOR
// -----------------------------------------------------------------------------
inline void CMsgMediaResolver::SetCharacterSetRecognition( TBool aEnable )
    {
    iRecognizeCharSet = aEnable;
    }
