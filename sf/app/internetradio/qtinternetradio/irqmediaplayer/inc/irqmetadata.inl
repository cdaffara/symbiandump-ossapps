/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Meta data information wrapper
*
*/
/*
* ============================================================================
*  Name        : IRQMetaData.inl
*  Part of     : Internet Radio / MediaPlayer
*  Description : Meta data information wrapper
*  Version     :
*/

#include <QStringList>

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
inline IRQMetaData::IRQMetaData() :
    iBitrate(0)
{
}

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
inline IRQMetaData::IRQMetaData(const IRQMetaData& aMetaData)
{
    setMetaData(aMetaData);
}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
inline IRQMetaData::~IRQMetaData()
{
}

// ---------------------------------------------------------------------------
// Copies values from the supplied meta data object.
// ---------------------------------------------------------------------------
//
inline void IRQMetaData::setMetaData(const IRQMetaData& aMetaData)
{
    setStreamUrl(aMetaData.iUrl);
    setArtistSongName(aMetaData.iArtistSongName);
    setBitrate(aMetaData.iBitrate);
}

// ---------------------------------------------------------------------------
// Clears all the data in class.
// ---------------------------------------------------------------------------
//
inline void IRQMetaData::clear()
{
    iUrl.clear();
    iArtistSongName.clear();
    iArtistName.clear();
    iSongName.clear();
    iBitrate = 0;
}

// ---------------------------------------------------------------------------
// Sets the stream Url.
// ---------------------------------------------------------------------------
//
inline void IRQMetaData::setStreamUrl(const QString& aUrl)
{
    iUrl = aUrl;
}

// ---------------------------------------------------------------------------
// Sets the artist and song name.
// ---------------------------------------------------------------------------
//
inline void IRQMetaData::setArtistSongName(const QString& aArtistSongName)
{
    iArtistSongName = aArtistSongName;
    QStringList nameList = iArtistSongName.split(" - ");
    
    if (nameList.count() == 1)
    {
        // if there's no " - " found, we think there's only a song name
        iSongName = aArtistSongName;
    }
    else  // either artist and song name exist, or they are all empty
    {
        if (false == nameList.isEmpty())
        {
            iArtistName = nameList.first();
            nameList.removeFirst();
        }
    
        if (false == nameList.isEmpty())
        {
            iSongName = nameList.join(" - ");
        }
    }
}

// ---------------------------------------------------------------------------
// Sets the artist name.
// ---------------------------------------------------------------------------
//
inline void IRQMetaData::setArtistName(const QString& aArtistName)
{
    iArtistName = aArtistName;
}

// ---------------------------------------------------------------------------
// Sets the song name.
// ---------------------------------------------------------------------------
//
inline void IRQMetaData::setSongName(const QString& aSongName)
{
    iSongName = aSongName;
}

// ---------------------------------------------------------------------------
// Sets the audio bitrate.
// ---------------------------------------------------------------------------
//
inline void IRQMetaData::setBitrate(const int& aBitrate)
{
    iBitrate = aBitrate;
}

// ---------------------------------------------------------------------------
// Returns the stream Url.
// ---------------------------------------------------------------------------
//
inline const QString& IRQMetaData::getStreamUrl() const
{
    return iUrl;
}

// ---------------------------------------------------------------------------
// Returns the artist and song name.
// ---------------------------------------------------------------------------
//
inline const QString& IRQMetaData::getArtistSongName() const
{
    return iArtistSongName;
}

// ---------------------------------------------------------------------------
// Returns the artist name.
// ---------------------------------------------------------------------------
//
inline const QString& IRQMetaData::getArtistName() const
{
    return iArtistName;
}

// ---------------------------------------------------------------------------
// Returns the song name.
// ---------------------------------------------------------------------------
//
inline const QString& IRQMetaData::getSongName() const
{
    return iSongName;
}

// ---------------------------------------------------------------------------
// Returns the audio bitrate.
// ---------------------------------------------------------------------------
//
inline const int& IRQMetaData::getBitrate() const
{
    return iBitrate;
}

// ---------------------------------------------------------------------------
// Equality comparison operator.
// ---------------------------------------------------------------------------
//
bool IRQMetaData::operator==(const IRQMetaData& aMetaData) const
{
    bool ret = false;

    if (iUrl == aMetaData.iUrl
        && iArtistSongName == aMetaData.iArtistSongName
        && iArtistName == aMetaData.iArtistName
        && iSongName == aMetaData.iSongName
        && iBitrate == aMetaData.iBitrate)
    {
        ret = true;
    }

    return ret;
}

// ---------------------------------------------------------------------------
// Inequality comparison operator.
// ---------------------------------------------------------------------------
//
bool IRQMetaData::operator!=(const IRQMetaData& aMetaData) const
{
    return !(*this == aMetaData);
}
