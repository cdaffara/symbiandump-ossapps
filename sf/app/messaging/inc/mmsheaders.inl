/*
* Copyright (c) 2002 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline methods for CMmsHeaders
*
*/




inline TInt CMmsHeaders::MessageClass() const
   {
   return iMsgClass;
   }

inline void CMmsHeaders::SetMessageClass( TInt aClass )
   {
   iMsgClass = aClass;
   }

inline void CMmsHeaders::SetMessagePriority( TInt aPriority )
   {
   iPriority = aPriority;
   }

inline TInt CMmsHeaders::MessagePriority() const
   {
   return iPriority;
   }

inline void CMmsHeaders::SetSenderVisibility( TInt aVisibility )
   {
   iSenderVisi = aVisibility;
   }

inline TInt CMmsHeaders::SenderVisibility()
   {
   return iSenderVisi;
   }

inline void CMmsHeaders::SetDeliveryReport( TInt aRequest )
   {
   iDelivReport = aRequest;
   }

inline TInt CMmsHeaders::DeliveryReport()
   {
   return iDelivReport;
   }

inline void CMmsHeaders::SetReadReply( TInt aRequest )
   {
   iReadReply = aRequest;
   }

inline TInt CMmsHeaders::ReadReply()
   {
   return iReadReply;
   }

inline void CMmsHeaders::SetMessageType( TInt aType )
   {
   iMsgType = aType;
   }

inline TInt CMmsHeaders::MessageType()
   {
   return iMsgType;
   }

inline void CMmsHeaders::SetMmsVersion( TInt16 aVersion )
   {
   // Only one byte version number supported
   iVersion = aVersion & 0x00FF;
   }

inline TInt16 CMmsHeaders::MmsVersion()
   {
   return iVersion;
   }

inline void CMmsHeaders::SetDate( TInt64 aTime )
   {
   iDate = aTime;
   }

inline TInt64 CMmsHeaders::Date()
   {
   return iDate;
   }

inline const CDesCArray& CMmsHeaders::ToRecipients () const 
    {
    return *iToArray;
    }

inline const CDesCArray& CMmsHeaders::CcRecipients () const
    {
    return *iCcArray;
    }
    
inline const CDesCArray& CMmsHeaders::BccRecipients() const
    {
    return *iBccArray;
    }

inline void CMmsHeaders::SetMessageSize( TInt aSize )
    {
    iMessageSize = aSize; //lint !e732 nothing lost here
    }

inline TInt CMmsHeaders::MessageSize() const
    {
    return iMessageSize; //lint !e713 nothing lost here
    }

inline TInt32 CMmsHeaders::ReportAllowed() const
    {
    return iReportAllowed;
    }

inline void CMmsHeaders::SetReportAllowed( TInt32 aValue )
    {
    iReportAllowed = aValue;
    }

inline TInt32 CMmsHeaders::ResponseStatus() const
    {
    return iResponseStatus;
    }

inline void CMmsHeaders::SetResponseStatus( TInt32 aValue )
    {
    iResponseStatus = aValue;
    }
    
inline void CMmsHeaders::SetMessageRoot( const TMsvAttachmentId aId )
    {
    iStart = aId;
    }

inline TMsvAttachmentId CMmsHeaders::MessageRoot( ) const
    {
    return iStart;
    }

inline void CMmsHeaders::SetStatus( const TInt aStatus )
    {
    iStatus = aStatus;
    }

inline TInt CMmsHeaders::Status() const
    {
    return iStatus;
    }
    
inline void CMmsHeaders::SetReadStatus( const TInt aValue )
    {
    iReadStatus = aValue;
    }

inline TInt CMmsHeaders::ReadStatus() const
    {
    return iReadStatus;
    }

inline void CMmsHeaders::SetReplyCharging( const TInt aReplyCharging )
    {
    iReplyCharging = aReplyCharging;
    }

inline TInt CMmsHeaders::ReplyCharging() const
    {
    return iReplyCharging;
    }

inline void CMmsHeaders::SetReplyChargingSize( const TInt aReplyChargingSize )
    {
    iReplyChargingSize = aReplyChargingSize;
    }

inline TInt CMmsHeaders::ReplyChargingSize() const
    {
    return iReplyChargingSize;
    }

inline const RPointerArray<CMmsSendingChain>& CMmsHeaders::PreviouslySentList()
    {
    return iPreviouslySentArray;
    }

inline TInt CMmsHeaders::DistributionIndicator() const
    {
    return iDistributionIndicator;
    }

inline void CMmsHeaders::SetDistributionIndicator( const TInt aDistributionIndicator )
    {
    iDistributionIndicator = aDistributionIndicator;
    }

inline TMsvId CMmsHeaders::RelatedEntry() const
    {
    return iRelatedEntry;
    }

inline void CMmsHeaders::SetRelatedEntry( const TMsvId aRelatedEntry )
    {
    iRelatedEntry = aRelatedEntry;
    }

inline TInt CMmsHeaders::MultipartType() const
    {
    return iMultipartType;
    }

inline void CMmsHeaders::SetMultipartType( const TInt aMultipartType )
    {
    iMultipartType = aMultipartType;
    }

inline const CMmsElementDescriptor* CMmsHeaders::ReadOnlyElementDescriptor() const
    {
    return iElementDescriptor;
    }

inline const CMmsMMBoxMessageHeaders* CMmsHeaders::ReadOnlyMMBoxMessageHeaders() const
    {
    return iMmBoxMessageHeaders;
    }

inline const CMmsMMBoxViewHeaders* CMmsHeaders::ReadOnlyMMBoxViewHeaders() const
    {
    return iMmBoxViewHeaders;
    }

inline void CMmsHeaders::SetMessageComplete( TInt aExtensionStatus )
    {
    iExtensionStatus = aExtensionStatus;
    }

inline TInt CMmsHeaders::MessageComplete()
    {
    return iExtensionStatus;
    }

inline const RPointerArray<CMmsDeleteResultArray>& CMmsHeaders::DeleteResultArray()
    {
    return iDeleteResultArray;
    }
    
inline void CMmsHeaders::SetReceivingTime( TTime aTime )
    {
    iReceivingTime = aTime;
    }
    
inline TTime CMmsHeaders::ReceivingTime() const
    {
    return iReceivingTime;
    }
    
inline TInt CMmsHeaders::ContentClass() const
    {
    return iContentClass;
    }
    
inline void CMmsHeaders::SetContentClass( TInt aContentClass )
    {
    iContentClass = aContentClass;
    }
    
inline TInt CMmsHeaders::DrmContent() const
    {
    return iDrmContent;
    }
    
inline void CMmsHeaders::SetDrmContent( TInt aDrmContent )
    {
    iDrmContent = aDrmContent;
    }
    
inline void CMmsHeaders::SetAdaptationAllowed( TInt aAdaptationAllowed )
    {
    iAdaptationAllowed = aAdaptationAllowed;
    }
    
inline TInt CMmsHeaders::AdaptationAllowed() const
    {
    return iAdaptationAllowed;
    }
    
inline void CMmsHeaders::SetRecommendedRetrievalMode( TInt aRetrievalMode )
    {
    iRecommendedRetrievalMode = aRetrievalMode;
    }
    
inline TInt CMmsHeaders::RecommendedRetrievalMode() const
    {
    return iRecommendedRetrievalMode;
    }
    
inline void CMmsHeaders::SetCancelStatus( TInt aCancelStatus )
    {
    iCancelStatus = aCancelStatus;
    }
    
inline TInt CMmsHeaders::CancelStatus() const
    {
    return iCancelStatus;
    }
    
   
// End of File
