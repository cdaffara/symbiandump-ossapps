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
#include <xqserviceutil.h>
#include <xqsharablefile.h>

#include "irfileviewservice.h"
#include "irplaylist.h"
#include "irapplication.h"
#include "irqsettings.h"
#include "irqlogger.h"

#define INTERNETRADIO_SERVICE_NAME "internet_radio_10_1.com.nokia.symbian.IFileView"

IRFileViewService::IRFileViewService(IRApplication *aApplication) : XQServiceProvider(INTERNETRADIO_SERVICE_NAME),
iApplication(aApplication), iPlayList(NULL)
{
    LOG_METHOD;
    if (XQServiceUtil::isService())
    {
        //Publishes all public slots on this object
        publishAll();
    }
}

IRFileViewService::~IRFileViewService()
{
    delete iPlayList;
}

IRPlayList* IRFileViewService::getPlayList() const
{
    return iPlayList;
}

void IRFileViewService::view(const QString &aFileName)
{
    LOG_METHOD;
    if (NULL == iPlayList)
    {
        iPlayList = new IRPlayList;
    }
    
    iPlayList->parseFile(aFileName);
    
    if (1 == iPlayList->getNumberOfEntries())
    {
        iApplication->launchStartingView(EIRView_PlayingView);
    }
    else if (iPlayList->getNumberOfEntries() > 1)
    {
        iApplication->launchStartingView(EIRView_PlsView);
    }
    else
    {
        //normal launch, launch starting view
        TIRViewId viewId = EIRView_CategoryView;
        iApplication->getSettings()->getStartingViewId(viewId);        
        iApplication->launchStartingView(viewId);
    }
}

void IRFileViewService::view(const XQSharableFile &aSharableFile)
{
    LOG_METHOD;
    QString name = aSharableFile.fileName();
    LOG(STRING2CHAR(name));
    view(name);
}
