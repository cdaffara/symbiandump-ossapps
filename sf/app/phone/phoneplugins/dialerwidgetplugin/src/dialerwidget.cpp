/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QGraphicsSceneMouseEvent>

#include <HbStyle>
#include <HbStyleLoader>
#include <HbFrameItem>
#include <HbFrameDrawer>
#include <HbIconItem>
#include <HbTextItem>
#include <HbTouchArea>
#include <HbInstantFeedback>
#include <HbTapGesture>

#include "dialerwidget.h"

#ifdef Q_OS_SYMBIAN
    #include "dialerwidgetengine.h"
    #include "qtphonelog.h"
    #include <xqappmgr.h>
    #include <xqservicerequest.h>
    #include <xqpublishandsubscribeutils.h>
    #include <xqrequestinfo.h>
    #include <xqaiwdecl.h>
#else
    #define PHONE_TRACE
#endif

const QLatin1String KDialerWidgetIconNormal("qtg_graf_hs_dialer_normal");
const QLatin1String KDialerWidgetIconPressed("qtg_graf_hs_dialer_pressed");
const QLatin1String KDialerWidgetMissedCallBadge("qtg_fr_shortcut_badge_bg");
const QLatin1String KDialerWidgetWidgetml(":/resource/dialerwidget.widgetml");
const QLatin1String KDialerWidgetCss(":/resource/dialerwidget.css");
const QLatin1String KDialerWidgetNormalLayout("normal");
const QLatin1String KDialerWidgetMissedCallLayout("missed_call");
const int KDialerWidgetMaxBadgeTextLenght = 2; // displayable
// update corresponding values also in css side
const qreal KDialerWidgetNormalSize = 12.0; // unit
const qreal KDialerWidgetMissedCallSize = 12.75; // unit

/*!
    \class DialerWidget

    \ingroup group_dialerwidgetplugin
    \brief Implementation for the homescreen dialer launcher.

*/

/*!
    Constructs dialer widget with given \a parent and given window \a flags.
*/
DialerWidget::DialerWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
  : HbWidget(parent, flags),
    m_background(0),
    m_badgeBackground(0),
    m_text(0),
    m_touchArea(0),
    m_engine(0)
{   
    PHONE_TRACE
    createPrimitives();

    HbStyleLoader::registerFilePath(KDialerWidgetWidgetml);
    HbStyleLoader::registerFilePath(KDialerWidgetCss);

    setLayout(KDialerWidgetNormalLayout);
}

/*!
    Destructor.
*/
DialerWidget::~DialerWidget()
{
    HbStyleLoader::unregisterFilePath(KDialerWidgetWidgetml);
    HbStyleLoader::unregisterFilePath(KDialerWidgetCss);
}

/*!
    Lenght of badge text.
*/
int DialerWidget::badgeTextLenght() const
{
    return m_text->text().length();
}

/*!
    Layout name.
*/
QString DialerWidget::layoutName() const
{
    return m_layoutName;
}

/*!
    \fn void DialerWidget::startDialer()

    Starts dialer widget via view activation service.
*/
void DialerWidget::startDialer()
{
    PHONE_TRACE
#ifdef Q_OS_SYMBIAN

    QList<QVariant> args;
    QString service;
    QString interface;
    QString operation;

    service = "logs";
    interface = XQI_LOGS_VIEW;
    operation = XQOP_LOGS_SHOW;
    QVariantMap map;
    map.insert(XQLOGS_VIEW_INDEX, QVariant(int(XQService::LogsViewAll)));
    map.insert(XQLOGS_SHOW_DIALPAD, QVariant(true));
    map.insert(XQLOGS_DIALPAD_TEXT, QVariant(QString()));
    args.append(QVariant(map));

    XQApplicationManager appManager;
    QScopedPointer<XQAiwRequest> request(appManager.create(service, interface, operation, false));
    if (request == NULL) {
        PHONE_TRACE1("service not found");
        return;
    }
    request->setArguments(args);
    request->setSynchronous(false);
    bool ret = request->send();
    PHONE_TRACE2("request sent successfully:", ret);
#endif

}

void DialerWidget::onInitialize()
{
    PHONE_TRACE
#ifdef Q_OS_SYMBIAN
    QT_TRY{
        // Engine construction is 2 phased 
        m_engine = new DialerWidgetEngine();
        connect(m_engine, SIGNAL( exceptionOccured(const int&) )
                ,this, SLOT( onEngineException(const int&) ) );
        
        connect( m_engine, SIGNAL(missedCallsCountChanged(const int&)),
                        this, SLOT(onMissedCallsCountChange(const int&)));
        
        if(!m_engine->initialize()){
            //engine construction failed. Give up.
            emit error();
            return;
            }
        }
    QT_CATCH(...){
        emit error();
    }
#endif
}

/*!
    \fn void DialerWidget::onShow()

    Shows the widget
*/
void DialerWidget::onShow()
{
    PHONE_TRACE
}

/*!
    \fn void DialerWidget::onHide()

    Hides the widget
*/
void DialerWidget::onHide()
{
    PHONE_TRACE
}

/*!
    \fn void DialerWidget::onUninitialize()

    Uninitializes the widget
*/
void DialerWidget::onUninitialize()
{
    PHONE_TRACE
    delete m_engine;
    m_engine = 0;
}

void DialerWidget::onEngineException(const int& exc)
{
    Q_UNUSED(exc);
    emit error();
}

void DialerWidget::onMissedCallsCountChange(const int& count)
{
    if (count){
        QString newText = QLocale::system().toString(count);

        if (newText.length()>KDialerWidgetMaxBadgeTextLenght) {
            newText = QLatin1String("*");
        }

        bool doRepolish = (m_text->text().length() != newText.length());

        m_badgeBackground->setVisible(true);

        m_text->setText(newText);

        m_text->setVisible(true);

        if (doRepolish) {
            repolish();
        }

        setLayout(KDialerWidgetMissedCallLayout);
    } else {
        m_badgeBackground->setVisible(false);

        m_text->setVisible(false);

        setLayout(KDialerWidgetNormalLayout);
    }
}

void DialerWidget::gestureEvent(QGestureEvent *event)
{
    HbTapGesture *gesture = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture));
    if (gesture) {
        switch (gesture->state()) {
            case Qt::GestureStarted:
                setBackgroundToPressed();
                break;            
            case Qt::GestureCanceled:
                setBackgroundToNormal();
                break;
            case Qt::GestureFinished:
                setBackgroundToNormal();
                if (gesture->tapStyleHint() == HbTapGesture::Tap) {
                    HbInstantFeedback::play(HbFeedback::Basic);
                    startDialer();
                }
                break;
            default:
                break;
        }
    }
}

QRectF DialerWidget::boundingRect() const
{
    return childrenBoundingRect();
}

QPainterPath DialerWidget::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void DialerWidget::createPrimitives()
{   
    // Background
    if (!m_background) {
        HbFrameDrawer *drawer = new HbFrameDrawer(
            KDialerWidgetIconNormal, HbFrameDrawer::OnePiece);
        m_background = new HbFrameItem(drawer, this);
        style()->setItemName(m_background, QLatin1String("background"));
    }
    
    // Badge background
    if (!m_badgeBackground) {
        HbFrameDrawer *badgedrawer = new HbFrameDrawer(
            KDialerWidgetMissedCallBadge, HbFrameDrawer::ThreePiecesHorizontal);
        m_badgeBackground = new HbFrameItem(badgedrawer, this);
        style()->setItemName(m_badgeBackground, QLatin1String("badgeBackground"));
        m_badgeBackground->setVisible( false );
    }

    // Text
    if (!m_text) {
        m_text = new HbTextItem(this);
        style()->setItemName(m_text, QLatin1String("badgeText"));
        m_text->setVisible(false);
    }

    // Touch area
    if (!m_touchArea) {
        m_touchArea = new HbTouchArea(this);
        m_touchArea->setFlag(QGraphicsItem::ItemIsFocusable);
        style()->setItemName(m_touchArea, QLatin1String("touchArea"));
        setFiltersChildEvents(true);
        m_touchArea->grabGesture(Qt::TapGesture);
    }
}

void DialerWidget::setBackgroundToNormal()
{
    if (m_background) {
        m_background->frameDrawer().
            setFrameGraphicsName(KDialerWidgetIconNormal);
    }
}

void DialerWidget::setBackgroundToPressed()
{
    if (m_background) {
        m_background->frameDrawer().
            setFrameGraphicsName(KDialerWidgetIconPressed);
    }
}

void DialerWidget::setLayout(const QString& layoutName)
{
    if (layoutName==m_layoutName) {
        return;
    }

    qreal unit = HbDeviceProfile::profile(this).unitValue();

    prepareGeometryChange();

    if (layoutName==KDialerWidgetMissedCallLayout) {
        resize(KDialerWidgetMissedCallSize*unit,
               KDialerWidgetMissedCallSize*unit);
    } else {
        resize(KDialerWidgetNormalSize*unit,
               KDialerWidgetNormalSize*unit);
    }

    m_layoutName = layoutName;

    repolish();
}
