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

#ifndef NMHSWIDGETLISTMODELITEM_H_
#define NMHSWIDGETLISTMODELITEM_H_

#include <QStandardItem>

class NmMessageEnvelope;

class NmHsWidgetListModelItem : public QStandardItem
{
public:
    NmHsWidgetListModelItem();
    ~NmHsWidgetListModelItem();
    void setItemMetaData(NmMessageEnvelope* metaData);
    NmMessageEnvelope *itemMetaData();

private:
    NmMessageEnvelope* mMessageEnvelope;
};

#endif /* NMHSWIDGETLISTMODELITEM_H_ */
