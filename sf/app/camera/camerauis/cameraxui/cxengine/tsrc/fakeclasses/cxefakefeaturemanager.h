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

#ifndef CXEFAKEFEATUREMANAGER_H
#define CXEFAKEFEATUREMANAGER_H

#include "cxefeaturemanager.h"

class CxeFakeFeatureManager : public CxeFeatureManager
{
public:
    CxeFakeFeatureManager();
    virtual ~CxeFakeFeatureManager();

    CxeError::Id isFeatureSupported(const QString& key, bool& value) const;
    CxeError::Id configuredValues(const QString& key,QList<int>& values);
};

#endif // CXEFAKEFEATUREMANAGER_H

// end  of file
