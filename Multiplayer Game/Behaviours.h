#pragma once

struct Behaviour
{
	GameObject *gameObject = nullptr;

	virtual void start() { }

	virtual void update() { }

	virtual void onInput(const InputController &input) { }

	virtual void onCollisionTriggered(Collider &c1, Collider &c2) { }
};

struct Spaceship : public Behaviour
{
	int initialHealth = 100;
	int health = initialHealth;

	float size = 100.0f;
	float minsize = 10.0f;

	float speed = 200.0f;

	void start() override
	{
		gameObject->tag = (uint32)(Random.next() * UINT_MAX);
	}

	void onInput(const InputController &input) override
	{
		if (input.horizontalAxis != 0.0f)
		{
			const float rotateSpeed = 180.0f;
			gameObject->angle += input.horizontalAxis * rotateSpeed * Time.deltaTime;
			NetworkUpdate(gameObject);
		}

		if (input.actionDown == ButtonState::Pressed)
		{
			const float advanceSpeed = 200.0f;
			gameObject->position += vec2FromDegrees(gameObject->angle) * advanceSpeed * Time.deltaTime;
			NetworkUpdate(gameObject);
		}

		if (input.actionLeft == ButtonState::Press)
		{
			GameObject * laser = App->modNetServer->spawnBullet(gameObject);
			laser->tag = gameObject->tag;
		}
	}

	void onCollisionTriggered(Collider &c1, Collider &c2) override
	{
		if (c2.type == ColliderType::Laser && c2.gameObject->tag != gameObject->tag)
		{
			NetworkDestroy(c2.gameObject); // Destroy the laser

			// NOTE(jesus): spaceship was collided by a laser
			// Be careful, if you do NetworkDestroy(gameObject) directly,
			// the client proxy will poing to an invalid gameObject...
			// instead, make the gameObject invisible or disconnect the client.

			health -= 10;
			float ratio = ((float)initialHealth - (float)health) / (float)initialHealth;

			c1.gameObject->size.x = size - ((size - minsize) * ratio);
			c1.gameObject->size.y = size - ((size - minsize) * ratio);
			
			if (health <= 0)
			{
				App->modNetServer->KickProxy(c1.gameObject);
			}
			else 
			{
				NetworkUpdate(c1.gameObject);
			}

			
		}
	}
};

struct Laser : public Behaviour
{
	float secondsSinceCreation = 0.0f;

	void update() override
	{
		const float pixelsPerSecond = 1000.0f;
		gameObject->position += vec2FromDegrees(gameObject->angle) * pixelsPerSecond * Time.deltaTime;

		secondsSinceCreation += Time.deltaTime;

		NetworkUpdate(gameObject);

		const float lifetimeSeconds = 2.0f;
		if (secondsSinceCreation > lifetimeSeconds) NetworkDestroy(gameObject);
	}
};
