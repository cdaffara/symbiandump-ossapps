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
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QResizeEvent>
#include <QTimeLine>
#include <QGraphicsSceneResizeEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMoveEvent>
#include <QGraphicsDropShadowEffect>
#include "linearflowsnippet.h"

#define SAFE_DELETE(p) if (p) delete p;

#define SNIPPET_AUTOHIDE_TIMEOUT 5000 //5secs

#define CALL_ON_PREV_PREV_FILM_STRIP(func) if (d->m_centerIndex - 2 >= 0) d->m_films[d->m_centerIndex - 2]->func; else if (d->m_centerIndex == 1) d->m_films[d->m_films.size() - 1]->func; else d->m_films[d->m_films.size() - 2]->func;
#define CALL_ON_CENTER_FILM_STRIP(func) if (d->m_centerIndex >= 0 && d->m_centerIndex < d->m_films.size()) d->m_films[d->m_centerIndex]->func; else if (d->m_centerIndex < 0) d->m_films[d->m_films.size() + d->m_centerIndex]->func; else d->m_films[(d->m_centerIndex)%d->m_films.size()]->func;
#define CALL_ON_PREV_PREV_PREV_FILM_STRIP(func) if (d->m_centerIndex - 3 >= 0) d->m_films[d->m_centerIndex - 3]->func; else if (d->m_centerIndex == 1) d->m_films[d->m_films.size() - 2]->func; else if (d->m_centerIndex == 2) d->m_films[d->m_films.size() - 1]->func; else d->m_films[d->m_films.size() - 3]->func;
#define CALL_ON_NEXT_NEXT_NEXT_FILM_STRIP(func) if (d->m_centerIndex + 3 < d->m_films.size()) d->m_films[d->m_centerIndex + 3]->func; else d->m_films[(d->m_centerIndex + 3)%d->m_films.size()]->func;
#define CALL_ON_NEXT_NEXT_FILM_STRIP(func) if (d->m_centerIndex + 2 < d->m_films.size()) d->m_films[d->m_centerIndex + 2]->func; else d->m_films[(d->m_centerIndex + 2)%d->m_films.size()]->func;
#define CALL_ON_PREV_FILM_STRIP(func) if (d->m_centerIndex - 1 >= 0) d->m_films[d->m_centerIndex - 1]->func; else d->m_films[d->m_films.size() - 1]->func;
#define CALL_ON_NEXT_FILM_STRIP(func) if (d->m_centerIndex + 1 < d->m_films.size()) d->m_films[d->m_centerIndex + 1]->func; else d->m_films[(d->m_centerIndex + 1)%d->m_films.size()]->func;


#define INVALID_INDEX -1

#define L_CENTER_WIDTH_P_C 0.27
#define L_CENTER_HEIGHT_P_C 0.70
#define L_SIDE_WIDTH_P_C 0.27
#define L_SIDE_HEIGHT_P_C 0.70

#define L_CENTER_TOP_SPACE_P_C 0.15
#define L_SIDE_TOP_SPACE_P_C 0.15

#define L_SPACE_P_C 0.05

#define SPACE_P_C 0.11
#define CLOSE_ICON_ADJUST_SIZE 5
#define TITLE_SPACE 10
#define FRAME_WIDTH 4

#define ANIMATION_DURATION 300
#define ANIMATION_MAX_FRAME 20.0
#define NEW_FILM_TITLE ""

#define WIDTH 360
#define TITLE_HEIGHT 0.15
#define FILM_HEIGHT 0.85
#define P_MAX_SLIDE 3
#define L_MAX_SLIDE 5
#define MAX_FLICK_SPEED 500
#define THRESHOLD_FLICK_SPEED 400
#define THRESHOLD_FLICK_TIME 75

namespace GVA {

// movies (animation) name
const QString BreakinLeft   = "bil";//"breakin - left";
const QString BreakinRight  = "bir";//"breakin - right";
const QString BreakoutLeft  = "bol";//"breakout - left";
const QString BreakoutRight = "bor";//"breakout - right";

const QString BreakinLeftLeft = "bill";//"breakin - leftleft"
const QString BreakinRightRight = "birr";//"breakin - rightright"
const QString BreakoutLeftLeft = "boll";//"breakout - leftleft"
const QString BreakoutRightRight = "borr";//"breakout rightright"

const QString RightToCenter = "rc";//"right - center";
const QString LeftToCenter  = "lc";//"left - center";
const QString CenterToRight = "cr";//"center - right";
const QString CenterToLeft  = "cl";//"center - left";

const QString LeftLeftToLeft = "lltl"; //"leftLft - left"
const QString LeftToLeftLeft = "ltll";//left - leftleft"
const QString RightToRightRight = "rtrr";//"right - rightright"
const QString RightRightToRight = "rrtr";//"rightright - right"

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
    void createLeftLeftInMovie(FilmstripMovie* movie);
    void createRightRightInMovie(FilmstripMovie* movie);
    void createLeftLeftOutMovie(FilmstripMovie* movie);
    void createRightRightOutMovie(FilmstripMovie* movie);
    void createLeftLeftToLeftMovie(FilmstripMovie* movie);
    void createLeftToLeftLeftMovie(FilmstripMovie* movie);
    void createRightToRightRightMovie(FilmstripMovie* movie);
    void createRightRightToRightMovie(FilmstripMovie* movie);

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
    void updateMovieFrame(int frame) { if (!m_frozen) m_movieFrame = frame;}
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
    FilmstripFlowPrivate(): m_bgColor(QColor(68,88,125).rgb()), m_buffer(NULL), m_titleBuffer(NULL),m_closeIcon(NULL), m_centerTopSpace(0), m_sideTopSpace(0), m_space(0), m_incIndex(0), m_centerIndex(INVALID_INDEX) {}

    ~FilmstripFlowPrivate() {
        for (int i = 0; i < m_films.size(); i++)
            SAFE_DELETE(m_films[i]);
        m_films.clear();
        SAFE_DELETE(m_buffer);
        SAFE_DELETE(m_titleBuffer);
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
    QImage *m_titleBuffer;
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
    m_name = NEW_FILM_TITLE;
    m_img = QImage(w, h, QImage::Format_RGB32);

    QPainter painter(&m_img);
    painter.fillRect(0, 0, w, h, Qt::white);
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

    if (target.right() > 0 || target.left() < m_filmstripFlowData->m_widgetSize.width()) {
        if (needFade)
            painter->setOpacity((ANIMATION_MAX_FRAME - m_movieFrame) / ANIMATION_MAX_FRAME);

        painter->fillRect(target.adjusted(-FRAME_WIDTH,-FRAME_WIDTH,FRAME_WIDTH,FRAME_WIDTH), QColor(Qt::gray));

        if (!m_img.isNull())
            painter->drawImage(target, m_img);

        else {
            painter->save();
            painter->setPen(QColor(Qt::black));
            painter->setFont(QFont("Arial", 4));
            painter->drawText(target, Qt::AlignVCenter,m_name);
            painter->restore();
        }

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
        else if (name == BreakinLeftLeft)
            createLeftLeftInMovie(movie);
        else if (name == BreakinRightRight)
            createRightRightInMovie(movie);
        else if (name == BreakoutLeftLeft)
            createLeftLeftOutMovie(movie);
        else if (name == BreakoutRightRight)
            createRightRightOutMovie(movie);
        else if (name == LeftLeftToLeft)
            createLeftLeftToLeftMovie(movie);
        else if (name == LeftToLeftLeft)
            createLeftToLeftLeftMovie(movie);
        else if (name == RightToRightRight)
            createRightToRightRightMovie(movie);
        else if (name == RightRightToRight)
            createRightRightToRightMovie(movie);

        m_moviesCache[name] = movie;
    } else {
        movie =  m_moviesCache.value(name);
    }
    return movie;
}

void FilmstripMovieFactory::createLeftLeftInMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal cx = ((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0 - sw + m_filmstripFlowData->m_space;
    qreal sx = ((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0 - 2 * sw;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF( 2 *  sx - cx , sy, sw, sh);
    QRectF endRect = QRectF(sx, sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
}

void FilmstripMovieFactory::createRightRightInMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal cx = (w + 4 * m_filmstripFlowData->m_space + cw) / 2.0 - m_filmstripFlowData->m_space;
    qreal sx = ((w + 4 * m_filmstripFlowData->m_space + cw) / 2.0) + sw ;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF(2 * sx - cx, sy, sw, sh);
    QRectF endRect = QRectF(sx , sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
}

void FilmstripMovieFactory::createLeftLeftOutMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal cx = ((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0 - sw + m_filmstripFlowData->m_space;
    qreal sx = (((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0) - (2 *sw);
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx -cx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(2 * sx - cx , sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
}

void FilmstripMovieFactory::createRightRightOutMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal cx = (w + 4 * m_filmstripFlowData->m_space + cw) / 2.0 - m_filmstripFlowData->m_space;
    qreal sx = ((w + 4 * m_filmstripFlowData->m_space + cw) / 2.0) + sw ;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(2 * sx - cx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
}

void FilmstripMovieFactory::createLeftLeftToLeftMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = ((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0 - sw + m_filmstripFlowData->m_space;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;
    qreal sx = (((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0) - (2 *sw);
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / step;
    qreal stepy = (cy - sy) / step;
    qreal stepx2 = stepx + (cw - sw) / step;
    qreal stepy2 = stepy + (ch - sh) / step;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(cx, cy, cw, ch);

    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
}

void FilmstripMovieFactory::createLeftToLeftLeftMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = ((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0 - sw + m_filmstripFlowData->m_space;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;
    qreal sx = (((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0) - (2 *sw);
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / step;
    qreal stepy = (sy - cy) / step;
    qreal stepx2 = stepx + (sw - cw) / step;
    qreal stepy2 = stepy + (sh - ch) / step;

    QRectF startRect = QRectF(cx, cy, cw, ch);
    QRectF endRect = QRectF(sx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
}

void FilmstripMovieFactory::createRightToRightRightMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = (w + 4 * m_filmstripFlowData->m_space + cw) / 2.0 - m_filmstripFlowData->m_space;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;
    qreal sx = (((w + 4 * m_filmstripFlowData->m_space) + cw) / 2.0) + sw;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / step;
    qreal stepy = (sy - cy) / step;
    qreal stepx2 = stepx + (sw - cw) / step;
    qreal stepy2 = stepy + (sh - ch) / step;

    QRectF startRect = QRectF(cx, cy, cw, ch);
    QRectF endRect = QRectF(sx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);

}

void FilmstripMovieFactory::createRightRightToRightMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    int sw = m_filmstripFlowData->m_sideWindowSize.width();
    int sh = m_filmstripFlowData->m_sideWindowSize.height();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();

    qreal step = ANIMATION_MAX_FRAME;
    qreal cx = (w + 4 * m_filmstripFlowData->m_space + cw) / 2.0 - m_filmstripFlowData->m_space;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;
    qreal sx = ((w + 4 * m_filmstripFlowData->m_space) + cw) / 2.0 + sw;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / step;
    qreal stepy = (cy - sy) / step;
    qreal stepx2 = stepx + (cw - sw) / step;
    qreal stepy2 = stepy + (ch - sh) / step;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(cx, cy, cw, ch);

    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
}


void FilmstripMovieFactory::addRectByFrame(FilmstripMovie* movie, QRectF& startRect, QRectF& endRect, qreal x1, qreal y1, qreal x2, qreal y2, bool debug)
{
    movie->m_movieClips.append(startRect);
    for (int i = 1; i < ANIMATION_MAX_FRAME; i++) {
        movie->m_movieClips.append(movie->m_movieClips[i - 1].adjusted(x1, y1, x2, y2));
    }
    movie->m_movieClips.append(endRect);
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
    qreal sx = (((w  - 4 * m_filmstripFlowData->m_space) - cw) / 2.0) - sw + m_filmstripFlowData->m_space;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / step;
    qreal stepy = (cy - sy) / step;
    qreal stepx2 = stepx + (cw - sw) / step;
    qreal stepy2 = stepy + (ch - sh) / step;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(cx, cy, cw, ch);

    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
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
    qreal sx = ((w + 4 * m_filmstripFlowData->m_space) + cw) / 2.0 - m_filmstripFlowData->m_space;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / step;
    qreal stepy = (cy - sy) / step;
    qreal stepx2 = stepx + (cw - sw) / step;
    qreal stepy2 = stepy + (ch - sh) / step;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(cx, cy, cw, ch);

    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
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
    qreal sx = (((w - 4 * m_filmstripFlowData->m_space) - cw) / 2.0) - sw + m_filmstripFlowData->m_space;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / step;
    qreal stepy = (sy - cy) / step;
    qreal stepx2 = stepx + (sw - cw) / step;
    qreal stepy2 = stepy + (sh - ch) / step;

    QRectF startRect = QRectF(cx, cy, cw, ch);
    QRectF endRect = QRectF(sx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
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
    qreal sx = (((w + 4 * m_filmstripFlowData->m_space) + cw) / 2.0) - m_filmstripFlowData->m_space;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / step;
    qreal stepy = (sy - cy) / step;
    qreal stepx2 = stepx + (sw - cw) / step;
    qreal stepy2 = stepy + (sh - ch) / step;

    QRectF startRect = QRectF(cx, cy, cw, ch);
    QRectF endRect = QRectF(sx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
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
    qreal sx = (w * (1 - 2 * m_filmstripFlowData->m_space) - cw) / 2.0 - sw;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx -cx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(2 * sx - cx , sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
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
    qreal sx = (w * (1 - 2 * m_filmstripFlowData->m_space) - cw) / 2.0 - sw;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF(2 * sx - cx , sy, sw, sh);
    QRectF endRect = QRectF(sx, sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
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
    qreal sx = (w * (1 + 2 * m_filmstripFlowData->m_space) + cw) / 2.0;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (sx - cx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF(sx, sy, sw, sh);
    QRectF endRect = QRectF(sx + sx -cx, sy, sw, sh);
    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
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
    qreal sx = (w * (1 + 2 * m_filmstripFlowData->m_space) + cw) / 2.0;
    qreal sy = m_filmstripFlowData->m_sideTopSpace;
    qreal stepx = (cx - sx) / ANIMATION_MAX_FRAME;

    QRectF startRect = QRectF(sx + sx -cx, sy, sw, sh);
    QRectF endRect = QRectF(sx, sy, sw, sh);

    addRectByFrame(movie, startRect, endRect, stepx, 0, stepx, 0);
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

    QRectF startRect = QRectF(cx, cy, cw, ch);
    QRectF endRect = QRectF(0, 0, w, h);

    addRectByFrame(movie, startRect, endRect, stepx, stepy, stepx2, stepy2);
}

void FilmstripMovieFactory::createZoomOutMovie(FilmstripMovie* movie)
{
    movie->m_movieClips.clear();
    int w = m_filmstripFlowData->m_widgetSize.width();
    int h = m_filmstripFlowData->m_widgetSize.height();
    int cw = m_filmstripFlowData->m_centerWindowSize.width();
    int ch = m_filmstripFlowData->m_centerWindowSize.height();
    qreal cx = (w - cw) / 2.0;

    qreal step = ANIMATION_MAX_FRAME;
    qreal cy = m_filmstripFlowData->m_centerTopSpace;

    qreal stepy = cy / step;

    QRectF startRect = QRectF(0, 0, w, h);
    QRectF endRect = QRectF(cx, cy, cw, ch);

    addRectByFrame(movie, startRect, endRect, 0, stepy, 0, stepy);
}

void FilmstripMovieFactory::updateAllMovie()
{
    QHash<QString, FilmstripMovie*>::const_iterator i = m_moviesCache.find(BreakoutRightRight);

    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakoutRightRight);
        createRightRightOutMovie(movie);
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

    i = m_moviesCache.find(BreakinLeftLeft);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakinLeftLeft);
        createLeftLeftInMovie(movie);
    }


    i = m_moviesCache.find(BreakinRightRight);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakinRightRight);
        createRightRightInMovie(movie);
    }

    i = m_moviesCache.find(BreakoutLeftLeft);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakoutLeftLeft);
        createLeftLeftOutMovie(movie);
    }

    i = m_moviesCache.find(BreakoutRightRight);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(BreakoutRightRight);
        createRightRightOutMovie(movie);
    }

    i = m_moviesCache.find(LeftLeftToLeft);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(LeftLeftToLeft);
        createLeftLeftToLeftMovie(movie);
    }

    i = m_moviesCache.find(LeftToLeftLeft);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(LeftToLeftLeft);
        createLeftToLeftLeftMovie(movie);
    }

    i = m_moviesCache.find(RightToRightRight);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(RightToRightRight);
        createRightToRightRightMovie(movie);
    }

    i = m_moviesCache.find(RightRightToRight);
    if (i != m_moviesCache.end()) {
        FilmstripMovie* movie =  m_moviesCache.value(RightRightToRight);
        createRightRightToRightMovie(movie);
    }
}

// -------------------------------------------------------
// LinearFlowSnippet.
/*!
  Creates a new LinearFlowSnippet.
*/
LinearFlowSnippet::LinearFlowSnippet(QGraphicsWidget* parent): ChromeItem(NULL, parent), d(new FilmstripFlowPrivate())
{
    setParent(parent);
    m_scrolled = false;
    m_displayMode = "";
    m_titleName = "";
    m_countFlicks = 0;
}

/*!
  Destroys the widget.
*/
LinearFlowSnippet::~LinearFlowSnippet()
{
    delete d;
}

/*!
  Init the FilmstripFlow
*/
void LinearFlowSnippet::init(QString displayMode, QString titleName)
{
    Q_UNUSED(displayMode);
    Q_UNUSED(titleName);

    m_displayMode = displayMode;
    m_titleName = titleName;

    if (!d->m_closeIcon) {
        d->m_closeIcon = new QImage("./chrome/close.png");
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
void LinearFlowSnippet::setCenterIndex(int i)
{
    Q_ASSERT(d);
    if (!d->m_films.size())
        return;

    if (d->m_centerIndex == i)
        return;

    if (i < 0)
        i = (d->m_films.size() + i);
    else if (d->m_centerIndex >= d->m_films.size() - 1)
        i = i % d->m_films.size();

    d->m_centerIndex = i;

    CALL_ON_PREV_PREV_PREV_FILM_STRIP(updateMovie(NULL));
    CALL_ON_NEXT_NEXT_NEXT_FILM_STRIP(updateMovie(NULL));
    d->m_films[d->m_centerIndex]->updateMovie(d->m_movieFactory.createMovie(CenterToRight));
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftLeftToLeft)));
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightToRightRight)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRightRight)));

    emit centerIndexChanged(i);
}

/*!
  add slide
*/
void LinearFlowSnippet::addSlide(const QImage& image)
{
    Q_ASSERT(d);
    Filmstrip* filmstrip = new Filmstrip(image, d);
    d->m_films.append(filmstrip);
}

/*!
  add slide
*/
void LinearFlowSnippet::addSlide(const QImage& image, const QString& title)
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
void LinearFlowSnippet::insert(int i, const QImage& image, const QString& title)
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
void LinearFlowSnippet::removeAt (int i)
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
int LinearFlowSnippet::centerIndex() const
{
    Q_ASSERT(d);
    return d->m_centerIndex;
}

/*! Clear all slides
*/
void LinearFlowSnippet::clear()
{
    Q_ASSERT(d);
    d->clear();
}

void LinearFlowSnippet::backgroundColor(const QRgb& c)
{
    Q_ASSERT(d);
    d->m_bgColor = c;
}

int LinearFlowSnippet::slideCount() const
{
    return d ? d->m_films.size() : 0;
}

//! Returns QImage of specified slide.
QImage LinearFlowSnippet::slide(int i) const
{
    Q_ASSERT(d);
    Q_ASSERT( i >=0 && i < d->m_films.size());
    return d->m_films.at(i)->image();
}

//! return true if slide animation is ongoing
bool LinearFlowSnippet::slideAnimationOngoing() const
{
    Q_ASSERT(d);
    return d->m_movieTimer.state() == QTimeLine::Running;
}

//! return center slide's rect
QRect LinearFlowSnippet::centralRect() const
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
void LinearFlowSnippet::prepareStartAnimation()
{
    CALL_ON_PREV_PREV_PREV_FILM_STRIP(updateMovie(NULL));
    CALL_ON_NEXT_NEXT_NEXT_FILM_STRIP(updateMovie(NULL));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinLeftLeft)));
    CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinRightRight)));
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftLeftToLeft)));
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightRightToRight)));
}

//! run start-animation
void LinearFlowSnippet::runStartAnimation()
{
    Q_ASSERT(d);
    if (d->m_movieTimer.state() == QTimeLine::Running)
        return;
    d->m_movieTimer.start();
}

//! run start-animation
void LinearFlowSnippet::runEndAnimation()
{
    Q_ASSERT(d);
    if (d->m_movieTimer.state() == QTimeLine::Running)
        return;

    CALL_ON_PREV_PREV_PREV_FILM_STRIP(freeze());
    CALL_ON_NEXT_NEXT_NEXT_FILM_STRIP(freeze());
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeftLeft)));
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToLeftLeft)));
    CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(CenterToLeft)));
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightToRightRight)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRightRight)));

    QObject::disconnect(&d->m_movieTimer, SIGNAL(finished()), this, SLOT(stopMovie()));
    QObject::connect(&d->m_movieTimer, SIGNAL(finished()), this, SIGNAL(endAnimationCplt()));
    d->m_movieTimer.start();
}

//! handle the display mode change
void LinearFlowSnippet::displayModeChanged(QString& newMode)
{
    Q_UNUSED(newMode);
    m_displayMode = newMode;
    Q_ASSERT(d);
    QSize s = this->size().toSize();
    adjustFilmstripSize(s);
    update();
}

// -------------------------------------------------------
// FilmstripFlow: private functions

void LinearFlowSnippet::adjustFilmstripSize(QSize& s)
{
    Q_ASSERT(d);
    SAFE_DELETE(d->m_buffer);
    SAFE_DELETE(d->m_titleBuffer);
    int w = s.width();
    int h = s.height() * FILM_HEIGHT;

    qreal ix;

    d->m_buffer = new QImage (w, h , QImage::Format_RGB32);
    d->m_titleBuffer = new QImage(w, s.height() * TITLE_HEIGHT , QImage::Format_RGB32);

    QPainter painter(d->m_titleBuffer);
    painter.fillRect(0,0,d->m_titleBuffer->width(),d->m_titleBuffer->height(),d->m_bgColor);

    QFont font;
    font.setBold(true);
    font.setPixelSize(20);
    font.setWeight(QFont::Bold);
    painter.setPen(QColor(255,255,255));
    painter.setFont(font);
    painter.drawText(QPointF(3,s.height() * TITLE_HEIGHT),m_titleName);

    d->m_sideWindowSize = QSize(WIDTH * L_SIDE_WIDTH_P_C, h * L_SIDE_HEIGHT_P_C);
    d->m_centerWindowSize = QSize(WIDTH * L_CENTER_WIDTH_P_C, h * L_CENTER_HEIGHT_P_C);
    d->m_centerTopSpace = h * L_CENTER_TOP_SPACE_P_C;
    d->m_sideTopSpace = h * L_SIDE_TOP_SPACE_P_C ;

    if ( m_displayMode == "Portrait") {
        d->m_space = ((qreal)w - ( P_MAX_SLIDE - 1) * (qreal)d->m_sideWindowSize.width() -  (qreal)d->m_centerWindowSize.width())/(P_MAX_SLIDE + 1);
    }
    else {
        d->m_space = ((qreal)w - ( L_MAX_SLIDE - 1) * (qreal)d->m_sideWindowSize.width() -  (qreal)d->m_centerWindowSize.width())/(L_MAX_SLIDE + 1);
    }

    ix = (WIDTH * (1 + L_CENTER_WIDTH_P_C) - d->m_closeIcon->size().width() + FRAME_WIDTH)/2.0;

    if (d->m_closeIcon && !d->m_closeIcon->isNull()) {
        qreal iy = d->m_centerTopSpace - d->m_closeIcon->size().height()/2.0;
        d->m_closeIconPos = QPoint(ix, iy);
    }
    d->m_movieFactory.updateAllMovie();

    if (d->m_centerIndex != -1)  {
        if (m_displayMode == "Portrait")
            setCenterIndex(d->m_centerIndex - 1);
        else
            setCenterIndex(d->m_centerIndex + 1);
    }
}

//! insert a new filmstrip on current's right
void LinearFlowSnippet::showInsertOnRight()
{
    Q_ASSERT(d);
    d->m_incIndex = 1;
    d->m_films[d->m_centerIndex]->updateMovie(d->m_movieFactory.createMovie(CenterToLeft)); // move center slide to left

    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeft))); // move left slide out
    CALL_ON_NEXT_NEXT_FILM_STRIP(freeze());// no movement for right slide
    d->m_movieTimer.start();
}

//! insert a new filmstrip on current's right
void LinearFlowSnippet::showInsertOnLeft()
{
    //FIXME
    Q_ASSERT(d);
}

//! Show the previous item
void LinearFlowSnippet::showPrevious()
{
    Q_ASSERT(d);

    d->m_incIndex = -1;
    CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(CenterToRight)));
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToCenter)));

    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftLeftToLeft)));
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightToRightRight)));

    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutRightRight)));
    CALL_ON_PREV_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinLeftLeft)));

    d->m_movieTimer.start();
}

//! Show the next item
void LinearFlowSnippet::showNext()
{
    Q_ASSERT(d);

    d->m_incIndex = 1;
    CALL_ON_CENTER_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(CenterToLeft)));
    CALL_ON_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(LeftToLeftLeft)));
    CALL_ON_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightToCenter)));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(RightRightToRight)));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakoutLeftLeft)));
    CALL_ON_NEXT_NEXT_NEXT_FILM_STRIP(updateMovie(d->m_movieFactory.createMovie(BreakinRightRight)));

    d->m_movieTimer.start();
}

void LinearFlowSnippet::scroll()
{
    if (d->m_movieTimer.state() == QTimeLine::Running)
        return;

    if (d->m_lastMoveEventPos.x() < (size().width() - d->m_centerWindowSize.width())/ 2) {
        showPrevious();
    }
    else if (d->m_lastMoveEventPos.x() > (size().width() + d->m_centerWindowSize.width())/ 2) {
        showNext();
    }
}

void LinearFlowSnippet::playMovie(int frame)
{
    Q_ASSERT(d);

    CALL_ON_CENTER_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_PREV_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_PREV_PREV_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_PREV_PREV_PREV_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_NEXT_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_NEXT_NEXT_FILM_STRIP(updateMovieFrame(frame));
    CALL_ON_NEXT_NEXT_NEXT_FILM_STRIP(updateMovieFrame(frame));


    update();
}

void LinearFlowSnippet::stopMovie()
{
    Q_ASSERT(d);
    int newIndex = d->m_incIndex + d->m_centerIndex;

    if (newIndex < 0)
        newIndex = (d->m_films.size() + newIndex);
    else if (newIndex >= d->m_films.size())
        newIndex = newIndex % d->m_films.size();

    if (newIndex >= 0 && newIndex < d->m_films.size())
        setCenterIndex(newIndex);

    update();

    if (m_countFlicks) {
        startFlickScroll();
    }
}

void LinearFlowSnippet::closeAnimation()
{
    Filmstrip* f = d->m_films.at(d->m_centerIndex);

    if (d->m_centerIndex == 0) {
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

bool LinearFlowSnippet::hitCloseIcon()
{
    if (!d->m_closeIcon)
        return false;

    int iw = d->m_closeIcon->size().width() / 2;
    QPoint p = d->m_lastMoveEventPos - (d->m_closeIconPos + QPoint(iw, iw));
    return (p.manhattanLength() < iw + CLOSE_ICON_ADJUST_SIZE) ? true : false;
}

// -------------------------------------------------------
// FilmstripFlow: override event handler

void LinearFlowSnippet::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    Q_ASSERT(d);
    QGraphicsWidget::resizeEvent(event);
    d->m_widgetSize = event->newSize().toSize();
    //qDebug() << "LinearFlowSnippet::resizeEvent" << d->m_widgetSize;
    // not adjust filmstrip size here because the event->size() is whole application's size
}

void LinearFlowSnippet::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //qDebug() << "!!!!!!!!!!!!!!!!move event" << this->size();
    if ( qAbs(qAbs(event->pos().toPoint().x()) - qAbs(d->m_lastMoveEventPos.x())) > 20)
    {
        if ( event->pos().toPoint().x() < event->lastPos().toPoint().x())
            showNext();
        else
            showPrevious();
        d->m_lastMoveEventPos = event->pos().toPoint();

        QTime now(QTime::currentTime());
        m_lastMoveEventTime.setHMS(now.hour(),now.minute(), now.second(), now.msec());

        DragPoint dragPoint;
        dragPoint.iPoint = d->m_lastMoveEventPos;
        dragPoint.iTime = QTime::currentTime();
        m_dragPoints.append(dragPoint);

        while (m_dragPoints.size() > 4)
            m_dragPoints.removeFirst();

        m_scrolled= true;
    }
}

bool LinearFlowSnippet::isFlick()
{
    bool flick = false;
    QPoint moveSpeed = speed();
    int xSpeed = moveSpeed.x();
    int ySpeed = moveSpeed.y();

    flick = (qAbs(xSpeed) > THRESHOLD_FLICK_SPEED ||
             qAbs(ySpeed) > THRESHOLD_FLICK_SPEED);

    return flick;
}

QPoint LinearFlowSnippet::speed()
{
    // Speed is only evaluated at the end of the swipe
    QPoint dragSpeed(0,0);
    qreal time =  dragTime() / 1000;
    if (time > 0) {
        QPoint distance = currentPos() - previousPos();
        dragSpeed.setX((distance.x()) / time);
        dragSpeed.setY((distance.y()) / time);
    }
    return dragSpeed;
}

QPoint LinearFlowSnippet::currentPos()
{
    return m_dragPoints[m_dragPoints.size()-1].iPoint;
}

QPoint LinearFlowSnippet::previousPos()
{
    return m_dragPoints[0].iPoint;
}

qreal LinearFlowSnippet::dragTime() const
{
    if (m_dragPoints.isEmpty())
        return 0.0;
    else
        return  m_dragPoints[0].iTime.msecsTo(m_dragPoints[m_dragPoints.size()-1].iTime);
}


void LinearFlowSnippet::moveEvent(QGraphicsSceneMoveEvent* event)
{
    d->m_lastMoveEventPos = event->newPos().toPoint();
}

void LinearFlowSnippet::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsSceneMouseEvent iev(event->type());
    iev.setPos(event->pos());
    //send mouse release event to open the link
    mouseReleaseEvent(&iev);
}

void LinearFlowSnippet::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit mouseEvent( event->type());
    if (m_countFlicks)
        m_countFlicks = 0;
    d->m_movieTimer.setDuration(ANIMATION_DURATION);
    m_dragPoints.clear();
    DragPoint dragPoint;
    dragPoint.iPoint = event->pos().toPoint();
    dragPoint.iTime = QTime::currentTime();
    m_dragPoints.append(dragPoint);
    m_scrolled = false;
    m_lastMoveEventTime.setHMS(0,0,0,0);
    d->m_lastMoveEventPos = event->pos().toPoint();
}

void LinearFlowSnippet::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    emit mouseEvent( event->type());
    //check whether strip is scrolled
    if (!m_scrolled) {
        QPoint releasePoint = event->pos().toPoint();

        int cw = d->m_centerWindowSize.width();
        int ch = d->m_centerWindowSize.height();
        int sw = d->m_sideWindowSize.width();
        int sh = d->m_sideWindowSize.height();
        int w = d->m_widgetSize.width();
        int h = d->m_widgetSize.height();

        qreal leftLeftPageX = (((w - 4 * d->m_space) - cw) / 2.0) - (2 *sw);
        qreal rightRightPageX = (w + 4 * d->m_space + cw) / 2.0 + sw;
        qreal leftPageX = ((w - 4 * d->m_space) - cw) / 2.0 - sw + d->m_space;
        qreal centerPageX = (w - cw)/2.0;
        qreal rightPageX = (w + 4 * d->m_space + cw) / 2.0 - d->m_space;
        int pageIndex;

        //check for leftleft film
        if ((releasePoint.x() >= leftLeftPageX) && (releasePoint.x() <= (leftLeftPageX + sw)) &&
            (releasePoint.y() >= (d->m_sideTopSpace+ d->m_widgetSize.height() * TITLE_HEIGHT)) && (releasePoint.y() <= (d->m_sideTopSpace + sh + d->m_widgetSize.height() * TITLE_HEIGHT))) {

            pageIndex = d->m_centerIndex - 2;

        }

        //check for left film
        else if ((releasePoint.x() >= leftPageX) && (releasePoint.x() <= (leftPageX + sw)) &&
                    (releasePoint.y() >= (d->m_sideTopSpace+ d->m_widgetSize.height() * TITLE_HEIGHT)) && (releasePoint.y() <= (d->m_sideTopSpace + sh + d->m_widgetSize.height() * TITLE_HEIGHT))) {

            pageIndex = d->m_centerIndex - 1;

        }

        //check for center film
        else if ((releasePoint.x() >= centerPageX) && (releasePoint.x() <= (centerPageX + cw)) &&
                    (releasePoint.y() >= (d->m_centerTopSpace+ d->m_widgetSize.height() * TITLE_HEIGHT)) && (releasePoint.y() <= (d->m_centerTopSpace + ch + d->m_widgetSize.height() * TITLE_HEIGHT))) {
            pageIndex = d->m_centerIndex;
        }

        //check for right film
        else if ((releasePoint.x() >= rightPageX) && (releasePoint.x() <= (rightPageX + sw)) &&
                    (releasePoint.y() >= (d->m_sideTopSpace+ d->m_widgetSize.height() * TITLE_HEIGHT)) && (releasePoint.y() <= (d->m_sideTopSpace + sh + d->m_widgetSize.height() * TITLE_HEIGHT))) {
            pageIndex = d->m_centerIndex + 1;
        }

        //check for right right film
        else if ((releasePoint.x() >= rightRightPageX) && (releasePoint.x() <= (rightRightPageX + sw)) &&
                    (releasePoint.y() >= (d->m_sideTopSpace+ d->m_widgetSize.height() * TITLE_HEIGHT)) && (releasePoint.y() <= (d->m_sideTopSpace + sh + d->m_widgetSize.height() * TITLE_HEIGHT))) {
            pageIndex = d->m_centerIndex + 2;
        }

        //click else where ignore
        else  {
            return;
        }
        if ( pageIndex < 0)
            pageIndex = d->m_films.size() + pageIndex;
        else if (pageIndex >= d->m_films.size())
            pageIndex = (pageIndex) % d->m_films.size();

        emit ok(pageIndex);
    }

    else  {
        int msecs = 0;
        if (!m_lastMoveEventTime.isNull()) {
            //Start deceleration only if the delta since last drag event is less than threshold
            QTime now(QTime::currentTime());
            msecs = m_lastMoveEventTime.msecsTo(now);
            m_lastMoveEventTime.setHMS(0,0,0,0);
        }
        if ((msecs>0) && (msecs < THRESHOLD_FLICK_TIME)) {
            if ( isFlick()) {
                m_countFlicks = qBound (-MAX_FLICK_SPEED,speed().x(),+MAX_FLICK_SPEED)/((m_displayMode == "portrait") ? 200 : 100);
                startFlickScroll();
            }
        }
    }
}

void LinearFlowSnippet::startFlickScroll()
{
    d->m_movieTimer.setDuration(qAbs(ANIMATION_DURATION / m_countFlicks));
    if (m_countFlicks < 0) {
        showNext();
        m_countFlicks++;
    }
    else {
        showPrevious();
        m_countFlicks--;
    }
}

void LinearFlowSnippet::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_ASSERT(d);
    Q_ASSERT(d->m_buffer);
    Q_ASSERT(d->m_titleBuffer);

    painter->drawImage(QPointF(0,0), *(d->m_titleBuffer));

    d->m_buffer->fill(d->m_bgColor);

    if (d->m_films.size() > 0 && d->m_centerIndex != INVALID_INDEX) {

        QPainter bufPaint(d->m_buffer);
        // draw center film strip
        CALL_ON_CENTER_FILM_STRIP(paint(&bufPaint));

        // draw left film strip
        CALL_ON_PREV_FILM_STRIP(paint(&bufPaint));

        // draw left left film strip
        CALL_ON_PREV_PREV_FILM_STRIP(paint(&bufPaint));

        // draw left left film strip
        CALL_ON_PREV_PREV_PREV_FILM_STRIP(paint(&bufPaint));

        // draw right film strip
        CALL_ON_NEXT_FILM_STRIP(paint(&bufPaint));

        // draw right right film strip
        CALL_ON_NEXT_NEXT_FILM_STRIP(paint(&bufPaint));

        // draw right right film strip
        CALL_ON_NEXT_NEXT_NEXT_FILM_STRIP(paint(&bufPaint));

        // 1. draw image from the buffer
        painter->drawImage(QPoint(0,d->m_widgetSize.height() * TITLE_HEIGHT), *(d->m_buffer));
        if (d->m_movieTimer.state() != QTimeLine::Running) {
            // 2. draw close icon
            if (d->m_films.size() > 1 && d->m_closeIcon)
                painter->drawImage(d->m_closeIconPos, *(d->m_closeIcon));
        }
    }
    else
        painter->drawImage(QPoint(0,d->m_widgetSize.height() * TITLE_HEIGHT), *(d->m_buffer));
}

} // end of namespace
