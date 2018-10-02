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

#ifndef INCLUDED_SOAPY_SINK_IMPL_H
#define INCLUDED_SOAPY_SINK_IMPL_H

#include <soapy/sink.h>
#include <boost/bind.hpp>

#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Device.hpp>

typedef boost::function<void(pmt::pmt_t , size_t)> cmd_handler_t;

namespace gr {
  namespace soapy {

    class sink_impl : public sink
    {
     private:
      SoapySDR::Device* d_device;
      SoapySDR::Stream* d_stream;

      size_t d_mtu;
      std::vector<const void*> d_bufs;
      pmt::pmt_t d_message_port;

      float d_frequency;
      float d_gain;
      float d_sampling_rate;
      float d_bandwidth;
      std::string d_antenna;
      size_t d_channel;
      gr_complex d_dc_offset;
      bool d_dc_offset_auto_mode;
      double d_frequency_correction;
      gr_complex d_iq_balance;
      bool d_gain_mode;
      double d_clock_rate;
      std::string d_clock_source;
      std::string d_frontend_mapping;

      void register_msg_cmd_handler(const pmt::pmt_t &cmd, cmd_handler_t handler);
      std::map<pmt::pmt_t, cmd_handler_t> d_cmd_handlers;

     public:
      sink_impl(float frequency, float gain, float sampling_rate,
                float bandwidth, const std::string antenna, size_t channel,
                gr_complexd dc_offset, bool dc_offset_mode,
                double correction, gr_complexd balance,
                const std::string clock_source, const std::string device);
      ~sink_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      /*!
       * Create and store a new Device object using the make function of SoapySDR
       * API.
       * For every makeDevice call an unmakeDevice call is also made.
       *
       * \param argStr device construction key/value argument string
       * \return integer indicating success or failure
       */
      int makeDevice(const std::string &argStr);

      /*!
       * Destroy a Device object created by makeDevice call.
       * Called for every makeDevice call.
       * \param dev a pointer to a Device object
       * \return integer indicating success or failure
       */
      int unmakeDevice(SoapySDR::Device* dev);

      /*!
       * Set the center frequency for the specified TX chain.
       * Default implementation tunes RF component frequency as close as
       * possible to the requested frequency. See specific device module
       * for more information.
       * \param channel an available channel on the device
       * \param frequency center frequency in Hz
       */
      void set_frequency (size_t channel, float frequency);

      /*!
       * Set the overall gain for the specified TX chain.
       * The gain will be distributed automatically across available
       * elements according to Soapy API.
       * \param channel an available channel on the device
       * \param gain the new amplification value in dB
       */
      void set_gain(size_t channel, float gain);

      /*!
       * Set the automatic gain mode for the TX chain if supported.
       * If not supported set gain value manually.
       * \param channel an available channel on the device
       * \param gain the new amplification value in dB
       * \param automatic true for automatic gain mode
       */
      void set_gain_mode(size_t channel, float gain, bool automatic);

      /*!
       * Set the baseband sample rate for the TX chain.
       * \param channel an available channel on the device
       * \param sample_rate the sample rate samples per second
       */
      void set_sample_rate(size_t channel, float sample_rate);

      /*!
       * Set the baseband filter width of the TX chain
       * \param channel an available channel on the device
       * \param bandwidth the baseband filter width in Hz
       */
      void set_bandwidth(size_t channel, float bandwidth);

      /*!
       * Set the antenna element for the TX chain.
       * \param channel an available channel on the device
       * \param name the name of an available antenna
       */
      void set_antenna(size_t channel, const std::string &name);

      /*!
       * Set the dc offset correction for the TX chain.
       * If the dc offset correction automatic mode is on
       * the value is omitted and the device sets the dc offset
       * correction automatically.
       * \param channel an available channel on the device
       * \param dc_offset the relative correction (1.0 max)
       * \param dc_offset_auto_mode true for automatic dc offset correction
       */
      void set_dc_offset(size_t channel, gr_complexd dc_offset, bool dc_offset_auto_mode);

      /*!
       * Set automatic dc offset correction to the TX chain
       * if supported by the device.
       * \param channel an available channel on the device
       * \param dc_offset_auto_mode true for automatic dc offset correction
       */
      void set_dc_offset_mode(size_t channel, bool dc_offset_auto_mode);

      /*!
       * Set the frequency correction to the TX chain.
       * @param channel an available channel on the device
       * @param freq_correction the correction value in PPM
       */
      void set_frequency_correction(size_t channel, double freq_correction);

      /*!
       * Set iq balance correction to the TX chain
       * \param channel an available channel on the device
       * \param iq_balance the relative correction (1.0 max)
       */
      void set_iq_balance(size_t channel, gr_complexd iq_balance);

      /*!
       * Set the master clock rate of the device
       * \param clock_rate the clock rate in Hz
       */
      void set_master_clock_rate(double clock_rate);

      /*!
       * Set the clock source of the device
       * \param clock_source the name of clock source
       */
      void set_clock_source(const std::string &clock_source);

      /*!
       * Set the frontend mapping of available DSP units to RF frontends.
       * This mapping controls channel mapping and channel availability.
       * \param frontend_mapping a vendor-specific mapping string
       */
      void set_frontend_mapping(const std::string &frontend_mapping);

      /*!
       * Get the down conversion frequency of the TX chain.
       * \param channel an available channel on the device
       * \return the center frequency in Hz
       */
      double get_frequency(size_t channel);

      /*!
       * Get the overall value of the gain elements of the TX chain
       * \param channel an available channel on the device
       * \return the value of the gain in dB
       */
      double get_gain(size_t channel);

      /*!
       * Get the automatic gain mode on the TX chain.
       * \param channel an available channel on the device
       * \return true for automatic gain setting
       */
      bool get_gain_mode(size_t channel);

      /*!
       * Get the baseband sample rate of the TX chain.
       * \param channel an available channel on the device
       * \return the sample rate in samples per second
       */
      double get_sampling_rate(size_t channel);

      /*!
       * Get baseband filter width of the TX chain.
       * \param channel an available channel on the device
       * \return the baseband filter width in Hz
       */
      double get_bandwidth(size_t channel);

      /*!
       * Get the selected antenna on TX chain.
       * \param channel an available channel on the device
       * \return the name of the selected antenna
       */
      std::string get_antenna(size_t channel);

      /*!
       * Get the DC offset correction.
       * \param channel an available channel on the device
       * \return the relative correction (1.0 max)
       */
      std::complex<double> get_dc_offset(size_t channel);

      /*!
       * Get the automatic DC offset correction mode.
       * \param channel an available channel on the device
       * \return true for automatic offset correction
       */
      bool get_dc_offset_mode(size_t channel);

      /*!
       * Get the frequency correction value.
       * \param channel an available channel on the device
       * \return the correction value in PPM
       */
      double get_frequency_correction(size_t channel);

      /*!
       * Get the IQ balance correction.
       * \param channel an available channel on the device
       * \return the relative correction (1.0 max)
       */
      std::complex<double> get_iq_balance(size_t channel);

      /*!
       * Get the master clock rate of the device.
       * \return the clock rate in Hz
       */
      double get_master_clock_rate();

      /*!
       * Get the clock source of the device
       * \return the name of the clock source
       */
      std::string get_clock_source();

      /*!
       * Calls the correct message handler according to the received message symbol.
       * A dictionary with key the handler name is used in order to call the
       * corresponding handler.
       * \param msg a PMT dictionary
       */
       void msg_handler_command(pmt::pmt_t msg);

      /*!
       * Set the center frequency of the RX chain.
       * @param val center frequency in Hz
       * @param chann an available channel on the device
       */
       void cmd_handler_frequency(pmt::pmt_t val, size_t chann);

      /*!
       * Set the overall gain for the specified chain.
       * The gain will be distributed automatically across available
       * elements according to Soapy API.
       * @param val the new amplification value in dB
       * @param chann an avalaible channel on the device
       */
       void cmd_handler_gain(pmt::pmt_t val, size_t chann);

      /*!
       * Set the baseband sample rate for the RX chain.
       * @param val the sample rate samples per second
       * @param chann an available channel on the device
       */
       void cmd_handler_samp_rate(pmt::pmt_t val, size_t chann);

      /*!
       * Set the baseband filter width for the TX chain.
       * @param val baseband filter width in Hz
       * @param chann an available channel on the device
       */
       void cmd_handler_bw(pmt::pmt_t val, size_t chann);

      /*!
       * Set the anntena element for the TX chain.
       * @param val name of the anntena
       * @param chann an available channel on the device
       */
       void cmd_handler_antenna(pmt::pmt_t val, size_t chann);
    };
  } // namespace soapy
} // namespace gr

#endif /* INCLUDED_SOAPY_SINK_IMPL_H */

