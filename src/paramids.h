/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Igor Zinken - https://www.igorski.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __PARAMIDS_HEADER__
#define __PARAMIDS_HEADER__

enum
{
    // ids for all visual controls

    kDelayTimeId = 0,         // delay time
    kDelayHostSyncId,         // delay host sync
    kDelayFeedbackId,         // delay feedback
    kDelayMixId,              // delay mix

    kBitResolutionId,         // bit resolution
    kBitResolutionChainId,    // bit resolution pre/post delay mix
    kLFOBitResolutionId,      // bit resolution LFO rate
    kLFOBitResolutionDepthId, // depth for bit resolution LFO

    kDecimatorId,             // decimator
    kDecimatorChainId,        // decimator pre/post delay mix
    kLFODecimatorId,          // decimator LFO rate

    kFilterChainId,           // filter pre/post delay mix
    kFilterCutoffId,          // filter cutoff
    kFilterResonanceId,       // filter resonance
    kLFOFilterId,             // filter LFO rate
    kLFOFilterDepthId,        // depth for filter LFO

    kFlangerChainId,          // flanger pre/post delay mix
    kFlangerRateId,           // flanger LFO rate
    kFlangerWidthId,          // flanger width
    kFlangerFeedbackId,       // flanger feedback
    kFlangerDelayId,          // flanger delay

    kVuPPMId                  // for the Vu value return to host
};

#endif
