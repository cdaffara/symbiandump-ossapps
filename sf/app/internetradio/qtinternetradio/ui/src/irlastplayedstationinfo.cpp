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
#include <QDataStream>
#include <QFile>
#ifdef HS_WIDGET_ENABLED
#include <QSettings>
#include "irservicedef.h"
#endif

#include "irqisdsdatastructure.h"
#include "irlastplayedstationinfo.h"
#include "irquid3.hrh"

const QString KFileName(QString("C:\\Private\\")+QString(STR_UID3_INTERNET_RADIO_10_1_EXE)+QString("\\lastPlayed.dat"));

IRLastPlayedStationInfo::IRLastPlayedStationInfo() : iLastPlayedStation(NULL), iUpdated(false),
                                                     iConnectedFrom(EIRQIsds)
{
    readLastPlayedStation();
}
    
IRLastPlayedStationInfo::~IRLastPlayedStationInfo()
{
    delete iLastPlayedStation;
    iLastPlayedStation = NULL;
}
    
void IRLastPlayedStationInfo::updateLastPlayedStation(IRQPreset *aPreset, IRQConnectedFrom aConnectedFrom)
{
    Q_ASSERT(aPreset);
    
    if (iLastPlayedStation == NULL)
    {
        iLastPlayedStation = new IRQPreset;
    }
    
    *iLastPlayedStation = *aPreset;
    
    iConnectedFrom = aConnectedFrom;
    
    iUpdated = true;
}
    
void IRLastPlayedStationInfo::commitLastPlayedStation()
{
    if (NULL == iLastPlayedStation || false == iUpdated)
    {
        return;
    }
    
#ifdef HS_WIDGET_ENABLED	
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    settings.setValue(KIrSettingStationName,iLastPlayedStation->nickName);
#endif
    
    QFile file(KFileName);
    bool ret = file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    if (!ret)
    {
        return;
    }
    
    QDataStream outStream(&file);
    qint32 connectedFrom = iConnectedFrom;
    outStream<<connectedFrom;
    iLastPlayedStation->externalize(outStream);
    file.close();
    iUpdated = false;
}

IRQPreset * IRLastPlayedStationInfo::getLastPlayedStation() const
{
    return iLastPlayedStation;
}

void IRLastPlayedStationInfo::readLastPlayedStation()
{ 
    if (!QFile::exists(KFileName))
    {
        return;
    }
    
    QFile file(KFileName);
    bool ret = file.open(QIODevice::ReadOnly);
    if (!ret)
    {
        return;
    }
    
    QDataStream inStream(&file);
    qint32 connectedFrom = EIRQIsds;
    inStream>>connectedFrom;
    if(connectedFrom < EIRQConnectedFromMaxValue)
    {
        iConnectedFrom = static_cast<IRQConnectedFrom>(connectedFrom);
    }
    else
    {
        iConnectedFrom = EIRQIsds;
    }
    
    iLastPlayedStation = new IRQPreset;
    iLastPlayedStation->internalize(inStream);
    file.close();
}

IRQConnectedFrom IRLastPlayedStationInfo::connectedFrom() const
{
    return iConnectedFrom;
}

