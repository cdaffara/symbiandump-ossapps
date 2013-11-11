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

#ifndef CNTSERVICEPROVIDEROLD_H
#define CNTSERVICEPROVIDEROLD_H

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
class CntServiceProviderOld : public XQServiceProvider, public CntAbstractServiceProvider
{
    Q_OBJECT
public:
    CntServiceProviderOld(CntServices& aServices, QObject *parent = 0);
    ~CntServiceProviderOld();

public slots:  // operations offered to clients

    /**
     * Interface name has changed (see other impl. class)
     * @deprecated
     * Replaced with:
     *      multiFetch(QString, QString, QString)
     *      com.nokia.symbian.IContactFetch.
     */
    void fetch(const QString &title,
               const QString &action,
               const QString &filter);

    /**
     * Interface name has changed (see other impl. class)
     * @deprecated
     * Replaced with:
     *      singleFetch(QString, QString, QString)
     *      com.nokia.symbian.IContactFetch.
     */
    void Dofetch(const QString &title,
                 const QString &action,
                 const QString &filter,
                 const QString &mode = KCntNoSelectionMode);

    /**
     * Interface name has changed (see other impl. class)
     * @deprecated
     * Replaced with:
     *      editCreateNew(QString, QString)
     *      com.nokia.symbian.IContactEdit
     */
    void editCreateNew(const QString& definitionName, const QString& value);

    /**
     * Interface name has changed (see other impl. class)
     * @deprecated
     * Replaced with:
     *      editCreateNewFromVCard(QString)
     *      com.nokia.symbian.IContactEdit
     */
    void editCreateNew(const QString& vCardFile);
    
    /**
     * Interface name has changed (see other impl. class)
     * @deprecated
     * Replaced with:
     *      editUpdateExisting(QString, QString)
     *      com.nokia.symbian.IContactEdit
     */
    void editUpdateExisting(const QString& definitionName, const QString& value);
    
    /**
     * Interface name has changed (see other impl. class)
     * @deprecated
     * Replaced with:
     *      openContactCard(int contactId)
     *      com.nokia.symbian.IContactView
     */
    void open(int contactId);

    /**
     * Interface name has changed (see other impl. class)
     * @deprecated
     * Replaced with:
     *      openContactCard(QString, QString)
     *      com.nokia.symbian.IContactView
     */
    void open(const QString& definitionName, const QString& value);

private: // from CntAbstractServiceProvider
    void CompleteServiceAndCloseApp(const QVariant& retValue);
    void overrideReturnValue(const QVariant& retValue);

private:
    CntServices& mServices;
    int mCurrentRequestIndex;
    
    QVariant mOverriddenReturnValue;
};

#endif /* CNTSERVICEPROVIDEROLD_H */
