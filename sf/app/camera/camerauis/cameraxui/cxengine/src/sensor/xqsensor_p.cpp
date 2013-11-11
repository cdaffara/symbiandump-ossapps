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
#include "xqsensor_p.h"
#include "xqsensor.h"

#include <sensrvchannelfinder.h>

XQSensorPrivate::XQSensorPrivate(XQSensor& qq) : q(qq)
{
}

XQSensorPrivate::~XQSensorPrivate()
{
    delete iSensrvChannelFinder;
}

void XQSensorPrivate::DataError(CSensrvChannel& /*aChannel*/,
    TSensrvErrorSeverity /*aError*/)
{
}

CSensrvChannel* XQSensorPrivate::CreateChannelL(TSensrvChannelTypeId aType)
{
    TSensrvChannelInfo channelInfo;
    FindChannelL(aType, channelInfo);
    return CSensrvChannel::NewL(channelInfo);
}
    
void XQSensorPrivate::FindChannelL(TSensrvChannelTypeId aType, TSensrvChannelInfo& aChannelInfo)
{
    if (!iSensrvChannelFinder) {
        iSensrvChannelFinder = CSensrvChannelFinder::NewL();
    }

    RSensrvChannelInfoList sensrvChannelInfoList;
    CleanupClosePushL(sensrvChannelInfoList);

    TSensrvChannelInfo sensrvChannelInfo;
    sensrvChannelInfo.iChannelType = aType;

    iSensrvChannelFinder->FindChannelsL(sensrvChannelInfoList, sensrvChannelInfo);

    if(sensrvChannelInfoList.Count() == 0) {
        User::Leave(KErrNotFound);
    }
    aChannelInfo = sensrvChannelInfoList[0];
    CleanupStack::PopAndDestroy(&sensrvChannelInfoList);
}

// End of file
