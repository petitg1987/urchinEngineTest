#include <game/npc/NPCNavigation.h>
#include <game/npc/NPCNavigationEventHandler.h>
using namespace urchin;

NPCNavigation::NPCNavigation(float speedInKmH, float mass, const Map& map, AIEnvironment& aiEnvironment, PhysicsWorld& physicsWorld) {
    model = map.getObjectEntity("character").getModel();

    aiCharacter = std::make_shared<AICharacter>(mass, speedInKmH, model->getTransform().getPosition());
    aiCharacterController = std::make_unique<AICharacterController>(aiCharacter, aiEnvironment);
    aiCharacterController->setupEventHandler(std::make_shared<NPCNavigationEventHandler>(model));
    aiCharacterController->moveTo(Point3<float>(-5.0f, 15.0f, 5.0f));

    float characterRadius = std::max(model->getAABBox().getHalfSizes().X, model->getAABBox().getHalfSizes().Z);
    float characterHeight = model->getAABBox().getHalfSizes().Y * 2.0f;
    auto characterShape = std::make_unique<const CollisionCapsuleShape>(characterRadius, characterHeight - (2.0f * characterRadius), CapsuleShape<float>::CAPSULE_Z);
    PhysicsTransform characterTransform(model->getTransform().getPosition(), model->getTransform().getOrientation());
    physicsCharacter = std::make_shared<PhysicsCharacter>("npcCharacter", mass, std::move(characterShape), characterTransform);
    CharacterControllerConfig characterControllerConfig;
    characterControllerConfig.setWalkSpeed(aiCharacter->retrieveMaxVelocityInMs());
    characterControllerConfig.setRunSpeed(aiCharacter->retrieveMaxVelocityInMs());
    characterController = std::make_unique<CharacterController>(physicsCharacter, characterControllerConfig, physicsWorld);
}

std::shared_ptr<const PathRequest> NPCNavigation::getPathRequest() const {
    return aiCharacterController->getPathRequest();
}

void NPCNavigation::display(const Scene& scene) {
    float dt = scene.getDeltaTime();

    //update values
    aiCharacterController->update();
    characterController->walk(aiCharacter->getVelocity());
    characterController->update(dt);

    //apply updated values
    aiCharacter->updatePosition(physicsCharacter->getTransform().getPosition());
    model->setTransform(Transform<float>(physicsCharacter->getTransform().getPosition(), physicsCharacter->getTransform().getOrientation(), model->getTransform().getScale()));
}
