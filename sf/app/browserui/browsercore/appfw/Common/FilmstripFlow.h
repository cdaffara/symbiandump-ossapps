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
*   FilmstripFlow - animated image show widget
*/

#ifndef FILMSTRIPFLOW_H
#define FILMSTRIPFLOW_H

#include <QImage>
#include <QWidget>

#include "FlowInterface.h"

namespace WRT {

class Filmstrip;
class FilmstripFlowPrivate;
class FilmstripMovieFactory;

/*!
  Class FilmstripFlow
 */ 
class FilmstripFlow: public FlowInterface  
{
Q_OBJECT
friend class Filmstrip;
friend class FilmstripMovieFactory;

public:
    /*!
    Creates a new FilmstripFlow widget.
    */  
    FilmstripFlow(QWidget* parent = 0);

    /*!
    Destroys the widget.
    */
    ~FilmstripFlow();

    /*!
    init the FilmstripFlow
    */
    void init();

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

signals:
    void removed(int index);
    void endAnimationCplt();

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void moveEvent(QMoveEvent * event);

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
};

/*!
  Class GraphicsFilmstripFlow
 */ 
class GraphicsFilmstripFlow: public GraphicsFlowInterface
{
Q_OBJECT
friend class Filmstrip;
friend class FilmstripMovieFactory;

public:
    /*!
    Creates a new FilmstripFlow widget.
    */  
    GraphicsFilmstripFlow(QObject* parent = 0);

    /*!
    Destroys the widget.
    */
    ~GraphicsFilmstripFlow();

    /*!
    init the FilmstripFlow
    */
    void init();

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

signals:
    void removed(int index);
    void endAnimationCplt();

protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0); 
    void resizeEvent(QGraphicsSceneResizeEvent* event); 
    void moveEvent(QGraphicsSceneMoveEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
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
};

}
#endif // FILMSTRIPFLOW_H

