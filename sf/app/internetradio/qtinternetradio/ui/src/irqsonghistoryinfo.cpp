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
#include "irqsonghistoryinfo.h"

void IRQSongInfo::setSongName(const QString& aName)
{
    iSongName = aName;
}

const QString& IRQSongInfo::getSongName() const
{
    return iSongName;
}

void IRQSongInfo::setArtistName(const QString& aArtist)
{
    iSongArtist = aArtist;
}

void IRQSongInfo::setStationName(const QString& aStationName)
{
    iStationName = aStationName;
}

const QString& IRQSongInfo::getArtistName() const
{
    return iSongArtist;
}

const QString& IRQSongInfo::getStationName() const
{
    return iStationName;
}
void IRQSongInfo::setMusicshopStatus(const QString& aStatus)
{
    iMusicshopStatus = aStatus;
}

const QString& IRQSongInfo::getMusicshopStatus() const
{
    return iMusicshopStatus;
}

void IRQSongInfo::setChannelID(const QString& aChannelID)
{
    iChannelID = aChannelID;
}

const QString& IRQSongInfo::getChannelID() const
{
    return iChannelID;
}

void IRQSongInfo::setAllInfo(const QString& aStationName, const QString& aSongName, const QString& aArtist, const QString& aStatus, const QString& aChannelID)
{
    iStationName = aStationName;
    iSongName =   aSongName;
    iSongArtist = aArtist;    
    iMusicshopStatus = aStatus;
    iChannelID = aChannelID;
}



