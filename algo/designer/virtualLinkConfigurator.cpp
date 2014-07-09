#include "virtualLinkConfigurator.h"
#include "partition.h"
#include "dataFlow.h"
#include "virtualLink.h"

#include <stdio.h>
#include <assert.h>
#include <math.h>

#define FRAME_TRANSMITION_APPROXIMATION 1
#define EPS 0.00000001


VirtualLink* VirtualLinkConfigurator::designUnroutedVL(DataFlow* df) {
    if ( !df->getFrom() || !df->getFrom()->getConnected() || df->getTo().size() == 0 ) {
        printf("Data flow has no source/destinations specified\n");
        return 0;
    }

    if ( df->getMsgSize() <= 1471 ) {
        return generateVLOneFrame(df->getMsgSize(), df->getPeriod(), df->getTMax());
    }

    return generateVLManyFrames(df->getMsgSize(), df->getPeriod(), df->getTMax());
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

long getBag(int bagMax) {
    int val = 128;
    while ( val > bagMax )
        val = val >> 1;
    return val;
}

VirtualLink* VirtualLinkConfigurator::generateVLOneFrame(long msgSize, long period, long tMax) {
    assert(msgSize <= 1471);
    // simple way to generate vl
    long lMax = msgSize + 47;
    if ( lMax < 64 )
        lMax = 64;

    // BagMax is min(period, tMax), bag is the closest value to BagMax, which is the power of 2.
    int bagMax = (int)period;
    if ( tMax > 0 ) // tMax == 0 means not constrained time
        bagMax = (int)min(period, tMax);

    int bag = getBag(bagMax);
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

// Returns upper module valus
inline int modMax(float val) {
    int mod = (int)(val + EPS);
    return mod > (int)(val - 2 * EPS) ? mod : mod + 1;
}

struct FramesParams {
    int number;
    int bag;
};
/*
 * Ni = sigma/bag + 1 or period/bag
 */
inline FramesParams identifyOptimalNumberOfFrames(int min_numberOfFrames, int max_numberOfFrames, long period, long sigma) {
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
        if ( min_numberOfFrames <= opt_n && opt_n <= max_numberOfFrames ) {
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
    assert(0);
    params.number = max_numberOfFrames;
    params.bag = 1;
    return params;
}

VirtualLink* VirtualLinkConfigurator::generateVLManyFrames(long msgSize, long period, long tMax) {
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
    FramesParams params = identifyOptimalNumberOfFrames(min_numberOfFrames, max_numberOfFrames, period, sigma);
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
