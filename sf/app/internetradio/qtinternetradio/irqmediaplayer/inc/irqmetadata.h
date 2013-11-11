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
* Description:
*
*/
#ifndef IRQMETADATA_H_
#define IRQMETADATA_H_

#include <QString>

/**
 * Station meta data information wrapper.
 *
 * Supports all ICY protocol specified meta data information.
 * Including station URL, station name, artist - song name and bitrate.
 */

class IRQMetaData
{
public:
    inline IRQMetaData();
    inline IRQMetaData(const IRQMetaData& aMetaData);
    inline ~IRQMetaData();

    /**
     * Copies values from the supplied meta data object.
     *
     * Resulting object is identical to the supplied one after the call
     * has been successfully completed.
     *
     * @param   aMetaData   Meta data to copy information from.
     */
    inline void setMetaData(const IRQMetaData& aMetaData);

    /**
     * Clears all data in class.
     */
    inline void clear();

    /**
     * Sets the stream Url.
     *
     * @param   aUrl     The stream Url.
     */
    inline void setStreamUrl(const QString& aUrl);

    /**
     * Sets the artist and song name.
     *
     * @param   aArtistSongName     The artist and song name to set.
     */
    inline void setArtistSongName(const QString& aArtistSongName);

    /**
     * Sets the artist name.
     *
     * @param   aArtistName     The artist name to set.
     */
    inline void setArtistName(const QString& aArtistName);

    /**
     * Sets the song name.
     *
     * @param   aSongName     The song name to set.
     */
    inline void setSongName(const QString& aSongName);

    /**
     * Sets the audio bitrate.
     *
     * @param   aBitrate     The bitrate to set.
     */
    inline void setBitrate(const int& aBitrate);

    /**
     * Returns the stream Url.
     *
     * @return  The stream Url.
     */
    inline const QString& getStreamUrl() const;

    /**
     * Returns the artist and song name.
     *
     * @return  The artist and song name.
     */
    inline const QString& getArtistSongName() const;

    /**
     * Returns the artist name.
     *
     * @return  The artist name.
     */
    inline const QString& getArtistName() const;

    /**
     * Returns the song name.
     *
     * @return  The song name.
     */
    inline const QString& getSongName() const;

    /**
     * Returns the bitrate.
     *
     * @return  The bitrate.
     */
    inline const int& getBitrate() const;

    /**
     * Equality comparison operator.
     *
     * Compares two meta data objects and returns whether or not they are identical.
     *
     * @param   aMetaData   Meta data to be used for comparison.
     * @return  <code>True</code> if the objects are identical, <code>False</code> otherwise.
     */
    inline bool operator==(const IRQMetaData& aMetaData) const;

    /**
     * Inequality comparison operator.
     *
     * Compares two meta data objects and returns whether or not they are not identical.
     *
     * @param   aMetaData   Meta data to be used for comparison.
     * @return  <code>True</code> if the objects are not identical, <code>False</code> otherwise.
     */
    inline bool operator!=(const IRQMetaData& aMetaData) const;

private:
    QString iUrl;
    QString iArtistSongName;
    QString iArtistName;
    QString iSongName;
    int iBitrate;
};

#include "irqmetadata.inl"

#endif /* IRQMETADATA_H_ */
