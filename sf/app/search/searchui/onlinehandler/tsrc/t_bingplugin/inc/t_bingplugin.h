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
 * Description:   Declaration of the search google plugin.
 *
 */

#ifndef T_BINGLUGIN_H
#define T_BINGLUGIN_H

#include <onlinesearchplugin.h>

#include <QPointer>
#include <QDate>

class bingplugin : public OnlineSearchPlugin
    {
Q_OBJECT

public:

    // Default Constructor
    bingplugin();
    // Destructor
    ~bingplugin();

protected:

    // Overrides from OnlineSearchPlugin
    void initializePlugin();
    void activatePlugin();
    void activatePlugin(const QString &searchQuery);
    void deactivatePlugin();
    void shutdownPlugin();

private slots:
    void BackEvent();
private:
    };

#endif // T_BINGLUGIN_H
