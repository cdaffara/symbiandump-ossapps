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

#ifndef CONTENT_AGENT_H
#define CONTENT_AGENT_H

#include <QObject>
#include <QMap>

namespace WRT {
      
    class ContentAgentPrivate;
    class BookmarkNode;
    class HistoryModel;
    

class ContentAgent : public QObject
{       
      Q_OBJECT

        public:
           ContentAgent(QWidget *parent = 0);
           ~ContentAgent();
           static ContentAgent* getSingleton();
        
        public slots:
      //javascript APIS
      QString getBookmarks();
      QMap<QString, QString> getSuggestedBookmarks(QString atitle);
      QMap<QString, QString> getSuggestedHistory(QString atitle);
      QObjectList getUISuggestion(QString suggest);
    
        private:
            ContentAgentPrivate* const d;
            // list of URL suggestions from history and bookmarks
            QObjectList combinedList;
};
}//wrt

#endif //CONTENT_AGENT_H
