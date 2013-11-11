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
/*
 * cxefeaturemanager.h
 *
 *  Created on: Dec 30, 2008
 *      
 */
#ifndef CXEFEATUREMANAGERIMP_H_
#define CXEFEATUREMANAGERIMP_H_

#include <QMetaType>
#include "cxefeaturemanager.h"
#include "cxeerror.h"

// forward declarations
class CxeSettings;



/*
* Handling and accessing configured run-time values for specific features
* Handles ICM and CxUi run-time features
*/
class CxeFeatureManagerImp : public CxeFeatureManager
{

public:
    
    CxeFeatureManagerImp(CxeSettings &settings);
    virtual ~CxeFeatureManagerImp();

    CxeError::Id isFeatureSupported(const QString &key, bool &value) const;
    CxeError::Id configuredValues(const QString &key, QList<int> &values);

private:
    CxeSettings &mSettings;

};

#endif /*CXEFEATUREMANAGERIMP_H_*/

// end  of file
