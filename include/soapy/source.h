/* -*- c++ -*- */
/*
 * gr-soapy: Soapy SDR Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018
 *  Libre Space Foundation <http://librespacefoundation.org/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef INCLUDED_SOAPY_SOURCE_H
#define INCLUDED_SOAPY_SOURCE_H

#include <soapy/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {


  namespace soapy {

    /*!
     * \addtogroup block
     * \brief <b>Source</b> block implements SoapySDR functionality for RX.
     * \ingroup soapy
     * \section source Soapy Source
     * The soapy source block receives samples and writes to a stream.
     * The source block also provides Soapy API calls for receiver settings.
     * Includes all parameters for full RX implementation.
     * Device is a string containing the driver and type name of the
     * device the user wants to use according to the Soapy* module
     * documentation.
     * Make parameters are passed through the xml block.
     * Some of the available parameters can be seen at Figure 2
     * Antenna and clock source can be left empty and default values
     * will be used.
     * This block has a message port, which consumes PMT messages.
     * For a description of the command syntax , see \ref cmd_handler.
     * \image html source_params.png "Figure 2"
     */
    class SOAPY_API source : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<source> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of soapy::source.
       *
       * To avoid accidental use of raw pointers, soapy::source's
       * constructor is in a private implementation
       * class. soapy::source::make is the public interface for
       * creating new instances.
       * \param nchan number of channels
       * \param device the device driver and type
       * \param args the arguments passed to the device
       * \param sampling_rate the sampling rate of the device
       * \param type output stream format
       *
       * Driver name can be any of "uhd", "lime", "airspy",
       * "rtlsdr" or others
       */
      static sptr make(size_t nchan, const std::string device, const std::string devname,
                       const std::string args,
                       float sampling_rate, const std::string type);

      virtual void set_overall_gain(size_t channel, float gain, bool manual_mode) = 0;

      virtual bool hasDCOffset(int channel) = 0;
      virtual bool hasIQBalance(int channel) = 0;
      virtual bool hasFrequencyCorrection(int channel) = 0;

      virtual std::vector<std::string> listAntennas(int channel) = 0;

      /*!
       * Callback to set overall gain
       * \param channel an available channel of the device
       * \param gain the overall gain value
       */
      virtual void set_gain(size_t channel, float gain) = 0;

      /*!
       * Callback to set specific gain value
       * \param channel an available channel on the device
       * \param name the gain name to set value
       * \param gain the gain value
       */
      virtual void set_gain(size_t channel, const std::string name, float gain, bool manual_mode) = 0;

      /*!
       * Callback to change the RF frequency of the device
       * \param channel an available channel of the device
       * \param freq the frequency to be set in Hz
       */
      virtual void set_frequency(size_t channel, float freq) = 0;

      /*!
       * Callback to change center frequency of a tunable element
       * \param channel an available channel of the device
       * \param name an available element name
       * \param frequency the frequency to be set in Hz
       */
      virtual void set_frequency(size_t channel, const std::string &name,
                                 float frequency) = 0;

      /*!
       * Callback to set automatic gain mode
       * \param channel an available channel on the device
       * \param gain_auto_mode true if automatic gain mode
       */
      virtual void set_gain_mode(size_t channel, bool gain_auto_mode) = 0;

      /*!
       * Callback to set sample rate
       * \param channel an available channel of the device
       * \param sample_rate number of samples in samples per second
       */
      virtual void set_sample_rate(size_t channel, float sample_rate) = 0;

      /*!
       * Callback to set digital filter bandwidth
       * \param channel an available channel on the device
       * \param bandwidth filter width in Hz
       */
      virtual void set_bandwidth(size_t channel, float bandwidth) = 0;

      /*!
       * Callback to set antenna for RF chain
       * \param channel an available channel of the device
       * \param name an available antenna string name
       */
      virtual void set_antenna(size_t channel, const std::string &name) = 0;

      /*!
       * Callback to set dc offset correction and mode
       * \param channel an available channel of the device
       * \param dc_offset complex for dc offset correction
       * \param dc_offset_auto_mode true if automatic dc offset correction
       */
      virtual void set_dc_offset(size_t channel, gr_complexd dc_offset,
                                 bool dc_offset_auto_mode) = 0;

      /*!
       * Callback to set automatic dc offset mode
       * \param channel an available channel of the device
       * \param dc_offset_auto_mode true if automatic dc offset correction
       */
      virtual void set_dc_offset_mode(size_t channel,
                                      bool dc_offset_auto_mode) = 0;

      /*!
       * Callback to set frequency correction
       * \param channel an available channel of the device
       * \param freq_correction relative value for frequency correction (1.0 max)
       */
      virtual void set_frequency_correction(size_t channel,
                                            double freq_correction) = 0;

      /*!
       * Callback to set iq balance correction
       * \param channel an available channel of the device
       * \param iq_balance complex value for iq balance correction
       */
      virtual void set_iq_balance(size_t channel, gr_complexd iq_balance) = 0;

      /*!
       * Callback to change master clock rate
       * \param clock_rate the clock rate in Hz
       */
      virtual void set_master_clock_rate(double clock_rate) = 0;

      /*!
       * Callback to set the clock source
       * \param clock_source an available clock source
       */
      virtual void set_clock_source(const std::string &clock_source) = 0;

    };

  } // namespace soapy
} // namespace gr

#endif /* INCLUDED_SOAPY_SOURCE_H */

