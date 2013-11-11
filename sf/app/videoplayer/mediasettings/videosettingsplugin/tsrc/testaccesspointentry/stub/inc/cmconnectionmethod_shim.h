/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: CmConnectionMethodShim stub
 * 
 */

#ifndef CMCONNECTIONMETHOD_SHIM_H
#define CMCONNECTIONMETHOD_SHIM_H

#include <qstring.h>
#include <qhash.h>
#include "cmmanager_shim.h"

class CmConnectionMethodShim
{

public:

    /*!
     *  Constructor.
     *  
     *  @param[in] cm CM Manager connection method object.
     */
    CmConnectionMethodShim(uint id, uint iapId, QString name);

    /*!
     *  Destructor.
     */
    ~CmConnectionMethodShim();
    
    uint getIntAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;

    QString getStringAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;
    
public: // data
    
    QHash<CMManagerShim::ConnectionMethodAttribute, QVariant> mAttributes;
    
};

#endif /* CMCONNECTIONMETHOD_SHIM_H */
