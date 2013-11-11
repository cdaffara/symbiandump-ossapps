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
 *     The main window function for Tone Fetcher
 */

#include "tonefetchermainwindow.h"
#include <hbview.h>

ToneFetcherMainWindow::ToneFetcherMainWindow(QWidget *parent)
    : HbMainWindow(parent)
{
    mToneFetcher = new ToneFetcher(this); 
   
}

ToneFetcherMainWindow::~ToneFetcherMainWindow()
{
    delete mToneFetcher;
}
//End of File
