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

#ifndef NMMAILBOXLISTMODELITEM_H_
#define NMMAILBOXLISTMODELITEM_H_

#include <QStandardItem>

class NmMailboxMetaData;

class NmMailboxListModelItem : public QStandardItem
{
public:
    NmMailboxListModelItem();
    ~NmMailboxListModelItem();
    void setItemMetaData(NmMailboxMetaData *meta);
    NmMailboxMetaData *itemMetaData();
    void callEmitDataChanged();

private:
    NmMailboxMetaData* mMailbox;
};

#endif /* NMMAILBOXLISTMODELITEM_H_ */
