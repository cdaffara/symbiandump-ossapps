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
* Description: Calendar widget XML reader/parser
*
*/
#include <QObject>
#include <QMetaMethod>
#include <HbDocumentLoader>
#include <HbWidget>

#include "saxhandler.h"
#include "calendarwidgetdebug.h"
#include "contentlayoutgesture.h"

SaxHandler::SaxHandler(QObject* caller, HbDocumentLoader &loader) : mLoader(loader)
{
    LOGS("SaxHandler::SaxHandler");
    mCaller = caller;
    mArgumentValue = false;
}

SaxHandler::~SaxHandler()
{
    LOGS("SaxHandler::~SaxHandler");
}

bool SaxHandler::startElement(const QString &namespaceURI,
    const QString &localName,
    const QString &qName,
    const QXmlAttributes &attributes)
{
    LOGS("SaxHandler::startElement");
    qDebug() << "namespaceURI: " << namespaceURI << " localName: " << localName << " qName: "
        << qName;
    
    for (int i = 0; i < attributes.length(); i++) {
        qDebug() << "attribute type: " << attributes.type(i) << " attribute qname: "
            << attributes.qName(i) << "attribute localname: " << attributes.localName(i)
            << " attribute value: " << attributes.value(i);
        
        if (attributes.qName(i) == "type") {
            mGestureType = attributes.value(i);
            mArgumentValue = false;
        }
        
        if (attributes.qName(i) == "layout") {
            mLayout = attributes.value(i);
            mArgumentValue = false;
        }
        
        if (attributes.qName(i) == "action") {
            mAction = attributes.value(i);
        }
        
        if (localName == "argument") {
            mArgumentValue = true;
        }
        
        if (localName == "parameter") {
            mParameters.append(attributes.value(i));
            mArgumentValue = false;
        }
    }
    
    return true;
}
    
bool SaxHandler::endElement(const QString &namespaceURI,
    const QString &localName,
    const QString &qName)
{
    LOGS("HsSaxHandler::endElement");
    qDebug() << "namespaceURI: " << namespaceURI << " localName: " << localName << " qName: "
        << qName;
    if (localName == "gesture") {
        QObject* gesture = mCaller->findChild<QObject*>(mLayout);
        if (!gesture) {
            HbWidget* layout = qobject_cast<HbWidget*> (mLoader.findWidget(mLayout));
            gesture = new ContentLayoutGesture(layout, mParameters, mLayout, mGestureType, mAction,
                mCaller);
        }
        else {
            int methodIndex = gesture->metaObject()-> indexOfSlot(
                "addGesture(QList<QString>,QString&,QString&)");
            if (methodIndex >= 0) {
                gesture->metaObject()->method(methodIndex).invoke(gesture,
                    Q_ARG(QList<QString>, mParameters), Q_ARG(QString, mGestureType),
                    Q_ARG(QString, mAction));
            }
        }

        connectObject(gesture);
        
        //remove parameters
        mParameters.clear();
    }
    return true;
}
    
bool SaxHandler::characters(const QString &str)
{
    LOGS("HsSaxHandler::characters");
    qDebug() << "characters: " << str;
    if (mArgumentValue) {
        mParameters.append(str);
        mArgumentValue = false;
        // set mGestureAction
    }
    return true;
}
    
bool SaxHandler::fatalError(const QXmlParseException &exception)
{
    Q_UNUSED(exception);
    LOGS("HsSaxHandler::fatalError");
    return true;
}

void SaxHandler::connectObject(QObject* object)
{
    LOGS("SaxHandler::connectObject");
    if (mGestureType == "onTap") {
        QObject::connect(mCaller, SIGNAL(tapGesture(QPointF&)), object, SLOT(onTap(QPointF&)));
        //connect contentlayouthandler to this gesture object to update the current date
        QObject* contentLayoutHandler = mCaller->findChild<QObject*> ("contentlayouthandler");
        if (contentLayoutHandler) {
            QObject::connect(contentLayoutHandler, SIGNAL(currentDate(QDateTime&)), object,
                SLOT(updateDate(QDateTime&)));
        }
    }
    
    if (mGestureType == "onSwipe") {
        //connect(mCaller, SIGNAL(swipeGesture()), object, SLOT(onSwipe));
    }
    
    //complete with all supported gestures
}
