#include "Networks.h"
#include "ModuleGameObject.h"

void GameObject::ResetPos(vec2 newpos, float newangle)
{
	initial_position = position;
	final_position = newpos;

	initial_angle = angle;
	final_angle = newangle;

	total_elapsed = seconds_elapsed;
	time_left = total_elapsed;
	seconds_elapsed = 0;

}

void GameObject::Interpolate()
{
	vec2 dif = final_position - initial_position;
	time_left -= Time.deltaTime;
	float ratio = time_left / total_elapsed;

	if (ratio > 1) {
		interpolate = false;
	}

	position = initial_position + (dif * ratio);
	angle = angle + (angle * ratio);
}

void GameObject::releaseComponents()
{
	if (behaviour != nullptr)
	{
		delete behaviour;
		behaviour = nullptr;
	}
	if (collider != nullptr)
	{
		App->modCollision->removeCollider(collider);
		collider = nullptr;
	}
}

bool ModuleGameObject::init()
{
	return true;
}

bool ModuleGameObject::preUpdate()
{
	for (GameObject &gameObject : gameObjects)
	{
		if (gameObject.state == GameObject::NON_EXISTING) continue;

		if (gameObject.state == GameObject::DESTROYING)
		{
			gameObject.releaseComponents();
			gameObject = GameObject();
			gameObject.state = GameObject::NON_EXISTING;
		}
		else if (gameObject.state == GameObject::CREATING)
		{
			if (gameObject.behaviour != nullptr)
				gameObject.behaviour->start();
			gameObject.state = GameObject::UPDATING;
		}
	}

	return true;
}

bool ModuleGameObject::update()
{
	for (GameObject &gameObject : gameObjects)
	{
		if (gameObject.state == GameObject::UPDATING)
		{
			if (gameObject.behaviour != nullptr && gameObject.behaviour->isServer)
				gameObject.behaviour->update();
			else if (gameObject.interpolate)
			{
				gameObject.seconds_elapsed += Time.deltaTime;
				gameObject.Interpolate();
			}
		}

	}

	return true;
}

bool ModuleGameObject::postUpdate()
{
	for (GameObject &gameObject : gameObjects)
	{

		if (gameObject.toDisconnect)
		{
			App->modNetServer->KickProxy(&gameObject);

		}
	}
	return true;
}

bool ModuleGameObject::cleanUp()
{
	for (GameObject &gameObject : gameObjects)
	{
		gameObject.releaseComponents();
	}

	return true;
}

GameObject * ModuleGameObject::Instantiate()
{
	for (GameObject &gameObject : App->modGameObject->gameObjects)
	{
		if (gameObject.state == GameObject::NON_EXISTING)
		{
			gameObject.state = GameObject::CREATING;
			return &gameObject;
		}
	}

	ASSERT(0); // NOTE(jesus): You need to increase MAX_GAME_OBJECTS in case this assert crashes
	return nullptr;
}

void ModuleGameObject::Destroy(GameObject * gameObject)
{
	ASSERT(gameObject->networkId == 0); // NOTE(jesus): If it has a network identity, it must be destroyed by the Networking module first
	
	gameObject->state = GameObject::DESTROYING;
}

GameObject * Instantiate()
{
	GameObject *result = ModuleGameObject::Instantiate();
	return result;
}

void Destroy(GameObject * gameObject)
{
	ModuleGameObject::Destroy(gameObject);
}
