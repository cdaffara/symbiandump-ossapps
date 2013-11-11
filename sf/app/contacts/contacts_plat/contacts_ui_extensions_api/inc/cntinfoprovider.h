/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Interface for info provider plugins to class CntListModel.
*
*/

#ifndef CNTINFOPROVIDER_H
#define CNTINFOPROVIDER_H

#include <QObject>
#include <qcontact.h>

QTM_USE_NAMESPACE

/*
   Info providers may provide up to three different types of info fields:
   text, icon1 and icon2.
 */
enum ContactInfoField {
    ContactInfoTextField = 0x01,
    ContactInfoIcon1Field = 0x02,
    ContactInfoIcon2Field = 0x04,
    ContactInfoAllFields = ContactInfoTextField | ContactInfoIcon1Field | ContactInfoIcon2Field
};
Q_DECLARE_FLAGS(ContactInfoFields, ContactInfoField)
Q_DECLARE_OPERATORS_FOR_FLAGS(ContactInfoFields)

/*
   Interface for info provider plugins. Info provider plugins provide the kind of
   info that a listview with contacts wants. Examples includes phone number, image url
   and online status (text and/or icon).
 */
class CntInfoProvider : public QObject
{

public:
    /* 
       The unqiue name of the plugin.

       /return the id of the plugin
     */
    virtual QString id() const = 0;

    /* 
       Checks fields that the client can provide.
       
       /return all the ContactInfoFields that this plugin can provide to clients
     */
    virtual ContactInfoFields supportedFields() const = 0;

    /* 
       Requests info about a contact. The requested info fields are passed
       back to the client via infoFieldReady() signals. Ideally this function
       should not consume more than 50 ms of time. Info that takes longer to
       fetch should use some asynchronous way of getting the data.
       
       Info values are by default empty, so an empty value does not need not be
       sent back in response to this request. However, if a value *changes* and
       becomes empty, that will of course need to be signaled.
        
       /param contact the contact for which info is requested
       /param requestedInfo one or more of the flags in ContactInfoFields
     */
    virtual void requestInfo(const QContact& contact, ContactInfoFields requestedInfo) = 0;

signals:
    /* 
       The requested info fields are passed back to the client via infoFieldReady()
       signals, one signal per field.
       
       /param sender the provider that sends the signal
       /param contactId the if of the contact that this info is about
       /param field the field that is ready (text, icon1 or icon2)
       /param value the value of the info field
     */
    void infoFieldReady(CntInfoProvider* sender, int contactId, ContactInfoField field, const QString& value);
};

#endif
