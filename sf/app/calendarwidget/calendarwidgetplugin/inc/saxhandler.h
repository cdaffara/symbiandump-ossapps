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

#ifndef SAXHANDLER_H_
#define SAXHANDLER_H_

#include <qxml.h>
#include <qobject.h>

class HbDocumentLoader;

/*!
    \class SaxHandler
    \brief Reads and parses XML-based configuration for the calendar widget.
*/
class SaxHandler: public QXmlDefaultHandler
{
public:
    /*!
        \fn SaxHandler::SaxHandler(QObject* caller, HbDocumentLoader &loader)

        Constructor of the SaxHandler class.
        @param caller The calling object
        @param loader Orbit document loader
    */
    SaxHandler(QObject* caller, HbDocumentLoader &loader);
    /*!
        \fn SaxHandler::~SaxHandler()

        Destructor of the SaxHandler class.
    */
    ~SaxHandler();
    //From QXmlDefaultHandler
    /*!
        \fn bool SaxHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName,
        const QXmlAttributes &attributes)

        @param namespaceURI
        @param localName
        @param qName
        @param attributes
    */
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName,
        const QXmlAttributes &attributes);
    /*!
        \fn bool SaxHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)

        @param namespaceURI
        @param localName
        @param qName
    */
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    /*!
        \fn bool SaxHandler::characters(const QString &str)

        @param str
    */
    bool characters(const QString &str);
    /*!
        \fn bool SaxHandler::fatalError(const QXmlParseException &exception)

        @param exception
        @return 
    */
    bool fatalError(const QXmlParseException &exception);

private:
    /*!
        \fn void SaxHandler::connectObject(QObject* object)

        @param object
        @return 
    */    
    void connectObject(QObject* object);
    /*!
        \fn QString SaxHandler::createGestureSignal()

        @return 
    */    
    QString createGestureSignal();
    /*!
        \fn QString SaxHandler::createGestureSlot()

        @return 
    */    
    QString createGestureSlot();

private:
    QObject* mCaller;
    QString mGestureType;
    QString mLayout;
    QString mAction;
    QList<QString> mParameters;
    bool mArgumentValue;
    HbDocumentLoader& mLoader;
};


#endif /* SAXHANDLER_H_ */
