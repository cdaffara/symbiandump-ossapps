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
#include "cntlistmodel_p.h"
#include "cntlistmodel.h"
#include "cntcache.h"
#include "cntdebug.h"

#include <qtcontacts.h>
#include <QSet>
#include <QTimerEvent>

#include <hbindexfeedback.h>
#include <hbframebackground.h>
#include <hbframedrawer.h>
#include <xqsettingsmanager.h> // for reading cenrep keys
#include <xqsettingskey.h>

const uint dummyMyCardId = 0;

/*!
 * Construct a new CntListModel object using manager as the QContactManager
 * instance to communicate with the contacts database.
 *
 * \param manager A QContactManager instance to be used for
 *  communications with the contacts persistant store.
 */
CntListModel::CntListModel(QContactManager* manager,
                           const QContactFilter& contactFilter,
                           bool showMyCard,
                           QObject *parent)
    : QAbstractListModel(parent)
{
    CNT_ENTRY

    // create icons
    m_defaultIcon = HbIcon("qtg_large_avatar");
    m_defaultMyCardIcon = HbIcon("qtg_large_avatar_mycard");

    // set up data
    d = new CntListModelData(contactFilter, showMyCard);
    d->m_contactManager = manager;
    d->m_cache = CntCache::instance(d->m_contactManager);
    d->m_myCardId = d->m_contactManager->selfContactId();     
    updateContactIdsArray();
    
    // get current setting how to show an item in the name list and subscribe for changes
    d->m_Settings = new XQSettingsManager;
    d->m_NameListRowSettingkey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
        KCRCntSettings.iUid,
        KCntNameListRowSetting);
    d->m_currentRowSetting = d->m_Settings->readItemValue(*d->m_NameListRowSettingkey,
        XQSettingsManager::TypeInt).toInt();
    d->m_Settings->startMonitoring(*d->m_NameListRowSettingkey, XQSettingsManager::TypeInt);
    connect(d->m_Settings, SIGNAL(valueChanged(const XQSettingsKey&, const QVariant&)), this, SLOT(handleRowSettingChanged(const XQSettingsKey&, const QVariant&)));
    
    // listen to cache for changes in contacts
    connect(d->m_cache, SIGNAL(contactInfoUpdated(QContactLocalId)), this, SLOT(handleContactInfoUpdated(QContactLocalId)));
    connect(d->m_cache, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), this, SLOT(handleAdded(const QList<QContactLocalId>&)));
    connect(d->m_cache, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), this, SLOT(handleChanged(const QList<QContactLocalId>&)));
    connect(d->m_cache, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), this, SLOT(handleRemoved(const QList<QContactLocalId>&)));
    connect(d->m_cache, SIGNAL(dataChanged()), this, SLOT(refreshModel()));

    // listen to contactmanager for changes in relationships or mycard
    connect(d->m_contactManager, SIGNAL(selfContactIdChanged(const QContactLocalId&, const QContactLocalId&)), this, SLOT(handleMyCardChanged(const QContactLocalId&, const QContactLocalId&)));
    connect(d->m_contactManager, SIGNAL(relationshipsAdded(const QList<QContactLocalId>&)), this, SLOT(handleAddedRelationship(const QList<QContactLocalId>&)));
    connect(d->m_contactManager, SIGNAL(relationshipsRemoved(const QList<QContactLocalId>&)), this, SLOT(handleRemovedRelationship(const QList<QContactLocalId>&)));

    CNT_EXIT
}

CntListModel::~CntListModel()
{
}

/*!
 * Return the data to be used by the view or delegates for a particular
 * item and role.
 *
 * \param index The index of the item to return data about.
 * \param role The data should be relevant for this particular purpose.
 * \return QVariant The data for the specified index and role.
 */
QVariant CntListModel::data(const QModelIndex &index, int role) const
{
    CNT_ENTRY
    int row = index.row();

    // check that row is ok
    if (!validRowId(row)) {
        // invalid row
        return QVariant();
    }
    
    // update current contact if needed
    if (row != d->m_currentRow ) {
        if (d->m_contactIds[row] == dummyMyCardId) {
            // row contains dummy MyCard, so create dummy CntContactInfo
            d->m_currentContact = CntContactInfo();
        }
        else {
            d->m_currentContact = d->m_cache->fetchContactInfo(row, d->m_contactIds);
        }
        d->m_currentRow = row;
    }
    
    if (role == Qt::DisplayRole) {
        return dataForRole(row, role);
    }
    else if (role == Hb::IndexFeedbackRole) {
        if (row == 0 && (d->m_myCardId == d->m_contactIds[0] || dummyMyCardId == d->m_contactIds[0])) {
            // do not include MyCard in index feedback
            return QVariant();
        }
        else {
            return dataForRole(row, role).toStringList().at(0).toUpper();
        }
    }
    else if (role == Qt::BackgroundRole) {
        if (d->m_myCardId == d->m_contactIds[row] || dummyMyCardId == d->m_contactIds[row]) {
            return HbFrameBackground("qtg_fr_list_parent_normal", HbFrameDrawer::NinePieces);
        }
    }
    else if (role == Qt::DecorationRole) {
        if (d->m_currentRowSetting == CntTwoRowsNameAndPhoneNumber) {
            //icon fits only if user selected 2 rows in each name list item
            QList<QVariant> icons;
            HbIcon avatar = d->m_currentContact.icon1();
            HbIcon statusIcon = d->m_currentContact.icon2();
        
            if (!avatar.isNull()) {
                icons.append(avatar);
            }
            else if (d->m_myCardId == d->m_contactIds[row] || dummyMyCardId == d->m_contactIds[row]) {
                icons.append(m_defaultMyCardIcon);
            }
            else {
                icons.append(m_defaultIcon);
            }
    
            if (!statusIcon.isNull()) {
                icons.append(statusIcon);
            }
    
            return icons;
        }
        else {
            return QVariant();
        }
    }
    CNT_EXIT
    return QVariant();
}

/*!
 * Get the number of rows (contacts) in this model.
 *
 * \param parent Optional parent index value.
 * \return Number of contacts in this model.
 */
int CntListModel::rowCount(const QModelIndex& /*parent*/) const
{
    return d->m_contactIds.count();
}

/*!
 * Read a full contact entry from the database for the given model
 * index value. Only the row part of the index information will be
 * read. This is just an overload of CntListModel::contact() that
 * supports old behaviour and calls:
 *      CntListModel::contact(int row);
 *
 * The entry at the requested row will have its full contact information
 * (all fields) read from the database and returned as a QContact instance.
 *
 * \param index Index for the sought contact entry in this model.
 * \return A newly constructed QContact instance for this entry - ownership
 *  is transferred to the caller.
 *
 */
QContact CntListModel::contact(const QModelIndex &index) const
{
    return contact(index.row());
}

/*!
 * Returns the id for the contact at the requested row.
 *
 * \param index Index for the sought contact entry in this model.
 * \return The id for the contact, 0 if invalid index.
 *
 */
QContactLocalId CntListModel::contactId(const QModelIndex &index) const
{
    CNT_ENTRY

    if (!validRowId(index.row())) {
        return 0;
    }

    CNT_EXIT
    return d->m_contactIds[index.row()];
}

/*!
 * Return an index that points to the row relating to the supplied contact.
 *  E.g. if the contact is at row 7, the index with the following properties
 *  is returned:
 *      index.row() == 7
 
 * \param contact The contact for whose row an index is required
 * \return a QModelIndex with the row set to match that of the contact.
 */
QModelIndex CntListModel::indexOfContact(const QContact &contact) const
{
    return createIndex(rowId(contact.localId()), 0);
}

/*!
 * Return an index that points to the row relating to the supplied contact id.
 *  E.g. if the contact with this id is at row 7, the index with the following
 *  properties is returned:
 *      index.row() == 7
 
 * \param contactId The id of the contact for whose row an index is required
 * \return a QModelIndex with the row set to match that of the contact id.
 */
QModelIndex CntListModel::indexOfContactId(const QContactLocalId &contactId) const
{
    return createIndex(rowId(contactId), 0);
}

/*!
 * Set new filter and sort order for the model.
 *
 * \param contactFilter New contact filter.
 * \param contactSortOrders New sort order.
 */
void CntListModel::setFilter(const QContactFilter& contactFilter)
{
    CNT_ENTRY
    
    d->setFilter(contactFilter);
    
    //refresh model
    updateContactIdsArray();

    beginResetModel();
    reset();
    endResetModel();
    
    CNT_EXIT
}

/*!
 * Enable/disable MyCard appearance in the model.
 *
 * \param enabled Status of MyCard appearance in the model.
 */
void CntListModel::showMyCard(bool enabled)
{
    CNT_ENTRY
    if (d->m_showMyCard == enabled) {
        return;
    }
    
    QContactLocalId myCardId = d->m_myCardId;
    if (enabled) {
        //add MyCard to the list
        if (myCardId <= 0) {
            // create a placeholder for MyCard
            d->m_contactIds.insert(0, dummyMyCardId);
        }
        else {
            d->m_contactIds.insert(0, myCardId);
        }
    }
    else {
        // remove MyCard from the list
        if (myCardId <= 0) {
            d->m_contactIds.removeOne(dummyMyCardId);
        }
        else {
            d->m_contactIds.removeOne(myCardId);
        }
    }
    d->m_showMyCard = enabled;
    d->m_currentRow = -1;

    beginResetModel();
    reset();
    endResetModel();
    CNT_EXIT
}

/*!
 * Returns MyCard status: shown or not.
 *
 * \return true if MyCard is shown, false otherwise.
 */
bool CntListModel::myCardStatus() const
{
    return d->m_showMyCard;
}

/*!
 * Returns MyCard id.
 *
 * \return MyCard id.
 */
QContactLocalId CntListModel::myCardId() const
{
    return d->m_myCardId;
}

/*!
 * Gets the filtered list of the contact Ids in a sorted order  
 * 
 * \return Error status
 */
void CntListModel::updateContactIdsArray()
{
    CNT_ENTRY

    QContactDetailFilter* detailFilter = NULL;

    if (d->m_filter.type() == QContactFilter::ContactDetailFilter) {
        detailFilter = static_cast<QContactDetailFilter*>(&d->m_filter);
    }

    // special handling for all-contacts filter
    if (detailFilter
        && detailFilter->detailDefinitionName() == QContactType::DefinitionName
        && detailFilter->detailFieldName() == QContactType::FieldType
        && detailFilter->value() == QContactType::TypeContact) {
        d->m_contactIds = d->m_cache->sortIdsByName(NULL);
    } else if (detailFilter
        && detailFilter->detailDefinitionName() == QContactDisplayLabel::DefinitionName
        && detailFilter->detailFieldName() == QContactDisplayLabel::FieldLabel
        && detailFilter->matchFlags() == Qt::MatchStartsWith) {
        QStringList searchList = detailFilter->value().toStringList();
        d->m_contactIds = d->m_cache->sortIdsByName(searchList);
    } else {
        QSet<QContactLocalId> filterIds = d->m_contactManager->contactIds(d->m_filter).toSet();
        d->m_contactIds = d->m_cache->sortIdsByName(&filterIds);
    }
    
    //find MyCard contact and move it to the first position
    QContactLocalId myCardId = d->m_myCardId;
    if (myCardId > 0) {
        // MyCard exists
        d->m_contactIds.removeOne(myCardId);
        if (d->m_showMyCard) {
            d->m_contactIds.insert(0, myCardId);
        }
    } 
    else if (d->m_showMyCard) {
        // create a placeholder for MyCard
        d->m_contactIds.insert(0, dummyMyCardId);
    }
    CNT_EXIT
}

/*!
 * Read a full contact entry from the database for the row number.
 *
 * The entry at the requested row will have its full contact information
 * (all fields) read from the database and returned as a QContact instance.
 *
 * \param row Row at which the sought contact entry is in this model.
 * \return A newly constructed QContact instance for this entry - ownership
 *  is transferred to the caller.
 *
 */
QContact CntListModel::contact(int row) const
{
    CNT_ENTRY
    if (!validRowId(row) || d->m_contactIds[row] == dummyMyCardId) {
        return QContact();
    }
    CNT_EXIT
    return d->m_contactManager->contact(d->m_contactIds[row]);
}

/*!
 * Verify specified row id is valid.
 *
 * \param row A row number
 * \return bool indicating validity of row id
 */
bool CntListModel::validRowId(int row) const
{
	return (row >= 0 && row < rowCount());
}

/*!
 * Fetch the id of the row containing the contact of the specified id.
 *
 * \param contactId The id of the contact
 * \return the row id of the contact or -1 if no item matched.
 */
int CntListModel::rowId(const QContactLocalId &contactId) const
{
    return d->m_contactIds.indexOf(contactId);
}

/*!
 * Return the data to be used by the view for a display role.
 *
 * \param row The row of the item to return data about.
 * \param column The column of the item to return data about.
 * \return QVariant The data for the specified index.
 */
QVariant CntListModel::dataForRole(int row, int role) const
{
    CNT_ENTRY
    QStringList list;
    QString name;
    QString infoText;
    bool isSelfContact = false;
    bool isNonEmptySelfContact = false;
    
    QContactLocalId id = d->m_contactIds[row];
    if (d->m_myCardId == id || dummyMyCardId == id) {
        isSelfContact = true;
        if (d->m_currentContact.id() == -1) {
            // empty card
            name = hbTrId("txt_phob_dblist_mycard");
            infoText = hbTrId("txt_phob_dblist_mycard_val_create_my_identity");
        }
        else {
            isNonEmptySelfContact = true;
        }
    }

    if (!isSelfContact || isNonEmptySelfContact) {
        name = d->m_currentContact.name();
        if (name.isEmpty()) {
            name = hbTrId("txt_phob_list_unnamed");
        }
        infoText = d->m_currentContact.text();
    }
    
    if ( role == Qt::DisplayRole )
    {
        list << d->m_Format->formattedText(name, d->m_filter);
    }
    else
    {
        list << name;
    }
    
    if (!isNonEmptySelfContact) {
        if (d->m_currentRowSetting == CntTwoRowsNameAndPhoneNumber) {
            //add additional text only if user wants 2 rows in each name list item
            list << infoText;
        }
    }
    CNT_EXIT
    return list;
}

/*!
 * Handle adding of contacts. 
 *
 * \param contactIds Ids of contacts added.
 */
void CntListModel::handleAdded(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    // if contacts are added already, no need to do anything
    bool newContacts = false;
    for (int k = 0; k < contactIds.count() && !newContacts; k++) { 
        if (!d->m_contactIds.contains(contactIds.at(k))) {
            newContacts = true;
        }
    }
    if (!newContacts) {
        return;
    }

    // invalidate cached contact
    d->m_currentRow = -1;
    
    QList<QContactLocalId> oldIdList = d->m_contactIds;
    updateContactIdsArray();

    QList<int> newRows;
    for (int i = 0; i < d->m_contactIds.count(); i++) {
        if (!oldIdList.contains(d->m_contactIds.at(i))) {
            newRows.append(i);
        }
    }

    if (newRows.size() == 1) {
        beginInsertRows(QModelIndex(), newRows.at(0), newRows.at(0));
        endInsertRows();
    } else {
        beginResetModel();
        reset();
        endResetModel();
    }

    CNT_EXIT
}

/*!
 * Handle changes in contacts. 
 *
 * \param contactIds Ids of contacts changed.
 */
void CntListModel::handleChanged(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    if (contactIds.count() == 0) {
        return;
    }

    //invalidate cached contact
    d->m_currentRow = -1; 

    int firstChangedContactPosBefore = rowId(contactIds.at(0));  
    updateContactIdsArray();
    int firstChangedContactPosAfter = rowId(contactIds.at(0)); 

    // if only one contact was updated and its position didn't change,
    // refresh the corresponding row
    if (contactIds.count() == 1 &&
        firstChangedContactPosBefore == firstChangedContactPosAfter &&
        firstChangedContactPosBefore >= 0) {
        QModelIndex top = index(firstChangedContactPosBefore);
        QModelIndex bottom = index(firstChangedContactPosBefore);
        emit dataChanged(top, bottom);
    }
    else {
        beginResetModel();
        reset();
        endResetModel();
    }
    
    CNT_EXIT
}

/*!
 * Handle removing of contacts. 
 *
 * \param contactIds Ids of contacts removed.
 */
void CntListModel::handleRemoved(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    bool removeContacts = false;
    QList<QContactLocalId> idList = d->m_contactIds;
    for (int k = 0; k < contactIds.count() && !removeContacts; k++) {
        if(idList.contains(contactIds.at(k))) {
            removeContacts = true;
        }
    }
    if (!removeContacts) {
        return;
    }
    
    //Find contacts to remove (=rows)
    QList<int> removeRows;
    for(int i = 0; i < contactIds.count(); i++) {
        if (idList.contains(contactIds.at(i))) {
            removeRows.append(rowId(contactIds.at(i)));
        }
    }
    
    // invalidate cached contact
    d->m_currentRow = -1;
    
    int myCardRow = -1;
    if (contactIds.contains(d->m_myCardId)) {
        myCardRow = rowId(d->m_myCardId);
        d->m_myCardId = 0;
    }

    // remove rows starting from the bottom
    qSort(removeRows.begin(), removeRows.end(), qGreater<int>());
    foreach (int row, removeRows) {
        if (row != myCardRow || !d->m_showMyCard) {
            beginRemoveRows(QModelIndex(), row, row);
            endRemoveRows();
        }
    }

    foreach (QContactLocalId id, contactIds) {
        d->m_contactIds.removeOne(id);
    }

    if (myCardRow != -1 && d->m_showMyCard) {
        d->m_contactIds.insert(0, dummyMyCardId);
        QModelIndex index = createIndex(0, 0);
        emit dataChanged(index, index);
    }

    CNT_EXIT
}

/*!
 * Handle my card change. 
 *
 * \param oldId Id of the old MyCard.
 * \param newId Id of the new MyCard.
 */
void CntListModel::handleMyCardChanged(const QContactLocalId& /*oldId*/, const QContactLocalId& newId)
{
    CNT_ENTRY
    
    //invalidate cached contact
    d->m_currentRow = -1;
    d->m_myCardId = newId;

    updateContactIdsArray();
    
    beginResetModel();
    reset();
    endResetModel();
    
    CNT_EXIT
}

/*!
 * Handle added relationships. 
 *
 * \param contactIds Ids of contacts added (group id and contact ids).
 */
void CntListModel::handleAddedRelationship(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY

    if (contactIds.contains(d->m_groupId)) {
        foreach (QContactLocalId id, contactIds) {
            if (id != d->m_groupId && !d->m_contactIds.contains(id)) {
                // at least one new contact id has been added to this group,
                // so update the model
                updateRelationships();
                break;
            }
        }
    }

    CNT_EXIT
}

/*!
 * Handle removed relationships. 
 *
 * \param contactIds Ids of contacts removed from a relationship (group id and contact ids).
 */
void CntListModel::handleRemovedRelationship(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY

    if (contactIds.contains(d->m_groupId)) {
        foreach (QContactLocalId id, contactIds) {
            if (d->m_contactIds.contains(id)) {
                // at least one new contact id has been removed from this group,
                // so update the model
                updateRelationships();
                break;
            }
        }
    }

    CNT_EXIT
}

/*!
 * Updates the model to reflect changes in the relationships.
 */
void CntListModel::updateRelationships()
{
    CNT_ENTRY

    //invalidate cached contact
    d->m_currentRow = -1;
    
    QList<QContactLocalId> oldIdList = d->m_contactIds;
    updateContactIdsArray();
    
    // find all changed rows
    QList<int> newRows, removedRows;
    for (int i = 0; i < d->m_contactIds.count(); i++) {
        if (!oldIdList.contains(d->m_contactIds.at(i))) {
            newRows.append(i);
        }
    }
    for (int i = 0; i < oldIdList.count(); i++) {
        if (!d->m_contactIds.contains(oldIdList.at(i))) {
            removedRows.append(i);
        }
    }

    // currently only one-row-changes are handled with beginInsertRows/beginRemoveRows
    // if there are more than one change, the whole model is reset
    if (removedRows.count() == 1 && newRows.count() == 0) {
        beginRemoveRows(QModelIndex(), removedRows.at(0), removedRows.at(0));
        endRemoveRows();
    }
    else if (newRows.count() == 1 && removedRows.count() == 0) {
        beginInsertRows(QModelIndex(), newRows.at(0), newRows.at(0));
        endInsertRows();
    }
    else {
        beginResetModel();
        endResetModel();
    }

    CNT_EXIT
}

/*!
* Notify views that info for a contact has become
* available or has changed.
*
* \param contactId the id of the contact
*/
void CntListModel::handleContactInfoUpdated(QContactLocalId contactId)
{
    CNT_ENTRY
    
    QModelIndex index = createIndex(rowId(contactId), 0);
    if (index.row() == d->m_currentRow) {
        d->m_currentRow = -1;
    }
    emit dataChanged(index, index);
    
    CNT_EXIT
}

/*!
* Handle a change in how name list item should be represented
*
* \param key Central repository key
* \param value New value in the key
*/
void CntListModel::handleRowSettingChanged(const XQSettingsKey& /*key*/, const QVariant& value)
{
    bool ok = false;
    int newSetting = value.toInt(&ok);
    if (ok) {
        d->m_currentRowSetting = newSetting;
        beginResetModel();
        reset();
        endResetModel();
    }
}

/*!
* Handle a change in data
*/
void CntListModel::refreshModel()
{
    d->m_currentRow = -1;

    updateContactIdsArray();

    beginResetModel();
    reset();
    endResetModel();
}
