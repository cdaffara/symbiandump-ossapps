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

#ifndef CNTSERVICEEDITVIEW_H
#define CNTSERVICEEDITVIEW_H

#include <QObject>
#include "cnteditview.h"
#include "cntabstractserviceprovider.h"

class CntServiceHandler;

class CntServiceEditView : public CntEditView
{

Q_OBJECT

public:

    CntServiceEditView( CntAbstractServiceProvider& aServiceProvider );
    ~CntServiceEditView();

    int viewId() const { return serviceEditView; }

private slots:
    void doContactUpdated(int aSuccess);
    void doContactRemoved(bool aSuccess);
    void doChangesDiscarded();

private:
    CntAbstractServiceProvider& mProvider;
    
};

#endif //CNTSERVICEEDITVIEW_H
// EOF
