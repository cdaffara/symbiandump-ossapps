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

#ifndef CONTENT_AGENT_P_H
#define CONTENT_AGENT_P_H

#include <browsercontentdll.h>

namespace WRT {
    class ContentAgent;
    class ContentAgentPrivate
    {
    public:
        ContentAgentPrivate(ContentAgent * qq, QWidget *parent = 0);
        ~ContentAgentPrivate();

    public: // public actions available for this manager
        
    public:
        ContentAgent* const q;
        //! flag to indicate was able to connect to book marks
        bool m_connectedToBookmarks;
        //! flag to indicate bookmarks are loaded from data base
        bool m_loadedBookmarks;
        //! flag to indicate history is loaded from data base
        bool m_loadedHistory;
        int m_maxUrls;
        BrowserContent* m_bookmarkSession;
    };
}
#endif //CONTENT_AGENT_P_H
