/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline methods for TMmsMsvEntry
*
*/



inline TBool TMmsMsvEntry::IsMobileTerminated() const
    {
    return iMtmData1 & KMmsMessageMobileTerminated; //lint !e737 never mind the sign
    }

inline void TMmsMsvEntry::SetMobileTerminated( TBool aParam ) 
    {
    iMtmData1 = ( iMtmData1 & ~KMmsMessageMobileTerminated ) | //lint !e737 never mind the sign
        ( aParam ? KMmsMessageMobileTerminated : 0x00 ); //lint !e713 nothing lost here
    }

inline TBool TMmsMsvEntry::IsForwardedMessage() const
    {
    return iMtmData1 & KMmsMessageForwarded; //lint !e737 never mind the sign
    }

inline void TMmsMsvEntry::SetForwardedMessage( TBool aParam ) 
    {
    iMtmData1 = ( iMtmData1 & ~KMmsMessageForwarded ) | //lint !e737 never mind the sign
                ( aParam ? KMmsMessageForwarded : 0x00 ); //lint !e713 nothing lost here
    }

inline TBool TMmsMsvEntry::IsMSendReq() const
    {
    return ( iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMSendReq; //lint !e737 never mind the sign
    }

inline TBool TMmsMsvEntry::IsMNotificationInd() const
    {
    return ( iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd; //lint !e737 never mind the sign
    }

inline TBool TMmsMsvEntry::IsMRetrieveConf() const
    {
    return ( iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMRetrieveConf; //lint !e737 never mind the sign
    }

inline TBool TMmsMsvEntry::IsMDeliveryInd() const
    {
    return ( iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageDeliveryInd; //lint !e737 never mind the sign
    }

inline void TMmsMsvEntry::SetMSendReq( )
    {
    iMtmData1 = ( iMtmData1 & ~KMmsMessageTypeMask ) | KMmsMessageMSendReq; //lint !e713 !e737 never mind the sign
    }

inline void TMmsMsvEntry::SetMNotificationInd( )
    {
    iMtmData1 = ( iMtmData1 & ~KMmsMessageTypeMask  ) | //lint !e737 never mind the sign
        KMmsMessageMNotificationInd; //lint !e713 nothing lost here
    }

inline void TMmsMsvEntry::SetMRetrieveConf( )
    {
    iMtmData1 = ( iMtmData1 & ~KMmsMessageTypeMask ) | KMmsMessageMRetrieveConf; //lint !e713 !e737 never mind the sign
    }

inline void TMmsMsvEntry::SetMDeliveryInd( ) 
    {
    iMtmData1 = ( iMtmData1 & ~KMmsMessageTypeMask ) | KMmsMessageDeliveryInd; //lint !e713 !e737 never mind the sign
    }

inline TBool TMmsMsvEntry::MNotifyRespIndSent() const
    {
    return iMtmData2 & KMmsNotifyResponseSent; //lint !e737 never mind the sign
    }

inline void TMmsMsvEntry::SetMNotifyRespIndSent( TBool aParam )
    {
    iMtmData2 = ( iMtmData2 & ~KMmsNotifyResponseSent ) | //lint !e737 never mind the sign
        ( aParam ? KMmsNotifyResponseSent : 0x00 ); //lint !e713 nothing lost here
    }

inline TBool TMmsMsvEntry::EditorOriented() const
    {
    return iMtmData1 & KMmsMessageEditorOriented; //lint !e737 never mind the sign
    }

inline void TMmsMsvEntry::SetEditorOriented( TBool aParam ) 
    {
    iMtmData1 = ( iMtmData1 & ~KMmsMessageEditorOriented ) |  //lint !e737 never mind the sign
        ( aParam ? KMmsMessageEditorOriented : 0x00 ); //lint !e713 nothing lost here
    }
                   
// End of File
