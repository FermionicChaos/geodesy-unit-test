#include <iostream>

// Includes core engine, and base types.
#include <geodesy/engine.h>

// Builtin Components
#include <geodesy/builtin.h>

// Example Application - Unit Test
#include <geodesy-unit-test/unit_test.h>

// Using entry point for app.
int main(int aCmdArgCount, char* aCmdArgList[]) {

	// Process command line arguments into a set for easy lookup.
	std::set<std::string> CommandLineArguments;
	for (int i = 0; i < aCmdArgCount; i++) {
		CommandLineArguments.insert(aCmdArgList[i]);
	}

	// Pre-Initialization phase, used for setting up engine configuration before the engine is initialized.
	geodesy::engine::config EngineConfig = geodesy::unit_test::initialize(CommandLineArguments);

	// Initialize all third party libraries needed by the engine.
	if (!geodesy::engine::initialize()) return -1;

	try {
		// Initialize Engine
		geodesy::engine Engine(CommandLineArguments, EngineConfig);
		{
			// Initialize User App
			geodesy::unit_test UnitTest(&Engine);

			// Run User App
			Engine.run(&UnitTest);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return -1;
	}

	// Terminate all third party libraries.
	geodesy::engine::terminate();

	return 0;
}
