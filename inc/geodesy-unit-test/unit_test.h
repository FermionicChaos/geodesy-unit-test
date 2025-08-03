#pragma once
#ifndef GEODESY_BLTN_APP_UNIT_TEST_H
#define GEODESY_BLTN_APP_UNIT_TEST_H

#include <geodesy/engine.h>
#include <geodesy/bltn.h>

namespace geodesy::bltn {

	class unit_test : public runtime::app {
	public:

		std::shared_ptr<core::gpu::context> 		DeviceContext; 		// Device Context which will be used for all gfx and computation operations.
		std::shared_ptr<bltn::obj::system_window>	Window; 			// Main Application Window.

		unit_test(engine* aEngine);
		~unit_test();

		void run() override;

		void math_test();
		void create_worlds();

	};

}

#endif // GEODESY_BLTN_APP_UNIT_TEST_H