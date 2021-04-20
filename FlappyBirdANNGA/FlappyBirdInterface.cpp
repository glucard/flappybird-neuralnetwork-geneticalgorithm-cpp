#include "FlappyBirdInterface.h"

namespace FlappyBirdInterce {

    void drawLine(sf::RenderWindow *window, float color_scale, float alpha_scale, float start_x, float start_y, float end_x, float end_y) {
        /*
        float x, y;
        x = end_x - start_x;
        y = end_y - start_y;
        float angle = atan2f(x, y) / 3.14;
        
        sf::RectangleShape rectangle;
        rectangle.setPosition(start_x, start_y);
        rectangle.setSize(sf::Vector2f(x, 5));
        rectangle.setRotation(angle * 180);
        window->draw(rectangle);
        */
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(start_x, start_y)),
            sf::Vertex(sf::Vector2f(end_x, end_y))
        };
        if (color_scale < 0) {
            line[0].color = sf::Color(0, color_scale * 255, 0, (alpha_scale < color_scale ? alpha_scale : color_scale) * 255); //, (alpha_scale < color_scale ? alpha_scale : color_scale) * 255 
            line[1].color = sf::Color(0, color_scale * 255, 0, (alpha_scale < color_scale ? alpha_scale : color_scale) * 255);
        }
        else {
            line[0].color = sf::Color(0, 0, color_scale * 255, (alpha_scale < color_scale ? alpha_scale : color_scale) * 255);
            line[1].color = sf::Color(0, 0, color_scale * 255, (alpha_scale < color_scale ? alpha_scale : color_scale) * 255);
        }
        window->draw(line, 2, sf::Lines);
    }

    void drawNN(sf::RenderWindow *window, NeuralNetwork::NeuralNetwork *nn, Array::Array2D input, float position_x, float position_y, float width, float height) {
        float minor_screen_size = (width < height ? width : height);
        float default_neuron_radius = minor_screen_size / ((nn->n_layers+1) * 2); // + 1 (input_layer)
        float neuron_radius;
        float prev_neuron_radius = default_neuron_radius / nn->input_shape[1];
        void* layer;
        float higher_neuron_layer_value; // used for determine color.
        float higher_weight_layer_value; // used for determine color.
        float color_scale;

        /*
        sf::RectangleShape background(sf::Vector2f(minor_screen_size, default_neuron_radius * 2));
        background.setFillColor(sf::Color::Black);
        background.setPosition(sf::Vector2f(position_x, position_y));

        window->draw(background);
        */

        sf::CircleShape neuron_shape(default_neuron_radius);
        neuron_shape.setOutlineColor(sf::Color::Red);
        neuron_shape.setOutlineThickness(1);

        // Draw neurons from input layer.
        higher_neuron_layer_value = input.absf();
        neuron_radius = default_neuron_radius / input.width;
        neuron_shape.setRadius(neuron_radius - 2);
        for (int i = 0; i < input.width; i++) {
            neuron_shape.setPosition(sf::Vector2f(position_x,
                position_y + neuron_radius * 2 * i));
            color_scale = ((float**)input.data)[0][i] / higher_neuron_layer_value;
            neuron_shape.setFillColor(sf::Color(255 * color_scale, 0, 0));
            window->draw(neuron_shape);
        }

        Array::Array2D* weights = NULL;

        // Draw weights and outputs from nn.
        for (int i_layer = 0; i_layer < nn->n_layers; i_layer++) {

            layer = nn->layers[i_layer]->data;
            switch (nn->layers[i_layer]->type) {
            case Layer::dense:
                input.overwrite(((Layer::Dense*)layer)->predict(input));
                weights = &((Layer::Dense*)layer)->weights;

            }
            neuron_radius = default_neuron_radius / input.width;
            neuron_shape.setRadius(neuron_radius - 2);
            higher_neuron_layer_value = input.absf();
            for (int i_input = 0; i_input < input.width; i_input++) {
                color_scale = ((float**)input.data)[0][i_input] / higher_neuron_layer_value;

                // the last layer is a single sigmoid.
                if (i_layer == nn->n_layers - 1) {
                    color_scale = ((float**)input.data)[0][i_input];
                    weights->displayData();
                }

                higher_weight_layer_value = weights->absf();
                for (int i_weights = 0; i_weights < weights->height; i_weights++) {
                    drawLine(window, ((float**)weights->data)[i_weights][i_input] / higher_weight_layer_value, color_scale,
                        position_x + default_neuron_radius * 2 * i_layer + prev_neuron_radius,
                        position_y + prev_neuron_radius * 2 * i_weights + prev_neuron_radius,
                        position_x + default_neuron_radius * 2 * (1 + i_layer) + neuron_radius,
                        position_y + neuron_radius * 2 * i_input + neuron_radius);
                }
                neuron_shape.setFillColor(sf::Color(255 * color_scale, 0, 0));
                neuron_shape.setPosition(sf::Vector2f(position_x + default_neuron_radius *2*(1+i_layer),
                                                      position_y + neuron_radius *2*i_input));
                window->draw(neuron_shape);
            }
            prev_neuron_radius = neuron_radius;
        }
        input.destroyData();
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
        sf::RenderWindow window(sf::VideoMode(RESOLUTION_X + 200.f, RESOLUTION_Y), "Flappy Bird");
        window.setFramerateLimit(60);

        // Initiating sfml shapes. /////////////////////////// 
        sf::RectangleShape tunnel_shape(sf::Vector2f(game.getTunnelWidth(), game.getTunnelHeight()));
        sf::RectangleShape tunnel_entrace_shape(sf::Vector2f(0, 0));
        sf::CircleShape** bird_shapes = (sf::CircleShape**)malloc(population_size * sizeof(sf::CircleShape*));
        for (int i = 0; i < population_size; i++) {
            bird_shapes[i] = new sf::CircleShape(game_bird_radius);
            bird_shapes[i]->setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
        }

        // Setting background panel shape.
        sf::RectangleShape panel_background_shape(sf::Vector2f(200.f, RESOLUTION_Y));
        panel_background_shape.setFillColor(sf::Color::Black);
        panel_background_shape.setPosition(sf::Vector2f(RESOLUTION_X, 0));

        // Loading textures.
        sf::Texture tunnel_texture, bird_texture;
        if (tunnel_texture.loadFromFile("sprites/tunnel.png")) {
            tunnel_shape.setTexture(&tunnel_texture);
        }
        else {
            tunnel_shape.setFillColor(sf::Color::Green);
        }
        tunnel_entrace_shape.setFillColor(sf::Color(0, 0, 0, 220));

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

            window.clear(sf::Color(0, 175, 255)); // clear screen.

            if (game.update(GRAVITY_ACCELERATION, BIRD_MAX_SPEED)) { // if the game is updating

                Array::Array2D a(FLOAT_TYPE, 0, 0, NULL);
                std::list <Bird> ::iterator i_bird;

                i_population = 0;
                for (i_bird = game.bird_list.begin(); i_bird != game.bird_list.end(); ++i_bird) {
                    if (game.isLost(*i_bird)) { 
                        i_bird->isAlive = false;
                        i_population++;
                        continue;
                    }
                    nn = (NeuralNetwork::NeuralNetwork*)population[i_population]->getData();

                    a.overwrite(game.getIaInput(*i_bird));

                    a.overwrite(nn->predict(a));
                    if (((float**)a.data)[0][0] > 0.5) {
                        i_bird->flap(BIRD_FLAP_ACCELERATION);
                    }

                    i_population++;
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

                // Draw point text.
                osstr.str(""); // erase osstr.
                osstr << game.getPoints(); // write in osstr.
                text.setString(osstr.str()); // set text string.
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

                ///////////////////// drawing panel. /////////////////////////////////
                window.draw(panel_background_shape);

                // drawing the first nn that is alive. If the ga is ordered by fitness, then will draw the best nn.
                i_population = 0;
                for (i_bird = game.bird_list.begin(); i_bird != game.bird_list.end(); ++i_bird) {
                    if (i_bird->isAlive) {
                        a.overwrite(game.getIaInput(*i_bird));
                        nn = (NeuralNetwork::NeuralNetwork*)population[i_population]->getData();
                        drawNN(&window, nn, a.copy(), RESOLUTION_X+5, 50, 200-5, 200);
                        break;
                    }
                    i_population++;
                }
                ///////////////////////////////////////////////////////////////////////
                // Display them.
                window.display();
            }
            else { // if the game is not updating (the player lost):
                game.restart(); // restart the game.
                break;
            }
        }
        // Free all alocated memory.
        for (int i = 0; i < population_size; i++) {
            delete bird_shapes[i];
        }
        free(bird_shapes);
        a.destroyData();
    }
}
