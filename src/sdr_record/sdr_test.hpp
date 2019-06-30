#ifndef __SDR_TEST_H__
#define __SDR_TEST_H__

#include "iq_data.hpp"
#include "AbstractSDR.hpp"
// #include "sdr.hpp"
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <fstream>
#include <thread>
namespace RTT{
	class SDR_TEST final : public AbstractSDR{
	private:
		std::string _input_dir;
		std::fstream _stream;
		std::vector<std::string> _files;
		std::size_t _buffer_size;

		std::thread* _thread;
		std::condition_variable* _input_cv;

		std::size_t _sampling_freq;

		volatile bool _run;
		volatile bool& _p_run;

	public:
		void _process(std::queue<std::complex<double>*>&, std::mutex&, 
			std::condition_variable&);
		SDR_TEST(std::string input_dir, volatile bool& program_run);
		~SDR_TEST();

		void setBufferSize(size_t buff_size);
		int getBufferSize();

		void startStreaming(std::queue<std::complex<double>*>&, std::mutex&, 
			std::condition_variable&);
		void stopStreaming();

		const std::size_t getStartTime_ms() const;

		static int getRunNum(const std::string input_dir);
		static uint64_t getRxFreq(const std::string input_dir);
		static uint64_t getRate(const std::string input_dir);
	};
}

#endif
