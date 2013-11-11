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

#include <QStringList>
#include <QtAlgorithms>
#include <hbglobal.h>
#include <hbicon.h>
#include <hbframebackground.h>

#include "cnthistorymodel_p.h"
#include "cntdebug.h"


// Unnamed namespace for helper functions
namespace
{
    bool greaterThan(const HItemPointer& t1, const HItemPointer& t2)
    {
        return ((*t1).timeStamp > (*t2).timeStamp);
    }
    
    bool lessThan(const HItemPointer& t1, const HItemPointer& t2)
    {
        return ((*t1).timeStamp < (*t2).timeStamp);
    }
    
#ifdef TRACES
#define LOG_ITEM(i) logItem(i)    
    void logItem(const HistoryItem& item)
    {
        QStringList l;
        
        l << item.message << item.number << item.timeStamp.toString() << item.title;
        l << (item.flags & CntIncoming ? "incoming" : "outgoing");
        l << (item.flags & CntUnseen ? "unseen" : "seen");
        l << (item.flags & CntAttachment ? "attachment" : "");
        l << (item.flags & CntCallLog ? "call" : "message");
        if (item.flags & CntReceivedCall)
            l << "recieved";
        else if (item.flags & CntMissedCall)
            l << "missed";
        else if (item.flags & CntDialledCall)
            l << "dialled";
        
        l.removeAll("");
        
        qDebug() << l;
    }
#else
#define LOG_ITEM(i)
#endif
}

Q_DECLARE_METATYPE(LogsEvent *)

/*!
 * Construct a new CntHistoryModelPrivatePrivate object to communicate 
 * with the conversations and logs databases.
 *
 * \param contactId History specific to this contact is cached. 
 * If no contact is specified all the call logs and conversation 
 * history from all contacts will be cached.
 */
CntHistoryModelPrivate::CntHistoryModelPrivate(QContactLocalId contactId, QContactManager* manager)
    : QObject(),
      m_logsModel(NULL),
      m_logsFilter(NULL),
      m_AbstractLogsModel(NULL),
      m_msgHistory(NULL),
      m_contactId(contactId),
      m_contactManager(manager),
      m_isMyCard(false),
      m_isMarkedAsSeen(false),
      m_initLogs(false),
      m_extendedLocale(HbExtendedLocale::system())
{
    CNT_ENTRY
    
    // Create the model structure and cache history data from the databases
    initializeModel();
    
    CNT_EXIT
}

CntHistoryModelPrivate::~CntHistoryModelPrivate()
{
    CNT_ENTRY
    
    if (m_logsModel) {
        delete m_logsModel;
        m_logsModel = NULL;
    }
    if (m_logsFilter) {
        delete m_logsFilter;
        m_logsFilter = NULL;
    }
    if (m_msgHistory) {
        delete m_msgHistory;
        m_msgHistory = NULL;
    }
    
    CNT_EXIT
}

/*!
 * Return the data to be used by the view or delegates for a particular
 * item and role.
 *
 * \param index The index of the item to return data about.
 * \param role The data should be relevant to this particular purpose.
 * \return QVariant The data for the specified index and role.
 */
QVariant CntHistoryModelPrivate::data(const QModelIndex& index, int role) const
{
    CNT_ENTRY_ARGS(index << role)
    // Invalid index
    int row = index.row();
    
    if ( !validateRowIndex(row) )
        return QVariant();
    
    HItemPointer p = m_List.at(row);
    if ( p.isNull() )
        return QVariant();
        
    switch( role )
    {       
        case Qt::DisplayRole:
            return displayRoleData(*p);
        case Qt::DecorationRole:
            return decorationRoleData(*p);
        case Qt::BackgroundRole:
            return backgroundRoleData(*p);
        case CntFlagsRole:
            return QVariant((*p).flags);
        case CntPhoneNumberRole:
            return QVariant((*p).number);
        case CntConverstaionIdRole:
            return conversationIdRoleData(row);
        default:
            return QVariant();
    }
}

/*!
 * Return the data to be used by the view for a display role.
 *
 * \param item The History item to return data about.
 *  return QVariant List of strings to be displayed on the view.
 *  The stings can also be NULL
 *  index 0 Title of the conversation item.
 *  index 1 Body text
 *  index 2 Time stamp
 */
QVariant CntHistoryModelPrivate::displayRoleData(const HistoryItem& item) const
{
    CNT_ENTRY
    
    QStringList list;
    HbExtendedLocale locale = m_extendedLocale;
    
    if (item.timeStamp.date() == QDateTime::currentDateTime().date())
    {
        list << item.title << item.message << locale.format(item.timeStamp.time(), r_qtn_time_usual);
    }
    else
    {
        list << item.title << item.message << locale.format(item.timeStamp.date(), r_qtn_date_usual);
    }
    
    CNT_EXIT_ARGS(list)
    
    return QVariant(list);
}

/*!
 * Return the data to be used by the view for a decoration role.
 *
 * \param item The History item to return data about.
 *  return QVariant String of the icon path.
 */
QVariant CntHistoryModelPrivate::decorationRoleData(const HistoryItem& item) const
{
    CNT_ENTRY
    
    // Messages
    if (item.flags & CntMessage)
        return QVariant(HbIcon(MESSAGE_ICON));
    
    // Call logs
    if (item.flags & CntCallLog) {
        if (item.flags & CntMissedCall)
            return QVariant(HbIcon(MISSED_CALL_ICON));
        if (item.flags & CntDialledCall)
            return QVariant(HbIcon(DAILED_CALL_ICON));
        if (item.flags & CntReceivedCall)
            return QVariant(HbIcon(RECEIVED_CALL_ICON));
    }
    
    return QVariant();
}

/*!
 * Return the data to be used to draw the background of list items
 *
 * \param item The History item to return data about.
 *  return QVariant HbFrameBackground of the list item.
 */
QVariant CntHistoryModelPrivate::backgroundRoleData(const HistoryItem& item) const
{
    if (item.flags & CntIncoming)
        return QVariant(HbFrameBackground("qtg_fr_convlist_received_normal", HbFrameDrawer::NinePieces));
    else
        return QVariant(HbFrameBackground("qtg_fr_convlist_sent_normal", HbFrameDrawer::NinePieces));
}

/*!
 * Return the conversation id of this row if it is a message
 *
 * \param item The History item to return data about.
 *  return QVariant HbFrameBackground of the list item.
 */
QVariant CntHistoryModelPrivate::conversationIdRoleData(const int row) const
{
    HItemPointer p = m_List.at(row);
    
    int id(-1);
    if ( p.data()->flags & CntMessage )
        id = m_msgMap.key(p, -1);

    if (id != -1)
        return QVariant(id);
    else
        return QVariant();
}

/*!
 * Get the number of rows (conversations) in this model.
 *
 * \param parent Optional parent index value.
 * \return Number of rows in this model.
 */
int CntHistoryModelPrivate::rowCount(const QModelIndex& /*parent*/) const
{
    return m_List.size();
}

/*!
 * Sort list items on the model.
 *
 * \param column Column to be sorted. It is not used.
 * \param order Order to sort the list items.
 */
void CntHistoryModelPrivate::sort(int /*column*/, Qt::SortOrder order)
{
    CNT_ENTRY_ARGS(order)
    
    if ( order == Qt::AscendingOrder )
        qStableSort(m_List.begin(), m_List.end(), lessThan);
    else
        qStableSort(m_List.begin(), m_List.end(), greaterThan);
    
    CNT_EXIT
}

/*!
 * Clear history from the database. If the history cached
 * is specific to one contact, only that history is cleared.
 * 
 */
void CntHistoryModelPrivate::clearHistory()
{
    CNT_ENTRY
    
    Q_Q(CntHistoryModel);
    
    if ( m_List.isEmpty() )
        return;
    
    // Clear all data from the history model.
    int count = rowCount();
    
    q->doBeginRemoveRows( QModelIndex(), 0, count );
    
    // Call logs
    if ( !m_isMyCard && m_logsFilter )
        m_logsFilter->clearEvents();
    else if ( m_logsModel )
        m_logsModel->clearList(LogsModel::TypeLogsClearAll);
    
    // Messages
    if (m_msgHistory)
        m_msgHistory->clearMessages( (int)m_contactId );
    
    m_List.clear();
    m_msgMap.clear();
    m_logsMap.clear();
    
    
    q->doEndRemoveRows();
    
    CNT_EXIT
}

/*!
 * Mark all the conversations in the view as seen.
 * 
 */
void CntHistoryModelPrivate::markAllAsSeen()
{
    CNT_ENTRY
    
    if ( m_isMarkedAsSeen )
        return;
    
    // Messages
    if (m_msgHistory->markRead( m_contactId ))
        m_isMarkedAsSeen = true;
    
    CNT_EXIT
}

/*!
 * Create the model structure and cache history data from
 * conversations and call logs databases.
 *
 */
void CntHistoryModelPrivate::initializeModel()
{
    CNT_ENTRY
    
    initializeLogsModel();
    initializeMsgModel();
    
    CNT_EXIT
}

void CntHistoryModelPrivate::initializeMsgModel()
{
    CNT_ENTRY
    
    if( m_isMyCard )
        return;
    
    // Contact centric
    MsgHistory* m = new MsgHistory();
    
    m_msgHistory = m;
    
    // Connect to signals emitted by msg model
    connect(m, SIGNAL(messagesReady(QList<MsgItem>& )), this, SLOT(messagesReady(QList<MsgItem>& )));
    connect(m, SIGNAL(messageAdded(MsgItem& )), this, SLOT(messageAdded(MsgItem& )));
    connect(m, SIGNAL(messageChanged(MsgItem& )), this, SLOT(messageChanged(MsgItem& )));
    connect(m, SIGNAL(messageDeleted(MsgItem& )), this, SLOT(messageDeleted(MsgItem& )));
    
    // Subscribe to get new messages
    // received from this contact
    m->subscribe(m_contactId);
    
    // Initial fetch of all messages
    m->getMessages(m_contactId);
    
    CNT_EXIT
}

void CntHistoryModelPrivate::initializeLogsModel()
{
    CNT_ENTRY
    
    //populate model with call events
    m_logsModel = new LogsModel(LogsModel::LogsFullModel);
    if (!m_isMyCard) {
        //do the filtering to get call events for the target contact
        m_logsFilter = new LogsCustomFilter;
        m_logsFilter->setContactId(m_contactId);
        m_logsFilter->setSourceModel(m_logsModel);
        m_AbstractLogsModel = m_logsFilter;
    } else {
        //get all call events
        m_AbstractLogsModel = m_logsModel;
    }
    
    //read first call events if any and start listening for more 
    for ( int i = 0; i < m_AbstractLogsModel->rowCount(); ++i ) {
        LogsEvent* event = qVariantValue<LogsEvent*>(
                m_AbstractLogsModel->data(m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        if ( event ) {
            HItemPointer item = HItemPointer(new HistoryItem());
            readLogEvent(event, *item);
            m_logsMap.insert(i, item);
            m_List.append( item );
        }
    }
    
    connect(m_AbstractLogsModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), 
            this, SLOT(logsRowsInserted(const QModelIndex &, int, int)));
    connect(m_AbstractLogsModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), 
            this, SLOT(logsRowsRemoved(const QModelIndex &, int, int)));
    connect(m_AbstractLogsModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), 
                this, SLOT(logsDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(m_AbstractLogsModel, SIGNAL(modelReset()), this, SLOT(handleLogsReset()));

    CNT_EXIT
}

/*!
 * Read call event into a history item
 *
 * \param event Call log event
 * \param item Conversation history item
 */
void CntHistoryModelPrivate::readLogEvent(LogsEvent* event, HistoryItem& item)
{
    QString bodyText;
    QString title;
    
    if ( m_isMyCard ) {
        if ( event->remoteParty().length() > 0 ) {
            title = QString(event->remoteParty());
        }
        else {
            title = QString(event->number());
        }
    } else {
        if ( event->direction() == LogsEvent::DirIn ) {
            bodyText = hbTrId("txt_phob_list_received");
            item.flags |= CntReceivedCall;
        } else if ( event->direction() == LogsEvent::DirOut ) {
            bodyText = hbTrId("txt_phob_list_dialled_call");
            item.flags |= CntDialledCall;
        } else if ( event->direction() == LogsEvent::DirMissed ) {
            bodyText = hbTrId("txt_phob_list_missed_call");
            item.flags |= CntMissedCall;
        }
    }

    if ( event->direction() == LogsEvent::DirMissed
        || event->direction() == LogsEvent::DirIn ) {
        item.flags |= CntIncoming;
    } else {
        item.flags &= ~CntIncoming;
    }
    
    item.message = bodyText;
    item.title = title;
    item.timeStamp = event->time().toLocalTime();
    item.flags |= CntCallLog;
    item.number = QString(event->number());
    
    LOG_ITEM(item);
}

/*!
 * Slot used for receiving new rows from the LogsModel.
 *
 * \param parent Optional parent index value.
 * \param first The first row item to be received from the model.
 * \param last The last row item to be received from the model.
 */
void CntHistoryModelPrivate::logsRowsInserted(const QModelIndex& /*parent*/, int first, int last)
{
    CNT_ENTRY_ARGS(first << last)
    
    Q_Q(CntHistoryModel);
    
    // Check if this is the first time to receive events
    if ( !m_initLogs ) {
        q->doBeginResetModel();
    }
    else {
        q->doBeginInsertRows( QModelIndex(), rowCount(), rowCount() + (last - first) );
    }
    
    QList<HItemPointer> l;
    
    for ( int i = first; i < m_AbstractLogsModel->rowCount() && i <= last; ++i ) {
        LogsEvent* event = qVariantValue<LogsEvent*>(
                m_AbstractLogsModel->data(m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        if ( event ) {
            HItemPointer item(new HistoryItem());
            readLogEvent(event, *item);
            m_logsMap.insert(i, item);
            m_List.append( item );
        }
    }
    
    // Check if this is the first time to receive events
    // and sort the entire list.
    if ( !m_initLogs ) {
        sort();
        m_initLogs = true;
        q->doEndResetModel();
    }
    else {
        q->doEndInsertRows();
    }
    
    CNT_EXIT
}

/*!
 * Slot used for receiving new rows from the LogsModel.
 *
 * \param parent Optional parent index value.
 * \param first The first row item to be received from the model.
 * \param last The last row item to be received from the model.
 */
void CntHistoryModelPrivate::logsRowsRemoved(const QModelIndex& /*parent*/, int first, int last)
{
    CNT_ENTRY_ARGS(first << last)
    
    Q_Q(CntHistoryModel);
    
    QList< int > indices;
    
    for ( int i = first; i <= last; ++i ) {
        HItemPointer item = m_logsMap.value( i );
        int index = m_List.indexOf( item );
        if ( index > -1 ) {
            m_logsMap.remove( i );        
            indices.append( index );
        }
    }
    
    foreach(int i, indices) {
        m_List.removeAt( i );
    }
    
    // Remove list items in batches
    if ( !indices.isEmpty() ) {
        QList< QList<int> > batches = findIndices(indices);
        foreach( QList<int> l, batches ) {
            q->doBeginRemoveRows(QModelIndex(), l.first(), l.last());
            q->doEndRemoveRows();
        }
    }
    
    CNT_EXIT
}

/*!
 * Update events from logs model. Events are
 * received as a batch
 *
 * \param first First updated model index
 * \param last Last updated model index
 */
void CntHistoryModelPrivate::logsDataChanged(const QModelIndex& first, const QModelIndex& last)
{
    CNT_ENTRY_ARGS(first << last)
    
    Q_Q(CntHistoryModel);
    
    int f = first.row();
    int l = last.row();
    QList< int > indices;
    
    for ( int i = f; i < m_AbstractLogsModel->rowCount() && i <= l; ++i ) {
        
        LogsEvent* event = qVariantValue<LogsEvent*>(
                m_AbstractLogsModel->data(m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        // Fetch item from the mapped logs model items
        HItemPointer item = m_logsMap.value( i );
        
        // Found item in the logs map
        if ( !item.isNull() && event ) {
            int index = m_List.indexOf( item );
            readLogEvent( event, *item );
            indices.append( index );
        }
    }
    
    // Emit dataChanged signal only if there were updates
    if ( !indices.isEmpty() ) {
        QList< QList<int> > batches = findIndices( indices );
        foreach( QList<int> l, batches )
            q->doDataChanged( q->index(l.first(), 0), q->index(l.last(), 0) );
    }
    
    CNT_EXIT
}

/*
 * Clear all call logs and refetches new call events after 
 * receiving a reset model signal from logs model
 */
void CntHistoryModelPrivate::handleLogsReset()
{
    CNT_ENTRY
    
    Q_Q(CntHistoryModel);
    
    q->doBeginResetModel();
    
    // Remove all call logs
    QList<HItemPointer> values = m_logsMap.values();
    foreach(HItemPointer p, values) {
        m_List.removeOne( p );
    }
    
    m_logsMap.clear();
    
    //read first call events if any and start listening for more 
    for ( int i = 0; i < m_AbstractLogsModel->rowCount(); ++i ) {
        LogsEvent* event = qVariantValue<LogsEvent*>(
                m_AbstractLogsModel->data(m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        if ( event ) {
            HItemPointer item = HItemPointer(new HistoryItem());
            readLogEvent(event, *item);
            m_logsMap.insert(i, item);
            m_List.append( item );
        }
    }
    
    sort();

    q->doEndResetModel();
    
    CNT_EXIT
}

/*!
 * Check whether an idex is out of bound of our list
 *
 * \param index Index to be validated
 */

bool CntHistoryModelPrivate::validateRowIndex( const int index) const
{
    return( index < rowCount() && index >= 0 );
}

/*!
 * Find the sequences of indices for the given indices from the private list
 * 
 * \param indices List of indices
 * \return sequences of indices
 */
QList< QList<int> > CntHistoryModelPrivate::findIndices( const QList< int >& indices )
{
    CNT_ENTRY_ARGS(indices)
    
    QList< QList<int> > sequences;
    QList<int> currSequence;
    int prevIndex = indices.at(0) - 1;
    
    foreach( int currIndex, indices )
    {
        if ( currIndex >= 0 )
        {
            if ( prevIndex+1 != currIndex && !currSequence.isEmpty() )
            {
                sequences.append( currSequence );
                currSequence.clear();
            } 
            currSequence.append( currIndex );
            prevIndex = currIndex;
        }
    }
    
    if ( !currSequence.isEmpty() )
    {
        // Add last sequence if such exist
        sequences.append( currSequence );
    }
    
    CNT_EXIT_ARGS(sequences)
    
    return sequences;
}

/*!
 * Read message event into a history item
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModelPrivate::readMsgEvent(MsgItem& event, HistoryItem& item)
{
    CNT_ENTRY
    
    // Msg direction
    if ( event.direction() == MsgItem::MsgDirectionIncoming ) {
        item.flags |= CntIncoming;
        // Read status
        if ( event.isAttributeSet(MsgItem::MsgAttributeUnread) )
            item.flags |= CntUnseen;
        else
            item.flags &= ~CntUnseen;
    } else if ( event.direction() == MsgItem::MsgDirectionOutgoing )
        item.flags &= ~CntIncoming;
    
    // Attachment
    if (event.isAttributeSet(MsgItem::MsgAttributeAttachment))
        item.flags |= CntAttachment;
    
    item.flags |= CntMessage;
    item.number = event.phoneNumber();
    
    if (event.body().isEmpty())
    {
        item.message = " ";
    }
    else
    {
        item.message = event.body();
    }
    
    item.timeStamp = event.timeStamp().toLocalTime();
    
    LOG_ITEM(item);
    
    CNT_EXIT
}

/*!
 * Slot to receive new messages for the first time
 * from the messages model
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModelPrivate::messagesReady(QList<MsgItem>& msgs)
{
    CNT_ENTRY
    
    Q_Q(CntHistoryModel);
    
    q->doBeginResetModel();
    
    foreach( MsgItem m, msgs ) {
        // Create a new hst item
        HItemPointer item(new HistoryItem());
        
        // Parse the MsgItem and add data into hst item
        readMsgEvent( m, *item );
        
        // Map the hist item to a MsgItem in the msgModel
        m_msgMap.insert( m.id(), item );
        
        // Append the hst item to our list
        m_List.append( item );
    }
    
    sort();
    
    q->doEndResetModel();
    
    CNT_EXIT
}

/*!
 * Slot to receive new messages from the messages model
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModelPrivate::messageAdded(MsgItem& msg)
{
    CNT_ENTRY
    
    Q_Q(CntHistoryModel);
    
    q->doBeginInsertRows( QModelIndex(), rowCount(), rowCount() );
    
    // Create a new hst item
    HItemPointer item(new HistoryItem());
    
    // Parse the MsgItem and add data into hst item
    readMsgEvent( msg, *item );
    
    // Map the hist item to a MsgItem in the msgModel
    m_msgMap.insert( msg.id(), item );
    
    // Append the hst item to our list
    m_List.append( item );
    
    q->doEndInsertRows();
    
    CNT_EXIT
}

/*!
 * Slot to update a message from the messages model
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModelPrivate::messageChanged(MsgItem& msg)
{
    CNT_ENTRY
    
    Q_Q(CntHistoryModel);
    
    // Fetch the hst item that maps to this MsgItem
    HItemPointer p = m_msgMap.value( msg.id() );
    
    // No item was found.
    if ( p.isNull() )
        return;

    // Parse the MsgItem and add data into hst item
    readMsgEvent(msg, *p);
    
    // Get the index of the the hst item in the list
    int pIndex = m_List.indexOf( p );
    
    q->doDataChanged(q->index(pIndex, 0), q->index(pIndex, 0));
    
    CNT_EXIT
}

/*!
 * Slot to delete a message from the messages model
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModelPrivate::messageDeleted(MsgItem& msg)
{
    CNT_ENTRY
    
    Q_Q(CntHistoryModel);
    
    // Fetch the hst item that maps to this MsgItem
    HItemPointer p = m_msgMap.value( msg.id() );
    
    // No item was found.
    if ( p.isNull() )
        return;
    
    // Remove the item in stored containers
    m_msgMap.remove( msg.id() );
    int index = m_List.indexOf( p );
    if ( index > -1 ) {
        q->doBeginRemoveRows(QModelIndex(), index, index);
        m_List.removeAt( index );
        q->doEndRemoveRows();
    }
    
    CNT_EXIT
}
