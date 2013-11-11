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

#include <hbnamespace.h>
#include <nmmessageenvelope.h>
#include "nmhswidgetlistmodel.h"
#include "nmhswidgetlistmodelitem.h"

/*!
    \class NmHsWidgetListModel
    \brief The NmHsWidgetListModel class represents data model for message list.
    @alpha

    The NmHsWidgetListModel class uses NmHsWidgetListModelItem class to represent data returned in its'
    data method to get all information needed for one list row for a widget by calling the method
    once.
*/

/*!
    Constructor
 */
NmHsWidgetListModel::NmHsWidgetListModel(QObject *parent)
:QStandardItemModel(parent)
{
    NM_FUNCTION;
}

/*!
	Destructor
 */
NmHsWidgetListModel::~NmHsWidgetListModel()
{
    NM_FUNCTION;
    
    clear();
}

/*!
    Returns data specified by \a index. Only Qt::DisplayRole is supported in \a role.
    The refresh method must have been called before this method can return any real data.
 */
QVariant NmHsWidgetListModel::data(const QModelIndex &index, int role) const
{
    NM_FUNCTION;
    
    QVariant qVariant;
    if (index.isValid() && Qt::DisplayRole == role) {
        NmHsWidgetListModelItem *item = static_cast<NmHsWidgetListModelItem*>(itemFromIndex(index));
        NmMessageEnvelope *mailbox = item->itemMetaData();
        qVariant = QVariant::fromValue(mailbox);
    }
    return qVariant;
}

/*!
    This refreshes the data of the model.
    NOTE: safe guard any call to this function with try-catch.
 */
void NmHsWidgetListModel::refresh(
        QList<NmMessageEnvelope*> &envelopeList)
{
    NM_FUNCTION;
    
    clear();
    foreach(NmMessageEnvelope *env, envelopeList){
       NmHsWidgetListModelItem *item = createMessageListModelItem(env);
       appendRow(item);
    }
}


/*!
    Create mailbox item
    \param mailbox
 */
NmHsWidgetListModelItem* NmHsWidgetListModel::createMessageListModelItem(const NmMessageEnvelope* envelope)
{
    NM_FUNCTION;
    
    NmMessageEnvelope *newMeta = new NmMessageEnvelope(*envelope);
    NmHsWidgetListModelItem *item = new NmHsWidgetListModelItem();
    item->setItemMetaData(newMeta);
    item->setData(Hb::StandardItem, Hb::ItemTypeRole);
    return item;
}
