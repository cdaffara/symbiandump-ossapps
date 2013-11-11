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

#ifndef LINEARFLOWSNIPPET_H
#define LINEARFLOWSNIPPET_H

#include <QTime>
#include <QImage>
#include <QWidget>
#include <QGraphicsWidget>
#include <QGraphicsItem>
#include "ChromeItem.h"

class DragPoint
    {
    public:
        QPoint iPoint;
        QTime  iTime;
    };

namespace GVA {

class Filmstrip;
class FilmstripFlowPrivate;
class FilmstripMovieFactory;

/*!
  Class GraphicsFilmstripFlow
 */
class LinearFlowSnippet: public ChromeItem
{
Q_OBJECT
friend class Filmstrip;
friend class FilmstripMovieFactory;

public:
    /*!
    Creates a new FilmstripFlow widget.
    */
    LinearFlowSnippet(QGraphicsWidget* parent = 0);

    /*!
    Destroys the widget.
    */
    ~LinearFlowSnippet();

    /*!
    init the FilmstripFlow
    */
    void init(QString displayMode, QString titleName);

    //! Clear all slides
    void clear();

    //! Add a slide to the flow
    void addSlide(const QImage& image);

    //! Add a slide to the flow with title
    void addSlide(const QImage& image, const QString& title);

    //! Set the center of the flow
    void setCenterIndex(int i);

    //! Show the previous item
    void showPrevious();

    //! Show the next item
    void showNext();

    //! Returns the total number of slides.
    int slideCount() const;

    //! Returns QImage of specified slide.
    QImage slide(int) const;

    //! Return the index of the current center slide
    int centerIndex() const;

    //! return true if slide animation is ongoing
    bool slideAnimationOngoing() const;

    //! return center slide's rect
    QRect centralRect() const;

    //! show the ith slide
    void showSlide(int) {}

    //! inserts filmstrip at index position i.
    void insert(int i, const QImage& image, const QString& title);

    //! removes filmstrip at index position i.
    void removeAt (int i);

    //! set background color
    void backgroundColor(const QRgb& c);

    //! handle the display mode change
    void displayModeChanged(QString& newMode);

    //! prepare start-animation
    void prepareStartAnimation();

    //! run start-animation
    void runStartAnimation();

    //! run end-animation
    void runEndAnimation();

    bool isFlick();

    QPoint speed();

    QPoint currentPos();

    QPoint previousPos();

    qreal dragTime() const;

    void startFlickScroll();

signals:
    void removed(int index);
    void endAnimationCplt();
    void centerIndexChanged(int index);
    void ok(int index);
    void cancel();
protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void resizeEvent(QGraphicsSceneResizeEvent* event);
    void moveEvent(QGraphicsSceneMoveEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private slots:
    void stopMovie();
    void playMovie(int frame);
    void closeAnimation();

private:
    void scroll();
    void adjustFilmstripSize(QSize& s);
    void showInsertOnRight();
    void showInsertOnLeft();
    bool hitCloseIcon();

private:
    FilmstripFlowPrivate* d;
    QList<DragPoint> m_dragPoints;
    QTime m_lastMoveEventTime;
    bool m_scrolled;
    QString m_displayMode;
    int m_countFlicks;
    QTimer *m_flickTimer;
    QString m_titleName;
};

}
#endif // LINEARFLOWSNIPPET_H

