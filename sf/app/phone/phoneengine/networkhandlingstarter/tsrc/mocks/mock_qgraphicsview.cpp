/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#include <QDebug>
#include <QVariant>
#include <smcmockclassincludes.h>
#include <qgraphicsview.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QGraphicsView::QGraphicsView
// -----------------------------------------------------------------------------
//
QGraphicsView::QGraphicsView( 
        QWidget * parent )
    //:
    //QAbstractScrollArea( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsView::QGraphicsView
// -----------------------------------------------------------------------------
//
QGraphicsView::QGraphicsView( 
        QGraphicsScene * scene,
        QWidget * parent )
    //:
    //QAbstractScrollArea( /*scene, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsView::~QGraphicsView
// -----------------------------------------------------------------------------
//
QGraphicsView::~QGraphicsView(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsView::sizeHint
// -----------------------------------------------------------------------------
//
QSize QGraphicsView::sizeHint(  ) const
    {
    SMC_MOCK_METHOD0( QSize )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::renderHints
// -----------------------------------------------------------------------------
//
QPainter::RenderHints QGraphicsView::renderHints(  ) const
    {
    SMC_MOCK_METHOD0( QPainter::RenderHints )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setRenderHint
// -----------------------------------------------------------------------------
//
void QGraphicsView::setRenderHint( 
        QPainter::RenderHint hint,
        bool enabled )
    {
    //SMC_MOCK_METHOD2( void, QPainter::RenderHint, hint, 
    //    bool, enabled )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setRenderHints
// -----------------------------------------------------------------------------
//
void QGraphicsView::setRenderHints( 
        QPainter::RenderHints hints )
    {
    //SMC_MOCK_METHOD1( void, QPainter::RenderHints, hints )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::alignment
// -----------------------------------------------------------------------------
//
Qt::Alignment QGraphicsView::alignment(  ) const
    {
    SMC_MOCK_METHOD0( Qt::Alignment )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setAlignment
// -----------------------------------------------------------------------------
//
void QGraphicsView::setAlignment( 
        Qt::Alignment alignment )
    {
    //SMC_MOCK_METHOD1( void, Qt::Alignment, alignment )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::transformationAnchor
// -----------------------------------------------------------------------------
//
QGraphicsView::ViewportAnchor QGraphicsView::transformationAnchor(  ) const
    {
    SMC_MOCK_METHOD0( ViewportAnchor )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setTransformationAnchor
// -----------------------------------------------------------------------------
//
void QGraphicsView::setTransformationAnchor( 
        ViewportAnchor anchor )
    {
    //SMC_MOCK_METHOD1( void, ViewportAnchor, anchor )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::resizeAnchor
// -----------------------------------------------------------------------------
//
/*
ViewportAnchor QGraphicsView::resizeAnchor(  ) const
    {
    SMC_MOCK_METHOD0( ViewportAnchor )
    }
*/

// -----------------------------------------------------------------------------
// QGraphicsView::setResizeAnchor
// -----------------------------------------------------------------------------
//
void QGraphicsView::setResizeAnchor( 
        QGraphicsView::ViewportAnchor anchor )
    {
    //SMC_MOCK_METHOD1( void, ViewportAnchor, anchor )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::viewportUpdateMode
// -----------------------------------------------------------------------------
//
QGraphicsView::ViewportUpdateMode QGraphicsView::viewportUpdateMode(  ) const
    {
    SMC_MOCK_METHOD0( ViewportUpdateMode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setViewportUpdateMode
// -----------------------------------------------------------------------------
//
void QGraphicsView::setViewportUpdateMode( 
        QGraphicsView::ViewportUpdateMode mode )
    {
    //SMC_MOCK_METHOD1( void, ViewportUpdateMode, mode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::optimizationFlags
// -----------------------------------------------------------------------------
//
QGraphicsView::OptimizationFlags QGraphicsView::optimizationFlags(  ) const
    {
    SMC_MOCK_METHOD0( OptimizationFlags )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setOptimizationFlag
// -----------------------------------------------------------------------------
//
void QGraphicsView::setOptimizationFlag( 
        QGraphicsView::OptimizationFlag flag,
        bool enabled )
    {
    SMC_MOCK_METHOD2( void, OptimizationFlag, flag, 
        bool, enabled )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setOptimizationFlags
// -----------------------------------------------------------------------------
//
void QGraphicsView::setOptimizationFlags( 
        OptimizationFlags flags )
    {
    //SMC_MOCK_METHOD1( void, OptimizationFlags, flags )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::dragMode
// -----------------------------------------------------------------------------
//
QGraphicsView::DragMode QGraphicsView::dragMode(  ) const
    {
    SMC_MOCK_METHOD0( DragMode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setDragMode
// -----------------------------------------------------------------------------
//
void QGraphicsView::setDragMode( 
        DragMode mode )
    {
    //SMC_MOCK_METHOD1( void, DragMode, mode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::rubberBandSelectionMode
// -----------------------------------------------------------------------------
//
Qt::ItemSelectionMode QGraphicsView::rubberBandSelectionMode(  ) const
    {
    SMC_MOCK_METHOD0( Qt::ItemSelectionMode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setRubberBandSelectionMode
// -----------------------------------------------------------------------------
//
void QGraphicsView::setRubberBandSelectionMode( 
        Qt::ItemSelectionMode mode )
    {
    //SMC_MOCK_METHOD1( void, Qt::ItemSelectionMode, mode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::cacheMode
// -----------------------------------------------------------------------------
//
QGraphicsView::CacheMode QGraphicsView::cacheMode(  ) const
    {
    SMC_MOCK_METHOD0( CacheMode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setCacheMode
// -----------------------------------------------------------------------------
//
void QGraphicsView::setCacheMode( 
        CacheMode mode )
    {
    //SMC_MOCK_METHOD1( void, CacheMode, mode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::resetCachedContent
// -----------------------------------------------------------------------------
//
void QGraphicsView::resetCachedContent(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::isInteractive
// -----------------------------------------------------------------------------
//
bool QGraphicsView::isInteractive(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setInteractive
// -----------------------------------------------------------------------------
//
void QGraphicsView::setInteractive( 
        bool allowed )
    {
    SMC_MOCK_METHOD1( void, bool, allowed )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::scene
// -----------------------------------------------------------------------------
//
QGraphicsScene * QGraphicsView::scene(  ) const
    {
    SMC_MOCK_METHOD0( QGraphicsScene * )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setScene
// -----------------------------------------------------------------------------
//
void QGraphicsView::setScene( 
        QGraphicsScene * scene )
    {
    //SMC_MOCK_METHOD1( void, QGraphicsScene *, scene )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::sceneRect
// -----------------------------------------------------------------------------
//
QRectF QGraphicsView::sceneRect(  ) const
    {
    SMC_MOCK_METHOD0( QRectF )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setSceneRect
// -----------------------------------------------------------------------------
//
void QGraphicsView::setSceneRect( 
        const QRectF & rect )
    {
    //SMC_MOCK_METHOD1( void, const QRectF &, rect )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::matrix
// -----------------------------------------------------------------------------
//
QMatrix QGraphicsView::matrix(  ) const
    {
    SMC_MOCK_METHOD0( QMatrix )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setMatrix
// -----------------------------------------------------------------------------
//
void QGraphicsView::setMatrix( 
        const QMatrix & matrix,
        bool combine )
    {
    //SMC_MOCK_METHOD2( void, const QMatrix &, matrix, 
    //    bool, combine )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::resetMatrix
// -----------------------------------------------------------------------------
//
void QGraphicsView::resetMatrix(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::transform
// -----------------------------------------------------------------------------
//
QTransform QGraphicsView::transform(  ) const
    {
    SMC_MOCK_METHOD0( QTransform )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::viewportTransform
// -----------------------------------------------------------------------------
//
QTransform QGraphicsView::viewportTransform(  ) const
    {
    SMC_MOCK_METHOD0( QTransform )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::isTransformed
// -----------------------------------------------------------------------------
//
bool QGraphicsView::isTransformed(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setTransform
// -----------------------------------------------------------------------------
//
void QGraphicsView::setTransform( 
        const QTransform & matrix,
        bool combine )
    {
    //SMC_MOCK_METHOD2( void, const QTransform &, matrix, 
    //    bool, combine )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::resetTransform
// -----------------------------------------------------------------------------
//
void QGraphicsView::resetTransform(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::rotate
// -----------------------------------------------------------------------------
//
void QGraphicsView::rotate( 
        qreal angle )
    {
    //SMC_MOCK_METHOD1( void, qreal, angle )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::scale
// -----------------------------------------------------------------------------
//
void QGraphicsView::scale( 
        qreal sx,
        qreal sy )
    {
    //SMC_MOCK_METHOD2( void, qreal, sx, 
    //    qreal, sy )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::shear
// -----------------------------------------------------------------------------
//
void QGraphicsView::shear( 
        qreal sh,
        qreal sv )
    {
    //SMC_MOCK_METHOD2( void, qreal, sh, 
    //    qreal, sv )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::translate
// -----------------------------------------------------------------------------
//
void QGraphicsView::translate( 
        qreal dx,
        qreal dy )
    {
    SMC_MOCK_METHOD2( void, qreal, dx, 
        qreal, dy )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::centerOn
// -----------------------------------------------------------------------------
//
void QGraphicsView::centerOn( 
        const QPointF & pos )
    {
    //SMC_MOCK_METHOD1( void, const QPointF &, pos )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::centerOn
// -----------------------------------------------------------------------------
//
void QGraphicsView::centerOn( 
        const QGraphicsItem * item )
    {
    //SMC_MOCK_METHOD1( void, const QGraphicsItem *, item )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::ensureVisible
// -----------------------------------------------------------------------------
//
void QGraphicsView::ensureVisible( 
        const QRectF & rect,
        int xmargin,
        int ymargin )
    {
    //SMC_MOCK_METHOD3( void, const QRectF &, rect, 
    //    int, xmargin, 
    //    int, ymargin )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::ensureVisible
// -----------------------------------------------------------------------------
//
void QGraphicsView::ensureVisible( 
        const QGraphicsItem * item,
        int xmargin,
        int ymargin )
    {
    //SMC_MOCK_METHOD3( void, const QGraphicsItem *, item, 
    //    int, xmargin, 
    //    int, ymargin )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::fitInView
// -----------------------------------------------------------------------------
//
void QGraphicsView::fitInView( 
        const QRectF & rect,
        Qt::AspectRatioMode aspectRadioMode )
    {
    //SMC_MOCK_METHOD2( void, const QRectF &, rect, 
    //    Qt::AspectRatioMode, aspectRadioMode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::fitInView
// -----------------------------------------------------------------------------
//
void QGraphicsView::fitInView( 
        const QGraphicsItem * item,
        Qt::AspectRatioMode aspectRadioMode )
    {
    //SMC_MOCK_METHOD2( void, const QGraphicsItem *, item, 
    //    Qt::AspectRatioMode, aspectRadioMode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::render
// -----------------------------------------------------------------------------
//
void QGraphicsView::render( 
        QPainter * painter,
        const QRectF & target,
        const QRect & source,
        Qt::AspectRatioMode aspectRatioMode )
    {
    //SMC_MOCK_METHOD4( void, QPainter *, painter, 
    //    const QRectF &, target, 
    //    const QRect &, source, 
    //    Qt::AspectRatioMode, aspectRatioMode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::items
// -----------------------------------------------------------------------------
//
QList <QGraphicsItem * > QGraphicsView::items(  ) const
    {
    SMC_MOCK_METHOD0( QList <QGraphicsItem * > )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::items
// -----------------------------------------------------------------------------
//
QList <QGraphicsItem * > QGraphicsView::items( 
        const QPoint & pos ) const
    {
    //SMC_MOCK_METHOD1( QList <QGraphicsItem * >, const QPoint &, pos )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::items
// -----------------------------------------------------------------------------
//
QList <QGraphicsItem * > QGraphicsView::items( 
        const QRect & rect,
        Qt::ItemSelectionMode mode ) const
    {
    //SMC_MOCK_METHOD2( QList <QGraphicsItem * >, const QRect &, rect, 
    //   Qt::ItemSelectionMode, mode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::items
// -----------------------------------------------------------------------------
//
QList <QGraphicsItem * > QGraphicsView::items( 
        const QPolygon & polygon,
        Qt::ItemSelectionMode mode ) const
    {
    //SMC_MOCK_METHOD2( QList <QGraphicsItem * >, const QPolygon &, polygon, 
    //    Qt::ItemSelectionMode, mode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::items
// -----------------------------------------------------------------------------
//
QList <QGraphicsItem * > QGraphicsView::items( 
        const QPainterPath & path,
        Qt::ItemSelectionMode mode ) const
    {
    //SMC_MOCK_METHOD2( QList <QGraphicsItem * >, const QPainterPath &, path, 
    //    Qt::ItemSelectionMode, mode )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::itemAt
// -----------------------------------------------------------------------------
//
QGraphicsItem * QGraphicsView::itemAt( 
        const QPoint & pos ) const
    {
    //SMC_MOCK_METHOD1( QGraphicsItem *, const QPoint &, pos )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mapToScene
// -----------------------------------------------------------------------------
//
QPointF QGraphicsView::mapToScene( 
        const QPoint & point ) const
    {
    //SMC_MOCK_METHOD1( QPointF, const QPoint &, point )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mapToScene
// -----------------------------------------------------------------------------
//
QPolygonF QGraphicsView::mapToScene( 
        const QRect & rect ) const
    {
    //SMC_MOCK_METHOD1( QPolygonF, const QRect &, rect )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mapToScene
// -----------------------------------------------------------------------------
//
QPolygonF QGraphicsView::mapToScene( 
        const QPolygon & polygon ) const
    {
    //SMC_MOCK_METHOD1( QPolygonF, const QPolygon &, polygon )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mapToScene
// -----------------------------------------------------------------------------
//
QPainterPath QGraphicsView::mapToScene( 
        const QPainterPath & path ) const
    {
    //SMC_MOCK_METHOD1( QPainterPath, const QPainterPath &, path )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mapFromScene
// -----------------------------------------------------------------------------
//
QPoint QGraphicsView::mapFromScene( 
        const QPointF & point ) const
    {
    SMC_MOCK_METHOD1( QPoint, const QPointF &, point )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mapFromScene
// -----------------------------------------------------------------------------
//
QPolygon QGraphicsView::mapFromScene( 
        const QRectF & rect ) const
    {
    //SMC_MOCK_METHOD1( QPolygon, const QRectF &, rect )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mapFromScene
// -----------------------------------------------------------------------------
//
QPolygon QGraphicsView::mapFromScene( 
        const QPolygonF & polygon ) const
    {
    //SMC_MOCK_METHOD1( QPolygon, const QPolygonF &, polygon )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mapFromScene
// -----------------------------------------------------------------------------
//
QPainterPath QGraphicsView::mapFromScene( 
        const QPainterPath & path ) const
    {
    //SMC_MOCK_METHOD1( QPainterPath, const QPainterPath &, path )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::inputMethodQuery
// -----------------------------------------------------------------------------
//
QVariant QGraphicsView::inputMethodQuery( 
        Qt::InputMethodQuery query ) const
    {
    //SMC_MOCK_METHOD1( QVariant, Qt::InputMethodQuery, query )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::backgroundBrush
// -----------------------------------------------------------------------------
//
QBrush QGraphicsView::backgroundBrush(  ) const
    {
    SMC_MOCK_METHOD0( QBrush )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setBackgroundBrush
// -----------------------------------------------------------------------------
//
void QGraphicsView::setBackgroundBrush( 
        const QBrush & brush )
    {
    //SMC_MOCK_METHOD1( void, const QBrush &, brush )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::foregroundBrush
// -----------------------------------------------------------------------------
//
QBrush QGraphicsView::foregroundBrush(  ) const
    {
    SMC_MOCK_METHOD0( QBrush )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setForegroundBrush
// -----------------------------------------------------------------------------
//
void QGraphicsView::setForegroundBrush( 
        const QBrush & brush )
    {
    //SMC_MOCK_METHOD1( void, const QBrush &, brush )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::updateScene
// -----------------------------------------------------------------------------
//
void QGraphicsView::updateScene( 
        const QList<QRectF> & rects )
    {
    //SMC_MOCK_METHOD1( Q_SLOTS : void, const QList<QRectF> &, rects )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::invalidateScene
// -----------------------------------------------------------------------------
//
void QGraphicsView::invalidateScene( 
        const QRectF & rect,
        QGraphicsScene::SceneLayers layers )
    {
    //SMC_MOCK_METHOD2( void, const QRectF &, rect, 
    //    QGraphicsScene::SceneLayers, layers )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::updateSceneRect
// -----------------------------------------------------------------------------
//
void QGraphicsView::updateSceneRect( 
        const QRectF & rect )
    {
    //SMC_MOCK_METHOD1( void, const QRectF &, rect )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::setupViewport
// -----------------------------------------------------------------------------
//
void QGraphicsView::setupViewport( 
        QWidget * widget )
    {
    //SMC_MOCK_METHOD1( Q_SLOTS : void, QWidget *, widget )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::QGraphicsView
// -----------------------------------------------------------------------------
//
QGraphicsView::QGraphicsView( 
        QGraphicsViewPrivate &,
        QWidget * parent )
    //:
    //QAbstractScrollArea( /*&, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsView::event
// -----------------------------------------------------------------------------
//
bool QGraphicsView::event( 
        QEvent * event )
    {
    //SMC_MOCK_METHOD1( bool, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::viewportEvent
// -----------------------------------------------------------------------------
//
bool QGraphicsView::viewportEvent( 
        QEvent * event )
    {
    //SMC_MOCK_METHOD1( bool, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::contextMenuEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::contextMenuEvent( 
        QContextMenuEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QContextMenuEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::dragEnterEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::dragEnterEvent( 
        QDragEnterEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QDragEnterEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::dragLeaveEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::dragLeaveEvent( 
        QDragLeaveEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QDragLeaveEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::dragMoveEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::dragMoveEvent( 
        QDragMoveEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QDragMoveEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::dropEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::dropEvent( 
        QDropEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QDropEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::focusInEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::focusInEvent( 
        QFocusEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QFocusEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::focusNextPrevChild
// -----------------------------------------------------------------------------
//
bool QGraphicsView::focusNextPrevChild( 
        bool next )
    {
    //SMC_MOCK_METHOD1( bool, bool, next )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::focusOutEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::focusOutEvent( 
        QFocusEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QFocusEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::keyPressEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::keyPressEvent( 
        QKeyEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QKeyEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::keyReleaseEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::keyReleaseEvent( 
        QKeyEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QKeyEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mouseDoubleClickEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::mouseDoubleClickEvent( 
        QMouseEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QMouseEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mousePressEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::mousePressEvent( 
        QMouseEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QMouseEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mouseMoveEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::mouseMoveEvent( 
        QMouseEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QMouseEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::mouseReleaseEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::mouseReleaseEvent( 
        QMouseEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QMouseEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::wheelEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::wheelEvent( 
        QWheelEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QWheelEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::paintEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::paintEvent( 
        QPaintEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QPaintEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::resizeEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::resizeEvent( 
        QResizeEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QResizeEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::scrollContentsBy
// -----------------------------------------------------------------------------
//
void QGraphicsView::scrollContentsBy( 
        int dx,
        int dy )
    {
    //SMC_MOCK_METHOD2( void, int, dx, 
    //    int, dy )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::showEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::showEvent( 
        QShowEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QShowEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::inputMethodEvent
// -----------------------------------------------------------------------------
//
void QGraphicsView::inputMethodEvent( 
        QInputMethodEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QInputMethodEvent *, event )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::drawBackground
// -----------------------------------------------------------------------------
//
void QGraphicsView::drawBackground( 
        QPainter * painter,
        const QRectF & rect )
    {
    //SMC_MOCK_METHOD2( void, QPainter *, painter, 
    //    const QRectF &, rect )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::drawForeground
// -----------------------------------------------------------------------------
//
void QGraphicsView::drawForeground( 
        QPainter * painter,
        const QRectF & rect )
    {
    //SMC_MOCK_METHOD2( void, QPainter *, painter, 
    //    const QRectF &, rect )
    }


// -----------------------------------------------------------------------------
// QGraphicsView::drawItems
// -----------------------------------------------------------------------------
//
/*
void QGraphicsView::drawItems( 
        QPainter * painter,
        int numItems,
        QGraphicsItem * items,
        const QStyleOptionGraphicsItem options )
    {
    //SMC_MOCK_METHOD4( void, QPainter *, painter, 
    //    int, numItems, 
    //    QGraphicsItem *, items, 
    //    const QStyleOptionGraphicsItem, options )
    }

*/
