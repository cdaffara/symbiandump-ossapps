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
 * Description: This class offers contact editing UI services.
 *
 */

#ifndef CNTSERVICEPROVIDEREDIT_H
#define CNTSERVICEPROVIDEREDIT_H

#include <cntservicescontact.h>
#include <xqserviceprovider.h>
#include <cntviewparams.h>
#include <qtcontacts.h>

#include "cntabstractserviceprovider.h"

class CntServices;

QTM_USE_NAMESPACE

/*!
  This class offers contact editing UI services.
*/
class CntServiceProviderEdit : public XQServiceProvider, public CntAbstractServiceProvider
{
    Q_OBJECT
public:
    CntServiceProviderEdit(CntServices& aServices, QObject *parent = 0);
    ~CntServiceProviderEdit();

public slots:  // operations offered to clients

    /**
     * Launch editor for creating a new contact with a given detail.
     * @param definitionName The field type
     * @param value The value for the field.
     */
    void editCreateNew(const QString& definitionName, const QString& value);

    /**
     * Launch editor for creating a new contact with a given detail and subtype.
     * If the given subtype string is empty, then QContactPhoneNumber::SubTypeMobile is used.
     * @param definitionName The field type
     * @param value The value for the field.
     * @param subType The sub type for the field. (for eg. QContactPhoneNumber::SubTypeLandline)
     */
    void editCreateNew(const QString &definitionName, const QString &value, const QString &subType);

    /**
     * Launch editor for creating a new contact based on vCard file indicated in arg.
     * @param vCardFile The vCard file name and path.
     */
    void editCreateNewFromVCard(const QString &vCardFile);
    
    /**
     * Open the editor for an existing contact.'
     * @param contactId
     */
    void editExisting(int contactId);
    
    /**
     * User can first choose a contact, and then an editor is opened.
     * A new detail is provided by caller and it will be prefilled in the editor.
     * @param definitionName The field type
     * @param value The value for the field.
     */
    void editUpdateExisting(const QString &definitionName, const QString &value);

    /**
     * User can first choose a contact, and then an editor is opened.
     * A new detail is provided by caller and it will be prefilled in the editor.
     * The detail sub type can also be given.
     * @param definitionName The field type
     * @param value The value for the field.
     * @param subType The sub type for the field. (for eg. QContactPhoneNumber::SubTypeLandline)
     */
    void editUpdateExisting(const QString &definitionName, const QString &value, const QString &subType);

private: // from CntAbstractServiceProvider
    void CompleteServiceAndCloseApp(const QVariant& retValue);

private:
    CntServices& mServices;
    int mCurrentRequestIndex;
};

#endif /* CNTSERVICEPROVIDEREDIT_H */
