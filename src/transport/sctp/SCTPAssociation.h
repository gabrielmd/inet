//
// Copyright (C) 2005-2010 Irene Ruengeler
// Copyright (C) 2009-2012 Thomas Dreibholz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __SCTPASSOCIATION_H
#define __SCTPASSOCIATION_H

#include "INETDefs.h"

#include "IPvXAddress.h"
#include "IPv4Address.h"
#include "SCTP.h"
#include "InterfaceTable.h"
#include "InterfaceTableAccess.h"
#include "SCTPGapList.h"
#include "SCTPQueue.h"
#include "SCTPSendStream.h"
#include "SCTPReceiveStream.h"
#include "SCTPMessage.h"
#include <list>
#include <iostream>
#include <errno.h>
#include <math.h>
#include <platdep/intxtypes.h>


class SCTPMessage;
class SCTPCommand;
class SCTPOpenCommand;
class SCTPReceiveStream;
class SCTPSendStream;
class SCTPAlgorithm;
class SCTP;

typedef std::vector<IPvXAddress> AddressVector;

enum SctpState
{
    SCTP_S_CLOSED               = 0,
    SCTP_S_COOKIE_WAIT          = FSM_Steady(1),
    SCTP_S_COOKIE_ECHOED        = FSM_Steady(2),
    SCTP_S_ESTABLISHED          = FSM_Steady(3),
    SCTP_S_SHUTDOWN_PENDING     = FSM_Steady(4),
    SCTP_S_SHUTDOWN_SENT        = FSM_Steady(5),
    SCTP_S_SHUTDOWN_RECEIVED    = FSM_Steady(6),
    SCTP_S_SHUTDOWN_ACK_SENT    = FSM_Steady(7)
};


//
// Event, strictly for the FSM state transition purposes.
// DO NOT USE outside performStateTransition()!
//
enum SCTPEventCode
{
    SCTP_E_ASSOCIATE,
    SCTP_E_OPEN_PASSIVE,
    SCTP_E_ABORT,
    SCTP_E_SHUTDOWN,
    SCTP_E_CLOSE,
    SCTP_E_SEND,
    SCTP_E_RCV_INIT,
    SCTP_E_RCV_ABORT,
    SCTP_E_RCV_VALID_COOKIE_ECHO,
    SCTP_E_RCV_INIT_ACK,
    SCTP_E_RCV_COOKIE_ACK,
    SCTP_E_RCV_SHUTDOWN,
    SCTP_E_RCV_SHUTDOWN_ACK,
    SCTP_E_RCV_SHUTDOWN_COMPLETE,
    SCTP_E_NO_MORE_OUTSTANDING,
    SCTP_E_TIMEOUT_INIT_TIMER,
    SCTP_E_TIMEOUT_SHUTDOWN_TIMER,
    SCTP_E_TIMEOUT_RTX_TIMER,
    SCTP_E_TIMEOUT_HEARTBEAT_TIMER,
    SCTP_E_IGNORE,
    SCTP_E_RECEIVE,
    SCTP_E_DUP_RECEIVED,
    SCTP_E_PRIMARY,
    SCTP_E_DELIVERED,
    SCTP_E_QUEUE_MSGS_LIMIT,
    SCTP_E_QUEUE_BYTES_LIMIT,
    SCTP_E_SEND_QUEUE_LIMIT,
    SCTP_E_SEND_SHUTDOWN_ACK,
    SCTP_E_STOP_SENDING
};

enum SCTPChunkTypes
{
    DATA                = 0,
    INIT                = 1,
    INIT_ACK            = 2,
    SACK                = 3,
    HEARTBEAT           = 4,
    HEARTBEAT_ACK       = 5,
    ABORT               = 6,
    SHUTDOWN            = 7,
    SHUTDOWN_ACK        = 8,
    ERRORTYPE           = 9,
    COOKIE_ECHO         = 10,
    COOKIE_ACK          = 11,
    SHUTDOWN_COMPLETE   = 14,
    NR_SACK             = 16,
    FORWARD_TSN         = 192
};

enum SCTPPrMethods
{
    PR_NONE   = 0,
    PR_TTL    = 1,
    PR_RTX    = 2,
    PR_PRIO   = 3,
    PR_STRRST = 4
};

enum SCTPParameterTypes
{
    UNRECOGNIZED_PARAMETER          = 8,
    SUPPORTED_ADDRESS_TYPES         = 12,
    FORWARD_TSN_SUPPORTED_PARAMETER = 49152
};




enum SCTPCCModules
{
    RFC4960 = 0
};

enum SCTPStreamSchedulers
{
    ROUND_ROBIN             = 0
};


#define SCTP_COMMON_HEADER              12      // without options
#define SCTP_INIT_CHUNK_LENGTH          20
#define SCTP_DATA_CHUNK_LENGTH          16
#define SCTP_SACK_CHUNK_LENGTH          16
#define SCTP_NRSACK_CHUNK_LENGTH        20
#define SCTP_HEARTBEAT_CHUNK_LENGTH     4
#define SCTP_ABORT_CHUNK_LENGTH         4
#define SCTP_COOKIE_ACK_LENGTH          4
#define SCTP_FORWARD_TSN_CHUNK_LENGTH   8
#define SCTP_SHUTDOWN_CHUNK_LENGTH      8
#define SCTP_SHUTDOWN_ACK_LENGTH        4
#define SCTP_ERROR_CHUNK_LENGTH         4       // without parameters
#define IP_HEADER_LENGTH                20
#define SCTP_DEFAULT_ARWND              (1<<16)
#define SCTP_DEFAULT_INBOUND_STREAMS    17
#define SCTP_DEFAULT_OUTBOUND_STREAMS   17
#define VALID_COOKIE_LIFE_TIME          10
#define SCTP_COOKIE_LENGTH              76
#define HB_INTERVAL                     30
#define PATH_MAX_RETRANS                5

#define SCTP_TIMEOUT_INIT_REXMIT        3     // initially 3 seconds
#define SCTP_TIMEOUT_INIT_REXMIT_MAX    240   // 4 mins
#define SACK_DELAY                      0.2
#define RTO_BETA                        0.25
#define RTO_ALPHA                       0.125
#define RTO_INITIAL                     3
#define IPTOS_DEFAULT                   0x10      // IPTOS_LOWDELAY

#define DEFAULT_MAX_SENDQUEUE           0           /* unlimited send queue */
#define DEFAULT_MAX_RECVQUEUE           0           /* unlimited recv queue - unused really */

#define MAX_ASSOCS                      10

#define SCTP_MAX_PAYLOAD                1488 // 12 bytes for common header

#define ADD_PADDING(x)                  ((((x) + 3) >> 2) << 2)

#define DEBUG                           1

#define SHUTDOWN_GUARD_TIMEOUT          180

/**
 * Returns the minimum of a and b.
 */
inline double min(const double a, const double b) { return (a < b) ? a : b; }

/**
 * Returns the maximum of a and b.
 */
inline double max(const double a, const double b) { return (a < b) ? b : a; }


class INET_API SCTPPathVariables : public cObject
{
    public:
        SCTPPathVariables(const IPvXAddress& addr, SCTPAssociation* assoc);
        ~SCTPPathVariables();

        SCTPAssociation*     association;
        IPvXAddress          remoteAddress;

        // ====== Path Management =============================================
        bool                activePath;
        bool                confirmed;
        bool                requiresRtx;
        bool                primaryPathCandidate;
        bool                forceHb;
        // ====== Last SACK ===================================================
        simtime_t          lastSACKSent;
        // ====== T3 Timer Handling ===========================================
        // Set to TRUE when CumAck has acknowledged TSNs on this path.
        // Needed to reset T3 timer.
        bool                newCumAck;
        // ====== Path Status =================================================
        uint32              pathErrorCount;
        uint32              pathErrorThreshold;
        uint32              pmtu;
        // ====== Congestion Control ==========================================
        uint32              cwnd;
        uint32              ssthresh;
        uint32              partialBytesAcked;
        uint32              queuedBytes;
        uint32              outstandingBytes;
        // ~~~~~~ Temporary storage for SACK handling ~~~~~~~
        uint32              outstandingBytesBeforeUpdate;
        uint32              newlyAckedBytes;
        // ====== Fast Recovery ===============================================
        bool                fastRecoveryActive;
        uint32              fastRecoveryExitPoint;
        simtime_t           fastRecoveryEnteringTime;
        // ====== Lowest TSN (used for triggering T3 RTX Timer Restart) =======
        bool                findLowestTSN;
        bool                lowestTSNRetransmitted;

        // ====== Timers ======================================================
        cMessage*           HeartbeatTimer;
        cMessage*           HeartbeatIntervalTimer;
        cMessage*           CwndTimer;
        cMessage*           T3_RtxTimer;

        // ====== Path Status =================================================
        simtime_t           heartbeatTimeout;
        simtime_t           heartbeatIntervalTimeout;
        simtime_t           rtxTimeout;
        simtime_t           cwndTimeout;
        simtime_t           rttUpdateTime;
        simtime_t           lastAckTime;
        simtime_t           pathRto;
        simtime_t           srtt;
        simtime_t           rttvar;

        // ====== Path Statistics =============================================
        unsigned int        gapAckedChunksInLastSACK;        // Per-path GapAck'ed chunks in last SACK (R+NR)
        unsigned int        gapNRAckedChunksInLastSACK;      // Per-path GapAck'ed chunks in last SACK (only NR)
        unsigned int        gapUnackedChunksInLastSACK;      // Per-path Not-GapAck'ed chunks in last SACK (i.e. chunks between GapAcks)
        unsigned int        numberOfDuplicates;
        unsigned int        numberOfFastRetransmissions;
        unsigned int        numberOfTimerBasedRetransmissions;
        unsigned int        numberOfHeartbeatsSent;
        unsigned int        numberOfHeartbeatAcksSent;
        unsigned int        numberOfHeartbeatsRcvd;
        unsigned int        numberOfHeartbeatAcksRcvd;

        // ====== Output Vectors ==============================================
        cOutVector*         vectorPathSentTSN;
        cOutVector*         vectorPathReceivedTSN;
        cOutVector*         vectorPathHb;
        cOutVector*         vectorPathRcvdHb;
        cOutVector*         vectorPathHbAck;
        cOutVector*         vectorPathRcvdHbAck;
        cOutVector*         statisticsPathRTO;
        cOutVector*         statisticsPathRTT;
        cOutVector*         statisticsPathSSthresh;
        cOutVector*         statisticsPathCwnd;
};



class INET_API SCTPDataVariables : public cObject
{
    public:
        SCTPDataVariables();
        ~SCTPDataVariables();

        inline void setInitialDestination(SCTPPathVariables* path) {
            initialDestination = path;
        }
        inline const IPvXAddress& getInitialDestination() const {
            if (initialDestination != NULL) {
                return (initialDestination->remoteAddress);
            }
            return (zeroAddress);
        }
        inline SCTPPathVariables* getInitialDestinationPath() const {
            return (initialDestination);
        }

        inline void setLastDestination(SCTPPathVariables* path) {
            lastDestination = path;
        }
        inline const IPvXAddress& getLastDestination() const {
            if (lastDestination != NULL) {
                return (lastDestination->remoteAddress);
            }
            return (zeroAddress);
        }
        inline SCTPPathVariables* getLastDestinationPath() const {
            return (lastDestination);
        }

        inline void setNextDestination(SCTPPathVariables* path) {
            nextDestination = path;
        }
        inline const IPvXAddress& getNextDestination() const {
            if (nextDestination != NULL) {
                return (nextDestination->remoteAddress);
            }
            return (zeroAddress);
        }
        inline SCTPPathVariables* getNextDestinationPath() const {
            return (nextDestination);
        }

        // ====== Chunk Data Management =======================================
        cPacket*            userData;
        uint32              len;                                 // Different from wire
        uint32              booksize;
        uint32              tsn;
        uint16              sid;
        uint16              ssn;
        uint32              ppid;
        uint32              fragments;                 // Number of fragments => TSNs={tsn, ..., tsn+fragments-1}
        bool                enqueuedInTransmissionQ;     // In transmissionQ? Otherwise, it is just in retransmissionQ.
        bool                countsAsOutstanding;         // Is chunk outstanding?
        bool                hasBeenFastRetransmitted;
        bool                hasBeenAbandoned;
        bool                hasBeenReneged;              // Has chunk been reneged?
        bool                hasBeenAcked;                    // Has chunk been SACK'ed?
        bool                hasBeenCountedAsNewlyAcked; // Chunk has been counted as newly SACK'ed
        bool                bbit;
        bool                ebit;
        bool                ordered;

        // ====== Retransmission Management ===================================
        uint32              gapReports;
        simtime_t           enqueuingTime;
        simtime_t           sendTime;
        simtime_t           expiryTime;
        uint32              numberOfRetransmissions;
        uint32              numberOfTransmissions;
        uint32              allowedNoRetransmissions;

        // ====== Advanced Chunk Information ==================================
        SCTPPathVariables*  queuedOnPath;              // The path to account this chunk in qCounters.queuedOnPath
        SCTPPathVariables*  ackedOnPath;               // The path this chunk has been acked on
        bool                hasBeenTimerBasedRtxed;    // Has chunk been timer-based retransmitted?
        bool                hasBeenMoved;              // Chunk has been moved to solve buffer blocking
        simtime_t           firstSendTime;
        uint32              prMethod;
        uint32              priority;
        bool                sendForwardIfAbandoned;

    public:
        static const IPvXAddress zeroAddress;

        // ====== Private Control Information =================================
    private:
        SCTPPathVariables* initialDestination;
        SCTPPathVariables* lastDestination;
        SCTPPathVariables* nextDestination;
};



class INET_API SCTPStateVariables : public cObject
{
    public:
        SCTPStateVariables();
        ~SCTPStateVariables();
    public:
        inline void setPrimaryPath(SCTPPathVariables* path) {
            primaryPath = path;
        }
        inline const IPvXAddress& getPrimaryPathIndex() const {
            if (primaryPath != NULL) {
                return (primaryPath->remoteAddress);
            }
            return (SCTPDataVariables::zeroAddress);
        }
        inline SCTPPathVariables* getPrimaryPath() const {
            return (primaryPath);
        }

        bool                        active;
        bool                        fork;
        bool                        ackPointAdvanced;
        bool                        dataChunkReceived;
        bool                        initReceived;
        bool                        cookieEchoReceived;
        bool                        newChunkReceived;
        bool                        firstChunkReceived;
        bool                        swsAvoidanceInvoked;
        bool                        probingIsAllowed;
        bool                        zeroWindowProbing;
        bool                        alwaysBundleSack;
        bool                        fastRecoverySupported;
        bool                        nagleEnabled;
        bool                        sackAllowed;
        bool                        reactivatePrimaryPath;
        bool                        resetPending;
        bool                        stopReceiving;        // incoming data will be discarded
        bool                        stopOldData;              // data with TSN<peerTsnAfterReset will be discarded
        bool                        queueUpdate;
        bool                        firstDataSent;
        bool                        peerWindowFull;
        bool                        zeroWindow;
        bool                        stopSending;              // will be called when SCTP_E_SHUTDOWN arrived
        bool                        inOut;
        bool                        noMoreOutstanding;
        uint32                      numGapReports;
        IPvXAddress                 initialPrimaryPath;
        std::list<SCTPPathVariables*> lastDataSourceList;   // DATA chunk sources for new SACK
        SCTPPathVariables*          lastDataSourcePath;
        AddressVector               localAddresses;
        std::list<uint32>           dupList;              // Duplicates list for incoming DATA chunks
        uint32                      errorCount;           // overall error counter
        uint64                      peerRwnd;
        uint64                      initialPeerRwnd;
        uint64                      localRwnd;
        uint32                      nextTSN;                  // TSN to be sent
        uint32                      lastTsnAck;           // stored at the sender side; cumTSNAck announced in a SACK
        uint32                      highestTsnAcked;
        uint32                      lastTsnReceived;          // SACK
        uint32                      lastTSN;                  // my very last TSN to be sent
        uint32                      ackState;                 // number of packets to be acknowledged
        SCTPGapList                 gapList;              // GapAck list for incoming DATA chunks
        uint32                      packetsInTotalBurst;
        simtime_t                   lastTransmission;
        uint64                      outstandingBytes;     // Number of bytes outstanding
        uint64                      queuedSentBytes;      // Number of bytes in sender queue
        uint64                      queuedDroppableBytes; // Bytes in send queue droppable by PR-SCTP
        uint64                      queuedReceivedBytes;  // Number of bytes in receiver queue
        uint32                      lastStreamScheduled;
        uint32                      assocPmtu;                // smallest overall path mtu
        uint32                      msgNum;                   // indicates the sequence number of the message
        uint64                      bytesRcvd;
        uint32                      numRequests;
        uint32                      bytesToRetransmit;
        uint32                      messagesToPush;
        int32                       pushMessagesLeft;
        uint32                      count;
        uint8                       localTieTag[32];
        uint8                       peerTieTag[32];
        uint64                      queuedMessages;       // Messages buffered at the sender side
        uint32                      messageAcceptLimit;
        uint32                      queueLimit;
        uint16                      header;
        int32                       probingTimeout;
        std::vector<int32>          numMsgsReq;
        int32                       cookieLifeTime;
        /** Counter for init and cookie retransmissions */
        int16                       initRetransCounter;
        simtime_t                   initRexmitTimeout;
        /** pointer to the init chunk data structure (for retransmissions) */
        SCTPInitChunk*              initChunk;
        /** pointer to the cookie chunk data structure (for retransmissions) */
        SCTPCookieEchoChunk*        cookieChunk;
        /** pointer to the resetChunk (for retransmission) */
        SCTPShutdownChunk*          shutdownChunk;
        SCTPShutdownAckChunk*       shutdownAckChunk;
        SCTPMessage*                sctpmsg;
        uint64                      sendQueueLimit;
        uint64                      sendBuffer;
        bool                        appSendAllowed;
        simtime_t                   lastSendQueueAbated;
        uint32                      nextRSid;
        uint32                      swsLimit;
        bool                        lastMsgWasFragment;
        bool                        enableHeartbeats;
        SCTPMessage*                sctpMsg;
        uint16                      chunksAdded;
        uint16                      dataChunksAdded;
        uint32                      packetBytes;
        bool                        authAdded;

        // ====== NR-SACK =====================================================
        bool                        nrSack;
        uint32                      gapReportLimit;
        bool                        disableReneging;

        // ====== Max Burst ===================================================
        uint32                      maxBurst;
        
        // ====== SACK Sequence Number Checker ================================
        bool                        checkSackSeqNumber;         // Ensure handling SACKs in original sequence
        uint32                      outgoingSackSeqNum;
        uint32                      incomingSackSeqNum;

        // ====== Further features ============================================
        uint32                      advancedPeerAckPoint;
        uint32                      prMethod;
        simtime_t                   lastThroughputTime;
        simtime_t                   lastAssocThroughputTime;
        uint32                      assocThroughput;
        double                      throughputInterval;
        bool                        ssNextStream;
        bool                        ssLastDataChunkSizeSet;

    private:
        SCTPPathVariables*          primaryPath;
};



class INET_API SCTPAssociation : public cObject
{
    friend class SCTP;
    friend class SCTPPathVariables;

    // map for storing the path parameters
    typedef std::map<IPvXAddress,SCTPPathVariables*> SCTPPathMap;
    // map for storing the queued bytes per path
    typedef std::map<IPvXAddress, uint32> CounterMap;
    typedef struct counter {
        uint64    roomSumSendStreams;
        uint64    bookedSumSendStreams;
        uint64    roomSumRcvStreams;
        CounterMap roomTransQ;
        CounterMap bookedTransQ;
        CounterMap roomRetransQ;
    } QueueCounter;
    typedef struct calcBytesToSend {
        bool chunk;
        bool packet;
        uint32 bytesToSend;
    } BytesToBeSent;
    typedef struct congestionControlFunctions {
        void(SCTPAssociation::*ccInitParams)(SCTPPathVariables* path);
        void(SCTPAssociation::*ccUpdateAfterSack)();
        void(SCTPAssociation::*ccUpdateAfterCwndTimeout)(SCTPPathVariables* path);
        void(SCTPAssociation::*ccUpdateAfterRtxTimeout)(SCTPPathVariables* path);
        void(SCTPAssociation::*ccUpdateMaxBurst)(SCTPPathVariables* path);
        void(SCTPAssociation::*ccUpdateBytesAcked)(SCTPPathVariables* path, const uint32 ackedBytes, const bool ctsnaAdvanced);
    } CCFunctions;
    typedef std::map<uint32, SCTPSendStream*>    SCTPSendStreamMap;
    typedef std::map<uint32, SCTPReceiveStream*> SCTPReceiveStreamMap;

    public:
        // connection identification by apps: appgateIndex+assocId
        int32                   appGateIndex; // Application gate index
        int32                   assocId;        // Identifies connection within the app
        IPvXAddress             remoteAddr; // Remote address from last message
        IPvXAddress             localAddr;      // Local address from last message
        uint16                  localPort;      // Remote port from last message
        uint16                  remotePort; // Local port from last message
        uint32                  localVTag;      // Local verification tag
        uint32                  peerVTag;       // Remote verification tag
        bool                    listen;

        // Timers
        cMessage*               T1_InitTimer;
        cMessage*               T2_ShutdownTimer;
        cMessage*               T5_ShutdownGuardTimer;
        cMessage*               SackTimer;
        cMessage*               StartTesting;
        cOutVector*             assocThroughputVector;

    protected:
        AddressVector           localAddressList;
        AddressVector           remoteAddressList;
        uint32                  numberOfRemoteAddresses;
        uint32                  inboundStreams;
        uint32                  outboundStreams;

        int32                   status;
        uint32                  initTsn;
        uint32                  initPeerTsn;
        uint32                  sackFrequency;
        double                  sackPeriod;
        CCFunctions             ccFunctions;
        uint16                  ccModule;

        cOutVector*             advRwnd;
        cOutVector*             cumTsnAck;
        cOutVector*             sendQueue;
        cOutVector*             numGapBlocks;

        // Variables associated with the state of this association
        SCTPStateVariables*     state;
        BytesToBeSent           bytes;
        SCTP*                   sctpMain;                   // SCTP module
        cFSM*                   fsm;                            // SCTP state machine
        SCTPPathMap             sctpPathMap;
        QueueCounter            qCounter;
        SCTPQueue*              transmissionQ;
        SCTPQueue*              retransmissionQ;
        SCTPSendStreamMap       sendStreams;
        SCTPReceiveStreamMap    receiveStreams;
        SCTPAlgorithm*          sctpAlgorithm;

    public:
        /**
        * Constructor.
        */
        SCTPAssociation(SCTP* mod, int32 appGateIndex, int32 assocId);
        /**
        * Destructor.
        */
        ~SCTPAssociation();
        /**
        * Utility: Send data from sendQueue.
        */
        void sendOnPath(SCTPPathVariables* pathId, const bool firstPass = true);
        void sendOnAllPaths(SCTPPathVariables* firstPath);

        /** Utility: returns name of SCTP_I_xxx constants */
        static const char* indicationName(const int32 code);

        /** Utility: return IPv4 or IPv6 address level */
        static int getAddressLevel(const IPvXAddress& addr);

        /* @name Various getters */
        //@{
        inline int32 getFsmState() const { return fsm->getState(); };
        inline SCTPStateVariables* getState() const { return state; };
        inline SCTPQueue* getTransmissionQueue() const { return transmissionQ; };
        inline SCTPQueue* getRetransmissionQueue() const { return retransmissionQ; };
        inline SCTPAlgorithm* getSctpAlgorithm() const { return sctpAlgorithm; };
        inline SCTP* getSctpMain() const { return sctpMain; };
        inline cFSM* getFsm() const { return fsm; };
        inline cMessage* getInitTimer() const { return T1_InitTimer; };
        inline cMessage* getShutdownTimer() const { return T2_ShutdownTimer; };
        inline cMessage* getSackTimer() const { return SackTimer; };

        /** Utility: returns name of SCTP_S_xxx constants */
        static const char* stateName(const int32 state);

        static uint32 chunkToInt(const char* type);

        /* Process self-messages (timers).
        * Normally returns true. A return value of false means that the
        * connection structure must be deleted by the caller (SCTPMain).
        */
        bool processTimer(cMessage* msg);
        /**
        * Process incoming SCTP segment. Normally returns true. A return value
        * of false means that the connection structure must be deleted by the
        * caller (SCTP).
        */
        bool processSCTPMessage(SCTPMessage* sctpmsg, const IPvXAddress& srcAddr, const IPvXAddress& destAddr);
        /**
        * Process commands from the application.
        * Normally returns true. A return value of false means that the
        * connection structure must be deleted by the caller (SCTP).
        */
        bool processAppCommand(cPacket* msg);
        void removePath();
        void removePath(const IPvXAddress& addr);
        void deleteStreams();
        void stopTimer(cMessage* timer);
        void stopTimers();
        inline SCTPPathVariables* getPath(const IPvXAddress& pathId) const {
            SCTPPathMap::const_iterator iterator = sctpPathMap.find(pathId);
            if (iterator != sctpPathMap.end()) {
                return iterator->second;
            }
            return NULL;
        }
        void printSctpPathMap() const;
             
        /**
        * Compare TSNs
        */
        inline static int32 tsnLt(const uint32 tsn1, const uint32 tsn2) { return ((int32)(tsn1-tsn2)<0); }
        inline static int32 tsnLe(const uint32 tsn1, const uint32 tsn2) { return ((int32)(tsn1-tsn2)<=0); }
        inline static int32 tsnGe(const uint32 tsn1, const uint32 tsn2) { return ((int32)(tsn1-tsn2)>=0); }
        inline static int32 tsnGt(const uint32 tsn1, const uint32 tsn2) { return ((int32)(tsn1-tsn2)>0); }
        inline static int32 tsnBetween(const uint32 tsn1, const uint32 midtsn, const uint32 tsn2) { return ((tsn2-tsn1)>=(midtsn-tsn1)); }

        inline static int16 ssnGt(const uint16 ssn1, const uint16 ssn2) { return ((int16)(ssn1-ssn2)>0); }


    protected:
        /** @name FSM transitions: analysing events and executing state transitions */
        //@{
        /** Maps app command codes (msg kind of app command msgs) to SCTP_E_xxx event codes */
        SCTPEventCode preanalyseAppCommandEvent(int32 commandCode);
        /** Implemements the pure SCTP state machine */
        bool performStateTransition(const SCTPEventCode& event);
        void stateEntered(int32 state);
        //@}
        /** @name Processing app commands. Invoked from processAppCommand(). */
        //@{
        void process_ASSOCIATE(SCTPEventCode& event, SCTPCommand* sctpCommand, cPacket* msg);
        void process_OPEN_PASSIVE(SCTPEventCode& event, SCTPCommand* sctpCommand, cPacket* msg);
        void process_SEND(SCTPEventCode& event, SCTPCommand* sctpCommand, cPacket* msg);
        void process_CLOSE(SCTPEventCode& event);
        void process_ABORT(SCTPEventCode& event);
        void process_STATUS(SCTPEventCode& event, SCTPCommand* sctpCommand, cPacket* msg);
        void process_RECEIVE_REQUEST(SCTPEventCode& event, SCTPCommand* sctpCommand);
        void process_PRIMARY(SCTPEventCode& event, SCTPCommand* sctpCommand);
        //@}

        /** @name Processing SCTP message arrivals. Invoked from processSCTPMessage(). */
        //@{
        bool process_RCV_Message(SCTPMessage* sctpseg, const IPvXAddress& src, const IPvXAddress& dest);
        /**
        * Process incoming SCTP packets. Invoked from process_RCV_Message
        */
        bool processInitArrived(SCTPInitChunk* initChunk, int32 sport, int32 dport);
        bool processInitAckArrived(SCTPInitAckChunk* initAckChunk);
        bool processCookieEchoArrived(SCTPCookieEchoChunk* cookieEcho, IPvXAddress addr);
        bool processCookieAckArrived();
        SCTPEventCode processDataArrived(SCTPDataChunk* dataChunk);
        SCTPEventCode processSackArrived(SCTPSackChunk* sackChunk);
        SCTPEventCode processHeartbeatAckArrived(SCTPHeartbeatAckChunk* heartbeatack, SCTPPathVariables* path);
        SCTPEventCode processForwardTsnArrived(SCTPForwardTsnChunk* forChunk);
        //@}

        /** @name Processing timeouts. Invoked from processTimer(). */
        //@{
        void process_TIMEOUT_RTX(SCTPPathVariables* path);

        void process_TIMEOUT_HEARTBEAT(SCTPPathVariables* path);
        void process_TIMEOUT_HEARTBEAT_INTERVAL(SCTPPathVariables* path, bool force);
        void process_TIMEOUT_INIT_REXMIT(SCTPEventCode& event);
        void process_TIMEOUT_PROBING();
        void process_TIMEOUT_SHUTDOWN(SCTPEventCode& event);
        int32 updateCounters(SCTPPathVariables* path);
        //@}

        void startTimer(cMessage* timer, const simtime_t& timeout);

        /** Utility: clone a listening association. Used for forking. */
        SCTPAssociation* cloneAssociation();

        /** Utility: creates send/receive queues and sctpAlgorithm */
        void initAssociation(SCTPOpenCommand* openCmd);

        /** Methods dealing with the handling of TSNs  **/
        bool tsnIsDuplicate(const uint32 tsn) const;
        bool makeRoomForTsn(const uint32 tsn, const uint32 length, const bool uBit);

        /** Methods for creating and sending chunks */
        void sendInit();
        void sendInitAck(SCTPInitChunk* initchunk);
        void sendCookieEcho(SCTPInitAckChunk* initackchunk);
        void sendCookieAck(const IPvXAddress& dest);
        void sendAbort();
        void sendHeartbeat(const SCTPPathVariables* path);
        void sendHeartbeatAck(const SCTPHeartbeatChunk* heartbeatChunk,
                                     const IPvXAddress&         src,
                                     const IPvXAddress&         dest);
        void sendSack();
        void sendShutdown();
        void sendShutdownAck(const IPvXAddress& dest);
        void sendShutdownComplete();
        SCTPSackChunk* createSack();
        /** Retransmitting chunks */
        void retransmitInit();
        void retransmitCookieEcho();
        void retransmitShutdown();
        void retransmitShutdownAck();

        /** Utility: adds control info to message and sends it to IP */
        void sendToIP(SCTPMessage* sctpmsg, const IPvXAddress& dest);
        inline void sendToIP(SCTPMessage* sctpmsg) {
            sendToIP(sctpmsg, remoteAddr);
        }
        void recordInPathVectors(SCTPMessage* pMsg, const IPvXAddress& rDest);
        void scheduleSack();
        /** Utility: signal to user that connection timed out */
        void signalConnectionTimeout();

        /** Utility: start a timer */
        inline void scheduleTimeout(cMessage* msg, const simtime_t& timeout) {
            sctpMain->scheduleAt(simulation.getSimTime() + timeout, msg);
        }

        /** Utility: cancel a timer */
        inline cMessage* cancelEvent(cMessage* msg) {
            return sctpMain->cancelEvent(msg);
        }

        /** Utility: sends packet to application */
        void sendToApp(cPacket* msg);

        /** Utility: sends status indication (SCTP_I_xxx) to application */
        void sendIndicationToApp(const int32 code, const int32 value = 0);

        /** Utility: sends SCTP_I_ESTABLISHED indication with SCTPConnectInfo to application */
        void sendEstabIndicationToApp();
        void pushUlp();
        void sendDataArrivedNotification(uint16 sid);
        void putInDeliveryQ(uint16 sid);
        /** Utility: prints local/remote addr/port and app gate index/assocId */
        void printAssocBrief();
        /** Utility: prints important header fields */
        static void printSegmentBrief(SCTPMessage* sctpmsg);


        /** Utility: returns name of SCTP_E_xxx constants */
        static const char* eventName(const int32 event);

        void addPath(const IPvXAddress& addr);
        SCTPPathVariables* getNextPath(const SCTPPathVariables* oldPath) const;
        inline const IPvXAddress& getNextAddress(const SCTPPathVariables* oldPath) const {
            const SCTPPathVariables* nextPath = getNextPath(oldPath);
            if (nextPath != NULL) {
                return (nextPath->remoteAddress);
            }
            return (SCTPDataVariables::zeroAddress);
        }
        SCTPPathVariables* getNextDestination(SCTPDataVariables* chunk) const;

        void bytesAllowedToSend(SCTPPathVariables* path, const bool firstPass);

        void pathStatusIndication(const SCTPPathVariables* path, const bool status);

        bool allPathsInactive() const;

        SCTPForwardTsnChunk* createForwardTsnChunk(const IPvXAddress& pid);

        bool msgMustBeAbandoned(SCTPDataMsg* msg, int32 stream, bool ordered); //PR-SCTP
        bool chunkMustBeAbandoned(SCTPDataVariables* chunk, 
                                     SCTPPathVariables* sackPath);
        void advancePeerTsn();
        /**
        * Manipulating chunks
        */
        SCTPDataChunk* transformDataChunk(SCTPDataVariables* chunk);
        SCTPDataVariables* makeVarFromMsg(SCTPDataChunk* datachunk);

        /**
        *Dealing with streams
        */

        int32 streamScheduler(SCTPPathVariables* path, bool peek);
        void initStreams(uint32 inStreams, uint32 outStreams);
        int32 numUsableStreams();
        typedef struct streamSchedulingFunctions {
            void(SCTPAssociation::*ssInitStreams)(uint32 inStreams, uint32 outStreams);
            int32(SCTPAssociation::*ssGetNextSid)(SCTPPathVariables* path, bool peek);
            int32(SCTPAssociation::*ssUsableStreams)();
        } SSFunctions;
        SSFunctions ssFunctions;
        uint16 ssModule;

        /**
        *    Queue Management
        */
        void process_QUEUE_MSGS_LIMIT(const SCTPCommand* sctpCommand);
        void process_QUEUE_BYTES_LIMIT(const SCTPCommand* sctpCommand);
        int32 getOutstandingBytes() const;
        void dequeueAckedChunks(const uint32          tsna,
                                          SCTPPathVariables* path,
                                          simtime_t&            rttEstimation);
        SCTPDataMsg* peekOutboundDataMsg();
        SCTPDataVariables* peekAbandonedChunk(const SCTPPathVariables* path);
        SCTPDataVariables* getOutboundDataChunk(const SCTPPathVariables* path,
                                                const int32              availableSpace,
                                                const int32              availableCwnd);
        SCTPDataMsg* dequeueOutboundDataMsg(SCTPPathVariables* path,
                                            const int32 availableSpace,
                                            const int32 availableCwnd);
        bool nextChunkFitsIntoPacket(SCTPPathVariables* path, int32 bytes);
        void putInTransmissionQ(uint32 tsn, SCTPDataVariables* chunk);

        uint32 getAllTransQ();

        /**
        * Flow control
        */
        void pmStartPathManagement();
        void pmDataIsSentOn(SCTPPathVariables* path);
        void pmClearPathCounter(SCTPPathVariables* path);
        void pmRttMeasurement(SCTPPathVariables* path,
                                     const simtime_t&     rttEstimation);

        void disposeOf(SCTPMessage* sctpmsg);
        void calculateRcvBuffer();
        void listOrderedQ();
        void tsnWasReneged(SCTPDataVariables*   chunk,
                       const SCTPPathVariables* sackPath,
                       const int                type);
        void printOutstandingTsns();

        /** SCTPCCFunctions **/
        void initCCParameters(SCTPPathVariables* path);
        void updateFastRecoveryStatus(const uint32 lastTsnAck);
        void cwndUpdateAfterSack();
        void cwndUpdateAfterCwndTimeout(SCTPPathVariables* path);
        void cwndUpdateAfterRtxTimeout(SCTPPathVariables* path);
        void cwndUpdateMaxBurst(SCTPPathVariables* path);
        void cwndUpdateBytesAcked(SCTPPathVariables* path,
                                          const uint32          ackedBytes,
                                          const bool            ctsnaAdvanced);

    private:
        SCTPDataVariables* makeDataVarFromDataMsg(SCTPDataMsg*       datMsg,
                                                                SCTPPathVariables* path);
        SCTPPathVariables* choosePathForRetransmission();
        void timeForSack(bool& sackOnly, bool& sackWithData);
        void recordCwndUpdate(SCTPPathVariables* path);
        void sendSACKviaSelectedPath(SCTPMessage* sctpMsg);
        void checkOutstandingBytes();
        uint32 getInitialCwnd(const SCTPPathVariables* path) const;
        void generateSendQueueAbatedIndication(const uint64 bytes);
        void renegablyAckChunk(SCTPDataVariables* chunk,
                           SCTPPathVariables* sackPath);
        void nonRenegablyAckChunk(SCTPDataVariables* chunk,
                              SCTPPathVariables* sackPath,
                              simtime_t&         rttEstimation,
                              SCTP::AssocStat*   assocStat);
        void handleChunkReportedAsAcked(uint32&        highestNewAck,
                                    simtime_t&         rttEstimation,
                                    SCTPDataVariables* myChunk,
                                    SCTPPathVariables* sackPath,
                                    const bool         sackIsNonRevokable);
        void handleChunkReportedAsMissing(const SCTPSackChunk*  sackChunk,
                                          const uint32          highestNewAck,
                                          SCTPDataVariables*    myChunk,
                                          SCTPPathVariables*    sackPath);
        void moveChunkToOtherPath(SCTPDataVariables* chunk,
                                          SCTPPathVariables* newPath);
        void decreaseOutstandingBytes(SCTPDataVariables* chunk);
        void increaseOutstandingBytes(SCTPDataVariables* chunk,
                                                SCTPPathVariables* path);
        int32 calculateBytesToSendOnPath(const SCTPPathVariables* pathVar);
        void storePacket(SCTPPathVariables* pathVar,
                              SCTPMessage*          sctpMsg,
                              const uint16          chunksAdded,
                              const uint16          dataChunksAdded);
        void loadPacket(SCTPPathVariables* pathVar,
                             SCTPMessage**        sctpMsg,
                             uint16*                  chunksAdded,
                             uint16*                  dataChunksAdded);

        inline void ackChunk(SCTPDataVariables* chunk, SCTPPathVariables* sackPath) {
            chunk->hasBeenAcked = true;
            chunk->ackedOnPath = sackPath;
        }
        inline void unackChunk(SCTPDataVariables* chunk) {
            chunk->hasBeenAcked = false;
        }
        inline bool chunkHasBeenAcked(const SCTPDataVariables* chunk) const {
            return (chunk->hasBeenAcked);
        }
        inline bool chunkHasBeenAcked(const uint32 tsn) const {
            const SCTPDataVariables* chunk = retransmissionQ->getChunk(tsn);
            if (chunk) {
                return (chunkHasBeenAcked(chunk));
            }
            return (false);
        }

        void recordTransmission(SCTPMessage* sctpMsg, SCTPPathVariables* path);
        void recordAcknowledgement(SCTPDataVariables* chunk, SCTPPathVariables* path);
        void recordDequeuing(SCTPDataVariables* chunk);
};

#endif
