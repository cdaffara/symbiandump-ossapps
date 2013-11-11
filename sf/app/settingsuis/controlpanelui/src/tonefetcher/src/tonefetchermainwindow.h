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
 * 
 * Description:
 *     The header file of Tone Fetcher Mainwindow
 */
#ifndef TONEFETCHERMAINWINDOW_H
#define TONEFETCHERMAINWINDOW_H

#include <hbmainwindow.h>
#include "tonefetcher.h"
#include <QCoreApplication>

class ToneFetcherMainWindow : public HbMainWindow
{
    Q_OBJECT
    
public:
    explicit ToneFetcherMainWindow(QWidget *parent = 0);
    virtual ~ToneFetcherMainWindow();
    
private:
    ToneFetcher *mToneFetcher;
};

#endif // TONEFETCHERWINDOW_H
//End of File
