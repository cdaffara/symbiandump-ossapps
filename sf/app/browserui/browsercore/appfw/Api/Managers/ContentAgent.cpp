/**
   This file is part of CWRT package **

   Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies). **

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU (Lesser) General Public License as 
   published by the Free Software Foundation, version 2.1 of the License. 
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
   (Lesser) General Public License for more details. You should have 
   received a copy of the GNU (Lesser) General Public License along 
   with this program. If not, see <http://www.gnu.org/licenses/>.
*/



#include <QtCore/QDebug>
#include <QString>
#include "ContentAgent_p.h"
#include "ContentAgent.h"
#include "bookmarks.h"
#include <browsercontentdll.h>


namespace WRT {

ContentAgentPrivate::ContentAgentPrivate(ContentAgent* mgr, QWidget* /*parent*/) :
    q(mgr),
    m_connectedToBookmarks(false),
    m_loadedBookmarks(false),
    m_loadedHistory(false),
    m_maxUrls(10) 
{
    QFileInfo dbFile("browserContent.db");
  
    m_bookmarkSession=new BrowserContent("Bedrock");
    if (m_bookmarkSession) {
        m_connectedToBookmarks = true;
    } else {
        qDebug() << "ContentAgentPrivate: Failed to connect to bookmarks";
    }
}

ContentAgentPrivate::~ContentAgentPrivate()
{
    delete m_bookmarkSession;
}

/*!
 * \class ContentAgent
 *
 * This class is responsible for managing bookmarks, This class could be used 
 * mainly for displying bookmarks in UI.
 *
 */
 
/*!
 * Basic constructor
 * @param parent : parent widget (Defaulted to NULL ) if not specified
 */
ContentAgent::ContentAgent(QWidget *parent) :
    d(new ContentAgentPrivate(this, parent))
{
      
}

ContentAgent::~ContentAgent()
{
    delete d;
}

ContentAgent* ContentAgent::getSingleton()
{
    static ContentAgent* singleton = 0;
    
    if(!singleton){
           singleton = new ContentAgent();
           singleton->setObjectName("ContentAgent");
    }
    return singleton;    
}

QString ContentAgent::getBookmarks()
{
    QList<BookmarkLeaf*> nodes;
    nodes = d->m_bookmarkSession->fetchAllBookmarks();
    QString bookmarkData = "[";
    for(int i=0;i<nodes.count();i++) {
        bookmarkData.append("{");
        bookmarkData.append("\"title\": \"");
        bookmarkData.append(nodes[i]->getTitle());
        bookmarkData.append("\", \"urlvalue\": \"");
        bookmarkData.append(nodes[i]->getUrl());
            if(i != (nodes.count()-1))
            bookmarkData.append("\"},");
        else
            bookmarkData.append("\"}");
    }
    bookmarkData.append("]");

    return bookmarkData;
}

QMap<QString, QString> ContentAgent::getSuggestedBookmarks(QString atitle)
{
    QList<BookmarkLeaf*> nodes;
    nodes = d->m_bookmarkSession->suggestBookMarks(atitle);
    QMap<QString, QString> map;
    for(int i=0;i<nodes.count();i++) {
        map.insert( nodes[i]->getTitle(), nodes[i]->getUrl() );
    }
    return map;
}

QMap<QString, QString> ContentAgent::getSuggestedHistory(QString atitle)
{
    QList<HistoryLeaf*> nodes;
    nodes = d->m_bookmarkSession->suggestHistory(atitle);
    QMap<QString, QString> map;
    for(int i=0;i<nodes.count();i++) {
        map.insert( nodes[i]->getTitle(), nodes[i]->getUrl() );
    }
    return map;
}

QObjectList ContentAgent::getUISuggestion(QString suggest)
{
  return (d->m_bookmarkSession->suggestContent(suggest));
}

}//wrt
