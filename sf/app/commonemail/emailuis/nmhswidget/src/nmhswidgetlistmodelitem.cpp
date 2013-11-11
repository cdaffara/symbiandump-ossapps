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
 * Description:
 *
 */

#include <nmmessageenvelope.h>
#include "nmhswidgetlistmodelitem.h"

/*!
 \class NmHsWidgetListModelItem
 \brief The NmHsWidgetListModelItem class represents data model for mailbox list.
 */

/*!
 Constructor
 */
NmHsWidgetListModelItem::NmHsWidgetListModelItem() :
    QStandardItem(), 
    mMessageEnvelope(NULL)
{
    NM_FUNCTION;
}

/*!
 Destructor
 */
NmHsWidgetListModelItem::~NmHsWidgetListModelItem()
{
    NM_FUNCTION;

    delete mMessageEnvelope;
}

/*!
 setItemMetaData. Set function for item related meta data object
 */
void NmHsWidgetListModelItem::setItemMetaData(NmMessageEnvelope *metaData)
{
    NM_FUNCTION;

    if (mMessageEnvelope) {
        delete mMessageEnvelope;
    }
    mMessageEnvelope = metaData;
    emitDataChanged();
}

/*!
itemMetaData. Get function for item related meta data object
 */
NmMessageEnvelope *NmHsWidgetListModelItem::itemMetaData()
{
    NM_FUNCTION;

    return mMessageEnvelope;
}

