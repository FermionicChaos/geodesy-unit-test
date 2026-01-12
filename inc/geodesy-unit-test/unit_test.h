#pragma once
#ifndef GEODESY_UNIT_TEST_H
#define GEODESY_UNIT_TEST_H

#include <geodesy/engine.h>

namespace geodesy {

	class unit_test : public runtime::app {
	public:

		std::shared_ptr<gpu::context> Context;

		unit_test(engine* aEngine);
		~unit_test();

		void math_test();
		void create_worlds();

	};

}

#endif // GEODESY_BLTN_APP_UNIT_TEST_H