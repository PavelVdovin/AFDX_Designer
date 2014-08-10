#include "virtualLinkConfigurator.h"
#include "partition.h"
#include "dataFlow.h"
#include "virtualLink.h"

#include <stdio.h>
#include <assert.h>
#include <math.h>

#define FRAME_TRANSMITION_APPROXIMATION 1
//#define EPS 0.00000001


VirtualLink* VirtualLinkConfigurator::designUnroutedVL(DataFlow* df) {
    if ( !df->getFrom() || !df->getFrom()->getConnected() || df->getTo().size() == 0 ) {
        printf("Data flow has no source/destinations specified\n");
        return 0;
    }

    if ( df->getMsgSize() <= 1471 ) {
        return generateVLOneFrame(df->getMsgSize(), df->getPeriod(), df->getTMax(), df->getMaxJitter());
    }

    return generateVLManyFrames(df->getMsgSize(), df->getPeriod(), df->getTMax(), df->getMaxJitter());
}

inline int min(int val1, int val2) {
    return val1 > val2 ? val2 : val1;
}

inline long min(long val1, long val2) {
    return val1 > val2 ? val2 : val1;
}

inline float min(float val1, float val2) {
    return val1 > val2 ? val2 : val1;
}

inline float max(float val1, float val2) {
    return val1 > val2 ? val1 : val2;
}

inline float max(float val1, float val2, float val3) {
    float val = max(val1, val2);
    return val > val3 ? val : val3;
}

// Check whether max message (with one-frame) jitter is satisfied
inline bool checkJmaxForOneFrame(int val, long period, long tMax, long jMax) {
    if ( !tMax )
        return true;

    if ( val <= (period - jMax) )
        return true;

    long sigma = tMax - FRAME_TRANSMITION_APPROXIMATION;
    if ( (val - (period - jMax)) > sigma ) {
        printf("Message jitter is not satisfied for bag = %d, period = %d, tMax = %d, jMax = %d\n",
                val, period, tMax, jMax);
        return false;
    }
    return true;
}

long getBag(int bagMax, long period, long tMax, long jMax) {
    int val = 128;
    while ( val > bagMax || !checkJmaxForOneFrame(val, period, tMax, jMax) )
        val = val >> 1;
    return val;
}

VirtualLink* VirtualLinkConfigurator::generateVLOneFrame(long msgSize, long period, long tMax, long jMax) {
    assert(msgSize <= 1471);
    // simple way to generate vl
    long lMax = msgSize + 47;
    if ( lMax < 64 )
        lMax = 64;

    // BagMax is min(period, tMax), bag is the closest value to BagMax, which is the power of 2.
    int bagMax = (int)period;
//    if ( tMax > 0 ) // tMax == 0 means not constrained time
//        bagMax = (int)min(period, tMax);

    int bag = getBag(bagMax, period, tMax, jMax);
    if ( !bag ) {
        printf("Cannot generate virtual link for data flow with parameters: msgSize: %d, period: %d, tMax: %d\n",
                msgSize, period, tMax);
        return 0;
    }

    printf("Generated virtual link for ONE-FRAME data flow with parameters: msgSize: %d, period: %d, tMax: %d\n",
                msgSize, period, tMax);
    printf("\tParams are: n_opt: %d, lmax: %d, bag: %d\n\n",
                1, lMax, bag);
    VirtualLink* vl = new VirtualLink();
    vl->setLMax(lMax);
    vl->setBag(bag);
    return vl;
}

// Returns upper module values
inline int modMax(float val) {
    int mod = (int)(val + EPS);
    return mod > (int)(val - 2 * EPS) ? mod : mod + 1;
}

// Check message jitter constraints
bool checkJMaxManyFrames(int n, int bag, long period, long sigma, long jMax ) {
    if ( !sigma || ((n*bag) <= (period - jMax)) )
        return true;

    if ( ((2*n-1)*bag) > (sigma + period - jMax) ) {
        printf("Message jitter is not satisfied for n = %d, bag = %d, period = %d, sigma = %d, jMax = %d\n",
                        n, bag, period, sigma, jMax);
        return false;
    }

    return true;
}

struct FramesParams {
    int number;
    int bag;
};
/*
 * Ni = sigma/bag + 1 or period/bag
 */
inline FramesParams identifyOptimalNumberOfFrames(int min_numberOfFrames, int max_numberOfFrames,
        long period, long sigma, long jMax) {
    int k = 256;
    FramesParams params;

    for ( int i = 7; i >= 0; --i ) {
        k = k >> 1;
        int opt_n = (int)((float)period/k);
        if ( sigma > 0 ) {
            int opt_by_sigma = (int)((float)sigma/k) + 1;
            if ( opt_by_sigma < opt_n )
                opt_n = opt_by_sigma;
        }
        if ( min_numberOfFrames <= opt_n && opt_n <= max_numberOfFrames
                && checkJMaxManyFrames(opt_n, k, period, sigma, jMax) ) {
            params.number = opt_n;
            params.bag = k;

            return params;
        } else if ( opt_n > max_numberOfFrames ) {
            printf("Optimal n is out of range.\n");
            params.number = max_numberOfFrames;
            params.bag = k;

            return params;
        }
    }
    // if we are here, then we should use max_numberOfFrames to minimize bandwidth
    printf("Reached the end of sequence while identifying opt number of frames.\n");
    params.number = max_numberOfFrames;
    params.bag = 1;

    if ( !checkJMaxManyFrames(params.number, params.bag, period, sigma, jMax) ) {
        // cannot generate vl
        params.number = 0;
    }
    return params;
}

VirtualLink* VirtualLinkConfigurator::generateVLManyFrames(long msgSize, long period, long tMax, long jMax) {
    long tFrame = FRAME_TRANSMITION_APPROXIMATION,
         sigma = 0;


    // Counting number of frames.
    // n_min = max([msg_size/1471]);
    // n_max = min(sigma + 1, period); (all values in ms);
    // where sigma is Tmax - tFrame
    int min_numberOfFrames = modMax((float)msgSize / 1471),
        max_numberOfFrames = period;

    if ( tMax > 0 ) {
        if ( tFrame >= tMax ) {
            printf("Cannot generate virtual link for data flow with parameters: msgSize: %d, period: %d, tMax: %d\n",
                                msgSize, period, tMax);
            return 0;
        }
        sigma = tMax - tFrame;
        max_numberOfFrames = min((long)max_numberOfFrames, sigma + 1);
    }

    if ( min_numberOfFrames > max_numberOfFrames ) {
        printf("Cannot generate virtual link for data flow with parameters: msgSize: %d, period: %d, tMax: %d\n",
                                        msgSize, period, tMax);
        return 0;
    }

    // Identifying the optimal number of frames

    // It is either counted by period, or by sigma. Count by period first
    FramesParams params = identifyOptimalNumberOfFrames(min_numberOfFrames, max_numberOfFrames, period, sigma, jMax);

    if ( params.number == 0 ) {
        return 0;
    }

    int opt_numberOfFrames = params.number,
        bag = params.bag,
        lMax = modMax((float)msgSize / opt_numberOfFrames) + 47;

    assert(bag > 0);

    printf("Generated virtual link for data flow with parameters: msgSize: %d, period: %d, tMax: %d\n",
            msgSize, period, tMax);
    printf("\tParams are: n_opt: %d, lmax: %d, bag: %d\n\n",
            opt_numberOfFrames, lMax, bag);

    VirtualLink* vl = new VirtualLink();
    vl->setLMax(lMax);
    vl->setBag(bag);
    return vl;
}

VirtualLink* VirtualLinkConfigurator::generateAggregatedVirtualLink(DataFlows& dataFlows) {
    DataFlows::iterator it = dataFlows.begin();
    int nMin = 0;
    long sigmaMin = 0,
         periodMin = 0,
         jMax = 0;

    for ( ; it != dataFlows.end(); ++it ) {
        DataFlow* df = *it;
        // nMin = sum([msg_size / 1471]);
        nMin += modMax((float)df->getMsgSize() / 1471);

        if ( df->getTMax() > 0 && ( sigmaMin == 0 || (df->getTMax() - FRAME_TRANSMITION_APPROXIMATION) < sigmaMin) )
            sigmaMin = df->getTMax() - FRAME_TRANSMITION_APPROXIMATION;

        if ( periodMin == 0 || df->getPeriod() < periodMin )
            periodMin = df->getPeriod();

        if ( df->getMaxJitter() > 0 && (jMax == 0 || df->getMaxJitter() > jMax) ) {
            jMax = df->getMaxJitter();
        }
    }

    long nMax = periodMin;
    if ( sigmaMin > 0 && (sigmaMin + 1) < nMax )
        nMax = sigmaMin + 1;

    if ( nMin > nMax ) {
        return 0;
    }

    FramesParams params = identifyOptimalNumberOfFrames(nMin, nMax, periodMin, sigmaMin, jMax);
    if ( params.number == 0 ) {
        return 0;
    }

    int opt_numberOfFrames = params.number,
        bag = params.bag,
        lMax = calculateGreedyLMax(dataFlows, opt_numberOfFrames, bag) + 47;

    if ( lMax < 64 )
        lMax = 64;

    assert(bag > 0);
    VirtualLink* vl = new VirtualLink();
    vl->setLMax(lMax);
    vl->setBag(bag);
    return vl;
}

int VirtualLinkConfigurator::calculateGreedyLMax(DataFlows& dataFlows, int opt_numberOfFrames, int bag) {
    std::map<DataFlow*, int> numberOfFrames;
    std::map<DataFlow*, long> framesSize;

    DataFlows::iterator it = dataFlows.begin();
    for ( ; it != dataFlows.end(); ++it ) {
        numberOfFrames[*it] = 1;
        framesSize[*it] = (*it)->getMsgSize();
    }

    int N0 = dataFlows.size();
    long result;
    for ( int i = N0 + 1; i <= opt_numberOfFrames; ++i ) {
        long maxMsgSize = 0;
        DataFlow* df = 0;
        it = dataFlows.begin();
        for ( ; it != dataFlows.end(); ++it ) {
            if ( maxMsgSize == 0 || maxMsgSize < framesSize[*it] ) {
                maxMsgSize = framesSize[*it];
                df = *it;
            }
        }
        assert( df != 0);
        framesSize[df] = modMax(((float)df->getMsgSize() / ++numberOfFrames[df]));
    }

    long maxMsgSize = 0;
    // calculate max msg size again
    it = dataFlows.begin();
    for ( ; it != dataFlows.end(); ++it ) {
        if ( maxMsgSize == 0 || maxMsgSize < framesSize[*it] ) {
            maxMsgSize = framesSize[*it];
        }
    }

    return maxMsgSize;
}
