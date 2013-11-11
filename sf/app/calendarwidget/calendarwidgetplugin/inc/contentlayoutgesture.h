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
* Description: Content layout gesture class
*
*/

#ifndef CONTENTLAYOUTGESTURE_H_
#define CONTENTLAYOUTGESTURE_H_

#include <QObject>
#include <QRectF>
#include <QVector>
#include <QDateTime>

class QPointF;
class HbDocumentLoader;
class HbWidget;
class TestContentLayoutHandler;

/*!
    \class ContentLayoutGesture
    \brief Handles gestures for content layout.
*/

// Class declaration
class ContentLayoutGesture: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int testID READ testId WRITE setTestId)

public:
    /*!
        \fn ContentLayoutGesture::ContentLayoutGesture(HbWidget* widget, QList<QString> parameters, QString& layout, QString& gestureType, QString& action, QObject* parent = 0)

        Constructor of the ContentLayoutGesture class.
        @param widget
        @param parameters
        @param layout
        @param gestureType
        @param action
        @param parent
    */
    ContentLayoutGesture(HbWidget* widget, QList<QString> parameters, QString& layout, QString& gestureType, QString& action, QObject* parent = 0);
    /*!
        \fn ContentLayoutGesture::~ContentLayoutGesture()

        Destructor of the ContentLayoutGesture class.
    */
    ~ContentLayoutGesture();

public slots:
    /*!
        \fn void ContentLayoutGesture::onTap(QPointF& point)

        @param point
    */
    void onTap(QPointF& point);
    /*!
        \fn void ContentLayoutGesture::handleOk(const QVariant& var)

        @param var
    */
    void handleOk(const QVariant& var);
    /*!
        \fn void ContentLayoutGesture::handleError(int err, const QString& str)

        @param err
        @param str
    */
    void handleError(int err, const QString& str);
    /*!
        \fn void ContentLayoutGesture::updateDate(QDateTime& date)

        @param date
    */
    void updateDate(QDateTime& date);
    /*!
        \fn void ContentLayoutGesture::addGesture(QList<QString> parameters, QString& gestureType, QString& action)

        @param parameters
        @param gestureType
        @param action
    */
    void addGesture(QList<QString> parameters, QString& gestureType, QString& action);

signals:
    /*!
        \fn void ContentLayoutGesture::requestError()
    */
    void requestError();
    /*!
        \fn void ContentLayoutGesture::requestComplete()
    */
    void requestComplete();
    
private:
    /*!
        \fn void ContentLayoutGesture::assignParameters(QList<QString> parameters, QString& gestureType, QString& action)

        @param parameters
        @param gestureType
        @param action
    */
    void assignParameters(QList<QString> parameters, QString& gestureType, QString& action);
    /*!
        \fn void ContentLayoutGesture::launchApplication(QList<QString> parameters)

        @param parameters
    */
    void launchApplication(QList<QString> parameters, QList<QString> parametersType);
	
    /*!
        \fn void ContentLayoutGesture::parseParameters( QString& gestureType )

        @param gestureType
    */
	void parseParameters( QString& gestureType );
	
	//Test property functions
    /*! 
        \fn int ContentLayoutHandler::testId()
        
        Used to check which test needs to run.
        @return Test id
     */
    int testId();
    /*! 
        \fn void ContentLayoutHandler::setTestId(int testID)
        
        Used to set which test it needs to run.
     */
    void setTestId(int testID);

private:
    QDateTime mDate;
    
    QList<QString> mTapParameters;
    QList<QString> mSwipeParameters;
    QList<QString> mPinchParameters;
    QList<QString> mPanParameters;
    QList<QString> mHighlightParameters;
    QList<QString> mTapParametersType;
    QList<QString> mSwipeParametersType;
    QList<QString> mPinchParametersType;
    QList<QString> mPanParametersType;
    QList<QString> mHighlightParametersType;
    HbWidget* mLayout;
    QString mTapAction;
    QString mSwipeAction;
    QString mPinchAction;
    QString mPanAction;
    QString mHighlightAction;
    
    //test variables
    friend class TestContentLayoutGesture;   
    int mTestID;
    int mTestResult;
};

#endif /* CONTENTLAYOUTGESTURE_H_ */
