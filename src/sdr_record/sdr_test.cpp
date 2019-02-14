/**
 * @file sdr_test.cpp
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * This file provides a software interface to allow testing from existing
 * datasets.  This should be able to provide a drop-in replacement for the
 * actual sdr_record app.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sdr_test.hpp"
#include <glob.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <chrono>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>

// #define DEBUG

#ifdef DEBUG
#include <fstream>
#endif

namespace RTT{
	SDR_TEST::SDR_TEST(std::string input_dir) : 
		_input_dir(input_dir),
		_stream(),
		_files(),
		_buffer_size(16384),
		_sampling_freq(2000000){
		glob_t glob_output;
		std::ostringstream globfilestream{};
		globfilestream << _input_dir << "/" << "RAW_DATA*";
		if(glob(globfilestream.str().c_str(), 0, nullptr, &glob_output)){
			throw new std::invalid_argument("Could not execute glob");
		}
		for(std::size_t i = 0; i < glob_output.gl_pathc; i++){
			_files.push_back(std::string(glob_output.gl_pathv[i]));
		}
		globfree(&glob_output);
		std::cout << "Using test module" << std::endl;
	}

	SDR_TEST::~SDR_TEST(){

	}

	void SDR_TEST::setBufferSize(size_t buff_size){
		_buffer_size = buff_size;
	}
	int SDR_TEST::getBufferSize(){
		return _buffer_size;
	}

	void SDR_TEST::startStreaming(std::queue<IQdataPtr>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv, 
		const volatile bool* ndie){

		syslog(LOG_INFO, "SDR Test starting threads");

		_thread = new std::thread(&SDR_TEST::_process, this, 
			std::ref(input_queue), std::ref(input_mutex), std::ref(input_cv),
			ndie);
	}
	void SDR_TEST::stopStreaming(){
		_thread->join();
		delete _thread;
	}

	void SDR_TEST::_process(std::queue<IQdataPtr>& data_queue, 
		std::mutex& data_mutex, std::condition_variable& data_cv, 
		const volatile bool* ndie){
		std::size_t sample_count = 0;
		std::size_t buffer_count = 0;
		std::size_t ms_per_buffer = (std::size_t)((double)_buffer_size / 
			_sampling_freq * 1000);

		std::int16_t buffer[2 * _buffer_size];

		#ifdef DEBUG
		std::ofstream _ostr{"sdr_test.log"};
		#endif

		auto start = std::chrono::steady_clock::now();

		for(std::size_t i = 0; i < 1; i++){
			_stream.open(_files[i].c_str(), std::ios::binary | std::ios::in);
			if(!_stream){
				std::cout << "Stream not ready!" << std::endl;
				std::cout << _stream.good() << std::endl;
				std::cout << _stream.eof() << std::endl;
				std::cout << _stream.fail() << std::endl;
				std::cout << _stream.bad() << std::endl;
				return;
			}
			struct stat sb;
			if(stat(_files[i].c_str(), &sb)){
				continue;
			}
			std::size_t file_size = sb.st_size;
			std::cout << "Opening " << _files[i] << std::endl;
			std::size_t num_samples = file_size / (sizeof(int16_t) * 2);
			std::cout << "samples " << file_size << std::endl;
			for(std::size_t j = 0; j < num_samples / _buffer_size; j++){
				// std::cout << "Reading frame " << j << " of " << num_samples / 
				// 	_buffer_size << std::endl;
				IQdataPtr databuf (new IQdata(_buffer_size));
				databuf->time_ms = (std::size_t)((double)sample_count / 
					_sampling_freq * 1000);
				if(!_stream){
					std::cout << "Stream not ready!" << std::endl;
					std::cout << _stream.good() << std::endl;
					std::cout << _stream.eof() << std::endl;
					std::cout << _stream.fail() << std::endl;
					std::cout << _stream.bad() << std::endl;
					return;
				}
				_stream.read((char*) buffer, sizeof(std::int16_t) * 2 * 
					_buffer_size);
				for(std::size_t k = 0; k < _buffer_size; k++){
					databuf->data->at(k) = std::complex<short>(buffer[2 * k], 
						buffer[2 * k + 1]);
					// std::cout << buffer[2 * k] << ", " << buffer[2 * k + 1] << std::endl;
					sample_count++;
					#ifdef DEBUG
					_ostr << buffer[2*k];
					if(buffer[2*k+1] >= 0){
						_ostr << "+";
					}
					_ostr << buffer[2*k+1] << "i" << std::endl;
					#endif
				}
				std::unique_lock<std::mutex> olock(data_mutex);
				data_queue.push(databuf);
				buffer_count++;
				olock.unlock();
				data_cv.notify_all();
				syslog(LOG_NOTICE, "SDR output %d", _buffer_size);
				auto measure = std::chrono::steady_clock::now();
				auto diff = measure - start;
				syslog(LOG_NOTICE, "SDR sleeping");
				usleep(ms_per_buffer * 1000);
				// if(!*ndie){
				// 	_stream.close();
				// 	return;
				// }
			}
			std::cout << "issued all samples for file" << std::endl;

			_stream.close();
		}
		#ifdef DEBUG
		_ostr.close();
		#endif
		std::cout << "TEST SDR issued " << buffer_count << " data packets" << std::endl;
		std::cout << "Test SDR output " << sample_count << " samples" << std::endl;
		std::cout << "You can stop now" << std::endl;
	}

}