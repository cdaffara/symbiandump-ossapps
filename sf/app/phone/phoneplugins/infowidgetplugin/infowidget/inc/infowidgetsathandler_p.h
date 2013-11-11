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

#ifndef INFOWIDGETSATHANDLER_P_H_
#define INFOWIDGETSATHANDLER_P_H_

#include <rsatservice.h>
#include <rsatsession.h>
#include <QString>

// Forward declarations 
class InfoWidgetSatHandler; 

// Class declaration
class InfoWidgetSatHandlerPrivate: public CActive
{
public:  
    
    InfoWidgetSatHandlerPrivate(
            InfoWidgetSatHandler *iwSatHandler, 
            RSatService& iSatService);
    ~InfoWidgetSatHandlerPrivate();
 
    void startObserving();
    bool connect();
    void disconnect();
    void activate();
    void getIdleModeData();

protected: // From CActive
    void RunL();
    void DoCancel();
       
private: // Data
    InfoWidgetSatHandler *m_satHandler; // Not owned. 
    RSatService& m_satService;
    RSatSession m_satSession;
    RSatService::TSATIdleResult m_idleResult;

    QString m_idleTxt;
};

#endif // INFOWIDGETSATHANDLER_P_H_

