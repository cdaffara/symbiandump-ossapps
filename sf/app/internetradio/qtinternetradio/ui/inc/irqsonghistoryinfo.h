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
#ifndef IRQSONGHISTORYINFO_H_
#define IRQSONGHISTORYINFO_H_

#include <QString>
 
class IRQSongInfo
{
public:
  
    
    void setSongName(const QString& aName);
    const QString& getSongName() const;
    
    void setArtistName(const QString& aArtist);
    const QString& getArtistName()const;   

    void setMusicshopStatus(const QString& aStatus);
    const QString& getMusicshopStatus()const;
    
    
    void setStationName(const QString& aStationName);
    const QString& getStationName() const;
    
    void setChannelID(const QString& aChannelID);
    const QString& getChannelID()const;
    
    void setAllInfo(const QString& aStationName, const QString& aSongName, const QString& aArtist, const QString& aStatus, const QString& aChannelID);
    
private:
    
    QString iSongName;
    QString iSongArtist;    
    QString iMusicshopStatus;
    QString iStationName;
    QString iChannelID;

};
#endif // IRQSONGHISTORYINFO_H_
