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

#ifndef NMMESSAGELISTMODELITEM_H_
#define NMMESSAGELISTMODELITEM_H_

#include <QMetaType>
#include <QStandardItem>
#include "nmuienginedef.h"
#include "nmcommon.h"

class NmMessageListModelItemPrivate;
class NmMessageEnvelope;


class NMUIENGINE_EXPORT NmMessageListModelItem : public QStandardItem
{
public:

    enum NmMessageItemType
    {
        NmMessageItemMessage = 1,
        NmMessageItemTitleDivider
    };

    NmMessageListModelItem();
    virtual ~NmMessageListModelItem();
    void setEnvelope(const NmMessageEnvelope &envelope);
    void setEnvelope(NmMessageEnvelope *envelope);
    const NmMessageEnvelope &envelope() const;
    NmMessageEnvelope *envelopePtr();
    NmMessageItemType itemType() const;
    void setItemType(NmMessageItemType type);
    QString titleDivider() const;
    void setTitleDivider(const QString &divider);
    bool expanded() const;
    void setExpanded(bool expanded);
    void callEmitDataChanged();

private:

    NmMessageListModelItemPrivate *d;
};

Q_DECLARE_METATYPE(NmMessageListModelItem*)

#endif /* NMMESSAGELISTMODELITEM_H_ */
