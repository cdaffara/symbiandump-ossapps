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

#ifndef CNTSERVICEPROVIDERFETCH_H
#define CNTSERVICEPROVIDERFETCH_H

#include <cntservicescontact.h>
#include <xqserviceprovider.h>
#include <cntviewparams.h>
#include <qtcontacts.h>

#include "cntabstractserviceprovider.h"

class CntServices;

QTM_USE_NAMESPACE

/*!
  This class offers services through the old interface name & old signatures.
*/
class CntServiceProviderFetch : public XQServiceProvider, public CntAbstractServiceProvider
{
    Q_OBJECT
public:
    CntServiceProviderFetch(CntServices& aServices, QObject *parent = 0);
    ~CntServiceProviderFetch();

public slots:  // operations offered to clients

    /**
     * Opens contact selection view where multiple contacts can be selected.
     * @param title The view title
     * @param action Filter definition for filtering contacts based on supported action. Can be left empty.
     */
    void multiFetch(const QString &title,
               const QString &action );

    /**
     * Opens contact selection view where multiple contacts can be selected.
     * @param title The view title
     * @param action Filter definition for filtering contacts based on supported action. Can be left empty.
     */
    void singleFetch(const QString &title,
               const QString &action );

private: // from CntAbstractServiceProvider
    void CompleteServiceAndCloseApp(const QVariant& retValue);

private:
    CntServices& mServices;
    int mCurrentRequestIndex;
};

#endif /* CNTSERVICEPROVIDERFETCH_H */
