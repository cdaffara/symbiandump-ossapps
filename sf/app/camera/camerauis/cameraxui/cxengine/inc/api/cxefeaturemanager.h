/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXEFEATUREMANAGER_H
#define CXEFEATUREMANAGER_H

#include <QMetaType>
#include "cxeerror.h"


/*!
* Handling and accessing configured run-time values for specific features
* Handles ICM and CxUi run-time features
*/
class CxeFeatureManager
{
public:
    virtual ~CxeFeatureManager() {}

    /*!
    * Returns if a feature is supported or not
    * true -> supported
    * false -> not supported
    */
    virtual CxeError::Id isFeatureSupported(const QString& key, bool& value) const = 0;

    /*!
    * We retrieve all the configured values for the given key
    */
    virtual CxeError::Id configuredValues(const QString& key, QList<int>& values) = 0;

protected:
    CxeFeatureManager() {}
};

#endif // CXEFEATUREMANAGER_H

// end  of file
