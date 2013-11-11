/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSTMZOOMGESTURERECOGNISER_H_
#define QSTMZOOMGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{

/*!
 * QStm_ZoomGestureRecogniser handles zoomin gesture: start from near lower left / upper right corner and drag
 * This is useful if one wants to implement single handed zooming so can be done using thumb
 * (no need to use pinch i.e. hold device in one hand and use two fingers of the other)
 * Current spec is:
 * - touch lower left corner and start dragging => initialize ZOOM IN
 * - touch upper right corner and start dragging => initialize ZOOM OUT
 * store the touch point and then calculate the distance until release
 * The distance is the zoom factor (probably needs some adjustment, not just the pixels...)
 */
class QStm_ZoomGestureRecogniser : public QStm_GestureRecogniser
{
public:
	static const QStm_GestureUid KUid = EGestureUidCornerZoom;

	virtual ~QStm_ZoomGestureRecogniser();

	virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
	virtual void release(QStm_GestureEngineIf* ge) ;
	virtual QStm_GestureUid gestureUid() const { return KUid; }

	/*!
	 * Additional methods to set up zoom in gesture recogniser:
	 * define the rectangle where the lower left corner is
	 * \param theArea
	 */
	void setArea(const QRect& theArea) ;

	/*!
	 * Additional methods to set up zoom in gesture recogniser:
	 * Define how close to the lower left corner the touch must happen
	 * \param rangeInPixels
	 */
	void setRange(int rangeInMm) ;

	QStm_ZoomGestureRecogniser(QStm_GestureListenerIf* listener) ;

	float calculateDistance(const QPoint& aTp) ;
	int adjustZoom(float& previousDistance, float newDistance) ;

private:
	QRect         m_area ;
	int           m_rangesizeInPixels ;
	QStm_ZoomType m_zoomtype ;
	bool          m_zooming ;
	QPoint        m_startingtouch ;
	QPoint        m_previoustouch ;
	int           m_delta ;

};

} // namespace

#endif /* QSTMZOOMGESTURERECOGNISER_H_ */
