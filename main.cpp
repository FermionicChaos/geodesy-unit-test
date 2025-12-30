#include <iostream>

// Includes core engine, and base types.
#include <geodesy/engine.h>

// Include Unit Test
#include <geodesy-unit-test/unit_test.h>

// Using entry point for app.
int main(int aCmdArgCount, char* aCmdArgList[]) {

	// Initialize all third party libraries.
	if (!geodesy::engine::initialize()) return -1;

	std::set<std::string> CommandLineArguments;
	for (int i = 0; i < aCmdArgCount; i++) {
		CommandLineArguments.insert(aCmdArgList[i]);
	}

	for (const auto& Argument : CommandLineArguments) {
		std::cout << "CommandLineArg = " << Argument << std::endl;
	}

	// ===== Load Engine Layers ===== //
	std::set<std::string> LayerList = {};
	// Insert engine layers into the layer list.
	LayerList.insert(geodesy::engine::EngineLayersModule.begin(), geodesy::engine::EngineLayersModule.end());
	// Insert System window layers into the layer list.
	// LayerList.insert(system_window::EngineLayersModule.begin(), system_window::EngineLayersModule.end());

	// ===== Load Engine Extensions ===== //
	std::set<std::string> ExtensionList = {};
	// Insert engine extensions into the extension list.
	ExtensionList.insert(geodesy::engine::EngineExtensionsModule.begin(), geodesy::engine::EngineExtensionsModule.end());
	// Insert System window extensions into the extension list.
	// ExtensionList.insert(system_window::EngineExtensionsModule.begin(), system_window::EngineExtensionsModule.end());
	// Load in XR Extensions.
	// ExtensionList.insert(cameravr::EngineExtensionsModule.begin(), cameravr::EngineExtensionsModule.end());

	try {
		// Initialize Engine
		geodesy::engine Engine(CommandLineArguments, LayerList, ExtensionList);
		{
			// Initialize User App
			geodesy::unit_test UnitTest(&Engine);

			// Run User App
			// Engine.run(&UnitTest);
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
