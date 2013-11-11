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
* Description: Interface for application launch.
*
*/

#ifndef PHONEAPPLAUNCHER_H
#define PHONEAPPLAUNCHER_H

#include <QObject>
#include <QRunnable>

class PhoneAppLauncherTask : public QRunnable
{
public:
    void run();
public:    
    QString mService;
    QString mInterface; 
    QString mOperation;
    QList<QVariant> mArguments;
};

/*!
 * \class PhoneAppLauncher
 * \brief Interface for application launch.
*/
class PhoneAppLauncher : 
    public QObject
{
    Q_OBJECT

public:
    
    /**
     * Constructor.
     */
    PhoneAppLauncher(QObject *parent = 0);

    /**
     * Destructor.
     */
    ~PhoneAppLauncher();

public:
    /**
     * Launch app
     */
    void launchMessaging(
            const QString& toField,
            const QString& name,
            const QString &messageBody);
    
    void launchContacts();
    
    void launchLogs(
            int activatedView,
            bool showDialpad,
            const QString& dialpadText);


private:
    void sendServiceRequest(
            const QString& service,
            const QString& interface, 
            const QString& operation,
            const QList<QVariant>& arguments,
            const bool foreground);
};

#endif // PHONEAPPLAUNCHER_H
