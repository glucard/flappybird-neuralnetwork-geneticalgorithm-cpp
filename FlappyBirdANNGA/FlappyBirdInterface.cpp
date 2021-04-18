#include "FlappyBirdInterface.h"

namespace FlappyBirdInterce {
	void drawInterfaceNN(NeuralNetwork::NeuralNetwork* nn) {
        // Setting the bird radius.
        float lower_resolution = (RESOLUTION_X > RESOLUTION_Y ? RESOLUTION_X : RESOLUTION_Y);
        float game_bird_radius = RESOLUTION_Y / 30;

        // Starting the instance of FlappyBird.
        FlappyBird game(1, RESOLUTION_X, RESOLUTION_Y, TUNNEL_VELOCITY, game_bird_radius, RESOLUTION_X / 10,
            RESOLUTION_Y / 2, 0, 0);

        // Setting sfml window.
        sf::RenderWindow window(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y), "Flappy Bird");
        window.setFramerateLimit(30);

        // Initiating sfml shapes. 
        sf::RectangleShape tunnel_shape(sf::Vector2f(game.getTunnelWidth(), game.getTunnelHeight()));
        sf::RectangleShape tunnel_entrace_shape(sf::Vector2f(0, 0));
        sf::CircleShape bird_shape(game_bird_radius);

        // Loading textures.
        sf::Texture tunnel_texture, bird_texture;
        if (tunnel_texture.loadFromFile("sprites/tunnel.png")) {
            tunnel_shape.setTexture(&tunnel_texture);
        }
        else {
            tunnel_shape.setFillColor(sf::Color::Green);
        }
        if (bird_texture.loadFromFile("sprites/bird.png")) {
            bird_shape.setTexture(&bird_texture);
        }
        else {
            bird_shape.setFillColor(sf::Color::Yellow);
        }
        tunnel_entrace_shape.setFillColor(sf::Color::Black);

        // configure the sf::text.
        sf::Font font;
        font.loadFromFile("PressStart2P-Regular.ttf");
        sf::Text text;
        text.setFont(font);
        text.setPosition(sf::Vector2f(RESOLUTION_X / 2, RESOLUTION_Y / 10));
        text.setCharacterSize(84);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        std::ostringstream osstr;

        Array::Array2D a(FLOAT_TYPE, 0, 0, NULL);
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed: // detect keyboard press.
                    switch (event.key.code) {
                    case sf::Keyboard::Space:
                        game.bird_list.begin()->flap(BIRD_FLAP_ACCELERATION);
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
            }

            window.clear(sf::Color::Black); // clear screen.

            if (game.update(GRAVITY_ACCELERATION, BIRD_MAX_SPEED)) { // if the game is updating

                a.overwrite(game.getIaInput(*game.bird_list.begin()));
                a.overwrite(nn->predict(a));
                if (((float**)a.data)[0][0] > 0.5) {
                    game.bird_list.begin()->flap(BIRD_FLAP_ACCELERATION);
                }
                // Set bird_shape pos.
                bird_shape.setPosition(sf::Vector2f(game.bird_list.begin()->position.getX(), RESOLUTION_Y - game.bird_list.begin()->position.getY()));

                // For each tunnel in tunnel list:
                std::list <Tunnel> ::iterator i_tunnel;
                for (i_tunnel = game.tunnel_list.begin(); i_tunnel != game.tunnel_list.end(); ++i_tunnel) {
                    // Set shape pos.
                    tunnel_shape.setPosition(sf::Vector2f(i_tunnel->position.getX(), i_tunnel->position.getY()));
                    tunnel_entrace_shape.setPosition(sf::Vector2f(i_tunnel->position.getX(), RESOLUTION_Y - i_tunnel->entrance.y - i_tunnel->entrance.height));
                    tunnel_entrace_shape.setSize(sf::Vector2f(i_tunnel->getWidth(), i_tunnel->entrance.height));

                    // Draw the shapes.
                    window.draw(tunnel_shape);
                    window.draw(tunnel_entrace_shape);
                }
                osstr.str(""); // erase osstr.
                osstr << game.getPoints(); // write in osstr.
                text.setString(osstr.str()); // set text string.

                // Draw the text and bird_shape.
                window.draw(text);
                window.draw(bird_shape);

                // Display them.
                window.display();
            }
            else { // if the game is not updating (the player lost):
                game.restart(); // restart the game.
            }
        }

	}

    void drawInterfaceGA(GeneticAlgorithm::GeneticAlgorithm* ga) {

        GeneticAlgorithm::Individual** population = ga->getPopulation();
        NeuralNetwork::NeuralNetwork* nn;
        int population_size, i_population;
        population_size = ga->getPopulationSize();

        // Setting the bird radius.
        float lower_resolution = (RESOLUTION_X > RESOLUTION_Y ? RESOLUTION_X : RESOLUTION_Y);
        float game_bird_radius = RESOLUTION_Y / 30;

        // Starting the instance of FlappyBird.
        FlappyBird game(population_size, RESOLUTION_X, RESOLUTION_Y, TUNNEL_VELOCITY, game_bird_radius, RESOLUTION_X / 10,
            RESOLUTION_Y / 2, 0, 0);

        // Setting sfml window.
        sf::RenderWindow window(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y), "Flappy Bird");
        window.setFramerateLimit(30);

        // Initiating sfml shapes. 
        sf::RectangleShape tunnel_shape(sf::Vector2f(game.getTunnelWidth(), game.getTunnelHeight()));
        sf::RectangleShape tunnel_entrace_shape(sf::Vector2f(0, 0));
        sf::CircleShape** bird_shapes = (sf::CircleShape**)malloc(population_size * sizeof(sf::CircleShape*));
        for (int i = 0; i < population_size; i++) {
            bird_shapes[i] = new sf::CircleShape(game_bird_radius);
            bird_shapes[i]->setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
        }

        // Loading textures.
        sf::Texture tunnel_texture, bird_texture;
        if (tunnel_texture.loadFromFile("sprites/tunnel.png")) {
            tunnel_shape.setTexture(&tunnel_texture);
        }
        else {
            tunnel_shape.setFillColor(sf::Color::Green);
        }
        tunnel_entrace_shape.setFillColor(sf::Color::Black);

        // configure the sf::text.
        sf::Font font;
        font.loadFromFile("PressStart2P-Regular.ttf");
        sf::Text text;
        text.setFont(font);
        text.setPosition(sf::Vector2f(RESOLUTION_X / 2, RESOLUTION_Y / 10));
        text.setCharacterSize(84);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        std::ostringstream osstr;

        Array::Array2D a(FLOAT_TYPE, 0, 0, NULL);
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed: // detect keyboard press.
                    switch (event.key.code) {
                    case sf::Keyboard::Space:
                        game.bird_list.begin()->flap(BIRD_FLAP_ACCELERATION);
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
            }

            window.clear(sf::Color::Black); // clear screen.

            if (game.update(GRAVITY_ACCELERATION, BIRD_MAX_SPEED)) { // if the game is updating

                Array::Array2D a(FLOAT_TYPE, 0, 0, NULL);
                std::list <Bird> ::iterator i_bird;
                for (int i = 0; i < 1; i++) {
                    i_population = 0;
                    for (i_bird = game.bird_list.begin(); i_bird != game.bird_list.end(); ++i_bird) {
                        if(game.isLost(*i_bird)) i_bird->isAlive = false;
                        nn = (NeuralNetwork::NeuralNetwork*)population[i_population]->getData();
                        a.overwrite(game.getIaInput(*i_bird));
                        a.overwrite(nn->predict(a));
                        if (((float**)a.data)[0][0] > 0.5) {
                            i_bird->flap(BIRD_FLAP_ACCELERATION);
                        }
                        i_population++;
                    }
                }

                // For each tunnel in tunnel list:
                std::list <Tunnel> ::iterator i_tunnel;
                for (i_tunnel = game.tunnel_list.begin(); i_tunnel != game.tunnel_list.end(); ++i_tunnel) {
                    // Set shape pos.
                    tunnel_shape.setPosition(sf::Vector2f(i_tunnel->position.getX(), i_tunnel->position.getY()));
                    tunnel_entrace_shape.setPosition(sf::Vector2f(i_tunnel->position.getX(), RESOLUTION_Y - i_tunnel->entrance.y - i_tunnel->entrance.height));
                    tunnel_entrace_shape.setSize(sf::Vector2f(i_tunnel->getWidth(), i_tunnel->entrance.height));

                    // Draw the shapes.
                    window.draw(tunnel_shape);
                    window.draw(tunnel_entrace_shape);
                }
                osstr.str(""); // erase osstr.
                osstr << game.getPoints(); // write in osstr.
                text.setString(osstr.str()); // set text string.

                // Draw the text.
                window.draw(text);
                int i = 0;
                for (i_bird = game.bird_list.begin(); i_bird != game.bird_list.end(); ++i_bird) {
                    // Set bird_shape pos and draw bird.
                    if (i_bird->isAlive) {
                        bird_shapes[i]->setPosition(sf::Vector2f(i_bird->position.getX(), RESOLUTION_Y - i_bird->position.getY()));
                        window.draw(*bird_shapes[i]);
                    }
                    i++;
                }
                // Display them.
                window.display();
            }
            else { // if the game is not updating (the player lost):
                game.restart(); // restart the game.
                break;
            }
        }
        for (int i = 0; i < population_size; i++) {
            delete bird_shapes[i];
        }
        free(bird_shapes);
    }
}
