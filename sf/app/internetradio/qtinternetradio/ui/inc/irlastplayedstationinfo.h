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

#ifndef IRLASTPLAYEDSTATIONINFO_H
#define IRLASTPLAYEDSTATIONINFO_H

#include "irqenums.h"

class IRQPreset;

class IRLastPlayedStationInfo
{
public:
    IRLastPlayedStationInfo();
    ~IRLastPlayedStationInfo();
    
    void updateLastPlayedStation(IRQPreset *aPreset, IRQConnectedFrom aConnectedFrom);
    void commitLastPlayedStation();
    
    IRQPreset * getLastPlayedStation() const;
    IRQConnectedFrom connectedFrom() const;
    
private:
    void readLastPlayedStation();
    
private:
    IRQPreset *iLastPlayedStation;
    bool       iUpdated;
    IRQConnectedFrom iConnectedFrom;
};

#endif
