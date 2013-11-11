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

#ifndef RADIOCONTROLSERVICE_H
#define RADIOCONTROLSERVICE_H

// System includes
#include <xqserviceprovider.h>

// User includes

// Forward declarations
class RadioUiEngine;

class RadioControlService : public XQServiceProvider
{
    Q_OBJECT

public:

    RadioControlService( RadioUiEngine& engine );
    virtual ~RadioControlService();


public slots:

    void command( int commandId );

private: // data

    RadioUiEngine&  mUiEngine;

};

#endif // RADIOCONTROLSERVICE_H
