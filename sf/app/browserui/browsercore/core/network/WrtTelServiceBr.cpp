/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <QObject>
#include <QStringList>
#include <QMessageBox>
#include <QChar>
#include <eikenv.h> // CCoeEnv::Static()


#include <WrtTelService.rsg>
#include <data_caging_path_literals.hrh>
_LIT( KResourceFileName, "wrttelservice.rsc" );

#include "WrtTelServiceBr.h"
#include "WrtTelServiceBr_p.h"

static const QString VALIDDTMFCHARS = "wW/pP*#0123456789ABCDabcd," ;
static const QString DTMFWAITCHARS = "/wWpP,*#" ;
static const QString DTMFINVALIDWAITCHARS = "/," ;
static const QString POSTD = ";postd=";


// ================= MEMBER FUNCTIONS =======================


WrtTelService::WrtTelService( ): QObject()
    ,d(new WrtTelServicePrivate())
{
}

void WrtTelService::MakeVOIPCall( QString number, bool sendDTMFAlwaysConfirm )
{
    d->MakeCall( number, sendDTMFAlwaysConfirm, true );
}

void WrtTelService::MakeCall( QString orgNumber, bool sendDTMFAlwaysConfirm )
{
    d->MakeCall( orgNumber, sendDTMFAlwaysConfirm, false);
}

WrtTelService::~WrtTelService()
{
    delete d;
}

// ================= WrtTelServicePrivate FUNCTIONS =======================

WrtTelServicePrivate::WrtTelServicePrivate( ) : QObject(0)
    ,m_CallInProgress( false )
    ,m_ResourceLoader( *CCoeEnv::Static() )
    ,m_ResourceOpened( false )
    ,m_ServiceHandler( NULL )
#if !defined (SYMBIAN_PUB_SDK)
    ,m_DialData( NULL )
#endif
{
}

WrtTelServicePrivate::~WrtTelServicePrivate()
{
    delete m_ServiceHandler;
#if !defined (SYMBIAN_PUB_SDK)
    delete m_DialData;
#endif
    if ( m_ResourceOpened ) {
        m_ResourceLoader.Close();
        m_ResourceOpened = false;
    }
}

    
//---------------------------------------------------------------------------------------
// WrtTelService::MakeCall()
//---------------------------------------------------------------------------------------
//
void WrtTelServicePrivate::MakeCall( QString orgNumber, bool sendDTMFAlwaysConfirm, bool voipCall )
{
#if !defined (SYMBIAN_PUB_SDK)
    if( m_CallInProgress || orgNumber.length() == 0 ||
        orgNumber.length() > AIWDialDataExt::KMaximumPhoneNumberLength )
        return;

    // Handle postd format  
    QString postdParm = FindPostdParm( orgNumber );
    
    if(voipCall)
        FindAndRipVoipParameters( orgNumber );
    
    // Rip URI parameters from Tel URI, in order to that
    // number validation and DTMF works as earlier
    // Parameters will be added back in later phase
    QString params = FindAndRipURIParameters( orgNumber );

    QString number = ValidateNumber( orgNumber );
    
    QString dtmfNumber = FindAndRipDTMFSequence( number );

    if ( dtmfNumber.isEmpty() )
        dtmfNumber = postdParm;
        
    bool isPostd = (!dtmfNumber.isEmpty());

    m_CallInProgress = true;
    if( QMessageBox::question( 0, QString(), QObject::tr("Call %1").arg(number), (QMessageBox::Yes | QMessageBox::No) ) == QMessageBox::Yes ) {            
        if(! dtmfNumber.isEmpty() ) {
            // Confirm the user (s)he wants to send the DTMF
            if( !sendDTMFAlwaysConfirm || QMessageBox::question( 0, QString(), QObject::tr("Send DTMF %1").arg(dtmfNumber), (QMessageBox::Yes | QMessageBox::No) ) == QMessageBox::Yes ) {
                if ( isPostd )
                    number.append("p");
                number.append(dtmfNumber);
            }
         }

        // ... and make the call
        TRAP_IGNORE(DoMakeCallL( number, voipCall ));
    }
    m_CallInProgress = false;
#endif
}

//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::DoMakeCallL()
//---------------------------------------------------------------------------------------
//
void WrtTelServicePrivate::DoMakeCallL( QString number, bool voipCall )
{
    
#if !defined (SYMBIAN_PUB_SDK)
    if (!m_ServiceHandler)
        m_ServiceHandler = CAiwServiceHandler::NewL();
    if (!m_DialData)
        m_DialData = CAiwDialData::NewL();

    if (!m_ResourceOpened) {
        // Resouce file loading
        TParse* fileParser = new (ELeave) TParse;
        CleanupStack::PushL( fileParser );
        fileParser->Set( KResourceFileName, &KDC_APP_RESOURCE_DIR, NULL ); 
        m_ResourceFileName = fileParser->FullName();
        m_ResourceLoader.OpenL( m_ResourceFileName );
        m_ResourceOpened = ETrue;
        CleanupStack::PopAndDestroy( fileParser ); // fileParser
        
        m_ServiceHandler->AttachL( R_WRTTELSERVICES_MENU_AIW_INTERESTS );
        
    } 

    CAiwGenericParamList& paramList = m_ServiceHandler->InParamListL();
    CAiwGenericParamList& output = m_ServiceHandler->OutParamListL();

    HBufC* numberPtr = qt_QString2HBufCNewL( number );
    CleanupStack::PushL( numberPtr );
    m_DialData->SetPhoneNumberL( *numberPtr );
    CleanupStack::PopAndDestroy( ); // numberPtr
    
    if( !voipCall )
        m_DialData->SetCallType( CAiwDialData::EAIWVideo );
    else
        m_DialData->SetCallType( CAiwDialData::EAIWVoiP );

    m_DialData->FillInParamListL( paramList );
    
    
    m_ServiceHandler->ExecuteServiceCmdL(
                    KAiwCmdCall,
                    paramList,
                    output,
                    0,      // No options used.
                    0 ); //

#endif
    
}

//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::ValidateNumber()
//---------------------------------------------------------------------------------------
//
QString WrtTelServicePrivate::ValidateNumber( const QString number )
{
    QString newNumber;
    bool first = true;
    foreach(QChar currentChar, number )
        {
        if( currentChar.isDigit() )
            newNumber.append(currentChar);
        else {
            char ch = currentChar.toUpper().toLatin1();
            switch( ch ) {

                case '+':
                    if( !first )
                        return QString();
                    else
                        newNumber.append(ch);
                    break;
                case '*':
                case '#':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'P':
                case 'W':
                case ',':
                case '/':
                    if( first )
                        return QString();
                    else
                        newNumber.append(ch);
                    break;
               case '.': 
               case '-':
               case '(':
               case ')':
               case ' ':
                    break; 
               default:
                    return QString();
               }
            }
        first = false;
        }
    return newNumber;
}

//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::FindPostdParm()
//---------------------------------------------------------------------------------------
//
QString WrtTelServicePrivate::FindPostdParm( QString& number )
{
    QString tmpStr;
    int offset = number.indexOf( POSTD );
    if ( offset > 0 ) {
        tmpStr = number.mid( offset + POSTD.length() );
        number = number.left( offset );
    }
    return tmpStr;
}


//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::FindAndRipDTMFSequence()
//---------------------------------------------------------------------------------------
//
QString WrtTelServicePrivate::FindAndRipDTMFSequence( QString& number )
{
    QString tmpStr;
    int i = -1;
    // Look for the first DTMF-wait char and extract the
    // dtmf string from the phonenumber
    foreach( QChar ch, number ) {
        i++;
        if (DTMFWAITCHARS.indexOf(ch) > 0) {
            if (ch == '#' || ch == '*') // these are included
                tmpStr = number.mid(i);
            else
                tmpStr = number.mid(i+1);
            number = number.left(i);

            // Remove all the invalid wait chars from 
            // the sequence which is not accepted by CallUi
            foreach(QChar c, DTMFINVALIDWAITCHARS)
                tmpStr.remove(c);

            break;
            }
        }
    return tmpStr;    
}

//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::FindAndRipURIParameters()
//---------------------------------------------------------------------------------------
//
QString WrtTelServicePrivate::FindAndRipURIParameters( QString& number )
{
    QString tmpStr;
    int offset = number.indexOf(';');
    if( offset > 0 ) { // Separator is not allowed to be a first char
        tmpStr = number.mid( offset );
        number = number.left( offset );
    }
    return tmpStr;       
}

//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::FindAndRipVoipParameters()
//---------------------------------------------------------------------------------------
//
QString WrtTelServicePrivate::FindAndRipVoipParameters( QString& number )
{
    QString tmpStr;
    int offset = number.indexOf('@');
    if( offset > 0 ) { // Separator is not allowed to be a first char
        tmpStr = number.mid( offset );
        number = number.left( offset );
    }
    return tmpStr;
}

//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::ValidateDTMFNumber()
//---------------------------------------------------------------------------------------
//
void WrtTelServicePrivate::ValidateDTMFNumber( QString& number )
{
    QString orgNumber = number;
    foreach(QChar c, orgNumber)
        if (VALIDDTMFCHARS.indexOf(c) < 0)
              number.remove(c);
    
}

//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::HandleNotifyL()
//---------------------------------------------------------------------------------------
//
TInt WrtTelServicePrivate::HandleNotifyL( TInt /*aCmdId*/, TInt /*aEventId*/,
                                        CAiwGenericParamList& /*aEventParamList*/,
                                        const CAiwGenericParamList& /*aInParamList*/ )
{
    return KErrNone;
}

//---------------------------------------------------------------------------------------
// WrtTelServicePrivate::qt_QString2HBufCNewL()
//     copied from qcore_symbian_p.cpp
//---------------------------------------------------------------------------------------
//
HBufC* WrtTelServicePrivate::qt_QString2HBufCNewL(const QString& aString)
{
    HBufC *buffer;
#ifdef QT_NO_UNICODE
    TPtrC8 ptr(reinterpret_cast<const TUint8*>(aString.toLocal8Bit().constData()));
    buffer = HBufC8::NewL(ptr.Length());
    buffer->Des().Copy(ptr);
#else
    TPtrC16 ptr(reinterpret_cast<const TUint16*>(aString.utf16()));
    buffer = HBufC16::NewL(ptr.Length());
    buffer->Des().Copy(ptr);
#endif
    return buffer;
}


