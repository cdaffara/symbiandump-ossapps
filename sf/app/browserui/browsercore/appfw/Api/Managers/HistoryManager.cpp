/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/


#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QString>
#include "actionjsobject.h"
#include "HistoryManager_p.h"
#include "HistoryManager.h"

//#include "wrtsettings.h"
#include "bedrockprovisioning.h"
#include <browsercontentdll.h>
#include "webpagecontroller.h"
#include "wrtbrowsercontainer.h"


namespace WRT {

HistoryManagerPrivate::HistoryManagerPrivate(HistoryManager * mgr) :
    q(mgr),
    m_connectedToHistory(false),
    m_maxUrls(10) // TODO: read from settings
{
    QFileInfo dbFile("browserContent.db");

#ifdef Q_WS_MAEMO_5
    m_import = false;    
    if (dbFile.exists()){
      m_import = false;
    }
    else {
      m_import = true;
    }
#endif

    m_historySession=new BrowserContent("Bedrock");
    if (m_historySession) {
        m_connectedToHistory = true;
    } else {
        qDebug() << "HistoryManagerPrivate: Failed to connect to history database";
    }

    m_actionsParent = new QObject(mgr);
    m_actionsParent->setObjectName("actions");
    
    m_actionClearHistory = new QAction("clearHistory", m_actionsParent);
    
    m_actionClearJSO  = new ActionJSObject(m_actionsParent, m_actionClearHistory);
    
    m_actionClearHistory->setObjectName("clearHistory");

}

HistoryManagerPrivate::~HistoryManagerPrivate()
{
    delete m_historySession;
    delete m_actionClearHistory;
    delete m_actionClearJSO;
}

/*!
 * \class HistoryManager
 *
 * This class is responsible for managing history, This class could be used
 * mainly for Creating, adding history, adding recent history items, deleting
 * and editing history.
 *
 */
 
/*!
 * Basic constructor
 * @param parent : parent widget (Defaulted to NULL ) if not specified
 */
HistoryManager::HistoryManager(QWidget *parent) :
    QObject(parent),
    d(new HistoryManagerPrivate(this))
{

    m_isHistoryDbreadRequired=true;
    connect(d->m_actionClearHistory, SIGNAL(triggered()), this, SIGNAL(confirmHistoryClear()));
     
}
/*
void HistoryManager::actionClearHistory()
{
    emit confirmClearHistory();
}
*/

HistoryManager::~HistoryManager()
{
    disconnect(d->m_actionClearHistory, SIGNAL(triggered()), this, SIGNAL(historyCleared()));
    delete d;
}

HistoryManager* HistoryManager::getSingleton()
 {
    static HistoryManager* singleton = 0;
  
    if(!singleton){
           singleton = new HistoryManager;
           singleton->setObjectName("historyManager");
    }

    //assert(singleton);
    return singleton;    
 }
}

QString HistoryManager::getHistoryFoldersJSON(QString folderName)
    {

    bool flag = true;
  
    QString historyFolders = "[";
    if (folderName == "")
        {
    m_historyMap.clear();
    m_folderVector.clear();
    d->m_historySession->fetchSerializedHistory(m_folderVector, m_historyMap);

    for (int i = m_folderVector.size() - 1; i >= 0; i--)
        {
        //check for folder nodes
        historyFolders.append("\"");
        historyFolders.append(m_folderVector[i]);
        historyFolders.append("\"");


        if (i != 0)
            historyFolders.append(",");
        if (flag)
            {
            if (m_folderVector[i].count() > 0)
                {
                d->m_actionClearHistory->setEnabled(true);
                flag=false;
                }

            }
        }
     if (flag)
        {
        d->m_actionClearHistory->setEnabled(false);
        }
    historyFolders.append("]");
    m_folderVector.clear();
        }
    if (folderName == "")
        {
        return historyFolders;

        }
    else
        {
        return m_historyMap[folderName];
        }
    }

/*!
 * Add the  node to the folder in proxy model. If the folder doesnt exist in proxy, create
 * the folder  and add the node to it
 */

/*!
 * Add to recent urls
 * @param url: url to be added to recent url list
 * @param title: title for this item in recent url list
 * @param icon: icon to be added for this item in recent url list
 */
Q_DECL_EXPORT void HistoryManager::addHistory(const QString &url, const QString &title)
{

    //Check for a valid history entry
    if (url.isEmpty() || title.isEmpty())
        return;
    
    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveHistory");
    if(!enabled)
      return;
    
    QDateTime currentDateTime = QDateTime::currentDateTime();
    
    HistoryLeaf* leaf=new HistoryLeaf();
    leaf->setTitle(title);
    leaf->setUrl(url);
    leaf->setDate(currentDateTime.date());
    leaf->setLastVisited(currentDateTime.time());
    
    if (d->m_connectedToHistory){
        if(ErrNone == d->m_historySession->addHistory(leaf)){
          d->m_actionClearHistory->setEnabled(true);
        }
    }
    delete leaf;
}

/*!
 * Add to recent urls
 * @param url: url to be added to recent url list
 * @param title: title for this item in recent url list
 * @param icon: icon to be added for this item in recent url list
 */

void HistoryManager::addHistory(const QUrl &url, const QString &title)
{
    addHistory(url.toString(), title);
}


/*!
 * delete recent urls
 * clears all the recent url list.
 */
void HistoryManager::clearHistory()
{

    if (d->m_connectedToHistory) {
        d->m_historySession->clearHistory();
    }
    
    d->m_actionClearHistory->setEnabled(false);
    
    emit historyCleared();
}

QAction * HistoryManager::getActionClearHistory()
{
    return d->m_actionClearHistory;
}

int HistoryManager::getPageRank(const QString &url)
{
 //Check for a valid entry
    if (url.isNull())
        return 0;

    int rank = 0;
	QList<HistoryLeaf*> historyNodes = d->m_historySession->fetchHistory();

    for (int i=0; i < historyNodes.count(); i++) {
      //Update rank if there is a history for this URL.
        if (!historyNodes[i]->getUrl().compare(url))
           rank++;
    }

	while (!historyNodes.isEmpty())
     delete historyNodes.takeFirst();

    return rank;
}

QMap<QString, QString> HistoryManager::findHistory(QString title)
{
    return d->m_historySession->findSimilarHistoryItems(title);
}

