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
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QResizeEvent>
#include <QTimeLine>
#include <QGraphicsSceneResizeEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMoveEvent>

#include "FilmstripFlow.h"

#define SAFE_DELETE(p) if(p) delete p;


#define CALL_ON_PREV_PREV_FILM_STRIP(func) if (d->m_centerIndex - 2 >= 0) d->m_films[d->m_centerIndex - 2]->func;
#define CALL_ON_PREV_FILM_STRIP(func) if (d->m_centerIndex - 1 >= 0) d->m_films[d->m_centerIndex - 1]->func;
#define CALL_ON_CENTER_FILM_STRIP(func) if (d->m_centerIndex >= 0 && d->m_centerIndex < d->m_films.size()) d->m_films[d->m_centerIndex]->func;
#define CALL_ON_NEXT_FILM_STRIP(func) if (d->m_centerIndex + 1 < d->m_films.size()) d->m_films[d->m_centerIndex + 1]->func;
#define CALL_ON_NEXT_NEXT_FILM_STRIP(func) if (d->m_centerIndex + 2 < d->m_films.size()) d->m_films[d->m_centerIndex + 2]->func;

#define INVALID_INDEX -1

#define L_CENTER_WIDTH_P_C 0.55
#define L_CENTER_HEIGHT_P_C 0.70
#define L_SIDE_WIDTH_P_C 0.40
#define L_SIDE_HEIGHT_P_C 0.62

//#define P_CENTER_WIDTH_P_C 0.50
#define P_CENTER_WIDTH_P_C 0.55
#define P_CENTER_HEIGHT_P_C 0.70
#define P_SIDE_WIDTH_P_C 0.40
#define P_SIDE_HEIGHT_P_C 0.55

//#define L_CENTER_TOP_SPACE_P_C 0.2
#define L_CENTER_TOP_SPACE_P_C 0.28
//#define L_SIDE_TOP_SPACE_P_C 0.24
#define L_SIDE_TOP_SPACE_P_C 0.36
#define L_SPACE_P_C 0.12

//#define P_CENTER_TOP_SPACE_P_C 0.20
#define P_CENTER_TOP_SPACE_P_C 0.22
//#define P_SIDE_TOP_SPACE_P_C 0.30
#define P_SIDE_TOP_SPACE_P_C 0.30
#define P_SPACE_P_C 0.12

#define SPACE_P_C 0.12
#define CLOSE_ICON_ADJUST_SIZE 20
//#define TITLE_SPACE 18
#define TITLE_SPACE 25
#define FRAME_WIDTH 4

#define ANIMATION_DURATION 400
#define ANIMATION_MAX_FRAME 15.0
#define NEW_FILM_TITLE "New Window"

namespace WRT {

// movies (animation) name
const QString BreakinLeft   = "bil";//"breakin - left";
const QString BreakinRight  = "bir";//"breakin - right";
const QString BreakoutLeft  = "bol";//"breakout - left";
const QString BreakoutRight = "bor";//"breakout - right";
const QString RightToCenter = "rc";//"right - center";
const QString LeftToCenter  = "lc";//"left - center";
const QString CenterToRight = "cr";//"center - right";
const QString CenterToLeft  = "cl";//"center - left";
const QString FadeOut       = "fo";//"fadeout";
const QString ZoomIn        = "zi";//"zoomin";
const QString ZoomOut       = "zo";//"zoomto";
// -------------------------------------------------------
// Help functions

//static QImage* createFilmstrip(const QImage& img, QSize

// -------------------------------------------------------
// Help classes
class FilmstripMovieFactory;

class FilmstripMovie
{
friend class FilmstripMovieFactory;

public:

enum MovieType
{
FADE_IN,
FADE_OUT,
TRANSLATE
};

public:
    FilmstripMovie(const QString& name) { m_name = name; m_movieType = TRANSLATE;}
    ~FilmstripMovie() {m_movieClips.clear();}
    QRectF& movieClip(int frame);

public:
    MovieType m_movieType;

private:
    QString m_name;
    QList<QRectF> m_movieClips;
};

class FilmstripMovieFactory
{
public:
    FilmstripMovieFactory(FilmstripFlowPrivate* filmstripFlowData = NULL): m_filmstripFlowData(filmstripFlowData) {}
    ~FilmstripMovieFactory();

    FilmstripMovie* createMovie(const QString& name);

    void updateAllMovie();
    void setFilmstripFlowData(FilmstripFlowPrivate* filmstripFlowData) { m_filmstripFlowData = filmstripFlowData; }

protected:
    void addRectByFrame(FilmstripMovie* movie, QRectF& startRect, QRectF& endRect, qreal x1, qreal y1, qreal x2, qreal y2, bool debug = false);
    void createRightOutMovie(FilmstripMovie* movie);
    void createRightInMovie(FilmstripMovie* movie);
    void createLeftInMovie(FilmstripMovie* movie);
    void createLeftOutMovie(FilmstripMovie* movie);
    void createRightToCenterMovie(FilmstripMovie* movie);
    void createLeftToCenterMovie(FilmstripMovie* movie);
    void createCenterToLeftMovie(FilmstripMovie* movie);
    void createCenterToRightMovie(FilmstripMovie* movie);
    void createFadeOutMovie(FilmstripMovie* movie);
    void createZoomInMovie(FilmstripMovie* movie);
    void createZoomOutMovie(FilmstripMovie* movie);

private:
    FilmstripFlowPrivate* m_filmstripFlowData;
    QHash<QString, FilmstripMovie*> m_moviesCache;
};

class Filmstrip
{
public:
    Filmstrip(const QImage& img, FilmstripFlowPrivate* filmstripFlowData): m_img(img), m_frozen(false), m_movieFrame(0), m_movie(NULL), m_filmstripFlowData(filmstripFlowData) {}
    ~Filmstrip() {}

    void paint(QPainter* painter);
    void paintName(QPainter* painter);

    void freeze() {m_frozen = true;}
    void updateMovie(FilmstripMovie* movie) {m_movie = movie; m_movieFrame = 0; m_frozen = false;}
    void updateMovieFrame(int frame) { if(!m_frozen) m_movieFrame = frame;}
    void setName(const QString& name) {m_name = name;}
    QImage& image() {return m_img;}
    QString& name() {return m_name;}
private:
    void createEmptyImage();

public:
    QImage m_img;
    bool m_frozen;
    int m_movieFrame;
    QString m_name;
    FilmstripMovie* m_movie;
    FilmstripFlowPrivate* m_filmstripFlowData;
};

class FilmstripFlowPrivate
{
public:
    FilmstripFlowPrivate(): m_bgColor(QColor(Qt::white).rgb()), m_buffer(NULL), m_closeIcon(NULL), m_centerTopSpace(0), m_sideTopSpace(0), m_space(0), m_incIndex(0), m_centerIndex(INVALID_INDEX) {}

    ~FilmstripFlowPrivate() {
        for (int i = 0; i < m_films.size(); i++)
            SAFE_DELETE(m_films[i]);
        m_films.clear();
        SAFE_DELETE(m_buffer);
        SAFE_DELETE(m_closeIcon);
    }

    void clear() {
        m_centerIndex = INVALID_INDEX;
        for (int i = 0; i < m_films.size(); i++)
            SAFE_DELETE(m_films[i]);
        m_films.clear();
    }

public:
    QRgb m_bgColor;
    QImage* m_buffer;
    QImage* m_closeIcon;
    QList<Filmstrip*> m_films;
    QSize m_centerWindowSize;
    QSize m_sideWindowSize;
    QSize m_widgetSize;
    qreal m_centerTopSpace;
    qreal m_sideTopSpace;
    qreal m_space;
    QTimeLine m_movieTimer;
    FilmstripMovieFactory m_movieFactory;
    QPoint m_closeIconPos;
    QPoint m_lastMoveEventPos;
    int m_incIndex;
    int m_centerIndex;
};

// -------------------------------------------------------
// Filmstrip
void Filmstrip::createEmptyImage()
{
    QRectF target = m_movie->movieClip(0);
    int w = target.width();
    int h = target.height();
#ifndef BROWSER_LAYOUT_TENONE
    if (m_name.isEmpty())
        m_name =  qtTrId("txt_browser_windows_new_window");
#endif
    m_img = QImage(w, h, QImage::Format_RGB32);

    QPainter painter(&m_img);
    painter.fillRect(0, 0, w, h, Qt::white);
    /*QPoint p1(w*4/10, 0);
    QPoint p2(w*6/10, h);
    QLinearGradient linearGrad(p1, p2);
    linearGrad.setColorAt(0, Qt::black);
    linearGrad.setColorAt(1, Qt::white);
    painter.setBrush(linearGrad);
    painter.fillRect(0, 0, w, h, QBrush(linearGrad));
    */
    //painter.setPen(QPen(QColor(64,64,64), 4));
    //painter.setBrush(QBrush());
    //painter.drawRect(2, 2, w-3, h-3);
    painter.end();
}

void Filmstrip::paint(QPainter* painter)
{
    Q_ASSERT(painter);

    if (!m_movie)
        return;

    QRectF target;

    bool needFade = (m_movie->m_movieType == FilmstripMovie::FADE_OUT);
    if (needFade) // FIXME: no fade out support now
        target = m_movie->movieClip(0);
    else
        target = m_movie->movieClip(m_movieFrame);

    //qDebug() << "m_movieFrame:" << m_movieFrame << " -- " << target;
    if(target.right() > 0 || target.left() < m_filmstripFlowData->m_widgetSize.width()) {
        //painter->setPen(QPen(QColor(Qt::gray), 4));
        //painter->setBrush(QBrush());
        //painter->drawRect(target.x() - 2, target.y() - 2, target.width() + 5, target.height() + 5);
        if (needFade)
            painter->setOpacity((ANIMATION_MAX_FRAME - m_movieFrame) / ANIMATION_MAX_FRAME);

        painter->fillRect(target.adjusted(-FRAME_WIDTH,-FRAME_WIDTH,FRAME_WIDTH,FRAME_WIDTH), QColor(Qt::gray));
        if (m_img.isNull())
            createEmptyImage();

        painter->drawImage(target, m_img);

        if (needFade)
            painter->setOpacity(1); // restore opacity
    }
}

void Filmstrip::paintName(QPainter* painter)
{
    Q_ASSERT(painter);
    if (!m_name.isEmpty()) {
        QPoint startPoint;
        QString name = m_name;
        QFontMetrics fm = painter->fontMetrics();
        int pixelWidth = fm.width(name);
        int filmstripWidth = m_filmstripFlowData->m_centerWindowSize.width();
        if (pixelWidth > filmstripWidth) {
            qreal letterWidth = pixelWidth / (qreal)name.size();
            name = name.leftJustified(filmstripWidth / letterWidth - 3, '.', true) + "...";
            startPoint = QPoint((m_filmstripFlowData->m_widgetSize.width() - filmstripWidth) /2, m_filmstripFlowData->m_centerTopSpace - TITLE_SPACE);
        }
        else
            startPoint = QPoint((m_filmstripFlowData->m_widgetSize.width() - pixelWidth) /2, m_filmstripFlowData->m_centerTopSpace - TITLE_SPACE);

        painter->save();
        painter->setPen(QColor(Qt::black));
        painter->drawText(startPoint, name);
        painter->restore();
    }
}

// -------------------------------------------------------
// FilmstripMovie
QRectF& FilmstripMovie::movieClip(int frame)
{
    Q_ASSERT(frame >= 0 && frame < m_movieClips.size());
    return m_movieClips[frame];
}

// -------------------------------------------------------
// FilmstripMovieFactory
FilmstripMovieFactory::~FilmstripMovieFactory()
{
    QHashIterator<QString, FilmstripMovie*> i(m_moviesCache);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
    m_moviesCache.clear();
}

FilmstripMovie* FilmstripMovieFactory::createMovie(const QString& name)
{
    FilmstripMovie* movie = NULL;
    QHash<QString, FilmstripMovie*>::const_iterator i = m_moviesCache.find(name);
    if (i == m_moviesCache.end()) {
        //qDebug() << "create a new movie: " << name;
        movie = new FilmstripMovie(name);

        if (name == BreakoutRight)
            createRightOutMovie(movie);
        else if (name == BreakoutLeft)
            createLeftOutMovie(movie);
        else if (name == BreakinLeft)
            createLeftInMovie(movie);
        else if (name == BreakinRight)
            createRightInMovie(movie);
        else if (name == CenterToRight)
            createCenterToRightMovie(movie);
        else if (name == CenterToLeft)
            createCenterToLeftMovie(movie);
        else if (name == RightToCenter)
            createRightToCenterMovie(movie);
        else if (name == LeftToCenter)
            createLeftToCenterMovie(movie);
        else if (name == FadeOut)
            createFadeOutMovie(movie);
        else if (name == ZoomIn)
            createZoomInMovie(movie);
        else if (name == ZoomOut)
            createZoomOutMovie(movie);

        m_moviesCache[name] = movie;
    } else {
        movie =  m_moviesCache.value(name);
    }
    return movie;
}

void FilmstripMovieFactory::addRectByFrame(FilmstripMovie* movie, QRectF& startRect, QRectF& endRect, qreal x1, qreal y1, qreal x2, qreal y2, bool debug)
{
    movie->m_movieClips.append(startRect);
    if (debug)
        qDebug() << "0:" << startRect;
    for (int i = 1; i < ANIMATION_MAX_FRAME; i++) {
        if (debug)
            qDebug() << i << ":" << movie->m_movieClips[i - 1].adjusted(x1, y1, x2, y2);
        movie->m_movieClips.append(movie->m_movieClips[i - 1].adjusted(x1, y1, x2, y2));
    }
    movie->m_movieClips.append(endRect);
    if (debug)
        qDebug() << movie->m_movieClips.size() - 1 << ":"  << endRect;
}

void FilmstripMovieFactory::createLeftToCenterMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = (w - cw) / 2.0;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;
    qreal sx = (w * (1 - 2 * SPACE_P_C) - cw) / 2.0 - sw; // sild window's x
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / step;
    qreal stepy = (cy - sy) / step;
    qreal stepx2 = stepx + (cw - sw) / step;
    qreal stepy2 = stepy + (ch - sh) / step;
    //qDebug() << "sw:" << sw << " sh:" << sh << " cw:" << cw << " ch:" << ch;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(cx, cy, cw, ch);

    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
    //qDebug() << "!!!!!!createLeftToCenterMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createRightToCenterMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = (w - cw) / 2.0;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;
    qreal sx = (w * (1 + 2 * SPACE_P_C) + cw) / 2.0; // sild window's x
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / step;
    qreal stepy = (cy - sy) / step;
    qreal stepx2 = stepx + (cw - sw) / step;
    qreal stepy2 = stepy + (ch - sh) / step;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(cx, cy, cw, ch);

    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
    //qDebug() << "!!!!!!createRightToCenterMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createCenterToLeftMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = (w - cw) / 2.0;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;
    qreal sx = (w * (1 - 2 * SPACE_P_C) - cw) / 2.0 - sw; // sild window's x
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / step;
    qreal stepy = (sy - cy) / step;
    qreal stepx2 = stepx + (sw - cw) / step;
    qreal stepy2 = stepy + (sh - ch) / step;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(cx, cy, cw, ch);
    QRectF endRect = QRectF(sx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
    //qDebug() << "!!!!!!createCenterToLeftMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createCenterToRightMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = (w - cw) / 2.0;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;
    qreal sx = (w * (1 + 2 * SPACE_P_C) + cw) / 2.0; // sild window's x
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / step;
    qreal stepy = (sy - cy) / step;
    qreal stepx2 = stepx + (sw - cw) / step;
    qreal stepy2 = stepy + (sh - ch) / step;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(cx, cy, cw, ch);
    QRectF endRect = QRectF(sx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
    //qDebug() << "!!!!!!createCenterToRightMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createLeftOutMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal cx = (w - cw) / 2.0;
    qreal sx = (w * (1 - 2 * SPACE_P_C) - cw) / 2.0 - sw; // sild window's x
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx -cx) / ANIMATION_MAX_FRAME;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(2 * sx - cx , sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
    //qDebug() << "!!!!!!createLeftInMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createLeftInMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal cx = (w - cw) / 2.0;
    qreal sx = (w * (1 - 2 * SPACE_P_C) - cw) / 2.0 - sw; // sild window's x
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / ANIMATION_MAX_FRAME;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(2 * sx - cx , sy, sw, sh);
    QRectF endRect = QRectF(sx, sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
    //qDebug() << "!!!!!!createLeftInMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createRightOutMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal cx = (w - cw) / 2.0;
    qreal sx = (w * (1 + 2 * SPACE_P_C) + cw) / 2.0; // sild window's x
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(sx + sx -cx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
    //qDebug() << "!!!!!!createRightOutMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createRightInMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal cx = (w - cw) / 2.0;
    qreal sx = (w * (1 + 2 * SPACE_P_C) + cw) / 2.0; // sild window's x
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / ANIMATION_MAX_FRAME;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(sx + sx -cx, sy, sw, sh);
    QRectF endRect = QRectF(sx, sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
    //qDebug() << "!!!!!!createRightInMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createFadeOutMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();

    qreal cx = (w - cw) / 2.0;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;

    QRectF startRect = QRectF(cx, cy, cw, ch);
    movie->m_movieClips.append(startRect);

    movie->m_movieType = FilmstripMovie::FADE_OUT;
    //qDebug() << "!!!!!!createFadeOutMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createZoomInMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = (w - cw) / 2.0;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;

    qreal stepx = - cx / step;
    qreal stepy = - cy / step;
    qreal stepx2 = - stepx;
    qreal stepy2 = (h - cy - ch) / step;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(cx, cy, cw, ch);
    QRectF endRect = QRectF(0, 0, w, h);

    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
    //qDebug() << "!!!!!!createZoomInMovie" << startRect << " ---> " << endRect;
}

void FilmstripMovieFactory::createZoomOutMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = (w - cw) / 2.0;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;

    qreal stepx = cx / step;
    qreal stepy = cy / step;
    qreal stepx2 = -stepx;
    qreal stepy2 = (cy + ch - h) / step;
    //qDebug() << " --------- " << stepx << " " << stepy << " " << stepx2 << " " << stepy2;

    QRectF startRect = QRectF(0, 0, w, h);
    QRectF endRect = QRectF(cx, cy, cw, ch);

    //qDebug() << "!!!!!!createZoomOutMovie" << startRect << " ---> " << endRect;
    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
}

void FilmstripMovieFactory::updateAllMovie()
{
    QHash<QString, FilmstripMovie*>::const_iterator i = m_moviesCache.find(BreakoutRight);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakoutRight);
        createRightOutMovie(movie);
    }

    i = m_moviesCache.find(BreakoutLeft);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakoutLeft);
        createLeftOutMovie(movie);
    }

    i = m_moviesCache.find(BreakinLeft);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakinLeft);
        createLeftInMovie(movie);
    }

    i = m_moviesCache.find(BreakinRight);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakinRight);
        createRightInMovie(movie);
    }

    i = m_moviesCache.find(CenterToLeft);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(CenterToLeft);
        createCenterToLeftMovie(movie);
    }

    i = m_moviesCache.find(CenterToRight);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(CenterToRight);
        createCenterToRightMovie(movie);
    }

    i = m_moviesCache.find(LeftToCenter);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(LeftToCenter);
        createLeftToCenterMovie(movie);
    }

    i = m_moviesCache.find(RightToCenter);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(RightToCenter);
        createRightToCenterMovie(movie);
    }

    i = m_moviesCache.find(FadeOut);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(FadeOut);
        createFadeOutMovie(movie);
    }

    i = m_moviesCache.find(ZoomIn);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(ZoomIn);
        createZoomInMovie(movie);
    }

    i = m_moviesCache.find(ZoomOut);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(ZoomOut);
        createZoomOutMovie(movie);
    }
}

// -------------------------------------------------------
// FilmstripFlow.
/*!
  Creates a new FilmstripFlow widget.
*/
FilmstripFlow::FilmstripFlow(QWidget* parent): FlowInterface(parent), d(new FilmstripFlowPrivate())
{
}

/*!
  Destroys the widget.
*/
FilmstripFlow::~FilmstripFlow()
{
    delete d;
}

/*!
  Init the FilmstripFlow
*/
void FilmstripFlow::init()
{
    if (!d->m_closeIcon)
        d->m_closeIcon = new QImage(":/resources/close.png");
    d->m_movieFactory.setFilmstripFlowData(d);

    d->m_movieTimer.setDuration(ANIMATION_DURATION);
    d->m_movieTimer.setCurveShape(QTimeLine::EaseOutCurve);
    d->m_movieTimer.setFrameRange(0, ANIMATION_MAX_FRAME);

    QObject::connect(&d->m_movieTimer, SIGNAL(frameChanged(int)), this, SLOT(playMovie(int)));
    QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));

    QSize s = rect().size();
    d->m_widgetSize = s;
    adjustFilmstripSize(s);
    // ensure that system cursor is an arrow, not a random icon
    // This is not an issue if the platform does not have a system cursor
#ifndef __SYMBIAN32__
    setCursor(Qt::ArrowCursor);
#endif
    setFocusPolicy(Qt::WheelFocus);
    setFocus(Qt::OtherFocusReason);
}

/*!
  Set center index
*/
void FilmstripFlow::setCenterIndex(int i)
{
    //qDebug() << "Set Center Index:" << i;
    Q_ASSERT(d);
    if (!d->m_films.size())
        return;
    Q_ASSERT(i >= 0 && i < d->m_films.size());
    d->m_centerIndex = i;
    d->m_films[d->m_centerIndex]->updateMovie(d->m_movieFactory.createMovie(CenterToRight));

    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(NULL)); // hide left left
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRight)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(NULL)); // hide right right

    emit centerIndexChanged(i);
}

/*!
  add slide
*/
void FilmstripFlow::addSlide(const QImage& image)
{
    Q_ASSERT(d);
    Filmstrip* filmstrip = new Filmstrip(image, d);
    d->m_films.append(filmstrip);
}

/*!
  add slide
*/
void FilmstripFlow::addSlide(const QImage& image, const QString& title)
{
    Q_ASSERT(d);
    Filmstrip* filmstrip = new Filmstrip(image, d);
    filmstrip->setName(title);
    d->m_films.append(filmstrip);
}

/*!
  Inserts filmstrip at index position i.
  If i is 0, the filmstrip is prepended to the film list.
  If i is size(), the value is appended to the film list.
*/
void FilmstripFlow::insert(int i, const QImage& image, const QString& title)
{
    Q_ASSERT(d);
    Q_ASSERT(i >= 0 && i <= d->m_films.size());

    Filmstrip* filmstrip = new Filmstrip(image, d);
    filmstrip->setName(title);
    d->m_films.insert(i, filmstrip);

    if (isVisible()) {
        Q_ASSERT(d->m_movieTimer.state() != QTimeLine::Running);
        if (i == d->m_centerIndex + 1)  // insert on right
            showInsertOnRight();
        else if (i == d->m_centerIndex) // insert on left
            showInsertOnLeft();
    }
}

/*!
  Removes filmstrip at index position i.
*/
void FilmstripFlow::removeAt (int i)
{
    Q_ASSERT(d);
    Q_ASSERT(i >= 0 && i < d->m_films.size());


    if (isVisible()) {
        // Not use Q_ASSERT here because the deletion-op is UI buildin -- tap the close button
        if (d->m_movieTimer.state() == QTimeLine::Running)
            return;

        d->m_films[d->m_centerIndex]->updateMovie(d->m_movieFactory.createMovie(FadeOut)); // move center slide to left
        CALL_ON_PREV_FILM_STRIP(freeze()); // no movement for left slide
        CALL_ON_NEXT_FILM_STRIP(freeze()); // no movement for right slide

        QObject::disconnect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));
        QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(closeAnimation()));
        d->m_movieTimer.start();
    }
    else {
        Filmstrip* f = d->m_films.at(i);
        d->m_films.removeAt(i);
        SAFE_DELETE(f);
        d->m_centerIndex = d->m_films.size() - 1;
    }
}

/*!
  Return the index of the current center slide
*/
int FilmstripFlow::centerIndex() const
{
    Q_ASSERT(d);
    return d->m_centerIndex;
}

/*! Clear all slides
*/
void FilmstripFlow::clear()
{
    Q_ASSERT(d);
    d->clear();
}

void FilmstripFlow::backgroundColor(const QRgb& c)
{
    Q_ASSERT(d);
    d->m_bgColor = c;
}

int FilmstripFlow::slideCount() const
{
    return d ? d->m_films.size() : 0;
}

//! Returns QImage of specified slide.
QImage FilmstripFlow::slide(int i) const
{
    Q_ASSERT(d);
    Q_ASSERT( i >=0 && i < d->m_films.size());
    return d->m_films.at(i)->image();
}

//! return true if slide animation is ongoing
bool FilmstripFlow::slideAnimationOngoing() const
{
    Q_ASSERT(d);
    return d->m_movieTimer.state() == QTimeLine::Running;
}

//! return center slide's rect
QRect FilmstripFlow::centralRect() const
{
    Q_ASSERT(d);
    int cw = d->m_centerWindowSize.width();
    int ch = d->m_centerWindowSize.height();
    int w = size().width();
    int h = size().height();

    qreal cx = (w - cw) / 2.0;
    qreal cy = d->m_centerTopSpace;
    return QRect(cx, cy, cw, ch);
}

//! prepare start-animation
void FilmstripFlow::prepareStartAnimation()
{
    Q_ASSERT(d);
    CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(ZoomOut)));
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinLeft)));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(NULL)); // hide left left
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinRight)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(NULL)); // hide right right
}

//! run start-animation
void FilmstripFlow::runStartAnimation()
{
    Q_ASSERT(d);
    if (d->m_movieTimer.state() == QTimeLine::Running)
        return;

    d->m_movieTimer.start();
}

//! run start-animation
void FilmstripFlow::runEndAnimation()
{
    Q_ASSERT(d);
    if (d->m_movieTimer.state() == QTimeLine::Running)
        return;

    CALL_ON_PREV_PREV_FILM_STRIP(freeze());
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeft)));
    CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(ZoomIn)));
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRight)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(freeze());

    QObject::disconnect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));
    QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SIGNAL(endAnimationCplt()));
    d->m_movieTimer.start();
}

//! handle the display mode change
void FilmstripFlow::displayModeChanged(QString& newMode)
{
    Q_UNUSED(newMode);
    Q_ASSERT(d);
    QSize s = this->size();
    //qDebug() << "FilmstripFlow::displayModeChanged: "  << s;
    adjustFilmstripSize(s);
    repaint();
}

// -------------------------------------------------------
// FilmstripFlow: private functions

void FilmstripFlow::adjustFilmstripSize(QSize& s)
{
    Q_ASSERT(d);
    SAFE_DELETE(d->m_buffer);
    int w = s.width();
    int h = s.height();

    qreal ix;
    qreal ratio = ((qreal) w) / h;
    d->m_buffer = new QImage (w, h, QImage::Format_RGB32);

    if (w >= h) { // landscape
        d->m_sideWindowSize = QSize(w * L_SIDE_WIDTH_P_C, w * L_SIDE_WIDTH_P_C / ratio);
        d->m_centerWindowSize = QSize(w * L_CENTER_WIDTH_P_C, w * L_CENTER_WIDTH_P_C / ratio);
        d->m_centerTopSpace = h * L_CENTER_TOP_SPACE_P_C;
        d->m_sideTopSpace = h * L_SIDE_TOP_SPACE_P_C;
        d->m_space = w * L_SPACE_P_C;
        ix = (w * (1 + L_CENTER_WIDTH_P_C) - d->m_closeIcon->size().width() + FRAME_WIDTH)/2.0;
    } else { // portrait
        d->m_sideWindowSize = QSize(w * P_SIDE_WIDTH_P_C, w * P_SIDE_WIDTH_P_C / ratio);
        d->m_centerWindowSize = QSize(w * P_CENTER_WIDTH_P_C, w * P_CENTER_WIDTH_P_C / ratio);
        d->m_centerTopSpace = h * P_CENTER_TOP_SPACE_P_C;
        d->m_sideTopSpace = h * P_SIDE_TOP_SPACE_P_C;
        d->m_space = w * P_SPACE_P_C;
        ix = (w * (1 + P_CENTER_WIDTH_P_C) - d->m_closeIcon->size().width() + FRAME_WIDTH)/2.0;
    }

    if (d->m_closeIcon && !d->m_closeIcon->isNull()) {
        qreal iy = d->m_centerTopSpace - d->m_closeIcon->size().height()/2.0;
        d->m_closeIconPos = QPoint(ix, iy);
    }
    d->m_movieFactory.updateAllMovie();
}

//! insert a new filmstrip on current's right
void FilmstripFlow::showInsertOnRight()
{
    Q_ASSERT(d);
    d->m_incIndex = 1;
    d->m_films[d->m_centerIndex]->updateMovie(d->m_movieFactory.createMovie(CenterToLeft)); // move center slide to left

    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeft))); // move left slide out
    CALL_ON_NEXT_NEXT_FILM_STRIP(freeze());// no movement for right slide
    d->m_movieTimer.start();
}

//! insert a new filmstrip on current's right
void FilmstripFlow::showInsertOnLeft()
{
    //FIXME
    Q_ASSERT(d);
    qDebug() << "showInsertOnLeft is not implemented.";
}

//! Show the previous item
void FilmstripFlow::showPrevious()
{
    Q_ASSERT(d);
    if (d->m_centerIndex >= 1) {
        d->m_incIndex = -1;
        CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(CenterToRight)));
        CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));
        CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinLeft)));
        CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRight)));
        d->m_movieTimer.start();
    }
}

//! Show the next item
void FilmstripFlow::showNext()
{
    Q_ASSERT(d);
    if (d->m_centerIndex < d->m_films.size() - 1) {
        d->m_incIndex = 1;
        CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(CenterToLeft)));
        CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeft)));
        CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightToCenter)));
        CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinRight)));
        d->m_movieTimer.start();
    }
}

void FilmstripFlow::scroll()
{
    if(d->m_movieTimer.state() == QTimeLine::Running)
        return;

    if(d->m_lastMoveEventPos.x() < (size().width() - d->m_centerWindowSize.width())/ 2) {
        showPrevious();
    }
    else if (d->m_lastMoveEventPos.x() > (size().width() + d->m_centerWindowSize.width())/ 2) {
        showNext();
    }
}

void FilmstripFlow::playMovie(int frame)
{
    Q_ASSERT(d);

    CALL_ON_CENTER_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_PREV_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_NEXT_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovieFrame(frame));

    repaint();
}

void FilmstripFlow::stopMovie()
{
    Q_ASSERT(d);
    int newIndex = d->m_incIndex + d->m_centerIndex;
    if (newIndex >= 0 && newIndex < d->m_films.size())
        setCenterIndex(newIndex);

    repaint();
}

void FilmstripFlow::closeAnimation()
{
    //qDebug() << "close animation!!!!";
    Filmstrip* f = d->m_films.at(d->m_centerIndex);

    if (d->m_centerIndex == 0) { // the first film closed
        d->m_incIndex = 0;
        CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightToCenter)));
        CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinRight)));
        d->m_films.removeAt(d->m_centerIndex);
    } else {
        d->m_incIndex = -1;
        CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));
        CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinLeft)));
        CALL_ON_NEXT_FILM_STRIP(freeze()); // no movement for right slide
        d->m_films.removeAt(d->m_centerIndex);
    }

    // clear the closed film now
    SAFE_DELETE(f);
    // send the signal
    emit removed(d->m_centerIndex);

    QObject::disconnect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(closeAnimation()));
    QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));
    d->m_movieTimer.start();
}

bool FilmstripFlow::hitCloseIcon()
{
    if (!d->m_closeIcon)
        return false;

    int iw = d->m_closeIcon->size().width() / 2;
    QPoint p = d->m_lastMoveEventPos - (d->m_closeIconPos + QPoint(iw, iw));
    return (p.manhattanLength() < iw + CLOSE_ICON_ADJUST_SIZE) ? true : false;
}

// -------------------------------------------------------
// FilmstripFlow: override event handler

void FilmstripFlow::resizeEvent(QResizeEvent* event)
{
    Q_ASSERT(d);
    QWidget::resizeEvent(event);
    d->m_widgetSize = event->size();
    // not adjust filmstrip size here because the event->size() is whole application's size
    //adjustFilmstripSize(s);
}

void FilmstripFlow::moveEvent(QMoveEvent * event)
{
    //qDebug() << "!!!!!!!!!!!!!!!!move event" << this->size();
}

void FilmstripFlow::mouseMoveEvent(QMouseEvent* event)
{
    d->m_lastMoveEventPos = event->pos();
}

void FilmstripFlow::mousePressEvent(QMouseEvent* event)
{
    //qDebug() <<  event->pos();
    d->m_lastMoveEventPos = event->pos();
    if (d->m_films.size() > 1 && d->m_closeIcon && hitCloseIcon())
        removeAt(d->m_centerIndex);
    else
        scroll();
}

void FilmstripFlow::mouseReleaseEvent(QMouseEvent* event)
{
    if (slideAnimationOngoing())
        return;

    if (d->m_closeIcon && hitCloseIcon())
        return;

    if(event->x() > ( (size().width() - d->m_centerWindowSize.width())/ 2) &&
       event->x() < ( (size().width() + d->m_centerWindowSize.width())/ 2) )
    {
        emit ok(d->m_centerIndex);
    }
}

void FilmstripFlow::paintEvent(QPaintEvent *event)
{
    Q_ASSERT(d);
    Q_ASSERT(d->m_buffer);

    QPainter painter(this);
    d->m_buffer->fill(d->m_bgColor);

    if (d->m_films.size() > 0 && d->m_centerIndex != INVALID_INDEX) {

        QPainter bufPaint(d->m_buffer);
        // draw center film strip
        CALL_ON_CENTER_FILM_STRIP(paint(&bufPaint));

        // draw left film strip
        CALL_ON_PREV_FILM_STRIP(paint(&bufPaint));

        // draw left left film strip
        CALL_ON_PREV_PREV_FILM_STRIP(paint(&bufPaint));

        // draw right film strip
        CALL_ON_NEXT_FILM_STRIP(paint(&bufPaint));

        // draw right right film strip
        CALL_ON_NEXT_NEXT_FILM_STRIP(paint(&bufPaint));

        // 1. draw image from the buffer
        painter.drawImage(QPoint(0,0), *(d->m_buffer));
        if (d->m_movieTimer.state() != QTimeLine::Running) {
            // 2. draw close icon
            if (d->m_films.size() > 1 && d->m_closeIcon)
                painter.drawImage(d->m_closeIconPos, *(d->m_closeIcon));
            // 3. draw page title
            CALL_ON_CENTER_FILM_STRIP(paintName(&painter));
        }
    }
    else
        painter.drawImage(QPoint(0,0), *(d->m_buffer));
}

// -------------------------------------------------------
// GraphicsFilmstripFlow.
/*!
  Creates a new GraphicsFilmstripFlow.
*/
GraphicsFilmstripFlow::GraphicsFilmstripFlow(QObject* parent): GraphicsFlowInterface(NULL), d(new FilmstripFlowPrivate())
{
    setParent(parent);
}

/*!
  Destroys the widget.
*/
GraphicsFilmstripFlow::~GraphicsFilmstripFlow()
{
    delete d;
}

/*!
  Init the FilmstripFlow
*/
void GraphicsFilmstripFlow::init()
{
    if (!d->m_closeIcon) {
        d->m_closeIcon = new QImage(":/resources/close.png");
    }
    d->m_movieFactory.setFilmstripFlowData(d);

    d->m_movieTimer.setDuration(ANIMATION_DURATION);
    d->m_movieTimer.setCurveShape(QTimeLine::EaseOutCurve);
    d->m_movieTimer.setFrameRange(0, ANIMATION_MAX_FRAME);

    QObject::connect(&d->m_movieTimer, SIGNAL(frameChanged(int)), this, SLOT(playMovie(int)));
    QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));

    QSize s = size().toSize();
    d->m_widgetSize = s;
    adjustFilmstripSize(s);
    // ensure that system cursor is an arrow, not a random icon
    // This is not an issue if the platform does not have a system cursor
#ifndef __SYMBIAN32__
    setCursor(Qt::ArrowCursor);
#endif
    setFocusPolicy(Qt::WheelFocus);
    setFocus(Qt::OtherFocusReason);
}

/*!
  Set center index
*/
void GraphicsFilmstripFlow::setCenterIndex(int i)
{
    //qDebug() << "!!!Set Center Index:" << i;
    Q_ASSERT(d);
    if (!d->m_films.size())
        return;
    Q_ASSERT(i >= 0 && i < d->m_films.size());
    d->m_centerIndex = i;
    d->m_films[d->m_centerIndex]->updateMovie(d->m_movieFactory.createMovie(CenterToRight));

    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(NULL)); // hide left left
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRight)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(NULL)); // hide right right

    emit centerIndexChanged(i);
}

/*!
  add slide
*/
void GraphicsFilmstripFlow::addSlide(const QImage& image)
{
    Q_ASSERT(d);
    Filmstrip* filmstrip = new Filmstrip(image, d);
    d->m_films.append(filmstrip);
}

/*!
  add slide
*/
void GraphicsFilmstripFlow::addSlide(const QImage& image, const QString& title)
{
    Q_ASSERT(d);
    Filmstrip* filmstrip = new Filmstrip(image, d);
    filmstrip->setName(title);
    d->m_films.append(filmstrip);
}

/*!
  Inserts filmstrip at index position i.
  If i is 0, the filmstrip is prepended to the film list.
  If i is size(), the value is appended to the film list.
*/
void GraphicsFilmstripFlow::insert(int i, const QImage& image, const QString& title)
{
    Q_ASSERT(d);
    Q_ASSERT(i >= 0 && i <= d->m_films.size());

    Filmstrip* filmstrip = new Filmstrip(image, d);
    filmstrip->setName(title);
    d->m_films.insert(i, filmstrip);

    if (isVisible()) {
        Q_ASSERT(d->m_movieTimer.state() != QTimeLine::Running);
        if (i == d->m_centerIndex + 1)  // insert on right
            showInsertOnRight();
        else if (i == d->m_centerIndex) // insert on left
            showInsertOnLeft();
    }
}

/*!
  Removes filmstrip at index position i.
*/
void GraphicsFilmstripFlow::removeAt (int i)
{
    Q_ASSERT(d);
    Q_ASSERT(i >= 0 && i < d->m_films.size());


    if (isVisible()) {
        // Not use Q_ASSERT here because the deletion-op is UI buildin -- tap the close button
        if (d->m_movieTimer.state() == QTimeLine::Running)
            return;

        d->m_films[d->m_centerIndex]->updateMovie(d->m_movieFactory.createMovie(FadeOut)); // move center slide to left
        CALL_ON_PREV_FILM_STRIP(freeze()); // no movement for left slide
        CALL_ON_NEXT_FILM_STRIP(freeze()); // no movement for right slide

        QObject::disconnect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));
        QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(closeAnimation()));
        d->m_movieTimer.start();
    }
    else {
        Filmstrip* f = d->m_films.at(i);
        d->m_films.removeAt(i);
        SAFE_DELETE(f);
        d->m_centerIndex = d->m_films.size() - 1;
    }
}

/*!
  Return the index of the current center slide
*/
int GraphicsFilmstripFlow::centerIndex() const
{
    Q_ASSERT(d);
    return d->m_centerIndex;
}

/*! Clear all slides
*/
void GraphicsFilmstripFlow::clear()
{
    Q_ASSERT(d);
    d->clear();
}

void GraphicsFilmstripFlow::backgroundColor(const QRgb& c)
{
    Q_ASSERT(d);
    d->m_bgColor = c;
}

int GraphicsFilmstripFlow::slideCount() const
{
    return d ? d->m_films.size() : 0;
}

//! Returns QImage of specified slide.
QImage GraphicsFilmstripFlow::slide(int i) const
{
    Q_ASSERT(d);
    Q_ASSERT( i >=0 && i < d->m_films.size());
    return d->m_films.at(i)->image();
}

//! return true if slide animation is ongoing
bool GraphicsFilmstripFlow::slideAnimationOngoing() const
{
    Q_ASSERT(d);
    return d->m_movieTimer.state() == QTimeLine::Running;
}

//! return center slide's rect
QRect GraphicsFilmstripFlow::centralRect() const
{
    Q_ASSERT(d);
    int cw = d->m_centerWindowSize.width();
    int ch = d->m_centerWindowSize.height();
    int w = size().width();
    int h = size().height();

    qreal cx = (w - cw) / 2.0;
    qreal cy = d->m_centerTopSpace;
    return QRect(cx, cy, cw, ch);
}

//! prepare start-animation
void GraphicsFilmstripFlow::prepareStartAnimation()
{
    Q_ASSERT(d);
    CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(ZoomOut)));
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinLeft)));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(NULL)); // hide left left
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinRight)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(NULL)); // hide right right
}

//! run start-animation
void GraphicsFilmstripFlow::runStartAnimation()
{
    Q_ASSERT(d);
    if (d->m_movieTimer.state() == QTimeLine::Running)
        return;

    d->m_movieTimer.start();
}

//! run start-animation
void GraphicsFilmstripFlow::runEndAnimation()
{
    Q_ASSERT(d);
    if (d->m_movieTimer.state() == QTimeLine::Running)
        return;

    CALL_ON_PREV_PREV_FILM_STRIP(freeze());
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeft)));
    CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(ZoomIn)));
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRight)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(freeze());

    QObject::disconnect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));
    QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SIGNAL(endAnimationCplt()));
    d->m_movieTimer.start();
}

//! handle the display mode change
void GraphicsFilmstripFlow::displayModeChanged(QString& newMode)
{
    Q_UNUSED(newMode);
    Q_ASSERT(d);
    QSize s = this->size().toSize();
    //qDebug() << "FilmstripFlow::displayModeChanged: "  << s;
    adjustFilmstripSize(s);
    update();
}

// -------------------------------------------------------
// FilmstripFlow: private functions

void GraphicsFilmstripFlow::adjustFilmstripSize(QSize& s)
{
    Q_ASSERT(d);
    SAFE_DELETE(d->m_buffer);
    int w = s.width();
    int h = s.height();

    qreal ix;
    qreal ratio = ((qreal) w) / h;
    d->m_buffer = new QImage (w, h, QImage::Format_RGB32);

    if (w >= h) { // landscape
        d->m_sideWindowSize = QSize(w * L_SIDE_WIDTH_P_C, w * L_SIDE_WIDTH_P_C / ratio);
        d->m_centerWindowSize = QSize(w * L_CENTER_WIDTH_P_C, w * L_CENTER_WIDTH_P_C / ratio);
        d->m_centerTopSpace = h * L_CENTER_TOP_SPACE_P_C;
        d->m_sideTopSpace = h * L_SIDE_TOP_SPACE_P_C;
        d->m_space = w * L_SPACE_P_C;
        ix = (w * (1 + L_CENTER_WIDTH_P_C) - d->m_closeIcon->size().width() + FRAME_WIDTH)/2.0;
    } else { // portrait
        d->m_sideWindowSize = QSize(w * P_SIDE_WIDTH_P_C, w * P_SIDE_WIDTH_P_C / ratio);
        d->m_centerWindowSize = QSize(w * P_CENTER_WIDTH_P_C, w * P_CENTER_WIDTH_P_C / ratio);
        d->m_centerTopSpace = h * P_CENTER_TOP_SPACE_P_C;
        d->m_sideTopSpace = h * P_SIDE_TOP_SPACE_P_C;
        d->m_space = w * P_SPACE_P_C;
        ix = (w * (1 + P_CENTER_WIDTH_P_C) - d->m_closeIcon->size().width() + FRAME_WIDTH)/2.0;
    }

    if (d->m_closeIcon && !d->m_closeIcon->isNull()) {
        qreal iy = d->m_centerTopSpace - d->m_closeIcon->size().height()/2.0;
        d->m_closeIconPos = QPoint(ix, iy);
    }
    d->m_movieFactory.updateAllMovie();
}

//! insert a new filmstrip on current's right
void GraphicsFilmstripFlow::showInsertOnRight()
{
    Q_ASSERT(d);
    d->m_incIndex = 1;
    d->m_films[d->m_centerIndex]->updateMovie(d->m_movieFactory.createMovie(CenterToLeft)); // move center slide to left

    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeft))); // move left slide out
    CALL_ON_NEXT_NEXT_FILM_STRIP(freeze());// no movement for right slide
    d->m_movieTimer.start();
}

//! insert a new filmstrip on current's right
void GraphicsFilmstripFlow::showInsertOnLeft()
{
    //FIXME
    Q_ASSERT(d);
    qDebug() << "showInsertOnLeft is not implemented.";
}

//! Show the previous item
void GraphicsFilmstripFlow::showPrevious()
{
    Q_ASSERT(d);
    if (d->m_centerIndex >= 1) {
        d->m_incIndex = -1;
        CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(CenterToRight)));
        CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));
        CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinLeft)));
        CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRight)));
        d->m_movieTimer.start();
    }
}

//! Show the next item
void GraphicsFilmstripFlow::showNext()
{
    Q_ASSERT(d);
    if (d->m_centerIndex < d->m_films.size() - 1) {
        d->m_incIndex = 1;
        CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(CenterToLeft)));
        CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeft)));
        CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightToCenter)));
        CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinRight)));
        d->m_movieTimer.start();
    }
}

void GraphicsFilmstripFlow::scroll()
{
    if(d->m_movieTimer.state() == QTimeLine::Running)
        return;

    if(d->m_lastMoveEventPos.x() < (size().width() - d->m_centerWindowSize.width())/ 2) {
        showPrevious();
    }
    else if (d->m_lastMoveEventPos.x() > (size().width() + d->m_centerWindowSize.width())/ 2) {
        showNext();
    }
}

void GraphicsFilmstripFlow::playMovie(int frame)
{
    Q_ASSERT(d);

    CALL_ON_CENTER_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_PREV_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_NEXT_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovieFrame(frame));

    update();
}

void GraphicsFilmstripFlow::stopMovie()
{
    Q_ASSERT(d);
    int newIndex = d->m_incIndex + d->m_centerIndex;
    if (newIndex >= 0 && newIndex < d->m_films.size())
        setCenterIndex(newIndex);

    update();
}

void GraphicsFilmstripFlow::closeAnimation()
{
    //qDebug() << "close animation!!!!";
    Filmstrip* f = d->m_films.at(d->m_centerIndex);

    if (d->m_centerIndex == 0) { // the first film closed
        d->m_incIndex = 0;
        CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightToCenter)));
        CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinRight)));
        d->m_films.removeAt(d->m_centerIndex);
    } else {
        d->m_incIndex = -1;
        CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));
        CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinLeft)));
        CALL_ON_NEXT_FILM_STRIP(freeze()); // no movement for right slide
        d->m_films.removeAt(d->m_centerIndex);
    }

    // clear the closed film now
    SAFE_DELETE(f);
    // send the signal
    emit removed(d->m_centerIndex);

    QObject::disconnect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(closeAnimation()));
    QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));
    d->m_movieTimer.start();
}

bool GraphicsFilmstripFlow::hitCloseIcon()
{
    if (!d->m_closeIcon)
        return false;

    int iw = d->m_closeIcon->size().width() / 2;
    QPoint p = d->m_lastMoveEventPos - (d->m_closeIconPos + QPoint(iw, iw));
    return (p.manhattanLength() < iw + CLOSE_ICON_ADJUST_SIZE) ? true : false;
}

// -------------------------------------------------------
// FilmstripFlow: override event handler

void GraphicsFilmstripFlow::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    Q_ASSERT(d);
    QGraphicsWidget::resizeEvent(event);
    d->m_widgetSize = event->newSize().toSize();
    adjustFilmstripSize(d->m_widgetSize);
}

void GraphicsFilmstripFlow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //qDebug() << "!!!!!!!!!!!!!!!!move event" << this->size();
}

void GraphicsFilmstripFlow::moveEvent(QGraphicsSceneMoveEvent* event)
{
    d->m_lastMoveEventPos = event->newPos().toPoint();
}

void GraphicsFilmstripFlow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    //qDebug() <<  event->pos();
    d->m_lastMoveEventPos = event->pos().toPoint();
    if (d->m_films.size() > 1 && d->m_closeIcon && hitCloseIcon())
        removeAt(d->m_centerIndex);
    else
        scroll();
}

void GraphicsFilmstripFlow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (slideAnimationOngoing())
        return;

    if (d->m_closeIcon && hitCloseIcon())
        return;

    if(event->pos().x() > ( (size().width() - d->m_centerWindowSize.width())/ 2) &&
       event->pos().x() < ( (size().width() + d->m_centerWindowSize.width())/ 2) &&
       event->pos().y() > d->m_centerTopSpace &&
       event->pos().y() < d->m_centerTopSpace + d->m_centerWindowSize.height())
    {
        emit ok(d->m_centerIndex);
    }
}

void GraphicsFilmstripFlow::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_ASSERT(d);
    Q_ASSERT(d->m_buffer);

    d->m_buffer->fill(d->m_bgColor);

    if (d->m_films.size() > 0 && d->m_centerIndex != INVALID_INDEX) {

        QPainter bufPaint(d->m_buffer);
        // draw center film strip
        CALL_ON_CENTER_FILM_STRIP(paint(&bufPaint));

        // draw left film strip
        CALL_ON_PREV_FILM_STRIP(paint(&bufPaint));

        // draw left left film strip
        CALL_ON_PREV_PREV_FILM_STRIP(paint(&bufPaint));

        // draw right film strip
        CALL_ON_NEXT_FILM_STRIP(paint(&bufPaint));

        // draw right right film strip
        CALL_ON_NEXT_NEXT_FILM_STRIP(paint(&bufPaint));

        // 1. draw image from the buffer
        painter->drawImage(QPoint(0,0), *(d->m_buffer));
        if (d->m_movieTimer.state() != QTimeLine::Running) {
            // 2. draw close icon
            if (d->m_films.size() > 1 && d->m_closeIcon)
                painter->drawImage(d->m_closeIconPos, *(d->m_closeIcon));
            // 3. draw page title
            CALL_ON_CENTER_FILM_STRIP(paintName(painter));
        }
    }
    else
        painter->drawImage(QPoint(0,0), *(d->m_buffer));
}

} // end of namespace
