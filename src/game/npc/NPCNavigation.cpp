#include <game/npc/NPCNavigation.h>
#include <game/npc/NPCNavigationEventHandler.h>
using namespace urchin;

NPCNavigation::NPCNavigation(float speedInKmH, float mass, const Map& map, AIEnvironment& aiEnvironment, PhysicsWorld& physicsWorld) {
    model = map.getObjectEntity("character").getModel();

    auto aiCharacter = std::make_unique<AICharacter>(mass, speedInKmH, model->getTransform().getPosition());
    aiCharacterController = std::make_unique<AICharacterController>(std::move(aiCharacter), aiEnvironment);
    aiCharacterController->setupEventHandler(std::make_shared<NPCNavigationEventHandler>());
    aiCharacterController->moveTo(Point3<float>(-5.0f, 15.0f, 5.0f));

    float characterRadius = std::max(model->getAABBox().getHalfSizes().X, model->getAABBox().getHalfSizes().Z);
    float characterHeight = model->getAABBox().getHalfSizes().Y * 2.0f;
    auto characterShape = std::make_unique<const CollisionCapsuleShape>(characterRadius, characterHeight - (2.0f * characterRadius), CapsuleShape<float>::CAPSULE_Z);
    PhysicsTransform characterTransform(model->getTransform().getPosition(), model->getTransform().getOrientation());
    auto physicsCharacter = std::make_unique<PhysicsCharacter>("npcCharacter", mass, std::move(characterShape), characterTransform);
    CharacterControllerConfig characterControllerConfig;
    characterControllerConfig.setWalkSpeed(aiCharacterController->getAICharacter().retrieveMaxVelocityInMs());
    characterControllerConfig.setRunSpeed(aiCharacterController->getAICharacter().retrieveMaxVelocityInMs());
    characterController = std::make_unique<CharacterController>(std::move(physicsCharacter), characterControllerConfig, physicsWorld);
}

std::shared_ptr<const PathRequest> NPCNavigation::getPathRequest() const {
    return aiCharacterController->getPathRequest();
}

void NPCNavigation::display(const Scene& scene) {
    float dt = scene.getDeltaTime();

    //update values
    aiCharacterController->update();
    characterController->walk(aiCharacterController->getAICharacter().getVelocity());
    characterController->update(dt);

    //apply updated values
    aiCharacterController->getAICharacter().updatePosition(characterController->getPhysicsCharacter().getTransform().getPosition());
    model->setTransform(Transform<float>(characterController->getPhysicsCharacter().getTransform().getPosition(), characterController->getPhysicsCharacter().getTransform().getOrientation(), model->getTransform().getScale()));
}
