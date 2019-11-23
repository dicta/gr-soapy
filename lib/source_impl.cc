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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "source_impl.h"

const pmt::pmt_t CMD_CHAN_KEY = pmt::mp ("chan");
const pmt::pmt_t CMD_FREQ_KEY = pmt::mp ("freq");
const pmt::pmt_t CMD_GAIN_KEY = pmt::mp ("gain");
const pmt::pmt_t CMD_ANTENNA_KEY = pmt::mp ("antenna");
const pmt::pmt_t CMD_RATE_KEY = pmt::mp ("samp_rate");
const pmt::pmt_t CMD_BW_KEY = pmt::mp ("bw");

namespace gr
{
  namespace soapy
  {
    source::sptr
    source::make (size_t nchan, const std::string device,
                  const std::string args, float sampling_rate,
                  const std::string type)
    {
      return gnuradio::get_initial_sptr (
          new source_impl (nchan, device, args, sampling_rate, type));
    }

    /*
     * The private constructor
     */
    source_impl::source_impl (size_t nchan, const std::string device,
                              const std::string args, float sampling_rate,
                              const std::string type) :
            gr::sync_block ("source", gr::io_signature::make (0, 0, 0),
                            args_to_io_sig (type, nchan)),
            d_mtu (0),
            d_message_port (pmt::mp ("command")),
            d_sampling_rate (sampling_rate),
            d_nchan (nchan),
            d_type (type)
    {
      if (type == "fc32") {
        d_type_size = 8;
        d_type = "CF32";
      }
      if (type == "s16") {
        d_type_size = 2;
        d_type = "S16";
      }
      SoapySDR::Kwargs dev_args = SoapySDR::KwargsFromString (args);
      makeDevice (device);
      std::vector<size_t> channs;
      channs.resize (d_nchan);
      for (size_t i = 0; i < d_nchan; i++) {
        channs[i] = i;
        set_sample_rate (i, d_sampling_rate);
      }

      d_stream = d_device->setupStream (SOAPY_SDR_RX, d_type, channs, dev_args);
      d_mtu = d_device->getStreamMTU (d_stream);

      /* Apply device settings */
      for(const std::pair<std::string, std::string>& iter: dev_args) {
        d_device->writeSetting(iter.first, iter.second);
      }

      message_port_register_in (d_message_port);
      set_msg_handler (
          d_message_port,
          boost::bind (&source_impl::msg_handler_command, this, _1));

      register_msg_cmd_handler (
          CMD_FREQ_KEY,
          boost::bind (&source_impl::cmd_handler_frequency, this, _1, _2));
      register_msg_cmd_handler (
          CMD_GAIN_KEY,
          boost::bind (&source_impl::cmd_handler_gain, this, _1, _2));
      register_msg_cmd_handler (
          CMD_RATE_KEY,
          boost::bind (&source_impl::cmd_handler_samp_rate, this, _1, _2));
      register_msg_cmd_handler (
          CMD_BW_KEY, boost::bind (&source_impl::cmd_handler_bw, this, _1, _2));
      register_msg_cmd_handler (
          CMD_ANTENNA_KEY,
          boost::bind (&source_impl::cmd_handler_antenna, this, _1, _2));

      set_max_noutput_items (d_mtu);
    }

    source_impl::~source_impl ()
    {
      unmakeDevice (d_device);

    }

    void
    source_impl::register_msg_cmd_handler (const pmt::pmt_t &cmd,
                                           cmd_handler_t handler)
    {
      d_cmd_handlers[cmd] = handler;
    }

    int
    source_impl::makeDevice (const std::string &argStr)
    {
      try {
        d_device = SoapySDR::Device::make (argStr);
      }
      catch (const std::exception &ex) {
        std::cerr << "Error making device: " << ex.what () << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }

    int
    source_impl::unmakeDevice (SoapySDR::Device* dev)
    {
      try {
        SoapySDR::Device::unmake (dev);
      }
      catch (const std::exception &ex) {
        std::cerr << "Error unmaking device: " << ex.what () << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }

    void
    source_impl::set_frequency (size_t channel, float frequency)
    {
      d_device->setFrequency (SOAPY_SDR_RX, channel, frequency);
      d_frequency = d_device->getFrequency (SOAPY_SDR_RX, channel);
    }

    void
    source_impl::set_frequency (size_t channel, const std::string &name,
                                float frequency)
    {
      d_device->setFrequency (SOAPY_SDR_RX, channel, name, frequency);
    }

    void
    source_impl::set_gain (size_t channel, float gain)
    {
      d_device->setGain (SOAPY_SDR_RX, channel, gain);
      d_gain = d_device->getGain (SOAPY_SDR_RX, channel);
    }

    void
    source_impl::set_gain (size_t channel, const std::string name, float gain)
    {
      d_device->setGain (SOAPY_SDR_RX, channel, name, gain);
      d_gain = d_device->getGain (SOAPY_SDR_RX, channel);
    }

    void
    source_impl::set_gain_mode (size_t channel, bool gain_auto_mode)
    {
      d_device->setGainMode (SOAPY_SDR_RX, channel, gain_auto_mode);
    }

    void
    source_impl::set_sample_rate (size_t channel, float sample_rate)
    {
      d_device->setSampleRate (SOAPY_SDR_RX, channel, sample_rate);
      d_sampling_rate = sample_rate;
    }

    void
    source_impl::set_bandwidth (size_t channel, float bandwidth)
    {
      d_device->setBandwidth (SOAPY_SDR_RX, channel, bandwidth);
      d_bandwidth = bandwidth;
    }

    void
    source_impl::set_antenna (const size_t channel, const std::string &name)
    {
      d_device->setAntenna (SOAPY_SDR_RX, channel, name);
      d_antenna = name;
    }

    void
    source_impl::set_dc_offset (size_t channel, gr_complexd dc_offset,
                                bool dc_offset_auto_mode)
    {
      /* If DC Correction is supported but automatic mode is not set DC correction */
      if (!dc_offset_auto_mode
          && d_device->hasDCOffset (SOAPY_SDR_RX, channel)) {
        d_device->setDCOffset (SOAPY_SDR_TX, channel, dc_offset);
        d_dc_offset = dc_offset;
      }
    }

    void
    source_impl::set_dc_offset_mode (size_t channel, bool dc_offset_auto_mode)
    {
      /* If user specifies automatic DC Correction and is supported activate it */
      if (dc_offset_auto_mode
          && d_device->hasDCOffsetMode (SOAPY_SDR_RX, channel)) {
        d_device->setDCOffsetMode (SOAPY_SDR_RX, channel, dc_offset_auto_mode);
        d_dc_offset_auto_mode = dc_offset_auto_mode;
      }
    }

    void
    source_impl::set_frequency_correction (size_t channel,
                                           double freq_correction)
    {
      /* If the device supports Frequency correction set value */
      if (d_device->hasFrequencyCorrection (SOAPY_SDR_RX, channel)) {
        d_device->setFrequencyCorrection (SOAPY_SDR_RX, channel,
                                          freq_correction);
        d_frequency_correction = freq_correction;
      }
    }

    void
    source_impl::set_iq_balance (size_t channel, gr_complexd iq_balance)
    {
      /* If the device supports IQ blance correction set value */
      if (d_device->hasIQBalance (SOAPY_SDR_RX, channel)) {
        d_device->setIQBalance (SOAPY_SDR_RX, channel, iq_balance);
        d_iq_balance = iq_balance;
      }
    }

    void
    source_impl::set_master_clock_rate (double clock_rate)
    {
      d_device->setMasterClockRate (clock_rate);
      d_clock_rate = clock_rate;
    }

    void
    source_impl::set_clock_source (const std::string &clock_source)
    {
      d_device->setClockSource (clock_source);
      d_clock_source = clock_source;
    }

    void
    source_impl::set_frontend_mapping (const std::string &mapping)
    {
      d_device->setFrontendMapping (SOAPY_SDR_RX, mapping);
    }

    double
    source_impl::get_frequency (size_t channel)
    {
      return d_device->getFrequency (SOAPY_SDR_RX, channel);
    }

    double
    source_impl::get_gain (size_t channel)
    {
      return d_device->getGain (SOAPY_SDR_RX, channel);
    }

    bool
    source_impl::get_gain_mode (size_t channel)
    {
      return d_device->getGainMode (SOAPY_SDR_RX, channel);
    }

    double
    source_impl::get_sampling_rate (size_t channel)
    {
      return d_device->getSampleRate (SOAPY_SDR_RX, channel);
    }

    double
    source_impl::get_bandwidth (size_t channel)
    {
      return d_device->getBandwidth (SOAPY_SDR_RX, channel);
    }

    std::string
    source_impl::get_antenna (size_t channel)
    {
      return d_device->getAntenna (SOAPY_SDR_RX, channel);
    }

    std::complex<double>
    source_impl::get_dc_offset (size_t channel)
    {
      return d_device->getDCOffset (SOAPY_SDR_RX, channel);
    }

    bool
    source_impl::get_dc_offset_mode (size_t channel)
    {
      return d_device->getDCOffsetMode (SOAPY_SDR_RX, channel);
    }

    double
    source_impl::get_frequency_correction (size_t channel)
    {
      return d_device->getFrequencyCorrection (SOAPY_SDR_RX, channel);
    }

    std::complex<double>
    source_impl::get_iq_balance (size_t channel)
    {
      return d_device->getIQBalance (SOAPY_SDR_RX, channel);
    }

    double
    source_impl::get_master_clock_rate ()
    {
      return d_device->getMasterClockRate ();
    }

    std::string
    source_impl::get_clock_source ()
    {
      return d_device->getClockSource ();
    }

    void
    source_impl::cmd_handler_frequency (pmt::pmt_t val, size_t chann)
    {
      set_frequency (chann, pmt::to_float (val));
    }

    void
    source_impl::cmd_handler_gain (pmt::pmt_t val, size_t chann)
    {
      set_gain (chann, pmt::to_float (val));
    }

    void
    source_impl::cmd_handler_samp_rate (pmt::pmt_t val, size_t chann)
    {
      set_sample_rate (chann, pmt::to_float (val));
    }

    void
    source_impl::cmd_handler_bw (pmt::pmt_t val, size_t chann)
    {
      set_bandwidth (chann, pmt::to_float (val));
    }

    void
    source_impl::cmd_handler_antenna (pmt::pmt_t val, size_t chann)
    {
      set_antenna (chann, pmt::symbol_to_string (val));
    }

    int
    source_impl::work (int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      int flags = 0;
      long long timeNs = 0;
      int read = d_device->readStream (d_stream, &output_items[0], noutput_items,
                                       flags, timeNs);
      // Tell runtime system how many output items we produced.
      if (read < 0) return 0;
      return read;
    }

    void
    source_impl::msg_handler_command (pmt::pmt_t msg)
    {
      if (!pmt::is_dict (msg)) {
        return;
      }
      size_t chann = 0;
      if (pmt::dict_has_key (msg, CMD_CHAN_KEY)) {
        chann = pmt::to_long (
            pmt::dict_ref (msg, CMD_CHAN_KEY, pmt::from_long (0)));
        pmt::dict_delete (msg, CMD_CHAN_KEY);
      }
      for (size_t i = 0; i < pmt::length (msg); i++) {
        d_cmd_handlers[pmt::car (pmt::nth (i, msg))] (
            pmt::cdr (pmt::nth (i, msg)), chann);
      }
    }

    bool
    source_impl::start()
    {
      return (0 == d_device->activateStream(d_stream));
    }

    bool
    source_impl::stop()
    {
      d_device->closeStream(d_stream);
      return true;
    }
  } /* namespace soapy */
} /* namespace gr */

