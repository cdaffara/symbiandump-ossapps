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

#ifndef HISTORY_MANAGER_H
#define HISTORY_MANAGER_H

#include <QtGui/QIcon>
#include <QObject>
#include <QAction>
#include "BWFGlobal.h"
#include "bedrockprovisioning.h"

namespace WRT {

class HistoryManagerPrivate;
class HistoryModel;

class BWF_EXPORT HistoryManager : public QObject {
  
  Q_OBJECT
  
  enum EHistoryManagerErrorTypes
         {
          ErrNone,
      ErrGeneral = -1,
          
          };

  public:
    HistoryManager(QWidget *parent = 0);
    ~HistoryManager();

    void setSettings(BEDROCK_PROVISIONING::BedrockProvisioning *settings);
    //Gets ref count of the page from history
    int getPageRank(const QString &url);
    static HistoryManager* getSingleton();
    
  signals:
    void historyCleared();
    void confirmHistoryClear();

    public slots:

    //javascript APIS
    QString getHistoryFoldersJSON(QString folder="");
    void addHistory(const QString &url, const QString &title);
    void addHistory(const QUrl &url, const QString &title);
    void clearHistory();
    QMap<QString, QString> findHistory(const QString title);

    QAction * getActionClearHistory();

  private:
     HistoryManagerPrivate * const d;
     bool m_isHistoryDbreadRequired;
     QVector<QString> m_folderVector;
     QMap<QString, QString> m_historyMap;
     
};
}
#endif //HISTORY_MANAGER
