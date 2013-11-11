/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtGui>

#include "mostvisitedpagestore.h"
#include "bedrockprovisioning.h"

const QString KMostVistedStoreFile = "mostvisitedpages.dat";
const uint KMostVistedStoreVersion = 1;
const uint KMostVistedStoreLimit = 5;

const QString KDefaultPage1 = "http://www.nytimes.com/";
const QString KDefaultPage2 = "http://www.google.com/";
const QString KDefaultPage3 = "http://www.nokia.com/";
const QString KDefaultPage4 = "http://www.ovi.com/";
const QString KDefaultPage5 = "http://www.cnn.com/";

MostVisitedPage::MostVisitedPage()
    : m_pageThumbnail(0)
    , m_pageRank(0)
{
}

MostVisitedPage::MostVisitedPage(QString url, QImage *pageThumbnail, uint pageRank)
    : m_url(url)
    , m_pageThumbnail(pageThumbnail)
    , m_pageRank(pageRank)
{

}

MostVisitedPage::~MostVisitedPage()
{
    delete m_pageThumbnail;
}

QDataStream& operator<<(QDataStream &out, const MostVisitedPage &page)
{
    //Writing pattern is as follows
    //URL
    //false or (true & Image)
    //pageRank


    //write page data to stream
    out << page.m_url;
    //check if page thumbnail is avilable, store the availability too
    if (!page.thumbnailAvailable())
        out << false;
    else {
        //this is needed to get the correct offset while reading
        out << true << *page.m_pageThumbnail;
    }
    out << page.m_pageRank;
    return out;
}

QDataStream& operator>>(QDataStream &in, MostVisitedPage &page)
{
    //Reading pattern is as follows
    //URL
    //false or (true & Image)
    //pageRank

    //read page data from stream
    in >> page.m_url;
   
    bool hasThumbnail = false;
    in >> hasThumbnail;

    if (hasThumbnail) {
        page.m_pageThumbnail = new QImage();
        in >> *page.m_pageThumbnail;
    }

    in >> page.m_pageRank;

    return in;
}


MostVisitedPageStore::MostVisitedPageStore()
        : m_needPersistWrite(false)
{
    // initialize the dir used to store bookmarks
#ifndef QT_NO_DESKTOPSERVICES
        m_mvpFile = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/" + KMostVistedStoreFile;
#else
        m_mvpFile =  = QDir::homePath() + "/" +KMostVistedStoreFile;
#endif

    readStore();
}


void MostVisitedPageStore::clearMostVisitedPageStore()
{
    for (int i = m_pageList.size() - 1; i >= 0; --i)
        delete m_pageList.takeAt(i);
    
    //deleting MVP file
    QFile file(m_mvpFile);
    file.remove();

    //create default thumbnails
    initializeDefaultPageThumbnails();
}

MostVisitedPageStore::~MostVisitedPageStore()
{
    writeStore();
    for (int i = m_pageList.size() - 1; i >= 0; --i)
        delete m_pageList.takeAt(i);
}

void MostVisitedPageStore::pageAccessed(const QUrl& url, QImage* pageThumbnail, int pageRank)
{
    QString accessedHost = url.host();
    int found = -1;

    for (int i = 0; i < m_pageList.size(); ++i) {
        MostVisitedPage* page = m_pageList[i];
        QUrl localUrl = page->m_url;
        QString localUrlHost = localUrl.host();
        if (compareUrls(localUrlHost, accessedHost)) {
            page->m_pageRank = pageRank;
            // move it up if needed
            int j = i;
            // '<=' is for the last access sorting, recently used items move up
            while (--j >= 0 && page->m_pageRank >= m_pageList.at(j)->m_pageRank)
                ;
            // position adjusting and check whether we really moved
            if (++j != i) 
                m_pageList.move(i, j);
            found = j;
            break;
        }
    }
    
    if (found == -1) {
        // insert to the top of the 1 refcount items. recently used sort
        int i = -1;
        while (++i < m_pageList.size() && m_pageList.at(i)->m_pageRank > pageRank)
            ;
        
        //if this page is not fitting in MV list, delete thumbnail passed
        if (i == m_pageList.size()) {
            delete pageThumbnail;
            return;
        }

        m_pageList.insert(i, new MostVisitedPage(url.toString(), pageThumbnail, pageRank));

        //check if we have crossed the store limit and remove last item if so
        if (m_pageList.size() > KMostVistedStoreLimit) {
            delete m_pageList.takeLast();
        }
    } else if (pageThumbnail) {
        // add thumbnail, delete if it has any old thumbnail
        
        //This check is specifically added to take into account of
        //future code changes if thumbnail gets assigned somewhere else
        if (m_pageList[found]->m_pageThumbnail) {
            delete m_pageList[found]->m_pageThumbnail;
        }
        
        m_pageList[found]->m_pageThumbnail = pageThumbnail;
    }

    m_needPersistWrite = true;
    writeStore();
}

bool MostVisitedPageStore::contains(const QString& url, bool checkThumbnail)
{
    if (!url.size())
        return false;
    
    QUrl accessUrl(url);
    QString accessedHost = accessUrl.host();

    for (int i = 0; i < m_pageList.size(); ++i) {
        QUrl localUrl = m_pageList[i]->m_url;
        QString localUrlHost = localUrl.host();

        if (compareUrls(localUrlHost, accessedHost) 
                && (!checkThumbnail
                    || (checkThumbnail && m_pageList.at(i)->thumbnailAvailable()))) {
            return true;
        }
    }
    return false;
}

MostVisitedPage *MostVisitedPageStore::pageAt(int index)
{
    if (index < 0 || index >= m_pageList.size())
        return 0;

    return m_pageList.at(index);

}

bool MostVisitedPageStore::compareUrls(QString& url1, QString &url2)
{
    // www.nytimes.com == www.nytimes.com
    if (url1 == url2)
        return true;

    // www.nytimes.com == nytimes.com
    if ((url1.startsWith("www.") && url1.mid(4) == url2) || 
        (url2.startsWith("www.") && url2.mid(4) == url1))
        return true;
    return false;
}

void MostVisitedPageStore::readStore()
{
    QFile file(m_mvpFile);

    if (file.open(QFile::ReadOnly)) {
        QDataStream in(&file);   
        //Check the store version.
        uint version;
        in>>version;

        if (version == KMostVistedStoreVersion ) {
            while (!in.atEnd()) {
                MostVisitedPage *mvPage = new MostVisitedPage();
                in>>*mvPage;
                m_pageList.append(mvPage);
            }
        }
        file.close();
    }
    
    initializeDefaultPageThumbnails();
}

void MostVisitedPageStore::writeStore()
{  
    //if only store is modified then save it.
    
    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveHistory");    	
    
    if ((!m_needPersistWrite) || (!enabled))
        return;

    // save url store
    // version
    // url, thumbnail, rank
    QFile file(m_mvpFile);

    if (file.open(QFile::WriteOnly | QIODevice::Truncate)) {
        QDataStream out(&file);
        out<<KMostVistedStoreVersion;
        for (int i = 0; i < m_pageList.size(); ++i) {
            out<<*m_pageList[i];
        }
        file.close();
    } 
    m_needPersistWrite = false;
}

void MostVisitedPageStore::initializeDefaultPageThumbnails() 
{
    if (!m_pageList.isEmpty()) 
        return;
    
    m_pageList.append(new MostVisitedPage(KDefaultPage1));
    m_pageList.append(new MostVisitedPage(KDefaultPage2));
    m_pageList.append(new MostVisitedPage(KDefaultPage3));
    m_pageList.append(new MostVisitedPage(KDefaultPage4));
    m_pageList.append(new MostVisitedPage(KDefaultPage5));
}
