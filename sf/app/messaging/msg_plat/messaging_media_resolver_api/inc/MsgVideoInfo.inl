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
* Description: MsgVideoInfo.inl*
*/



// -----------------------------------------------------------------------------
// CMsgVideoInfo::Duration
// ACCESSOR
// -----------------------------------------------------------------------------
inline TReal32 CMsgVideoInfo::VideoFrameRate() const
    {
    return iFramesPerSecond;
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::VideoBitRate
// ACCESSOR
// -----------------------------------------------------------------------------
inline TInt CMsgVideoInfo::VideoBitRate() const
    {
    return iBitRate;
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::AudioBitRate
// ACCESSOR
// -----------------------------------------------------------------------------
inline TInt CMsgVideoInfo::AudioBitRate() const
    {
    return iAudioBitRate;
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::Dimensions
// ACCESSOR
// -----------------------------------------------------------------------------
inline TSize CMsgVideoInfo::Dimensions() const
    {
    return iOverallSizeInPixels;
    }

// -----------------------------------------------------------------------------
// CMsgVideoInfo::IsAudio
// ACCESSOR
// -----------------------------------------------------------------------------
inline TBool CMsgVideoInfo::IsAudio() const
    {
    return TBool(iAudioBitRate);
    }

