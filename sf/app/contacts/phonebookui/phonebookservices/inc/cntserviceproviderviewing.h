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
 * Description: This class offers contact viewing UI services.
 *
 */

#ifndef CNTSERVICEPROVIDERVIEW_H
#define CNTSERVICEPROVIDERVIEW_H

#include <cntservicescontact.h>
#include <xqserviceprovider.h>
#include <cntviewparams.h>
#include <qtcontacts.h>

#include "cntabstractserviceprovider.h"

class CntServices;

QTM_USE_NAMESPACE

/*!
  This class offers contact viewing UI services.
*/
class CntServiceProviderViewing : public XQServiceProvider, public CntAbstractServiceProvider
{
    Q_OBJECT
public:
    CntServiceProviderViewing(CntServices& aServices, QObject *parent = 0);
    ~CntServiceProviderViewing();

public slots:  // operations offered to clients

    /**
     * Opens Contact Card.
     * @param aContactId
     */
    void openContactCard( int aContactId );

    /**
     * Opens Contact Card for non-saved contact which is not saved yet.
     * @param aFieldName
     * @param aValue The value for the field.
     */
    void openTemporaryContactCard( QString aFieldName, QString aFieldValue );
    
    /**
     * Opens Group Member View.
     * @param aContactId
     */
    void openGroup( int aContactId );

private: // from CntAbstractServiceProvider
    void CompleteServiceAndCloseApp(const QVariant& retValue);
    void overrideReturnValue(const QVariant& retValue);
    bool allowSubViewsExit();

private:
    CntServices& mServices;
    int mCurrentRequestIndex;
    bool mAllowExit;
    
    QVariant mOverriddenReturnValue;
};

#endif /* CNTSERVICEPROVIDERVIEW_H */
