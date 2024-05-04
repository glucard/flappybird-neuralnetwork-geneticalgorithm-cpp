#ifndef FLAPPY_BIRD_H
#define FLAPPY_BIRD_H

#include <list>
#include "Array2D.hpp"

/* Postion Class */
class Position {
	float x; // position x value;
	float y; // position y value;
public:
	/* Position constructor */
	Position(float _x, float _y);

	/**
	* A get funtion.
	*
	* @return x value.
	*/
	float getX();

	/**
	* A get funtion.
	* 
	* @return y value.
	*/
	float getY();

	/**
	 * Set a new x value.
	 *
	 * @param _x must contain the new value.
	 */
	void setX(float _x);

	/**
	 * Set a new y value. 
	 *
	 * @param _y must contain the new value.
	 */
	void setY(float _y);

	/**
	* Add a value in x value.
	* 
	* @param move_x contains how much to add.
	*/
	void moveX(float move_x);

	/**
	* Add a value in y value.
	*
	* @param move_y must contain the value to add.
	*/
	void moveY(float move_x);
};

/* Class Velocity */
class Velocity {
	float x; // x value.
	float y; // y value.
public:
	/*
	* Velocity class constructor.
	* 
	* @param _x must contain the x value.
	* @param _y must contain the y value.
	*/
	Velocity(float _x, float _y);

	/**
	* A get function.
	* 
	* @return x value.
	*/
	float getX();

	/**
	* A get function.
	*
	* @return y value.
	*/
	float getY();

	/**
	 * Set a new x value.
	 *
	 * @param _x must contain the new value.
	 */
	void setX(float _x);

	/**
	 * Set a new y value.
	 *
	 * @param _y must contain the new value.
	 */
	void setY(float _y);

	/*
	* Add a value to the velocity x value.
	* 
	* @param acceleration must contain the value to add
	*/
	void accelerateX(float acceleration);

	/*
	* Add a value to the velocity y value.
	*
	* @param acceleration must contain the value to add.
	*/
	void accelerateY(float acceleration);
};

/* Class TunnelEntrance */
class TunnelEntrance {
public:
	float height; // height value.
	float y; // y value.
	/*
	* TunnelEntrace class constructor.
	* 
	* @param _height must contain the height value. Is the size of the entrance.
	* @param _y must contain the _y value. Is the position x of the entrance.
	*/
	TunnelEntrance(float _height, float _y);

	/*
	* @return the end of the entrance (y + height).
	*/
	float end();
};

/* A class tunnel */
class Tunnel {
	float width; // width value.
	float height; // height value.
public:
	Position position; // postition class.
	Velocity velocity; // velocity class.
	TunnelEntrance entrance; // entrance class.

	/**
	* Tunnel class constructor.
	* 
	* @param _width Contains the height value.
	* @param _height Contains the height value.
	* @param position_x Contains the position x value.
	* @param position_y Contains the position y value.
	* @param velocity_x Contains the velocity x value.
	* @param velocity_y Contains the velocity y value.
	* @param entrace_height Contains the entrace height value.
	* @param entrace_position_y Contains the entrace position y value.
	*/
	Tunnel(float _width, float _height, float position_x, float position_y,
		float velocity_x, float velocity_y, float entrace_height, float entrace_position_y);

	/**
	* A get function.
	* 
	* @return the width value.
	*/
	float getWidth();

	/**
	* This function must be called to update the game frame.
	*/
	void update();
};

/* Class Bird */
class Bird {
private:
	float radius; // radius value.
public:
	bool isAlive;
	Position position; // position class.
	Velocity velocity; // velocity class.

	/**
	* Bird class constructor.
	* 
	* @param radius Contains the bird radius.
	* @param position_x Contains the position x.
	* @param position_y Contains the position y.
	* @param velocity_x Contains the velocity x.
	* @param velocity_y Contains the velocity y.
	*/
	Bird(float radius, float position_x, float position_y, float velocity_x, float velocity_y);

	/**
	* A get function.
	* 
	* @return The bird radius.
	*/
	float getRadius();

	/**
	* Call this function to make the bird flap.
	* 
	* @param speed Contains how much the bird will flap
	*/
	void flap(float speed);

	/**
	* This function apply a gravity force in the bird.
	* 
	* @param gravity_acceleration How much the bird will accelerate.
	*/
	void gravity(float gravity_acceleration);

	/**
	* Call this function to update the bird.
	* 
	* @param gravity_acceleration How much the bird will accelerate.
	* @param max_velocity The bird maximum velocity.
	*/
	void update(float gravity_acceleration, float max_velocity);
};

/* class FlappyBird */
class FlappyBird {
private:
	int n_birds;
	int points; // The points made by the player.
	int resX; // Resolution X.
	int resY; // Resolution Y.
	float tunnel_width; // Contains the width of the tunnels.
	float tunnel_height; // Contains the height of the tunnels.
	float tunnel_velocity;  // Contains the velocity of the tunnels.
	std::list <Tunnel> ::iterator next_tunnel;
	std::list <Tunnel> ::iterator next_bird_tunnel; // Contains the next tunnel that the bird will pass.
public:
	std::list <Tunnel> tunnel_list; // a list of tunnels.
	std::list <Bird> bird_list;; // class bird.

	/**
	* FlappyBird class constructor.
	* 
	* @param _resX Contains the x resolution.
	* @param _resY Contains the y resolution.
	* @param tunnel_velocity Contains the tunnel velocity.
	* @param bird_radius Contains the bird radius.
	* @param bird_postion_x Contains the bird position x.
	* @param bird_postion_y Contains the bird position y.
	* @param bird_velocity_x Contains the bird velocity y.
	* @param bird_velocity_y Contains the bird velocity y.
	*/
	FlappyBird(int _n_birds, int _resX, int _resY, float tunnel_velocity, float bird_radius, float bird_position_x,
		float bird_position_y, float bird_velocity_x, float bird_velocity_y);

	/**
	* A get function.
	* 
	* @return The points.
	*/
	int getPoints();

	/**
	* A get function.
	*
	* @return The Width of the tunnels.
	*/
	float getTunnelWidth();

	/**
	* A get function.
	*
	* @return The Height of the tunnels.
	*/
	float getTunnelHeight();

	void addBird(float _radius,
		float position_x, float position_y, float velocity_x, float velocity_y); //////////////////////
	/**
	* Call this function to add a new tunnel at the start of the screen
	* of the game.
	*/
	void addTunnel();

	/**
	* Update a frame of the game.
	* 
	* @return A boolean. If its false, its because the player has lost the game.
	*/
	int update(float gravity_acceleration, float bird_max_velocity);

	/**
	*  Call this function to verify if the player has lost.
	* 
	* @return True if the player has lost, else return False
	*/
	int isLost(Bird bird);

	/**
	* Call this function to restart the game.
	*/
	void restart();
	int isOnTunnel(Bird bird);
	Array::Array2D getIaInput(Bird bird);
};

#endif // FLAPPY_BIRD_H