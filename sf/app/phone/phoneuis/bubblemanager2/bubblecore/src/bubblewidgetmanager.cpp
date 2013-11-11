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
* Description: Interface to HbDocumentLoader (DocML files).
*
*/

#include <QtCore>
#include <hbstyle.h>
#include <hbdocumentloader.h>
#include <hbframeitem.h>
#include "bubblewidgetmanager.h"
#include "bubblecontainerwidget.h"
#include "bubbleimagewidget.h"
#include "bubbleheadingwidget.h"
#include "bubbleexpandedhandler.h"
#include "bubblecollapsedhandler.h"
#include "bubbleconferencehandler.h"
#include "bubbleparticipantlistitem.h"
#include "bubblebutton.h"
#include "hbtextitem.h"

const char *BUBBLE_DOCUMENT_CONTENT = "content";

// document loader
class BubbleDocumentLoader : public HbDocumentLoader
{
public:
    BubbleDocumentLoader(BubbleImageManager& imageManager);
    virtual QObject *createObject(const QString& type,
                                  const QString &name);
private:
    BubbleImageManager& mImageManager;
};


BubbleWidgetManager::BubbleWidgetManager(
    BubbleImageManager& imageManager, QObject* parent)
    : QObject(parent), mImageManager(imageManager),
      mOrientation(Qt::Vertical)
{
    // .docml mappings
    mFileNames.insert(SingleCallView,
                      QLatin1String(":/bubble_layout_1.docml"));
    mFileNames.insert(TwoCallsView,
                      QLatin1String(":/bubble_layout_2.docml"));
    mFileNames.insert(ThreeCallsView,
                      QLatin1String(":/bubble_layout_3.docml"));
    mFileNames.insert(ConferenceView,
                      QLatin1String(":/bubble_layout_4.docml"));
    mFileNames.insert(MutedOverlay,
                      QLatin1String(":/bubble_layout_5.docml"));

    mOrientationName.insert(Qt::Vertical, QLatin1String("portrait"));
    mOrientationName.insert(Qt::Horizontal, QLatin1String("landscape"));

    mWidgetPrefix.insert(ExpandedBubble, QLatin1String("eb:"));
    mWidgetPrefix.insert(CollapsedBubble, QLatin1String("cb:"));
    mWidgetPrefix.insert(CollapsedBubble2, QLatin1String("cb2:"));
    mContainerName.insert(ExpandedBubble, QLatin1String("expandedBubble"));
    mContainerName.insert(CollapsedBubble, QLatin1String("collapsedBubble"));
    mContainerName.insert(CollapsedBubble2, QLatin1String("collapsedBubble2"));
    mWidgetName.insert(Heading, QLatin1String("bubbleHeading"));
    mWidgetName.insert(Image, QLatin1String("callImage"));
    mWidgetName.insert(LeftButton, QLatin1String("leftButton"));
    mWidgetName.insert(CenterButton, QLatin1String("centerButton"));
    mWidgetName.insert(RightButton, QLatin1String("rightButton"));
    mWidgetName.insert(ParticipantList, QLatin1String("participantList"));
    mWidgetName.insert(MutedIcon, QLatin1String("mutedIcon"));
}

BubbleWidgetManager::~BubbleWidgetManager()
{
    qDeleteAll(mDocumentLoaders);
    mDocumentLoaders.clear();

    QList<int> keys = mDocumentHandlers.keys();
    foreach (int key, keys) {
        qDeleteAll(*mDocumentHandlers[key]);
    }
    qDeleteAll(mDocumentHandlers);
    mDocumentHandlers.clear();
}

QGraphicsWidget* BubbleWidgetManager::view(View view)
{
    if (mDocumentContent.contains(view)) {
        // already loaded
        return mDocumentContent[view];
    } else {
        // load from .docml
        return loadDocument(view,mOrientation);
    }
}

bool BubbleWidgetManager::isLoaded(View view)
{
    if (mDocumentContent.contains(view)) {
        return true;
    } else {
        return false;
    }
}

void BubbleWidgetManager::releaseView(View view)
{
    Q_UNUSED(view)

    // release loaders
    BubbleDocumentLoader* loader = mDocumentLoaders.value(view,0);
    delete loader;
    mDocumentLoaders.remove(view);

    // release handler
    QList<BubbleHandler*>* handlers = mDocumentHandlers.value(view,0);
    if (handlers) {
        qDeleteAll(*handlers);
        delete handlers;
        mDocumentHandlers.remove(view);
    }

    // release document objects
    QObjectList objectList = mDocumentObjects.value(view);
    foreach(QObject* object, objectList) {
        QMetaObject::invokeMethod(object, "deleteLater", Qt::QueuedConnection);
    }
    mDocumentObjects.remove(view);
    mDocumentContent.remove(view);
}

QGraphicsWidget* BubbleWidgetManager::loadDocument(
    View view,
    Qt::Orientation orientation)
{
    BubbleDocumentLoader* loader =
        new BubbleDocumentLoader(mImageManager);
    bool ok;

    QObjectList objectList = loader->load(mFileNames[view],&ok);
    Q_ASSERT(ok);
    loader->load(mFileNames[view],mOrientationName[orientation],&ok);
    Q_ASSERT(ok);

    mDocumentLoaders.insert(view,loader);
    mDocumentObjects.insert(view,objectList);

    QGraphicsWidget* widget =
        mDocumentLoaders[view]->findWidget(BUBBLE_DOCUMENT_CONTENT);

    Q_ASSERT(widget);
    widget->setVisible(false);
    mDocumentContent.insert(view,widget);

    if (view!=MutedOverlay) {
        createDocumentHandlers(view);
    }

    applyCustomStyles(view);

    return widget;
}

void BubbleWidgetManager::applyCustomStyles(View view)
{
    setBackground(widget(view,None,MutedIcon));
}

void BubbleWidgetManager::setBackground(QGraphicsWidget* widget)
{
    HbWidget* w;
    w = qobject_cast<HbWidget*>(widget);
    if (w) {
        HbFrameItem* item =
            new HbFrameItem(QLatin1String("qtg_fr_popup_trans"),
                            HbFrameDrawer::NinePieces);
        w->setBackgroundItem(item); // takes ownership
    }
}

void BubbleWidgetManager::createDocumentHandlers(View view)
{
    Q_ASSERT(mDocumentLoaders.contains(view));

    QList<BubbleHandler*>* handlers = new QList<BubbleHandler*>;

    if (mDocumentLoaders[view]->findWidget(mContainerName[ExpandedBubble]) ) {
        if (view==ConferenceView) {
            BubbleConferenceHandler* handler =
                new BubbleConferenceHandler(*this,view,ExpandedBubble);
            handlers->append(handler);
        } else {
            BubbleExpandedHandler* handler =
                new BubbleExpandedHandler(*this,view,ExpandedBubble);
            handlers->append(handler);
        }
    }

    if (mDocumentLoaders[view]->findWidget(mContainerName[CollapsedBubble]) ) {
        BubbleCollapsedHandler* handler =
            new BubbleCollapsedHandler(*this,view,CollapsedBubble);
        handlers->append(handler);
    }

    if (mDocumentLoaders[view]->findWidget(mContainerName[CollapsedBubble2]) ) {
        BubbleCollapsedHandler* handler =
            new BubbleCollapsedHandler(*this,view,CollapsedBubble2);
        handlers->append(handler);
    }


    mDocumentHandlers.insert(view,handlers);
}

QGraphicsWidget* BubbleWidgetManager::container(
    View view,
    Container container) const
{
    QGraphicsWidget* w=0;

    if (mDocumentLoaders.contains(view)) {
        w = mDocumentLoaders.value(view)->findWidget(
            mContainerName[container]);
    }

    return w;
}


QGraphicsWidget* BubbleWidgetManager::widget(
    View view,
    Container container,
    Widget widget ) const
{
    QGraphicsWidget* w=0;

    if (mDocumentLoaders.contains(view)) {
        QString name;
        name.append(mWidgetPrefix.value(container));
        name.append(mWidgetName.value(widget));
        w = mDocumentLoaders.value(view)->findWidget(name);
    }

    return w;
}

QList<BubbleHandler*>* BubbleWidgetManager::handlers(
    QGraphicsWidget* widget) const
{
    int doc = mDocumentContent.key(widget);
    return mDocumentHandlers[doc];
}

void BubbleWidgetManager::handleOrientationChange(
    Qt::Orientation orientation)
{
    if (mOrientation!=orientation) {
        bool ok;
        QList<int> docs = mDocumentLoaders.keys();
        foreach (int doc, docs) {
            mDocumentLoaders[doc]->load(
                mFileNames[doc],mOrientationName.value(orientation),&ok);
            Q_ASSERT(ok);
        }
        mOrientation = orientation;
    }
}

QGraphicsWidget* BubbleWidgetManager::createParticipantListItem()
{
    return new BubbleParticipantListItem();
}

// custom widget loading
BubbleDocumentLoader::BubbleDocumentLoader(BubbleImageManager& imageManager)
    : mImageManager(imageManager)
{
}

QObject *BubbleDocumentLoader::createObject(
    const QString& type,
    const QString &name)
{
    if ( type == BubbleContainerWidget::staticMetaObject.className() ) {
        QObject *object = new BubbleContainerWidget();
        object->setObjectName(name);
        return object;
    }

    if ( type == BubbleImageWidget::staticMetaObject.className() ) {
        QObject *object = new BubbleImageWidget(mImageManager);
        object->setObjectName(name);
        return object;
    }

    if ( type == BubbleHeadingWidget::staticMetaObject.className() ) {
        QObject *object = new BubbleHeadingWidget();
        object->setObjectName(name);
        return object;
    }

    if ( type == BubbleButton::staticMetaObject.className() ) {
        QObject *object = new BubbleButton;
        object->setObjectName(name);
        return object;
    }

    return HbDocumentLoader::createObject(type, name);
}

