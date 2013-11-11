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
 *     The source file for tone fetcher.
 *     
 */
#include "tonefetcher.h"
#include <hbmainwindow.h>
#include "tonefetcherview.h"
#include "tonefetchermainwindow.h"
#include "tonefetcherutils.h"
#include <QChar>
#include <QDir>
#include <tonefetcherlogger.h>

ToneFetcher::ToneFetcher(HbMainWindow *mainWindow)
    : XQServiceProvider(QString("tonefetcher.com.nokia.symbian.IToneFetch"), mainWindow),
      mMainWindow(mainWindow),
      mAsyncRequestIndex(-1),
      mReturnValue(0)
{
    //publish tone service
    publishAll();
}

ToneFetcher::~ToneFetcher()
{

}

void ToneFetcher::fetch()
{
    mAsyncRequestIndex = setCurrentRequestAsync();    
    if (mMainWindow) {
        mMainWindow->show();
        ToneFetcherView *toneView = new ToneFetcherView(this);
        if (toneView) {            
            connect(toneView, SIGNAL(itemSelected(QString)), this, SLOT(setSelectedPath(QString)));
            mMainWindow->addView(toneView);
            mMainWindow->setCurrentView(toneView);            
        } else {
            TF_LOG("ToneFetcher::fetch: ToneFetcherView failed to be created");
        }
    }
}

void ToneFetcher::complete()
{
    completeRequest(mAsyncRequestIndex, mReturnValue);    
}

bool ToneFetcher::isActive()
{
    return mAsyncRequestIndex > 0;
}

void ToneFetcher::setSelectedPath(const QString & tonePath)
{    
    mReturnValue.setValue(ToneFetcherUtils::normalizeSeperator(tonePath));
}
//End of File
