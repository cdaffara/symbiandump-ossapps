/*
 * bookmarkscontroller.h
 *
 *  Created on: Aug 25, 2010
 *      Author: mmoretti
 */

#ifndef BOOKMARKSCONTROLLER_H_
#define BOOKMARKSCONTROLLER_H_

#include <QObject>
#include "BWFGlobal.h"
class QWidget;

class BookmarksManager;
class BookmarkResults;
class BookmarkFav;
//#include "BookmarksManager.h"

class BWF_EXPORT Suggestion : public QObject
{
        Q_OBJECT
        
        public:
            Suggestion(QString url, QString title) : m_url(url), m_title(title){};
            // Hack: Changing the name to url1 from url as some namespace clash
            // is happening on Linux
            Q_PROPERTY(QString title1 READ suggestTitle)
            Q_PROPERTY(QString url1 READ suggestUrl)
                                  
        private:
            QString m_url;
            QString m_title;
            QString suggestTitle() {return m_title;}
            QString suggestUrl() {return m_url;}
};


class BWF_EXPORT BookmarksController : public QObject {

	  Q_OBJECT

public:
	virtual ~BookmarksController();
    static BookmarksController* getSingleton();

public slots:
    int addBookmark(QString title, QString URL);
    int modifyBookmark(int origBookmarkId, QString newTitle, QString newURl);
    int deleteBookmark(int bookmarkId);
    int clearAll();
    int findAllBookmarks();
    QObject *findBookmark(int bookmarkID);
    int reorderBookmark(int bookmarkID, int newIndex);
    QObject *nextBookmark();
    bool hasMoreBookmarks();
    void showBookmarkEditDialog(QString title, QString url, int bookmarkID);
    QObjectList suggestSimilar(QString suggest);
    // TODO add tag stuff when we get a ui for it

signals:
      void bookmarkAdded(QString,QString,int);
      void bookmarksCleared();
      void launchBookmarkEditDailog(QString,QString,int);
      void bookmarkModified(QString, QString, int);

private:
  	BookmarksController(QWidget *parent = 0);

    BookmarksManager *m_bm;
    BookmarkResults *m_bmr;
    BookmarkFav *m_bmf;
};

#endif /* BOOKMARKSCONTROLLER_H_ */
