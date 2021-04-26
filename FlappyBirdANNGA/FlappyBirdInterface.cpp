#include "FlappyBirdInterface.h"

namespace FlappyBirdInterce {

    void drawLine(sf::RenderWindow *window, float color_scale, float alpha_scale, float start_x, float start_y, float end_x, float end_y) {

        float real_alpha_color = (alpha_scale * alpha_scale < color_scale * color_scale ? alpha_scale : color_scale);
        

        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(start_x, start_y)),
            sf::Vertex(sf::Vector2f(end_x, end_y))
        };
        
        if (color_scale < 0) {
            sf::Color negative_color(0, 255, 0, fabsf(real_alpha_color) * 255);
            line[0].color = negative_color; 
            line[1].color = negative_color;
        }
        else {
            sf::Color positive_color(0, 0, 255, fabsf(real_alpha_color) * 255);
            line[0].color = positive_color;
            line[1].color = positive_color;
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

        // Setting neuron_shape.
        sf::CircleShape neuron_shape(default_neuron_radius);
        neuron_shape.setOutlineColor(sf::Color::Red);
        neuron_shape.setOutlineThickness(1);

        // Draw neurons from input layer.
        higher_neuron_layer_value = input.absf();
        neuron_radius = default_neuron_radius / input.width;
        neuron_shape.setRadius(neuron_radius - 2);
        for (int i = 0; i < input.width; i++) {
            neuron_shape.setPosition(sf::Vector2f(position_x, position_y + neuron_radius * 2 * i));
            color_scale = ((float**)input.data)[0][i] / higher_neuron_layer_value;
            neuron_shape.setFillColor(sf::Color(255 * color_scale, 0, 0));
            window->draw(neuron_shape);
        }

        Array::Array2D* weights = NULL;

        // For each layer:
        for (int i_layer = 0; i_layer < nn->n_layers; i_layer++) {

            // set weights and predict the layer.
            layer = nn->layers[i_layer]->data;
            switch (nn->layers[i_layer]->type) {
            case Layer::dense:
                input.overwrite(((Layer::Dense*)layer)->predict(input));
                weights = &((Layer::Dense*)layer)->weights;

            }

            // Set neuron_shape radius.
            neuron_radius = default_neuron_radius / input.width;
            neuron_shape.setRadius(neuron_radius - 2);

            // For each layer predict (input array2d):
            higher_neuron_layer_value = input.absf();
            for (int i_input = 0; i_input < input.width; i_input++) {

                // The higher value will be the most colorful.
                color_scale = ((float**)input.data)[0][i_input] / higher_neuron_layer_value;

                // the last layer is a single sigmoid.
                if (i_layer == nn->n_layers - 1) {
                    color_scale = ((float**)input.data)[0][i_input];
                }

                // For each layer weight: 
                higher_weight_layer_value = weights->absf();
                for (int i_weights = 0; i_weights < weights->height; i_weights++) {
                    
                    // Draw a synapse (line).
                    drawLine(window, ((float**)weights->data)[i_weights][i_input] / higher_weight_layer_value, color_scale,
                        position_x + default_neuron_radius * 2 * i_layer + prev_neuron_radius,
                        position_y + prev_neuron_radius * 2 * i_weights + prev_neuron_radius,
                        position_x + default_neuron_radius * 2 * (1 + i_layer) + neuron_radius,
                        position_y + neuron_radius * 2 * i_input + neuron_radius);
                }

                // set neuron_shape color and position.
                neuron_shape.setFillColor(sf::Color(255 * color_scale, 0, 0));
                neuron_shape.setPosition(sf::Vector2f(position_x + default_neuron_radius *2*(1+i_layer),
                                                      position_y + neuron_radius *2*i_input));
                // draw neuron_shape.
                window->draw(neuron_shape);
            }
            // Set the previous neuron radius as the actual.
            prev_neuron_radius = neuron_radius;
        }
        // in the end, destroy de array2d input data.
        input.destroyData();
    }

    void drawInterfaceGA(AnnGA::AnnGA* annga) {

        GeneticAlgorithm::Individual** population;
        NeuralNetwork::NeuralNetwork* nn;
        int population_size, i_population;

        // set the population.
        population = annga->getGA()->getPopulation();
        population_size = annga->getGA()->getPopulationSize();

        // Setting the bird radius.
        float lower_resolution = (RESOLUTION_X > RESOLUTION_Y ? RESOLUTION_X : RESOLUTION_Y);
        float game_bird_radius = RESOLUTION_Y / 20;

        // Starting the instance of FlappyBird.
        FlappyBird game(population_size, RESOLUTION_X, RESOLUTION_Y, TUNNEL_VELOCITY, game_bird_radius, RESOLUTION_X / 10,
            RESOLUTION_Y / 2, 0, 0);

        // Setting sfml window.
        sf::RenderWindow window(sf::VideoMode(RESOLUTION_X + 200.f, RESOLUTION_Y), "Flappy Bird");
        window.setFramerateLimit(60);

        // Initiating sfml shapes. /////////////////////////// 
        sf::RectangleShape sky_shape(sf::Vector2f(RESOLUTION_X, RESOLUTION_Y * 4));
        sf::RectangleShape back_background_shape(sf::Vector2f(RESOLUTION_X * 4, RESOLUTION_Y));
        sf::RectangleShape front_background_shape(sf::Vector2f(RESOLUTION_X * 2, RESOLUTION_Y));
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
        sf::Texture tunnel_texture, bird_texture, front_background_texture, back_background_texture, sky_texture;

        if (bird_texture.loadFromFile("sprites/bird.png")) {
            for (int i = 0; i < population_size; i++) {
                bird_shapes[i]->setTexture(&bird_texture);
            }

        }

        if (sky_texture.loadFromFile("sprites/sky.png")) {
            sky_shape.setTexture(&sky_texture);
        }
        else {
            sky_shape.setFillColor(sf::Color(0, 175, 255));
        }

        if (back_background_texture.loadFromFile("sprites/back_background.png")) {
            back_background_shape.setTexture(&back_background_texture);
        }
        else {
            front_background_shape.setFillColor(sf::Color(0, 175, 255));
        }

        if (front_background_texture.loadFromFile("sprites/front_background.png")) {
            front_background_shape.setTexture(&front_background_texture);
        }
        else {
            front_background_shape.setFillColor(sf::Color(0, 175, 255, 0));
        }

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

        sf::Text generation_text;
        generation_text.setFont(font);
        generation_text.setPosition(sf::Vector2f(RESOLUTION_X + 5, 50));
        generation_text.setCharacterSize(14);
        generation_text.setFillColor(sf::Color::White);

        std::ostringstream osstr;

        Array::Array2D a(FLOAT_TYPE, 0, 0, NULL);
        float fitness = 0;
        int birds_alive = 0;

        float sky_position_y = 0;
        float back_background_position_x = 0;
        float front_background_position_x = 0;

        // While the window is open:
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

            // clear the window.
            window.clear();

            sky_position_y -= 0.1;
            if (sky_position_y < -RESOLUTION_Y * 3) sky_position_y = 0;
            sky_shape.setPosition(sf::Vector2f(0, sky_position_y));
            window.draw(sky_shape);

            back_background_position_x -= 0.5;
            if (back_background_position_x < -RESOLUTION_X * 3) back_background_position_x = 0;
            back_background_shape.setPosition(sf::Vector2f(back_background_position_x, 0));
            window.draw(back_background_shape);

            front_background_position_x -= 2;
            if (front_background_position_x < -RESOLUTION_X * 1) front_background_position_x = 0;
            front_background_shape.setPosition(sf::Vector2f(front_background_position_x, 0));
            window.draw(front_background_shape);

            // if the game is updating and have at least one bird alive:
            if (game.update(GRAVITY_ACCELERATION, BIRD_MAX_SPEED)) {

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

                Array::Array2D a(FLOAT_TYPE, 0, 0, NULL);
                std::list <Bird> ::iterator i_bird;

                // for each bird/neuralnetwork:
                birds_alive = 0;
                i_population = 0;
                for (i_bird = game.bird_list.begin(); i_bird != game.bird_list.end(); ++i_bird) {
                    // verify is the bird has lost:
                    if (game.isLost(*i_bird)) { // if lost:
                        i_bird->isAlive = false; // set bird is not alive;
                        
                        // skip to the next bird//nn.
                        i_population++;
                        continue;
                    }
                    birds_alive++;

                    // set the bird nn.
                    nn = (NeuralNetwork::NeuralNetwork*)population[i_population]->getData();

                    // get the nn input game state.
                    a.overwrite(game.getIaInput(*i_bird));

                    // predict the nn and overwrite the input array.
                    a.overwrite(nn->predict(a));

                    // if the output (sigmoid) is bigger than 0.5:
                    if (((float**)a.data)[0][0] > 0.5) {

                        // the bird flap.
                        i_bird->flap(BIRD_FLAP_ACCELERATION);
                        // population[i_population]->addFitness(-1);
                    }

                    if (game.isOnTunnel(*i_bird)) population[i_population]->addFitness(1);

                    // set the bird_shape position and draw it.
                    bird_shapes[i_population]->setPosition(sf::Vector2f(i_bird->position.getX(), RESOLUTION_Y - i_bird->position.getY()));
                    window.draw(*bird_shapes[i_population]);

                    i_population++;
                }

                // Draw point text.
                osstr.str(""); // erase osstr.
                osstr << game.getPoints(); // write in osstr.
                text.setString(osstr.str()); // set text string.
                window.draw(text);

                ///////////////////// drawing panel. /////////////////////////////////
                window.draw(panel_background_shape);

                // drawing the first nn that is alive. If the ga is ordered by fitness, then will draw the best nn.
                i_population = 0;
                // this for basically search for the first bird alive, and draw they neuralnetwork.
                for (i_bird = game.bird_list.begin(); i_bird != game.bird_list.end(); ++i_bird) {
                    if (i_bird->isAlive) {

                        // draw neural network.
                        a.overwrite(game.getIaInput(*i_bird));
                        nn = (NeuralNetwork::NeuralNetwork*)population[i_population]->getData();
                        drawNN(&window, nn, a.copy(), RESOLUTION_X+5, 470, 200-5, 200);
                        
                        // draw panel bird.

                        window.draw(*bird_shapes[i_population]); // draw again in front


                        bird_shapes[i_population]->setPosition(sf::Vector2f(RESOLUTION_X+10, 370));
                        window.draw(*bird_shapes[i_population]);

                        fitness = population[i_population]->getFitness();
                        break;
                    }
                    i_population++;
                }

                // Draw generation text.
                osstr.str(""); // erase osstr.
                osstr << "Generation " << annga->getGA()->getGeneration() + 1; // write in osstr.
                osstr << "\n\n\nRemains: " << birds_alive;
                osstr << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nfitness: " << fitness;
                generation_text.setString(osstr.str()); // set text string.
                window.draw(generation_text);

                ///////////////////////////////////////////////////////////////////////
                // Display the window.
                window.display();
            }
            else { // if the game is not updating (then no one bird is alive):
                game.restart(); // restart the game.
                annga->getGA()->orderByFitness();
                annga->getGA()->repocreate(population_size / 4, AnnGA::repocreateIndividual);
                annga->getGA()->eraseFitness();
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
