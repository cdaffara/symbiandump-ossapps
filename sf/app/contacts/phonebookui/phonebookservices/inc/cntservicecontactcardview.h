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

#ifndef CNTSERVICECONTACTCARDVIEW_H
#define CNTSERVICECONTACTCARDVIEW_H

#include <QObject>
#include "cntcontactcardview.h"

class CntServiceHandler;
class CntAbstractServiceProvider;

class CntServiceContactCardView : public CntContactCardView
{
    Q_OBJECT

public:
    CntServiceContactCardView( CntAbstractServiceProvider& aServiceProvider );
    ~CntServiceContactCardView();

    int viewId() const { return serviceContactCardView; }
    
private slots:
    void closeContactCard(int value);

private:
    CntAbstractServiceProvider& mProvider;
   
};

#endif // CNTSERVICECONTACTCARDVIEW_H

// EOF
