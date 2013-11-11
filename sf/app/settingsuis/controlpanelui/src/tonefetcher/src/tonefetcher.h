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
 *     The header file for tone fetcher.
 *     
 */
#ifndef TONEFETCHER_H
#define TONEFETCHER_H

#include <xqserviceprovider.h>

class HbMainWindow;
class ToneFetcher : public XQServiceProvider
{

    Q_OBJECT
    
public:
    explicit ToneFetcher(HbMainWindow *mainWindow = 0);
    ~ToneFetcher();
    /*
     * return the tone's absolution path to the service requestor.
     */
    void complete();
    /*
     * currently only one request is allowed.
     */
    bool isActive();
        
public slots:
    /*
     * tone fetcher service's operation
     */
    void fetch();
    
private slots:
    /*
     * set the selected tone's path to be the return value
     */
    void setSelectedPath(const QString &tonePath);

private:
    HbMainWindow *mMainWindow;
    int mAsyncRequestIndex;
    QVariant mReturnValue;
};

#endif // TONEFETCHER_H
