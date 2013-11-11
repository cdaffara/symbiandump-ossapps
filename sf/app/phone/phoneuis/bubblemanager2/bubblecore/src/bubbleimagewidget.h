/*!
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
* Description:  Widget to display contact image.
*
*/

#ifndef BUBBLEIMAGEWIDGET_H_
#define BUBBLEIMAGEWIDGET_H_

#include <hbwidget.h>

class BubbleImageManager;
class HbIconItem;

class BubbleImageWidget : public HbWidget
    {
    Q_OBJECT

    Q_PROPERTY(bool keepSquareShape READ keepSquareShape WRITE setKeepSquareShape)
    Q_PROPERTY(QString imageName READ imageName WRITE setImageName)
    
public:
    explicit BubbleImageWidget(
        BubbleImageManager& imageManager,
        QGraphicsItem* parent = 0 );
    virtual ~BubbleImageWidget();

    void setImageName(const QString& name);
    QString imageName() const;

    bool keepSquareShape() const;
    void setKeepSquareShape(bool keepSquare);

    void displayEmergencyCallIcon();

protected slots:
    void imageLoaded(QString imageFileName);

protected:
    void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget);

    QSizeF sizeHint(
        Qt::SizeHint which,
        const QSizeF & constraint) const;

    void resizeEvent(QGraphicsSceneResizeEvent * event);

private:
    BubbleImageManager& mImageManager;
    QString  mImageName;
    HbIconItem* mDefaultAvatar;
    bool mKeepSquareShape;
    };

#endif /* BUBBLEIMAGEWIDGET_H_ */
