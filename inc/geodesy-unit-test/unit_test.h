#pragma once
#ifndef GEODESY_UNIT_TEST_H
#define GEODESY_UNIT_TEST_H

#include <geodesy/engine.h>

namespace geodesy {

	class unit_test : public runtime::app {
	public:

		// Engine Pre-Initialization phase.
		static engine::config initialize(std::set<std::string> aCommandLineArguments);
		static void terminate();

		// Application Data
		std::shared_ptr<gpu::context> Context;

		// Application Initialization.
		unit_test(engine* aEngine);
		~unit_test();

		void math_test();
		void create_worlds();

	};

}

#endif // GEODESY_UNIT_TEST_H