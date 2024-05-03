#include "FlappyBird.h"

Position::Position(float _x, float _y) {
	x = _x;
	y = _y;
}
float Position::getX() {
	return x;
}
float Position::getY() {
	return y;
}
void Position::setX(float _x) {
	x = _x;
}
void Position::setY(float _y) {
	y = _y;
}
void Position::moveX(float move_x) {
	x += move_x;
}
void Position::moveY(float move_y) {
	y += move_y;
}

Velocity::Velocity(float _x, float _y) {
	x = _x;
	y = _y;
}
float Velocity::getX() {
	return x;
}
float Velocity::getY() {
	return y;
}
void Velocity::setX(float _x) {
	x = _x;
}
void Velocity::setY(float _y) {
	y = _y;
}
void Velocity::accelerateX(float acceleration_x) {
	x += acceleration_x;
}
void Velocity::accelerateY(float acceleration_y) {
	y += acceleration_y;
}

TunnelEntrance::TunnelEntrance(float _height, float _y) {
	height = _height;
	y = _y;
}
float TunnelEntrance::end() {
	return y + height;
}

Tunnel::Tunnel(float _width, float _height, float position_x, float position_y,
	float velocity_x, float velocity_y, float entrace_height, float entrace_position_y)
	:
	position(position_x, position_y),
	velocity(velocity_x, velocity_y),
	entrance(entrace_height, entrace_position_y)
{
	width = _width;
	height = _height;
}
float Tunnel::getWidth() {
	return width;
}
void Tunnel::update() {
	position.moveX(velocity.getX());
}

Bird::Bird(float _radius,
	float position_x, float position_y, float velocity_x, float velocity_y)
	:
	position(position_x, position_y),
	velocity(velocity_x, velocity_y)
{
	radius = _radius;
	isAlive = true;
}
float Bird::getRadius() {
	return radius;
}
void Bird::flap(float speed) {
	if (isAlive) {
		// If is falling, stop the bird
		if (velocity.getY() < 0) {
			velocity.setY(0);
		}

		// Flap the bird.
		velocity.accelerateY(speed);
	}
}
void Bird::gravity(float gravity_acceleration) {
	velocity.accelerateY(-gravity_acceleration);
}
void Bird::update(float gravity_acceleration, float max_velocity) {

	// cap the velocity.
	if (velocity.getY() > max_velocity) {
		velocity.setY(max_velocity);
	}

	Bird::gravity(gravity_acceleration); // Apply gravity.

	position.moveY(velocity.getY()); // sum position with velocity.
}

FlappyBird::FlappyBird(int _n_birds, int resolution_x, int resolution_y, float _tunnel_velocity,
	float bird_radius, float bird_position_x, float bird_position_y,
	float bird_velocity_x, float bird_velocity_y)
{
	n_birds = _n_birds;

	for (int i = 0; i < n_birds; i++) {
		addBird(bird_radius, resolution_x / 10.f, resolution_y / 2.f, 0.f, 0.f);
	}

	resX = resolution_x;
	resY = resolution_y;
	tunnel_width = resolution_x / 6;
	tunnel_height = resolution_y;
	tunnel_velocity = _tunnel_velocity;
	restart();
}
int FlappyBird::getPoints() {
	return points;
}
float FlappyBird::getTunnelWidth() {
	return tunnel_width;
}
float FlappyBird::getTunnelHeight() {
	return tunnel_height;
}
void FlappyBird::addBird(float _radius,
	float position_x, float position_y, float velocity_x, float velocity_y) {
	bird_list.push_back(Bird(_radius, position_x, position_y, velocity_x, velocity_y));
}
void FlappyBird::addTunnel() {
	float entrace_height = resY / 5;
	float entrace_position_y = (rand() % (int)(resY * 0.8)) + (resY * 0.1) - entrace_height / 2;
	tunnel_list.push_back(Tunnel(tunnel_width, resY, resX, 0, tunnel_velocity, 0, entrace_height, entrace_position_y));
}

int FlappyBird::update(float gravity_acceleration, float bird_max_velocity) {

	// detect if the player has lost..

	std::list <Bird> ::iterator i_bird;
	Bird* alive_bird = NULL;
	for (i_bird = bird_list.begin(); i_bird != bird_list.end(); ++i_bird) {
		if (not(isLost(*i_bird))) {
			alive_bird = &(*i_bird);
			break;
		}
	}

	if (alive_bird == NULL) return 0; // no bird alive.

	// detect if a tunnel has left the screen and remove him.
	if (tunnel_list.begin()->position.getX() + tunnel_list.begin()->getWidth() < 0) {
		tunnel_list.pop_front();
	}

	// add a new tunnel if the condition is true.
	if (next_tunnel->position.getX() < resX / 2) {
		addTunnel();
		next_tunnel++;
	}

	/* if the player pass into the tunnel without lost, add a point and update the next tunnel that
	* the bird will pass.
	*/

	if (next_bird_tunnel->position.getX() + next_bird_tunnel->getWidth() < alive_bird->position.getX()) {
		next_bird_tunnel++;
		points++;
	}
	// update the bird.
	for (i_bird = bird_list.begin(); i_bird != bird_list.end(); ++i_bird) {
		i_bird->update(gravity_acceleration, bird_max_velocity);
	}


	// update each tunnel in the tunnel_list.
	std::list <Tunnel> ::iterator i_tunnel;
	for (i_tunnel = tunnel_list.begin(); i_tunnel != tunnel_list.end(); ++i_tunnel) {
		i_tunnel->update();
	}

	return 1;
}
int FlappyBird::isLost(Bird bird) {
	if (bird.isAlive) {
		// lost if touch the ground.
		if (bird.position.getY() - bird.getRadius() * 2 < 0) {
			bird.isAlive = false;
			return 1;
		}

		// lost if touch the tunnel pipe.
		if (bird.position.getX() + bird.getRadius() * 2 > next_bird_tunnel->position.getX()) {
			if (bird.position.getY() - bird.getRadius() * 2 > next_bird_tunnel->entrance.y
				&& bird.position.getY() < next_bird_tunnel->entrance.end()) {
				return 0;
			}
			else {
				bird.isAlive = false;
				return 1;
			}
		}
	}
	else {
		return 1; // already lost.
	}
	return 0;
}
void FlappyBird::restart() {
	points = 0; // erase the points.

	// restart the bird position.
	std::list <Bird> ::iterator i_bird;
	for (i_bird = bird_list.begin(); i_bird != bird_list.end(); ++i_bird) {
		i_bird->position.setY(resY / 2);
		i_bird->velocity.setY(0);
		i_bird->isAlive = true;
	}

	//restart the tunnels.
	tunnel_list.clear();
	addTunnel();
	next_tunnel = tunnel_list.begin();
	next_bird_tunnel = tunnel_list.begin();
}

int FlappyBird::isOnTunnel(Bird bird) {
	if (bird.position.getY() - bird.getRadius() * 2 > next_bird_tunnel->entrance.y
		&& bird.position.getY() < next_bird_tunnel->entrance.end()) {
		return 1;
	}
	return 0;
}
Array::Array2D FlappyBird::getIaInput(Bird bird) {
	int input_length = 2;
	float** data = (float**)malloc(sizeof(float*));
	data[0] = (float*)malloc(input_length * sizeof(float));
	data[0][0] = bird.position.getY() - (next_bird_tunnel)->entrance.y - bird.getRadius();
	data[0][1] = bird.velocity.getY();
	return Array::Array2D(FLOAT_TYPE, 1, input_length, data);
}