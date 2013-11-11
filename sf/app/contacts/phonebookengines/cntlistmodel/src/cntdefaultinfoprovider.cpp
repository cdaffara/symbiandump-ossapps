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
* Description: Default info provider plugin for CntListModel. It can provide
*              the phone number and the image url of a contact (text and
*              icon1 field respectively).
*
*/

#include <qtcontacts.h>
#include "cntdefaultinfoprovider.h"
#include <hbglobal.h>

/*!
    /return the info fields supported by this provider
 */
ContactInfoFields CntDefaultInfoProvider::supportedFields() const
{
    // this provider does not have any info for the icon2 field
    return ContactInfoIcon1Field | ContactInfoTextField;
}

/*!
    The contact contains all the info this provider needs, so signals with the requested info
    fields are emitted immediately.

    /param contact the contact for which info is requested
    /param requestedInfo one or more of the flags in ContactInfoFields
 */
void CntDefaultInfoProvider::requestInfo(const QContact& contact, ContactInfoFields requestedInfo)
{
    if (requestedInfo & ContactInfoTextField) {
        QContactDetail detail = contact.preferredDetail("call");
        QString number;
        
        if (!detail.isEmpty())
        {
            number = static_cast<QContactPhoneNumber>(detail).number();
        }
        else
        {
            QList<QContactPhoneNumber> numbers = contact.details<QContactPhoneNumber>();
            if (numbers.count() > 1)
                number = hbTrId("txt_phob_dblist_val_ln_numbers", numbers.count());
            else if (numbers.count() == 1)
                number = numbers.at(0).number();
        }

        emit infoFieldReady(this, contact.localId(), ContactInfoTextField, number);
    }

    if (requestedInfo & ContactInfoIcon1Field) {
        QString imageUrl = contact.detail<QContactAvatar>().imageUrl().toString();
        emit infoFieldReady(this, contact.localId(), ContactInfoIcon1Field, imageUrl);
    }
}
