#include <pch.h>

#define PHYSICS_TESTO
#ifndef PHYSICS_TESTO
#include "PxPhysicsAPI.h" // Includes all necessary PhysX headers

using namespace physx;

// Global variables for PhysX
PxDefaultAllocator      gAllocator;
PxDefaultErrorCallback  gErrorCallback;
PxFoundation* gFoundation = nullptr;
PxPhysics* gPhysics = nullptr;
PxMaterial* gMaterial = nullptr;
PxScene* gScene = nullptr;
PxPvd* gPvd = nullptr;

// Initialize PhysX
void initPhysX() {
    // Create foundation, required by all PhysX objects
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    // Set up visual debugger (optional)
    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    // Create the main physics object
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

    // Create a default material (used by all rigid bodies unless otherwise specified)
    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f); // staticFriction, dynamicFriction, restitution

    // Create the scene (simulation world)
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // Gravity pointing downward

    // Use default CPU dispatcher
    PxDefaultCpuDispatcher* dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    gScene = gPhysics->createScene(sceneDesc);
}

// Cleanup PhysX
void cleanupPhysX() {
    gScene->release();
    gPhysics->release();
    gPvd->release();
    gFoundation->release();
}

// Add a rigid body box (AABB) to the scene
PxRigidDynamic* createAABBRigidBody() {
    // Create a box (AABB) centered at (0, 5, 0), size (2, 2, 2)
    PxBoxGeometry boxGeometry(PxVec3(1.0f, 1.0f, 1.0f)); // Half extents (1 unit per side)

    // Create a dynamic (movable) actor with the box shape
    PxTransform boxTransform(PxVec3(0.0f, 5.0f, 0.0f)); // Initial position
    PxRigidDynamic* boxActor = PxCreateDynamic(*gPhysics, boxTransform, boxGeometry, *gMaterial, 10.0f); // Mass = 10.0f

    // Set linear damping to slow down the movement over time
    boxActor->setLinearDamping(0.5f);

    // Add the actor to the scene
    gScene->addActor(*boxActor);

    return boxActor;
}

// Add a rigid body sphere to the scene
PxRigidDynamic* createSphereRigidBody() {
    // Create a sphere of radius 1.0
    PxSphereGeometry sphereGeometry(1.0f);

    // Create a dynamic (movable) actor with the sphere shape
    PxTransform sphereTransform(PxVec3(0.0f, 10.0f, 0.0f)); // Initial position
    PxRigidDynamic* sphereActor = PxCreateDynamic(*gPhysics, sphereTransform, sphereGeometry, *gMaterial, 10.0f); // Mass = 10.0f

    // Set linear damping for the sphere as well
    sphereActor->setLinearDamping(0.5f);

    // Add the actor to the scene
    gScene->addActor(*sphereActor);

    return sphereActor;
}

// Step the physics simulation
void stepPhysics() {
    // Simulate one time step (1/60 second)
    gScene->simulate(1.0f / 60.0f);

    // Wait for the simulation to complete
    gScene->fetchResults(true);
}

int main() {
    // Initialize PhysX
    initPhysX();

    // Create a rigid body box and sphere
    PxRigidDynamic* box = createAABBRigidBody();
    PxRigidDynamic* sphere = createSphereRigidBody();

    // Simulate for a few steps
    for (int i = 0; i < 300; ++i) {
        stepPhysics();

        // Output the current position of the box and sphere
        PxVec3 boxPosition = box->getGlobalPose().p;
        PxVec3 spherePosition = sphere->getGlobalPose().p;

        printf("Step %d: Box Position: (%.2f, %.2f, %.2f)\n", i, boxPosition.x, boxPosition.y, boxPosition.z);
        printf("Step %d: Sphere Position: (%.2f, %.2f, %.2f)\n", i, spherePosition.x, spherePosition.y, spherePosition.z);
    }

    // Cleanup
    cleanupPhysX();

    return 0;
}
#else 
#include <Application.h>
#include "Asset/IGEAssets.h"
#define IGE_ASSETMGR_SAMPLE

int main()
{
#ifdef IGE_ASSETMGR_SAMPLE
	AssetManagerSampleTest();
#endif
  Application myApp{ "ImaGE", WINDOW_WIDTH<int>, WINDOW_HEIGHT<int> };

  try
  {
    myApp.Init();

    myApp.Run();
  }
#ifdef _DEBUG
  catch (std::exception const& e)
  {
    std::cerr << e.what() << std::endl;
  }
#else
  catch (std::exception const&)
  {

  }
#endif

  myApp.Shutdown();
  return 0;
}
#endif