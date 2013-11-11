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

#ifndef INFOWIDGETNETWORKHANDLER_H_
#define INFOWIDGETNETWORKHANDLER_H_

#include <QObject>
#include <nwhandlingengine.h>
#include <cnwsession.h> 

// Class declaration
class InfoWidgetNetworkHandler : public QObject, public MNWMessageObserver 
    {
    Q_OBJECT
    
public:
    InfoWidgetNetworkHandler(QObject *parent = NULL);
    ~InfoWidgetNetworkHandler();    

    bool createSession(); 
    
    QString serviceProviderName() const;
    bool serviceProviderNameDisplayRequired() const; 
    QString homeZoneTextTag() const; 
    int homeZoneIndicatorType() const; 
    QString mcnName() const; 
    int mcnIndicatorType() const; 
    int networkRegistrationStatus() const;  
    bool isOnline() const;

public: // From MNWMessageObserver
    void HandleNetworkMessage(const TNWMessages aMessage);
    void HandleNetworkError(const TNWOperation aOperation, TInt aErrorCode);

public slots: 
    void suspend(); 
    void resume();
    void enableMcn(); 
    void disableMcn(); 
    
protected:     
    void writeMcnDisplayState(bool enabled);
    bool readMcnDisplayState(); 
    
signals: 
    void networkDataChanged();
    void networkError(int operation, int errorCode);
    
private:
    TNWInfo m_nwInfo;
    QScopedPointer<CNWSession> m_nwSession;
    }; 

#endif /* INFOWIDGETNETWORKHANDLER_H_ */


