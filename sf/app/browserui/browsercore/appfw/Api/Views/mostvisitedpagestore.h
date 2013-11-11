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
#include "BWFGlobal.h"

class QDataStream;

//Most visited page item
class BWF_EXPORT MostVisitedPage : public QObject
{
    Q_PROPERTY(QString pageUrl READ pageUrl)
public:

    //Construction and destruction
    MostVisitedPage();
    MostVisitedPage(QString url, QImage *pageThumbnail = 0, uint pageRank = 1);
    ~MostVisitedPage();

    //Return MVP Url
    QString pageUrl() {return m_url;}
    
    //Return whether or not page thumbnail present
    bool thumbnailAvailable() const { return m_pageThumbnail != 0; }

    //Serialization functions
    friend QDataStream& operator<<(QDataStream &out, const MostVisitedPage &source);
    friend QDataStream& operator>>(QDataStream &in, MostVisitedPage &destination);

public:
    QString m_url;
    QImage *m_pageThumbnail;
    uint m_pageRank;
#ifdef ENABLE_TESTS
    	friend class MostVistedPageTest;
#endif
};


typedef QList<MostVisitedPage*> MostVisitedPageList;

//Store for managing MV pages
class BWF_EXPORT MostVisitedPageStore : public QObject
{
public:
    //Construction and destruction
    MostVisitedPageStore();
    ~MostVisitedPageStore();

    //Page has been accessed. Function would update the MV Store.
    void pageAccessed(const QUrl& url, QImage* pageThumbnail, int pageRank);

    //Check whether page with URL exists in store.
    //When checkThumbnail = true, also check if it has thumbnail
    bool contains(const QString& url, bool checkThumbnail = false);
    
    MostVisitedPageList &pageList(){ return m_pageList;}
    MostVisitedPage *pageAt(int index);
    
    void initializeDefaultPageThumbnails();
    void clearMostVisitedPageStore();
protected:
    bool compareUrls(QString& url1, QString &url2);
    void readStore();
    void writeStore();

private:
    MostVisitedPageList m_pageList;
    QString m_mvpFile;
    bool m_needPersistWrite;
#ifdef ENABLE_TESTS
		friend class MostVistedPageTest;
#endif
};
