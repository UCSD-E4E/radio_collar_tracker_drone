#include "localization.hpp"
#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <math.h>
#include <iostream>
#include "UTM.h"
#include <iomanip>

#define DEBUG

#ifdef DEBUG
#include <fstream>
#include <iostream>
#endif

namespace RTT{

	void PingLocalizer::process(std::queue<PingPtr>& queue, std::mutex& mutex,
		std::condition_variable& var, GPS& gps_module){
		
		
		#ifdef DEBUG
		std::ofstream _pings{"localizer_in.log"};
		std::ofstream _estimates{"localizer_out.log"};
		std::ofstream _bad_gps{"localizer_err.log"};
		std::ofstream _test{"localizer_check.log"};
		#endif

		while(run || !queue.empty()){
		// 	std::cout << "itr" << std::endl;
			std::unique_lock<std::mutex> inputLock(mutex);
			if(queue.empty() && pingQueue.empty()){
				var.wait(inputLock);
			}
			if(!queue.empty() || !pingQueue.empty()){
				// add ping to data matrix
				PingPtr pingptr;
				if(!queue.empty()){
					std::cout << "Got new point" << std::endl;
					pingptr = queue.front();
					queue.pop();
				}else if(!pingQueue.empty()){
					std::cout << "Got old point" << std::endl;
					pingptr = pingQueue.front();
					pingQueue.pop();
				}else{
					std::cout << "WTF?" << std::endl;
					continue;
				}
				inputLock.unlock();
				if(gps_module.getFirst_ms() == 0){
					std::cout << "Rejecting early ping, GPS reports init time of " << gps_module.getFirst_ms() << std::endl;
					continue;
				}
				Ping ping = *pingptr;
				if(ping.time_ms < gps_module.getFirst_ms()){
					std::cout << "Rejecting point at " << ping.time_ms << ", less than " << gps_module.getFirst_ms() << std::endl;
					continue;
				}
				const Location* loc = gps_module.getPositionAtMs(ping.time_ms);
				if(loc == nullptr){
					#ifdef DEBUG
					std::cout << "No GPS data at " << ping.time_ms << "!" << std::endl;
					_bad_gps << ping.time_ms << std::endl;
					#endif
					// pingQueue.push(pingptr);
					continue;
				}

				// convert to UTM

				// double northing;
				// double easting;
				// char zone[4];
				// UTM::LLtoUTM(loc->lat * 1e-7, loc->lon * 1e-7, northing, easting, zone);

				// #ifdef DEBUG
				std::cout << "Got ping at " << (int)(ping.time_ms / 1e3)
					<< " at " << loc->lat << ", " << loc->lon << std::endl;
				// _pings << ping.time_ms << ", " << ping.amplitude << ", " 
				// 	<< (long long)northing << ", " << (long long)easting << ", " << loc->alt 
				// 	<< std::endl;
				// #endif
				_out << "{\"ping\": {\"time\": " << ping.time_ms << ", ";
				_out << 		"\"lat\": " << std::setprecision(10) << loc->lat << ", ";
				_out << 		"\"lon\": " << std::setprecision(10) << loc->lon << ", ";
				_out <<			"\"alt\": " << std::setprecision(10) << loc->alt << ", ";
				_out << 		"\"amp\": " << std::setprecision(5) << ping.amplitude << ", ";
				_out << 		"\"txf\": " << std::setprecision(5) << ping.frequency << "}}" << std::endl;;

			}
		}
		#ifdef DEBUG
		_pings.close();
		_estimates.close();
		_bad_gps.close();
		_test.close();
		#endif

	}

	void PingLocalizer::start(std::queue<PingPtr>& queue, std::mutex& mutex, std::condition_variable& var, GPS& gps_module){
		_input_cv = &var;
		run = true;
		localizer_thread = new std::thread(&PingLocalizer::process, this, std::ref(queue), std::ref(mutex), std::ref(var), std::ref(gps_module));
	}

	void PingLocalizer::stop(){
		run = false;
		_input_cv->notify_all();
		localizer_thread->join();
		delete localizer_thread;
	}

	PingLocalizer::PingLocalizer(std::ostream& out) : _out{out}{
		
	}
	PingLocalizer::~PingLocalizer(){
		
	}
}