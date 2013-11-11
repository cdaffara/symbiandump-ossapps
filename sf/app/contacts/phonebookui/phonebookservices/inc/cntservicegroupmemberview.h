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

#ifndef CNTSERVICEGROUPMEMBERVIEW_H
#define CNTSERVICEGROUPMEMBERVIEW_H

#include <QObject>
#include "cntgroupmemberview.h"

class CntServiceHandler;
class CntAbstractServiceProvider;

class CntServiceGroupMemberView : public CntGroupMemberView
{
    Q_OBJECT

public:
    CntServiceGroupMemberView( CntAbstractServiceProvider& aServiceProvider );
    ~CntServiceGroupMemberView();

    int viewId() const { return serviceGroupMemberView; }
    
private slots:
    void closeGroupMemberView();

private:
    CntAbstractServiceProvider& mProvider;
   
};

#endif // CNTSERVICEGROUPMEMBERVIEW_H

// EOF
