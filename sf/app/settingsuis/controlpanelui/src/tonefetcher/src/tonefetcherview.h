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
 *     The header file for tone fetcher view.
 *     
 */

#ifndef TONEFETCHERVIEW_H
#define TONEFETCHERVIEW_H

#include <hblistwidget.h>
#include <hbview.h>
class ToneFetcherWidget;
class HbAction;
class ToneFetcher;

class ToneFetcherView : public HbView
{
    Q_OBJECT
    
public:
    explicit ToneFetcherView(ToneFetcher *service);
    ~ToneFetcherView();
    void quit();

signals:
        void itemSelected(const QString &path);
        
private:
    void initMainWidget();
    void initToolBar();
    /*
     * tool bar actions must to be removed in case the app crashes.
     */
    void removeToolBarAction();
    
private slots:
    void on_leftAction_triggered();
    void on_rightAction_triggered();
    void enableToolBar(bool enable);
    
private:
    ToneFetcherWidget *mWidget;
    ToneFetcher *mServiceProvider; //not own
    HbAction *mToolBarLeftAction;
    HbAction *mToolBarRightAction;
    };

#endif /* TONEFETCHERVIEW_H */
