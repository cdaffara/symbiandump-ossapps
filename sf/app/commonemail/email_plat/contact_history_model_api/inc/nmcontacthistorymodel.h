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
* Description: Public declaration of Contact History Model API
*
*/
#ifndef NMCONTACTHISTORYMODEL_H
#define NMCONTACTHISTORYMODEL_H

#include <QMetaType>
#include <QAbstractListModel>
#include <QSharedDataPointer>

#include "nmcontacthistorymodelcommon.h"

// NmContactHistoryModelSubItem - display name is one item, email address/phone number another
class NmContactHistoryModelSubItem
{
public:

inline NmContactHistoryModelSubItem()
{
}

inline ~NmContactHistoryModelSubItem()
{
}
    // email address, or concatenated firstName+' '+lastName.
    QString mItemText;
    // amount of items in mMatchingRanges is 2*mItemTexts.length() because for each text there is begin/end index
    // TODO: Fix above^ always two indeces
    // for example "Ville Virtanen" with query "vi" returns indeces 0,1,6,7
    QList<int> mMatchingRanges;
};

class NmContactHistoryModelPrivate;
class NmContactHistoryModelItemData;

class NMCHMAPI_EXPORT NmContactHistoryModelItem
{
public:
    NmContactHistoryModelItem();
    NmContactHistoryModelItem(const NmContactHistoryModelItem &other);
    virtual ~NmContactHistoryModelItem();

public:
    // contact id is 0xDEAD if not associated with a contact item
    unsigned int contactId() const;

    void setContactId(unsigned int contactId);

    int subItemCount() const;

    NmContactHistoryModelSubItem subItemAt(int index) const;

    // First entry is display name, second email address.
    QList<NmContactHistoryModelSubItem> subEntries() const;

    void appendSubItem(NmContactHistoryModelSubItem entry);

private:
     //
    QSharedDataPointer<NmContactHistoryModelItemData> d;
};
Q_DECLARE_METATYPE(NmContactHistoryModelItem)

/**
 * Model for contact history queries.
 *
 */
class NMCHMAPI_EXPORT NmContactHistoryModel : public QAbstractListModel
{
    Q_OBJECT

public: // The exported API

    /**
     * Constructor
     * @param modelType
     */
    explicit NmContactHistoryModel(
        const NmContactHistoryModelType modelType);

    /**
     * Destructor
     */
    virtual ~NmContactHistoryModel();

public: // From QAbstractItemModel

    /**
     * Get number of rows currently in the model.
     * @return number of rows
     */
    int rowCount(const QModelIndex &parent) const;

    /**
     * Get various data from the model. Fetched data type is defined
     * by role input parameter. Only Qt::DisplayRole is supported.
     */
    QVariant data(const QModelIndex &index, int role) const;

signals:

    /**
     * Signaled once model is ready.
     * @param err, 0 if building model was success
     */
    void modelCompleted(int err);

public slots:

    // TODO: query could keep previous query in order to check if results will only get narrowed (a char is appended)
    void query(const QString &query);

private slots:

    /**
     * Signaled once query is completed, emits modelCompleted.
     * @param err, 0 if building model was success
     */
    void handleQueryCompleted(int err);

private:
    /*
     * Private Implementation
     * Own
     */
    NmContactHistoryModelPrivate *d_ptr;

private:

    friend class TestNmContactHistoryModel;
};

#endif //NMCONTACTHISTORYMODEL_H
