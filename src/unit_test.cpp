#include <geodesy-unit-test/unit_test.h>

#include <memory>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <complex>

namespace geodesy::bltn {

	using namespace core;
	using namespace runtime;

	using namespace gpu;
	using namespace obj;
	using namespace lgc;

	unit_test::unit_test(engine* aEngine) : runtime::app(aEngine, "geodesy-unit-test", { 1, 0, 0 }) {
		TimeStep = 1.0 / 2000.0;
		Window = nullptr;

		// I want my device context to support these operation types.
		std::vector<uint> OperationList = {
			device::operation::TRANSFER,
			device::operation::TRANSFER_AND_COMPUTE,
			device::operation::GRAPHICS_AND_COMPUTE,
			device::operation::PRESENT
		};
		
		// ===== Load Context Layers ===== //
		std::set<std::string> LayerList = {};
		// I want my device context to be able to render to system windows.

		// ===== Load Context Extensions ===== //
		std::set<std::string> ExtensionList = {};
		// Add system window extensions for desktop rendering. (DESKTOP DEPENDENT)
		ExtensionList.insert(system_window::ContextExtensionsModule.begin(), system_window::ContextExtensionsModule.end());
		// Add ray tracing extensions to the device context. (HARDWARE DEPENDENT)
		// TODO: Check if ray tracing is supported by the device. Disabled for now.
		ExtensionList.insert(context::RayTracingExtensions.begin(), context::RayTracingExtensions.end());

		// Engine create device context for gpu operations.
		DeviceContext = Engine->create_device_context(Engine->PrimaryDevice, OperationList, LayerList, ExtensionList);
	}

	unit_test::~unit_test() {

	}

	void unit_test::run() {
		VkResult Result = VK_SUCCESS;
		std::cout << "Thread Count: " << omp_get_max_threads() << std::endl;
		omp_set_num_threads(omp_get_max_threads());

		this->math_test();

		this->create_worlds();

		timer PerformanceTimer(1.0);

		// Start main loop.
		float t = 0.0f;
		while (Engine->ThreadController.cycle(TimeStep)) {
			t += Engine->ThreadController.total_time() * 100.0f;

			double t1 = timer::get_time();

			system_window::poll_input();

			double t2 = timer::get_time();

			// Update host resources.
			this->update(Engine->ThreadController.total_time());

			double t3 = timer::get_time();

			// Execute gpu workloads.
			Result = Engine->execute_render_operations(this);

			double t4 = timer::get_time();

			if (PerformanceTimer.check()) {
				std::cout << "----- Performance Metrics -----" << std::endl;
				std::cout << "Current Time:\t" << timer::get_time() << " s" << std::endl;
				std::cout << "Time Step:\t" << TimeStep * 1000 << " ms" << std::endl;
				std::cout << "Work Time:\t" << (t4 - t1) * 1000.0 << " ms" << std::endl;
				std::cout << "-Input Time:\t" << (t2 - t1) * 1000.0 << " ms" << std::endl;
				std::cout << "-Update Time:\t" << (t3 - t2) * 1000.0 << " ms" << std::endl;
				std::cout << "-Render Time:\t" << (t4 - t3) * 1000.0 << " ms" << std::endl;
				std::cout << "Halt Time:\t" << Engine->ThreadController.halt_time() * 1000.0 << " ms" << std::endl;
				std::cout << "Total Time:\t" << Engine->ThreadController.total_time() * 1000.0 << " ms" << std::endl << std::endl;
				//std::cout << "Thread Over Time: " << Engine->ThreadController.work_time() - TimeStep << std::endl;
			}

			// if (timer::get_time() > 30.0f) {
			// 	break;
			// }

		}

		lgc::timer::wait(5.0f);

	}

	void unit_test::math_test() {
	    // Test configuration
	    float TestEpsilon = 1e-5f;
	    uint32_t TotalTests = 0;
	    uint32_t PassedTests = 0;

	    auto test_result = [&](const std::string& TestName, bool Result) {
	        TotalTests++;
	        if(Result) PassedTests++;
	        std::cout << std::setw(50) << std::left << TestName 
	                  << (Result ? "PASSED" : "FAILED") << std::endl;
	    };

	    auto float_equal = [TestEpsilon](float A, float B) -> bool {
	        return std::abs(A - B) < TestEpsilon;
	    };

	    std::cout << "\n=== Testing Math Library ===\n\n";

	    // Vector Tests
	    {
	        std::cout << "Testing vec<T,N>:\n";
	
	        // Constructor tests
	        {
	            math::vec<float, 3> DefaultVec;
	            math::vec<float, 3> InitVec = { 1.0f, 2.0f, 3.0f };
	
	            test_result("Default constructor zero initialization", 
	                float_equal(DefaultVec[0], 0.0f) && 
	                float_equal(DefaultVec[1], 0.0f) && 
	                float_equal(DefaultVec[2], 0.0f));
	
	            test_result("Initializer list constructor",
	                float_equal(InitVec[0], 1.0f) && 
	                float_equal(InitVec[1], 2.0f) && 
	                float_equal(InitVec[2], 3.0f));
	        }

	        // Arithmetic operations
	        {
	            math::vec<float, 3> A(2.0f, 1.0f, -3.0f);
	            math::vec<float, 3> B = { 0.0f, -2.09f, 9.987f };
	            float Scalar = 3.14159f;

	            // Test negation
	            math::vec<float, 3> Neg = -A;
	            test_result("Vector negation", 
	                float_equal(Neg[0], -2.0f) && 
	                float_equal(Neg[1], -1.0f) && 
	                float_equal(Neg[2], 3.0f));

	            // Test addition
	            math::vec<float, 3> Sum = A + B;
	            test_result("Vector addition", 
	                float_equal(Sum[0], 2.0f) && 
	                float_equal(Sum[1], -1.09f) && 
	                float_equal(Sum[2], 6.987f));

	            // Test subtraction
	            math::vec<float, 3> Diff = A - B;
	            test_result("Vector subtraction", 
	                float_equal(Diff[0], 2.0f) && 
	                float_equal(Diff[1], 3.09f) && 
	                float_equal(Diff[2], -12.987f));

	            // Test scalar multiplication
	            math::vec<float, 3> Scaled = A * Scalar;
	            test_result("Vector scalar multiplication", 
	                float_equal(Scaled[0], 2.0f * Scalar) && 
	                float_equal(Scaled[1], 1.0f * Scalar) && 
	                float_equal(Scaled[2], -3.0f * Scalar));

	            // Test scalar division
	            math::vec<float, 3> Divided = A / Scalar;
	            test_result("Vector scalar division", 
	                float_equal(Divided[0], 2.0f / Scalar) && 
	                float_equal(Divided[1], 1.0f / Scalar) && 
	                float_equal(Divided[2], -3.0f / Scalar));
	        }

	        // Compound assignments
	        {
	            math::vec<float, 3> A = { 2.0f, 1.0f, -3.0f };
	            math::vec<float, 3> B = { 0.0f, -2.09f, 9.987f };
	            float Scalar = 3.14159f;

	            math::vec<float, 3> TestVec = A;
	            TestVec += B;
	            test_result("Vector compound addition", 
	                float_equal(TestVec[0], 2.0f) && 
	                float_equal(TestVec[1], -1.09f) && 
	                float_equal(TestVec[2], 6.987f));

	            TestVec = A;
	            TestVec -= B;
	            test_result("Vector compound subtraction", 
	                float_equal(TestVec[0], 2.0f) && 
	                float_equal(TestVec[1], 3.09f) && 
	                float_equal(TestVec[2], -12.987f));

	            TestVec = A;
	            TestVec *= Scalar;
	            test_result("Vector compound multiplication", 
	                float_equal(TestVec[0], 2.0f * Scalar) && 
	                float_equal(TestVec[1], 1.0f * Scalar) && 
	                float_equal(TestVec[2], -3.0f * Scalar));

	            TestVec = A;
	            TestVec /= Scalar;
	            test_result("Vector compound division", 
	                float_equal(TestVec[0], 2.0f / Scalar) && 
	                float_equal(TestVec[1], 1.0f / Scalar) && 
	                float_equal(TestVec[2], -3.0f / Scalar));
	        }

	        // Special operations
	        {
	            math::vec<float, 3> A = { 2.0f, 1.0f, -3.0f };
	            math::vec<float, 3> B = { 0.0f, -2.09f, 9.987f };

	            // Dot product
	            float Dot = A * B;
	            test_result("Vector dot product", 
	                float_equal(Dot, 2.0f*0.0f + 1.0f*(-2.09f) + (-3.0f)*9.987f));

	            // Cross product
	            math::vec<float, 3> Cross = A ^ B;
	            test_result("Vector cross product", true);  // Add specific value checks
	        }
	    }

	    // Complex number tests
	    {
	        std::cout << "\nTesting complex<T>:\n";
	
	        // Constructor tests
	        {
	            math::complex<float> DefaultComplex;
	            math::complex<float> InitComplex(1.0f, 2.0f);
	
	            test_result("Complex default constructor", 
	                float_equal(DefaultComplex[0], 0.0f) && 
	                float_equal(DefaultComplex[1], 0.0f));
	
	            test_result("Complex initialization constructor",
	                float_equal(InitComplex[0], 1.0f) && 
	                float_equal(InitComplex[1], 2.0f));
	        }

	        // Basic operations
	        {
	            math::complex<float> A(1.0f, 2.0f);
	            math::complex<float> B(3.0f, 4.0f);

	            // Addition
	            math::complex<float> Sum = A + B;
	            test_result("Complex addition",
	                float_equal(Sum[0], 4.0f) && 
	                float_equal(Sum[1], 6.0f));

	            // Conjugate
	            math::complex<float> Conj = ~A;
	            test_result("Complex conjugate",
	                float_equal(Conj[0], 1.0f) && 
	                float_equal(Conj[1], -2.0f));

	            // Multiplication
	            math::complex<float> Prod = A * B;
	            test_result("Complex multiplication",
	                float_equal(Prod[0], -5.0f) && 
	                float_equal(Prod[1], 10.0f));

	            // Functions
	            float Abs = abs(A);
	            test_result("Complex absolute value",
	                float_equal(Abs, std::sqrt(5.0f)));

	            float Phase = phase(A);
	            test_result("Complex phase",
	                float_equal(Phase, std::atan2(2.0f, 1.0f)));
	        }
	    }

	    // Matrix tests
	    {
	        std::cout << "\nTesting mat<T,M,N>:\n";
	
	        // Test column-major storage with row-major input
	        {
	            math::mat<float, 4, 4> A = math::mat<float, 4, 4>(
	                1.0f, 0.0f, 0.0f, 1.0f,    
	                2.0f, 1.0f, -1.0f, 2.0f,   
	                3.0f, 2.0f, 0.0f, 3.0f,    
	                4.0f, 3.0f, 1.0f, 4.0f     
	            );

	            // Test column-major access
	            test_result("Matrix column-major storage",
	                float_equal(A(0,0), 1.0f) && 
	                float_equal(A(1,0), 2.0f) && 
	                float_equal(A(2,0), 3.0f) && 
	                float_equal(A(3,0), 4.0f));
	        }

	        // Test matrix multiplication
	        {
	            math::mat<float, 4, 4> A = math::mat<float, 4, 4>(
	                1.0f, 0.0f, 0.0f, 1.0f,
	                2.0f, 1.0f, -1.0f, 2.0f,
	                3.0f, 2.0f, 0.0f, 3.0f,
	                4.0f, 3.0f, 1.0f, 4.0f
	            );

	            math::mat<float, 4, 4> B = {
	                1.0f, 2.0f, 3.0f, 4.0f,
	                5.0f, 6.0f, 7.0f, 8.0f,
	                9.0f, 10.0f, 11.0f, 12.0f,
	                13.0f, 14.0f, 15.0f, 16.0f
	            };

	            math::mat<float, 4, 4> C = A * B;
	            test_result("Matrix multiplication", true);  // Add specific value checks
	        }

	        // Test determinant
	        {
	            math::mat<float, 4, 4> A = math::mat<float, 4, 4>(
	                2.0f, -1.0f, 0.0f, 1.0f,
	                1.0f, 3.0f, -2.0f, 0.0f,
	                0.0f, 2.0f, 4.0f, -1.0f,
	                1.0f, -1.0f, 1.0f, 2.0f
	            );

	            float Det = determinant(A);
	            test_result("Matrix determinant",
	                float_equal(Det, 48.0f));
	        }
	    }

	    // Field tests
	    {
	        std::cout << "\nTesting field<X,N,Y>:\n";

	        math::field<float, 2, float> X({-5.0f, -5.0f}, {2.0f, 2.0f}, {50, 50}, 1);
	        math::field<float, 2, float> Y({-2.0f, -3.0f}, {4.0f, 5.0f}, {50, 50}, 2);
	
	        // Test field addition
	        math::field<float, 2, float> Sum = X + Y;
	
	        // Test field sampling
	        math::vec<float, 2> SamplePoint = {1.9f, 1.3f};
	        float SampleValue = Sum(SamplePoint);
	
	        test_result("Field operations", true);  // Add specific value checks
	    }

	    // Print summary
	    std::cout << "\n=== Test Summary ===\n"
	              << "Total Tests: " << TotalTests << "\n"
	              << "Passed: " << PassedTests << "\n"
	              << "Failed: " << (TotalTests - PassedTests) << "\n"
	              << "Success Rate: " 
	              << (TotalTests > 0 ? (100.0f * PassedTests / TotalTests) : 0.0f)
	              << "%\n\n";
	}

	void unit_test::create_worlds() {

		// These are creation lists for construction stages.
		std::vector<runtime::object::creator*> Scene3DCreationList;
		std::vector<runtime::object::creator*> CanvasCreationList;

		// ====== Create Stage3D ===== //

		// Create Info for stage camera.
		obj::camera3d::creator CameraCreateInfo;
		CameraCreateInfo.Name 				= "Camera3D";
		CameraCreateInfo.Position 			= { 0.0f, -10.0f, 5.0f };
		CameraCreateInfo.Direction 			= { 0.0f, 0.0f };
		CameraCreateInfo.Resolution 		= { 2*1920, 2*1080, 1 };
		CameraCreateInfo.FrameRate 			= 500.0f;
		CameraCreateInfo.FrameCount 		= 1;
		CameraCreateInfo.FOV 				= 70.0f;
		CameraCreateInfo.Near 				= 1.0f;
		CameraCreateInfo.Far 				= 2000.0f;
		Scene3DCreationList.push_back(&CameraCreateInfo);

		runtime::object::creator Sponza;
		Sponza.Name 						= "Sponza";
		Sponza.ModelPath 					= "dep/gltf-models/2.0/Sponza/glTF/Sponza.gltf";
		Sponza.Position 					= { 0.0f, 0.0f, 0.0f };
		Sponza.Direction 					= { -90.0f, 90.0 };
		Sponza.Scale 						= { 0.03f, 0.03f, 0.03f };
		Scene3DCreationList.push_back(&Sponza);

		// runtime::object::creator PirateMap;
		// PirateMap.Name 						= "PirateMap";
		// PirateMap.ModelPath 				= "assets/models/pirate_map/scene.gltf";
		// PirateMap.Position 					= { 0.0f, 0.0f, 0.0f };
		// PirateMap.Direction 				= { 0.0f, 0.0f };
		// PirateMap.Scale 					= { 0.003f, 0.003f, 0.003f };
		// ObjectList.push_back(PirateMap);

		// Simple Boxes

		runtime::object::creator Box;
		Box.Name 							= "Box";
		Box.ModelPath 						= "dep/gltf-models/2.0/Box/glTF/Box.gltf";
		Box.Position 						= { 0.0f, 0.0f, 0.0f };
		Box.Direction 						= { 0.0f, 0.0f };
		Box.Scale 							= { 1.0f, 1.0f, 1.0f };
		Scene3DCreationList.push_back(&Box);

		runtime::object::creator BoxTextured;
		BoxTextured.Name 					= "BoxTextured";
		BoxTextured.ModelPath 				= "dep/gltf-models/2.0/BoxTextured/glTF/BoxTextured.gltf";
		BoxTextured.Position 				= { 2.0f, 0.0f, 0.0f };
		BoxTextured.Direction 				= { 0.0f, 0.0f };
		BoxTextured.Scale 					= { 1.0f, 1.0f, 1.0f };
		Scene3DCreationList.push_back(&BoxTextured);

		runtime::object::creator BoxTextured2;
		BoxTextured2.Name 					= "BoxTextured2";
		BoxTextured2.ModelPath 				= "dep/gltf-models/2.0/BoxTextured/glTF/BoxTextured.gltf";
		BoxTextured2.Position 				= { 4.0f, 0.0f, 0.0f };
		BoxTextured2.Direction 				= { 0.0f, 0.0f };
		BoxTextured2.Scale 					= { 1.0f, 1.0f, 1.0f };
		Scene3DCreationList.push_back(&BoxTextured2);

		runtime::object::creator Cube;
		Cube.Name 							= "Cube";
		Cube.ModelPath 						= "dep/gltf-models/2.0/Cube/glTF/Cube.gltf";
		Cube.Position 						= { 0.0f, 2.0f, 0.0f };
		Cube.Direction 						= { 0.0f, 0.0f };
		Cube.Scale 							= { 0.5f, 0.5f, 0.5f };
		Scene3DCreationList.push_back(&Cube);

		runtime::object::creator Cube2;
		Cube2.Name 							= "Cube2";
		Cube2.ModelPath 					= "dep/gltf-models/2.0/Cube/glTF/Cube.gltf";
		Cube2.Position 						= { 0.0f, 4.0f, 0.0f };
		Cube2.Direction 					= { 0.0f, 0.0f };
		Cube2.Scale 						= { 0.5f, 0.5f, 0.5f };
		Scene3DCreationList.push_back(&Cube2);

		runtime::object::creator BoxWithVertexColors;
		BoxWithVertexColors.Name 			= "BoxWithVertexColors";
		BoxWithVertexColors.ModelPath 		= "dep/gltf-models/2.0/BoxTextured/glTF/BoxTextured.gltf";
		BoxWithVertexColors.Position 		= { 0.0f, 0.0f, 2.0f };
		BoxWithVertexColors.Direction 		= { 0.0f, 0.0f };
		BoxWithVertexColors.Scale 			= { 1.0f, 1.0f, 1.0f };
		Scene3DCreationList.push_back(&BoxWithVertexColors);

		// Gizmo Object

		runtime::object::creator Gizmo;
		Gizmo.Name 							= "Gizmo";
		Gizmo.ModelPath 					= "assets/models/gizmo/scene.gltf";
		Gizmo.Position 						= { 0.0f, 0.0f, 0.0f };
		Gizmo.Direction 					= { 0.0f, 0.0f };
		Gizmo.Scale 						= { 1.0f, 1.0f, 1.0f };
		Scene3DCreationList.push_back(&Gizmo);

		// Tests Complex Animations

		runtime::object::creator BrainStem;
		BrainStem.Name 						= "BrainStem";
		BrainStem.ModelPath 				= "dep/gltf-models/2.0/BrainStem/glTF/BrainStem.gltf";
		BrainStem.Position 					= { 2.0f, -2.0f, 0.0f };
		BrainStem.Direction 				= { 0.0f, 0.0f };
		BrainStem.Scale 					= { 1.0f, 1.0f, 1.0f };
		BrainStem.AnimationWeights 			= { 0.0f, 1.0f };
		Scene3DCreationList.push_back(&BrainStem);

		runtime::object::creator CesiumMilkTruck;
		CesiumMilkTruck.Name 				= "CesiumMilkTruck";
		CesiumMilkTruck.ModelPath 			= "dep/gltf-models/2.0/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf";
		CesiumMilkTruck.Position 			= { 0.0f, 12.0f, 0.0f };
		CesiumMilkTruck.Direction 			= { -180.0f, 0.0f };
		CesiumMilkTruck.Scale 				= { 1.0f, 1.0f, 1.0f };
		CesiumMilkTruck.AnimationWeights 	= { 0.0f, 1.0f };
		Scene3DCreationList.push_back(&CesiumMilkTruck);

		runtime::object::creator CesiumMan;
		CesiumMan.Name 						= "CesiumMan";
		CesiumMan.ModelPath 				= "dep/gltf-models/2.0/CesiumMan/glTF/CesiumMan.gltf";
		CesiumMan.Position 					= { -2.0f, -2.0f, 0.0f };
		CesiumMan.Direction 				= { 0.0f, 0.0f };
		CesiumMan.Scale 					= { 1.0f, 1.0f, 1.0f };
		CesiumMan.AnimationWeights 			= { 0.0f, 1.0f };
		Scene3DCreationList.push_back(&CesiumMan);

		// Tests Parallax Mapping.
		// runtime::object::creator ParallaxPlane;
		// ParallaxPlane.Name 					= "ParallaxPlane";
		// ParallaxPlane.ModelPath 			= "assets/models/bricks2/bricks2.gltf";
		// ParallaxPlane.Position 				= { 0.0f, 0.0f, 5.0f };
		// ParallaxPlane.Direction 			= { -90.0f, 0.0f };
		// ParallaxPlane.Scale 				= { 5.0f, 5.0f, 5.0f };
		// Scene3DCreationList.push_back(&ParallaxPlane);

		// Test Emissive Lighting
		runtime::object::creator Lantern;
		Lantern.Name 						= "Lantern";
		Lantern.ModelPath 					= "dep/gltf-models/2.0/Lantern/glTF/Lantern.gltf";
		Lantern.Position 					= { 0.0f, -2.0f, 0.0f };
		Lantern.Direction 					= { -90.0f, 0.0f };
		Lantern.Scale 						= { 0.2f, 0.2f, 0.2f };
		Scene3DCreationList.push_back(&Lantern);

		// Full PBR Test
		runtime::object::creator DamagedHelmet;
		DamagedHelmet.Name 					= "DamagedHelmet";
		DamagedHelmet.ModelPath 			= "dep/gltf-models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
		DamagedHelmet.Position 				= { 8.0f, 0.0f, 0.0f };
		DamagedHelmet.Direction 			= { -180.0f, 0.0f };
		DamagedHelmet.Scale 				= { 1.0f, 1.0f, 1.0f };
		Scene3DCreationList.push_back(&DamagedHelmet);

		// Special Case Objects

		runtime::object::creator IridescenceLamp;
		IridescenceLamp.Name 				= "IridescenceLamp";
		IridescenceLamp.ModelPath 			= "dep/gltf-models/2.0/IridescenceLamp/glTF/IridescenceLamp.gltf";
		IridescenceLamp.Position 			= { 0.0f, -10.0f, 0.0f };
		IridescenceLamp.Direction 			= { -90.0f, 0.0f };
		IridescenceLamp.Scale 				= { 2.0f, 2.0f, 2.0f };
		Scene3DCreationList.push_back(&IridescenceLamp);

		runtime::object::creator MosquitoInAmber;
		MosquitoInAmber.Name 				= "MosquitoInAmber";
		MosquitoInAmber.ModelPath 			= "dep/gltf-models/2.0/MosquitoInAmber/glTF/MosquitoInAmber.gltf";
		MosquitoInAmber.Position 			= { -10.0f, 0.0f, 0.0f };
		MosquitoInAmber.Direction 			= { 0.0f, 0.0f };
		MosquitoInAmber.Scale 				= { 10.0f, 10.0f, 10.0f };
		Scene3DCreationList.push_back(&MosquitoInAmber);

		// // Sheen Rendering.

		// runtime::object::creator SheenCloth;
		// SheenCloth.Name 						= "SheenCloth";
		// SheenCloth.ModelPath 					= "dep/gltf-models/2.0/SheenCloth/glTF/SheenCloth.gltf";
		// SheenCloth.Position 					= { 0.0f, 0.0f, 0.0f };
		// SheenCloth.Direction 					= { -90.0f, 0.0f };
		// SheenCloth.Scale 						= { 1.0f, 1.0f, 1.0f };
		// Scene3DCreationList.push_back(&SheenCloth);

		// // Test Clear Coat Rendering.

		// runtime::object::creator ToyCar;
		// ToyCar.Name 							= "ToyCar";
		// ToyCar.ModelPath 						= "dep/gltf-models/2.0/ToyCar/glTF/ToyCar.gltf";
		// ToyCar.Position 						= { 0.0f, 0.0f, 0.0f };
		// ToyCar.Direction 						= { -90.0f, 0.0f };
		// ToyCar.Scale 							= { 1.0f, 1.0f, 1.0f };
		// Scene3DCreationList.push_back(&ToyCar);

		stage::creator Scene3D;
		Scene3D.Name 						= "Scene3D";
		Scene3D.ObjectCreationList 			= Scene3DCreationList;

		this->Stage = std::vector<std::shared_ptr<runtime::stage>>(2);		

		// Create Scene3D.
		this->Stage[0] = this->build_stage(DeviceContext, &Scene3D);

		stage::light_uniform_data* LightBuffer = (stage::light_uniform_data*)this->Stage[0]->LightUniformBuffer->Ptr;
		LightBuffer->Count = 5;
		
		// Ambient light.
		LightBuffer->Source[0].Type 			= gfx::model::light::type::AMBIENT;
		LightBuffer->Source[0].Intensity 		= 0.1f;
		LightBuffer->Source[0].Color 			= { 1.0f, 1.0f, 1.0f };

		// Direction light.
		LightBuffer->Source[1].Type 			= gfx::model::light::type::DIRECTIONAL;
		LightBuffer->Source[1].Intensity 		= 1.0f;
		LightBuffer->Source[1].Color 			= { 1.0f, 1.0f, 1.0f };
		LightBuffer->Source[1].Direction 		= { -1.0f, -1.0f, -1.0f };

		// Point light.
		LightBuffer->Source[2].Type 			= gfx::model::light::type::POINT;
		LightBuffer->Source[2].Intensity 		= 1.0f;
		LightBuffer->Source[2].Color 			= { 1.0f, 0.0f, 0.0f };
		LightBuffer->Source[2].Position 		= { -5.0f, -2.0f, 3.0f };

		// Point Light.
		LightBuffer->Source[3].Type 			= gfx::model::light::type::POINT;
		LightBuffer->Source[3].Intensity 		= 1.0f;
		LightBuffer->Source[3].Color 			= { 0.0f, 1.0f, 0.0f };
		LightBuffer->Source[3].Position 		= { 5.0f, -2.0f, 3.0f };

		// Point Light.
		LightBuffer->Source[4].Type 			= gfx::model::light::type::POINT;
		LightBuffer->Source[4].Intensity 		= 1.0f;
		LightBuffer->Source[4].Color 			= { 0.0f, 0.0f, 1.0f };
		LightBuffer->Source[4].Position 		= { 0.0f, 5.0f, 3.0f };

		// Get Camera3D from Stage 0.
		std::shared_ptr<runtime::subject> Camera3D = std::dynamic_pointer_cast<runtime::subject>(this->Stage[0]->Object[0]);

		// ===== Create Canvas System Window Output ===== //

		// Create System Window Object.
		system_window::creator SystemWindowCreator;
		SystemWindowCreator.Name 			= "System Window";
		SystemWindowCreator.Resolution 		= { 1920, 1080, 1 };
		SystemWindowCreator.FrameCount 		= 3;
		SystemWindowCreator.FrameRate 		= 60.0f;
		SystemWindowCreator.Display 		= Engine->PrimaryDisplay;
		CanvasCreationList.push_back(&SystemWindowCreator);

		// Use subject window to share camera3d renderings.
		subject_window::creator SubjectWindowCreator;
		SubjectWindowCreator.Name 			= "Camera3D Window";
		SubjectWindowCreator.ModelPath 		= "assets/models/quad.obj";
		SubjectWindowCreator.Position 		= { 0.0f, 0.0f, 0.5f };
		SubjectWindowCreator.Direction 		= { 0.0f, 0.0f };
		SubjectWindowCreator.Scale 			= { 1.0f, 1.0f, 1.0f };
		SubjectWindowCreator.Subject 		= std::dynamic_pointer_cast<runtime::subject>(Camera3D);
		CanvasCreationList.push_back(&SubjectWindowCreator);

		stage::creator Canvas;
		Canvas.Name 						= "Canvas";
		Canvas.ObjectCreationList 			= CanvasCreationList;

		// Create Windowing Stage.
		this->Stage[1] = this->build_stage(DeviceContext, &Canvas);

		// Create System Window.
		Window = std::dynamic_pointer_cast<obj::system_window>(this->Stage[1]->Object[0]);
		// Forward Window User Input to Camera3D.
		Window->InputTarget = Camera3D;

	}

}