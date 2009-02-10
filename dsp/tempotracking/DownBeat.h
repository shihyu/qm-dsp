/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM DSP Library

    Centre for Digital Music, Queen Mary, University of London.
    This file copyright 2008-2009 Matthew Davies and QMUL.
    All rights reserved.
*/

#ifndef DOWNBEAT_H
#define DOWNBEAT_H

#include <vector>

#include "dsp/rateconversion/Decimator.h"

using std::vector;

/**
 * This class takes an input audio signal and a sequence of beat
 * locations (calculated e.g. by TempoTrackV2) and estimates which of
 * the beat locations are downbeats (first beat of the bar).
 * 
 * The input audio signal is expected to have been downsampled to a
 * very low sampling rate (e.g. 2700Hz).  A utility function for
 * downsampling and buffering incoming block-by-block audio is
 * provided.
 */
class DownBeat
{
public:
    /**
     * Construct a downbeat locator that will operate on audio at the
     * downsampled by the given decimation factor from the given
     * original sample rate, plus beats extracted from the same audio
     * at the given original sample rate with the given frame
     * increment.
     *
     * decimationFactor must be a power of two no greater than 64, and
     * dfIncrement must be a multiple of decimationFactor.
     */
    DownBeat(float originalSampleRate,
             size_t decimationFactor,
             size_t dfIncrement);
    ~DownBeat();

    /**
     * Estimate which beats are down-beats.
     * 
     * audio contains the input audio stream after downsampling, and
     * audioLength contains the number of samples in this downsampled
     * stream.
     *
     * beats contains a series of beat positions expressed in
     * multiples of the df increment at the audio's original sample
     * rate, as described to the constructor.
     *
     * The returned downbeat array contains a series of indices to the
     * beats array.
     */
    void findDownBeats(const double *audio, // downsampled
                       size_t audioLength, // after downsampling
                       const vector<double> &beats,
                       vector<int> &downbeats);
    
    /**
     * For your downsampling convenience: call this function
     * repeatedly with input audio blocks containing dfIncrement
     * samples at the original sample rate, to decimate them to the
     * downsampled rate and buffer them within the DownBeat class.
     *     
     * Call getBufferedAudio() to retrieve the results after all
     * blocks have been processed.
     */
    void pushAudioBlock(const double *audio);
    
    /**
     * Retrieve the accumulated audio produced by pushAudioBlock calls.
     */
    const double *getBufferedAudio(size_t &length) const;

private:
    typedef vector<int> i_vec_t;
    typedef vector<vector<int> > i_mat_t;
    typedef vector<double> d_vec_t;
    typedef vector<vector<double> > d_mat_t;

    void makeDecimators();
    double measureSpecDiff(d_vec_t oldspec, d_vec_t newspec);

    float m_rate;
    size_t m_factor;
    size_t m_increment;
    Decimator *m_decimator1;
    Decimator *m_decimator2;
    double *m_buffer;
    double *m_decbuf;
    size_t m_bufsiz;
    size_t m_buffill;
    size_t m_beatframesize;
    double *m_beatframe;
    double *m_fftRealOut;
    double *m_fftImagOut;
};

#endif
