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

#include <qversitcontactexporter.h>
#include <qtcontacts.h>
#include <qversitproperty.h>

#ifndef CNTVCARDDETAILHANDLER_H
#define CNTVCARDDETAILHANDLER_H

class CntVCardDetailHandler : public QVersitContactExporterDetailHandlerV2 
{
public:
    inline CntVCardDetailHandler(){}
    inline void detailProcessed(const QContact& contact,
                                 const QContactDetail& detail,
                                 const QSet<QString>& processedFields,
                                 const QVersitDocument& document,
                                 QList<QVersitProperty>* toBeRemoved,
                                 QList<QVersitProperty>* toBeAdded) 
    {
        Q_UNUSED(contact)
        Q_UNUSED(detail)
        Q_UNUSED(processedFields)
        Q_UNUSED(document)
        Q_UNUSED(toBeRemoved)
        Q_UNUSED(toBeAdded)
    }
    
    inline void contactProcessed(const QContact& contact,
                                  QVersitDocument* document)
    {
        Q_UNUSED(contact)
        // If there is not a FN or N value use an empty FN value. This ensures
        // that the vCard created is valid. vCard standard requires a vCard to
        // have atlease a name (N) of formatted name (FN) field to be valid.
        bool vCardValid(false);
        foreach (const QVersitProperty& property, document->properties()) {
            const QString& name = property.name();
            if (name == QLatin1String("FN") || name == QLatin1String("N")) {
                vCardValid = true;
                break;
            }
        }
        
        if (!vCardValid) {
            QVersitProperty fnProperty;
            fnProperty.setName(QLatin1String("FN"));
            fnProperty.setValue(QString());
            document->addProperty(fnProperty);
        }
    }
};

#endif
