/*
 * bookmarkscontroller.cpp
 *
 *  Created on: Aug 25, 2010
 *      Author: mmoretti
 */


#include <QObject>
#include <QWidget>
#include <QDebug>

#include "bookmarkscontroller.h"
#include "BookmarksManager.h"
#include "BookmarkFav.h"
#include "BookmarkResults.h"
#include "HistoryManager.h"

BookmarksController::BookmarksController(QWidget *parent) :
	    QObject(parent)
{
	setObjectName("bookmarksController");
	m_bm = new BookmarksManager(parent);
}

BookmarksController::~BookmarksController() {
	// TODO Auto-generated destructor stub
}

BookmarksController* BookmarksController::getSingleton()
{
    static BookmarksController* singleton = 0;

    if(!singleton){
           singleton = new BookmarksController;
    }

    //assert(singleton);
    return singleton;
}


int BookmarksController::addBookmark(QString title, QString URL)
{
	int bmid = m_bm->addBookmark(title, URL);
//	qDebug() << __PRETTY_FUNCTION__ << "added bookmark" << title << URL  << bmid << "emitting bookmarkAdded";
	emit bookmarkAdded(title, URL, bmid);
	return(bmid);
}

int BookmarksController::modifyBookmark(int origBookmarkId, QString newTitle, QString newURL)
{
	int retstat = m_bm->modifyBookmark(origBookmarkId, newTitle, newURL);
	emit bookmarkModified(newTitle, newURL, origBookmarkId);
	return retstat;
}

int BookmarksController::deleteBookmark(int bookmarkId)
{
	return m_bm->deleteBookmark(bookmarkId);
}

int BookmarksController::clearAll()
{
	int retstat = m_bm->clearAll();
	emit bookmarksCleared();
	return retstat;
}

int BookmarksController::findAllBookmarks()
{
	m_bmr = m_bm->findAllBookmarks();
	if (!m_bmr)
		return -1;
	return(0);
}

// This is only necessary because JS can't handle null objects being returned by nextBookmark
bool BookmarksController::hasMoreBookmarks()
{
	if (!m_bmr)
		return false;

	m_bmf = m_bmr->nextBookmark();
	if (!m_bmf) {
		delete m_bmr;
		m_bmr = 0;
		return false;
	}

	return true;
}


QObject *BookmarksController::nextBookmark()
{
	return m_bmf;
}

int BookmarksController::reorderBookmark(int bookmarkID, int newIndex)
{
	return m_bm->reorderBookmark(bookmarkID, newIndex);
}

QObject *BookmarksController::findBookmark(int bookmarkID)
{
	return m_bm->findBookmark(bookmarkID);
}

void BookmarksController::showBookmarkEditDialog(QString title, QString url, int bookmarkID)
{
	// This is SOOOO convoluted; in our js, in order to call a function in another scope, we have to call a function here that emits a signal that is connected to that function, wtf?
	emit launchBookmarkEditDailog(title, url, bookmarkID);
}

QObjectList BookmarksController::suggestSimilar(QString suggest)
{
    QMap<QString, QString> bookmarksMap = m_bm->findBookmarks(suggest);
    QMap<QString, QString> historyMap = WRT::HistoryManager::getSingleton()->findHistory(suggest);
    // Now combine the result
    QObjectList suggestions;
    
    QMapIterator<QString,QString> bhi(historyMap); 
    while (bhi.hasNext()) { 
        bhi.next(); 
        suggestions.append(new Suggestion(bhi.key(), bhi.value())); 
    } 

    QMapIterator<QString,QString> bmi(bookmarksMap); 
    while (bmi.hasNext()) { 
        bmi.next(); 
        if (historyMap.contains(bmi.key())) 
            continue; 
        suggestions.append(new Suggestion(bmi.key(), bmi.value())); 
    }
    
    return suggestions;

}

// TODO add tag stuff when we get a ui for it
