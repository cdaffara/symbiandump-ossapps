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
* Description:  Homescreen widget plugin
*
*/
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <QPointF>
#include <HbDocumentLoader>
#include <HbWidget>
#include <QStringList>

#include "contentlayoutgesture.h"
#include "calendarwidgetdebug.h"

const int KMethodIndex = 2;

/*
 * ContentLayoutGesture::ContentLayoutGesture
 */
ContentLayoutGesture::ContentLayoutGesture(HbWidget* widget, QList<QString> parameters, QString& layout, QString& gestureType, QString& action, QObject* parent) : QObject(parent)
{
    LOGS("ContentLayoutGesture::ContentLayoutGesture");
    this->setObjectName(layout);
    assignParameters(parameters, gestureType, action);
    mLayout = widget;
    mTestID = 0;
}

/*
 * ContentLayoutGesture::~ContentLayoutGesture
 */
ContentLayoutGesture::~ContentLayoutGesture()
{
    LOGS("ContentLayoutGesture::~ContentLayoutGesture");
}

/*
 * ContentLayoutGesture::assignParameter
 */
void ContentLayoutGesture::assignParameters(QList<QString> parameters, QString& gestureType, QString& action)
{
    LOGS("ContentLayoutGesture::assignParameter");
    if (gestureType == "onTap") {
        mTapParameters = parameters;
        mTapAction = action;
        if (mTapAction == "launchApplication") {
            //Parse the method and extract parameters type 
            parseParameters(gestureType);
        }
    }
    
    if (gestureType == "onSwipe") {
        mSwipeParameters = parameters;
        mSwipeAction = action;
    }
    
    if (gestureType == "onHighlight") {
        mHighlightParameters = parameters;
        mHighlightAction = action;
    }
    
    if (gestureType == "onPinch") {
        mPinchParameters = parameters;
        mPinchAction = action;
    }
    
    if (gestureType == "onPan") {
        mPanParameters = parameters;
        mPanAction = action;
        }
}

/*
 * ContentLayoutGesture::parseParameters()
 */
void ContentLayoutGesture::parseParameters( QString& gestureType )
{
	LOGS( "ContentLayoutGesture::parseParameters" );
	if (gestureType == "onTap") {
        int parameterStartIndex = 0;
        int parameterEndIndex = 0;
        bool parameterFound = false;
        int i = 0;
        while ((mTapParameters.at(KMethodIndex).at(i) != '(') && (i < mTapParameters.at(
            KMethodIndex).length())) {
            i++;
        }
        parameterStartIndex = i + 1;
        for (;i < mTapParameters.at(KMethodIndex).length(); i++) {
            if (mTapParameters.at(KMethodIndex).at(i) == ',' || mTapParameters.at(KMethodIndex).at(
                i) == ')') {
                parameterEndIndex = i;
                parameterFound = true;
            }

            if (parameterFound) {
                QString parameterType = mTapParameters.at(KMethodIndex).mid(parameterStartIndex,
                    parameterEndIndex - parameterStartIndex);
                mTapParametersType.append(parameterType);
                //check the next parameter type from the next cycle
                parameterStartIndex = i + 1;
                parameterFound = false;
            }
        }
    }
}

/*
 * ContentLayoutGesture::addGesture()
 */
void ContentLayoutGesture::addGesture(QList<QString> parameters, QString& gestureType, QString& action)
{
    LOGS("ContentLayoutGesture::addGesture");
    assignParameters(parameters, gestureType, action);
}

/*
 * ContentLayoutGesture::onTap
 */
void ContentLayoutGesture::onTap(QPointF& point)
{
    LOGS("ContentLayoutGesture::onTap");
    //traslate the container rect to right position according to the scene
    QRectF layoutRect( QPointF( mLayout->rect().topLeft().x() + mLayout->pos().x(), mLayout->rect().topLeft().y() + mLayout->pos().y() ), 
                       QPointF( mLayout->rect().bottomRight().x() + mLayout->pos().x(), mLayout->rect().bottomRight().y() + mLayout->pos().y() ));
	
    if ( layoutRect.contains(point) ) {
        if (mTapAction == "launchApplication") {
            launchApplication(mTapParameters, mTapParametersType);
            mTestID = 1;
        }
    }
}

/*
 * ContentLayoutGesture::launchApplication
 */
void ContentLayoutGesture::launchApplication( QList<QString> parameters, QList<QString> parametersType )
{
    LOGS("ContentLayoutGesture::launchApplication");
    
    QString service = parameters.at(0);
    QString interface = parameters.at(1);
    QString method = parameters.at(2);

    XQApplicationManager aiwMgr;
    //XQAiwRequest request = aiwMgr.create(url);
    XQAiwRequest* request = aiwMgr.create(service, interface, method, false);
    if (request == NULL) {
        return;
    }
    
    XQRequestInfo options;
    options.setForeground(true);
    request->setInfo(options);

    // Connect result handling signal
    connect(request, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
    // Connect error handling signal or apply lastError function instead.
    connect(request, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));

    QList<QVariant> args;
    
	for (int i = 3; i < parameters.count(); i++) {
        if (parameters.at(i) == "CURRENTDATE") {
            args << mDate;
        }
        else {
            if (parametersType.at(i - KMethodIndex - 1) == "int") {
                int par = parameters.at(i).toInt();
                args << par;
            }
        }
    }

    // Set function parameters
    request->setArguments(args);

    // Send the request
    bool res = request->send();
    if (!res) {
        // Request failed.
        int error = request->lastError();

        // Handle error
    }

    // All done.
    delete request;
}

/*
 * ContentLayoutGesture::handleOk()
 */
void ContentLayoutGesture::handleOk(const QVariant& var)
{
    Q_UNUSED(var);
    LOGS("DateIconGesture::handleOk");
    mTestID = 0;
    emit requestComplete();
}

/*
 * ContentLayoutGesture::handleError()
 */
void ContentLayoutGesture::handleError(int err, const QString& str)
{
    Q_UNUSED(str);
    Q_UNUSED(err);
    LOGS("DateIconGesture::handleError");
    mTestID = err;
    emit requestError();
}


/*
 * ContentLayoutGesture::updateDate()
 */
void ContentLayoutGesture::updateDate(QDateTime& date)
{
    LOGS("ContentLayoutGesture::updateDate");
    mDate = date;
}

//TEST FUNCTIONS

/*
 * ContentLayoutHandler::testId()
 */
int ContentLayoutGesture::testId()
{
    return mTestID;
}

/*
 * ContentLayoutHandler::setTestId()
 */
void ContentLayoutGesture::setTestId(int testID)
{
    mTestID = testID;
}

//END OF FILE

