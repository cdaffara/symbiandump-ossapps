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
* Description:  Container for call data.
*
*/

#ifndef BUBBLEHEADER_H_
#define BUBBLEHEADER_H_

#include "bubblemanager2.h"

class HbAction;

class BubbleHeader
    {
public:
    BubbleHeader();
    virtual ~BubbleHeader();
    
public:
    /**
    * To put everything in initial state.
    */
    virtual void reset();
    
    // used
    void setIsUsed( bool used );
    bool isUsed() const;
    
    // bubble id
    void setBubbleId( int bubbleId );
    int bubbleId() const;

    // call state
    void setCallState( BubbleManager::PhoneCallState state );
    BubbleManager::PhoneCallState callState() const;
    
    // call flags
    void setCallFlags( int flags ); 
    void setCallFlag( BubbleManager::PhoneCallFlags flag );
    void removeCallFlag( BubbleManager::PhoneCallFlags flag );
    int callFlags() const;
    
    // number type
    void setNumberType( BubbleManager::PhoneNumberType numberType );
    BubbleManager::PhoneNumberType numberType() const;
    
    // text "calling"
    void setText( 
        const QString& text,
        Qt::TextElideMode clip );
    const QString& text() const;
    Qt::TextElideMode textClipDirection() const;

    // call index or name
    void setCli( 
        const QString& cli, 
        Qt::TextElideMode clip );
    const QString& cli() const;
    Qt::TextElideMode cliClipDirection() const;
    
    // number
    void setSecondaryCli( 
        const QString& cli, 
        Qt::TextElideMode clip );
    const QString& secondaryCli() const;
    Qt::TextElideMode secondaryCliClipDirection() const;
    
    // timer or cost
    void setTimerCost( 
            const QString& timerCost );
    const QString& timerCost() const;
    
    // caller image
    void setCallImage( const QString& image );
    const QString& callImage() const;

    // default avatar
    void setShowDefaultAvatar(bool show);
    bool showDefaultAvatar() const;
    
    // actions
    void addAction( HbAction* action );
    const QList<HbAction*>& actions() const; 
    void clearActions();
    
    // header type
    virtual bool isConference() const;

    // conference participation
    void setInConference( bool inConference );
    bool isInConference() const;

    // expand action
    void setExpandAction( HbAction* action );
    HbAction* expandAction() const;
    
private: //data
    
    bool mIsUsed;
    int mBubbleId;
    BubbleManager::PhoneCallState mCallState;
    BubbleManager::PhoneNumberType mNumberType;
    int mCallFlags; 
    QString mText;
    Qt::TextElideMode mTextClip;
    QString mCli;
    Qt::TextElideMode mCliClip;
    QString mSecondaryCli;
    Qt::TextElideMode mSecondaryCliClip;
    QString mTimerCost;
    QString mCallImage;
    QList<HbAction*> mActions;
    HbAction* mExpandAction;
    bool mIsInConference;
    bool mShowDefaultAvatar;
    };

#endif /* BUBBLEHEADER_H_ */
