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
* Description:
*
*/

#ifndef LOGSENGDEFS_H
#define LOGSENGDEFS_H

#include <QString>


// CONSTANT LITERALS
const char logsEmergencyCall911[] = "911";   // Number in event considered to be emergency call 
const char logsEmergencyCall[] = "112";      // Number in event considered to be emergency call (in CDMA 
                                        // KLogsDataFldTag_Emergency is used instead 
//icons ids
const char logsVoiceCallIconId[] = "qtg_large_voice_call";
const char logsVideoCallIconId[] = "qtg_large_video_call";
const char logsVoipCallIconId[] = "qtg_large_voip";

const char logsRemotePartyInfoIconId[] = "qtg_large_info";
const char logsCallDurationIconId[] = "qtg_large_call_duration";
const char logsCallDateAndTimeIconId[] = "qtg_large_calendar";
const char logsThumbUnknownId[] = "qtg_large_avatar";

const char logsDialledVoiceCallIconId[] = "qtg_large_dialled_voice_call";
const char logsMissedVoiceCallIconId[] = "qtg_large_missed_voice_call";
const char logsMissedVoiceCallUnseenIconId[] = "qtg_large_missed_voice_call_unseen";
const char logsReceivedVoiceCallIconId[] = "qtg_large_received_voice_call";

const char logsDialledVideoCallIconId[] = "qtg_large_video_dialled_call";
const char logsMissedVideoCallIconId[] = "qtg_large_video_missed_call";
const char logsMissedVideoCallUnseenIconId[] = "qtg_large_missed_video_call_unseen";
const char logsReceivedVideoCallIconId[] = "qtg_large_video_received_call";

const char logsDialledVoipCallIconId[] = "qtg_large_voip_dialled_call";
const char logsMissedVoipCallIconId[] = "qtg_large_voip_missed_call";
const char logsMissedVoipCallUnseenIconId[] = "qtg_large_missed_voip_call_unseen";
const char logsReceivedVoipCallIconId[] = "qtg_large_voip_received_call";


// CONSTANT NUMBERS

const int logsEventALS = 0x4;
const int logsDeleteCountInit = -1;

const int logsReadSizeCompressEnabled = 10;
const int logsReadSizeUndefined = -1;

// Default tel number match length
const int logsDefaultMatchLength = 7;

// MACROS
#define DESC_TO_QSTRING( desc )\
    QString::fromUtf16( desc.Ptr(), desc.Length() );

// DATA TYPES

class LogsEventStrings 
    {
public:
    QString iInDirection;        // incoming direction text
    QString iOutDirection;       // outgoing direction text
    QString iMissedDirection;    // missed direction text    
    QString iInDirectionAlt;     // Incoming alternate line
    QString iOutDirectionAlt;    // Outgoing alternate line
    QString iFetched;            // Fetched
    QString iUnKnownRemote;      // unknown remote text
    };

#endif  // LOGSENGDEFS_H
           
// End of File
