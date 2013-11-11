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

#ifndef MSGERRORNOTIFIER_H
#define MSGERRORNOTIFIER_H

#include <QObject>
class MsgErrorNotifierSvc;

class MsgErrorNotifier: public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgErrorNotifier(QObject *parent = 0);

    /**
     * Destructor
     */
    ~MsgErrorNotifier();

private:
    /**
     * Own. The notifier service that pops up a window for
     * failed message.
     */
    MsgErrorNotifierSvc* mErrorNotifier;
};

#endif // MSGERRORNOTIFIER_H
