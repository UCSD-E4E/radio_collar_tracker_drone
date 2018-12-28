#ifndef __SDR_H__
#define __SDR_H__

#include "iq_data.hpp"
#include <uhd.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace RTT{
	class SDR{

		uhd_usrp_handle usrp;

		std::string device_args;
		std::string subdev;
		std::string ant;
		std::string ref;
		std::string cpu_format;
		std::string wire_format;
		size_t channel;
		double if_gain;
		long int rx_rate;
		long int rx_freq;
		void streamer(const volatile bool* die);
		std::queue<IQdataPtr>* output_queue;
		std::mutex* output_mutex;
		std::condition_variable* output_var;
		std::thread* stream_thread;

		uhd_rx_streamer_handle rx_streamer;
		std::string uhd_strerror(uhd_error err);
	protected:
		SDR();
	public:
		const size_t rx_buffer_size = 16384;
		SDR(double gain, long int rate, long int freq);
		~SDR();
		void setBufferSize(size_t buff_size);
		int getBufferSize();
		void startStreaming(std::queue<IQdataPtr>&, std::mutex&, 
			std::condition_variable&, const volatile bool* ndie);
		void stopStreaming();
	};
}

#endif