/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef USSDCOMMS_H
#define USSDCOMMS_H

#include <QObject>

class CPhCltUssdInt;

class UssdComms: public QObject
{
    Q_OBJECT

public:
    UssdComms(QObject* parent);
    ~UssdComms();

public slots:
    void appStarting();
    void appTerminating();
    void appToBackground();
    void appToForeground();
    void informExitReason(int aExitReason);

public:
    int send( const QString& message );
    void cancel();
    int errorCode();

private: // Data

    /** USSD server client api */
    CPhCltUssdInt *mUssd;

    /** error code */
    int mErrorCode;
};

#endif // USSDCOMMS_H

// End of file
