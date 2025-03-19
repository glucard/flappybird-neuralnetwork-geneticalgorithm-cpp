#include "FlappyBirdInterface.hpp"


#include <iostream>
#include <cmath>

double sunlightIntensity(double x) {
    if (x < 0 || x > 1800) return 0.0;
    x = x/1800;
    float f = cos(4 * M_PI * x);
    float g = 0.5 * (1+ cos(2*M_PI*x));
    
    f = (f >= g) ? f : g;
    g = sin(x*M_PI);
    f = (f >= g) ? f : g;
    f = f - 0.7;
    return f > 0.f ? f*2 : 0.f;
}

sf::Texture computeOcclusionTexture(std::vector<sf::RectangleShape*> shapes, const sf::RenderWindow& window)
{
    // Get window size
    sf::Vector2u windowSize = window.getSize();
    
    // Create an off-screen render texture with the same dimensions as the window
    sf::RenderTexture renderTexture(sf::Vector2u(windowSize.x, windowSize.y));

    // Clear with transparent color so areas with no shape remain transparent
    renderTexture.clear(sf::Color::Transparent);
    
    // Set the view to match the window so the shape is drawn in the correct coordinates
    renderTexture.setView(window.getView());
    
    // Draw the shape onto the off-screen render texture
    for (auto& shape : shapes){
        renderTexture.draw(*shape);
    }
    renderTexture.display();
    
    // Extract the texture from the render texture.
    // Note: getTexture() returns a reference to the texture owned by renderTexture,
    // so if you need an independent texture, copy it.
    
    return renderTexture.getTexture();
}


namespace FlappyBirdInterce {

    void drawLine(sf::RenderWindow *window, float color_scale, float alpha_scale, float start_x, float start_y, float end_x, float end_y) {

        float real_alpha_color = (alpha_scale * alpha_scale < color_scale * color_scale ? alpha_scale : color_scale);
        

        // sf::Vertex line[] =
        // {
        //     sf::Vertex(sf::Vector2f(start_x, start_y)),
        //     sf::Vertex(sf::Vector2f(end_x, end_y))
        // };
        
        sf::VertexArray line(sf::PrimitiveType::Lines);
        line[0].position = sf::Vector2f(start_x, start_y);
        line[1].position = sf::Vector2f(end_x, end_y);

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
        // window->draw(line, 2);
        window->draw(line);
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
	    float tunnel_base_acceleration = 0.f;//0.0001;
        FlappyBird game(population_size, RESOLUTION_X, RESOLUTION_Y, TUNNEL_VELOCITY, tunnel_base_acceleration, game_bird_radius, RESOLUTION_X / 10,
            RESOLUTION_Y / 2, 0, 0);

        // Setting sfml window.
        sf::RenderWindow window(sf::VideoMode({RESOLUTION_X + 200, RESOLUTION_Y}), "Flappy Bird");
        float frame_rate_limit = 60;
        window.setFramerateLimit(frame_rate_limit);
        window.setPosition(sf::Vector2i(100, 100));

        // Initiating sfml shapes. /////////////////////////// 
        sf::RectangleShape sky_shape(sf::Vector2f(RESOLUTION_X, RESOLUTION_Y * 4));
        sf::RectangleShape back_background_shape(sf::Vector2f(RESOLUTION_X * 4, RESOLUTION_Y));
        sf::RectangleShape front_background_shape(sf::Vector2f(RESOLUTION_X * 2, RESOLUTION_Y));
        sf::RectangleShape tunnel_shape(sf::Vector2f(game.getTunnelWidth(), game.getTunnelHeight()));
        sf::RectangleShape tunnel_entrace_shape(sf::Vector2f(0, 0));
        sf::CircleShape** bird_shapes = (sf::CircleShape**)malloc(population_size * sizeof(sf::CircleShape*));
        for (int i = 0; i < population_size; i++) {
            bird_shapes[i] = new sf::CircleShape(game_bird_radius);
            bird_shapes[i]->setFillColor(sf::Color(100 + rand() % 155, 100 + rand() % 155, 100 + rand() % 155));
        }

        // Setting background panel shape.
        sf::RectangleShape panel_background_shape(sf::Vector2f(200.f, RESOLUTION_Y));
        panel_background_shape.setFillColor(sf::Color::Black);
        panel_background_shape.setPosition(sf::Vector2f(RESOLUTION_X, 0));

        // Loading textures.
        sf::Texture tunnel_texture, bird_texture, front_background_texture, back_background_texture, sky_texture;

        if (bird_texture.loadFromFile("assets/sprites/bird.png")) {
            for (int i = 0; i < population_size; i++) {
                bird_shapes[i]->setRadius(game_bird_radius *1.2);
                bird_shapes[i]->setTexture(&bird_texture);
            }

        }

        if (sky_texture.loadFromFile("assets/sprites/sky.png")) {
            sky_shape.setTexture(&sky_texture);
        }
        else {
            sky_shape.setFillColor(sf::Color(0, 175, 255));
        }

        if (back_background_texture.loadFromFile("assets/sprites/back_background.png")) {
            back_background_shape.setTexture(&back_background_texture);
        }
        else {
            front_background_shape.setFillColor(sf::Color(0, 175, 255));
        }

        if (front_background_texture.loadFromFile("assets/sprites/front_background.png")) {
            front_background_shape.setTexture(&front_background_texture);
        }
        else {
            front_background_shape.setFillColor(sf::Color(0, 175, 255, 0));
        }

        if (tunnel_texture.loadFromFile("assets/sprites/tunnel.png")) {
            tunnel_shape.setTexture(&tunnel_texture);
        }
        else {
            tunnel_shape.setFillColor(sf::Color::Green);
        }
        tunnel_entrace_shape.setFillColor(sf::Color(0, 0, 0, 220));


        // shading
        // 1) Load the multi-light shader (before the main game loop)
        sf::Shader multiLightShader;
        if (!multiLightShader.loadFromFile("assets/shaders/multiLight.frag", sf::Shader::Type::Fragment))
        {
            std::cerr << "Error loading multi-light shader\n";
        }

        // 2) Prepare arrays for the light sources
        //    Suppose you have two lights: one for the moon and one for the sun.
        //    We'll store their positions, radii, and colors in std::vectors.
        int numLights = 5; // We have 2 light sources
        std::vector<sf::Vector2f> lightPositions(numLights);
        std::vector<float>         lightRadii(numLights);
        std::vector<float>         lightShapeRadius(numLights);
        std::vector<float>         attenuation_factor(numLights);
        std::vector<sf::Glsl::Vec3> lightColors(numLights);

        // Example initial values (you’ll update them later in the main loop)
        lightPositions[0] = sf::Vector2f(400, 300);  // e.g., moon near top
        lightPositions[1] = sf::Vector2f(400, -600);  // e.g., sun in the middle
        lightPositions[2] = sf::Vector2f(400, -1500);  // e.g., moon again
        lightPositions[3] = sf::Vector2f(677, 300);  // e.g., moon again
        lightPositions[4] = sf::Vector2f(152, 533);  // e.g., moon again

        lightRadii[0]     = 1000.f;  // moon’s radius
        lightRadii[1]     = 1200.f;  // sun’s radius
        lightRadii[2]     = 1000.f;  // moon’s radius
        lightRadii[3]     = 500.f;  // moon’s radius
        lightRadii[4]     = 500.f;  // moon’s radius

        
        attenuation_factor[0]     = 0.7f;  // moon’s radius
        attenuation_factor[1]     = 1.f;  // sun’s radius
        attenuation_factor[2]     = 0.7f;  // moon’s radius
        attenuation_factor[3]     = 0.2f;  // moon’s radius
        attenuation_factor[4]     = 0.2f;  // moon’s radius

        
        lightShapeRadius[0] = 100;
        lightShapeRadius[1] = 200;
        lightShapeRadius[2] = 100;
        lightShapeRadius[3] = 25;
        lightShapeRadius[4] = 25;

        lightColors[0]    = sf::Glsl::Vec3(1.0f, 1.0f, 1.0f);  // white-ish for moon
        lightColors[1]    = sf::Glsl::Vec3(1.0f, 1.0f, 0.6f);  // yellow-ish for sun
        lightColors[2]    = sf::Glsl::Vec3(1.0f, 1.0f, 1.0f);  // white-ish for moon
        lightColors[3]    = sf::Glsl::Vec3(1.0f, 1.0f, 1.0f);  // white-ish for moon
        lightColors[4]    = sf::Glsl::Vec3(1.0f, 1.0f, 1.0f);  // white-ish for moon

        // Set uniforms that won’t change often (e.g., the number of lights)
        multiLightShader.setUniform("numLights", numLights);
        multiLightShader.setUniform("ambientStrength", 0.0f); // Tweak to taste

        // configure the sf::text.
        sf::Font font;
        font.openFromFile("assets/PressStart2P-Regular.ttf");
        sf::Text text(font);
        text.setPosition(sf::Vector2f(RESOLUTION_X / 2, RESOLUTION_Y / 10));
        text.setCharacterSize(84);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        sf::Text generation_text(font);
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
            while (const std::optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    window.close();
                else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){ // detect keyboard press.
                    if (keyPressed->code == sf::Keyboard::Key::Space){
                        if (frame_rate_limit != 60) {
                            frame_rate_limit = 60;
                            window.setFramerateLimit(60);
                        }
                        else {
                            frame_rate_limit = 0;
                        }
                        window.setFramerateLimit(frame_rate_limit);
                        //game.bird_list.begin()->flap(BIRD_FLAP_ACCELERATION);
                    }
                        

                    // switch (event.key.code) {
                    //     if keyPressed->code == sf::
                    //         if (frame_rate_limit != 60) {
                    //             frame_rate_limit = 60;
                    //             window.setFramerateLimit(60);
                    //         }
                    //         else {
                    //             frame_rate_limit = 0;
                    //         }
                    //         window.setFramerateLimit(frame_rate_limit);
                    //         //game.bird_list.begin()->flap(BIRD_FLAP_ACCELERATION);
                    //     default:
                    //         break;
                    //     }
                }
            }
            // clear the window.
            window.clear();

            // Get window size
            sf::Vector2u windowSize = window.getSize();
            // Create an off-screen render texture with the same dimensions as the window
            sf::RenderTexture renderOcclusionTexture(sf::Vector2u(windowSize.x, windowSize.y));
            // Clear with transparent color so areas with no shape remain transparent
            renderOcclusionTexture.clear(sf::Color::Transparent);
            // Set the view to match the window so the shape is drawn in the correct coordinates
            renderOcclusionTexture.setView(window.getView());

            sky_position_y -= 0.1;
            if (sky_position_y < -RESOLUTION_Y * 3) sky_position_y = 0;
            sky_shape.setPosition(sf::Vector2f(0, sky_position_y));

            back_background_position_x -= 0.5;
            if (back_background_position_x < -RESOLUTION_X * 3) back_background_position_x = 0;
            back_background_shape.setPosition(sf::Vector2f(back_background_position_x, 0));

            front_background_position_x -= 2;
            if (front_background_position_x < -RESOLUTION_X * 1) front_background_position_x = 0;
            front_background_shape.setPosition(sf::Vector2f(front_background_position_x, 0));


            
            sf::Vector2u winSize = window.getSize();    

            // float backgroundOffsetY = /* some logic here */ 0.0f;

            // 300 => new_y //  res_y => new_res
            // 300/scaled_y = RESOLUTION_Y / winSize.y

            float x_scale_factor = winSize.x / (float)(RESOLUTION_X + 200);
            float y_scale_factor = winSize.y / (float)RESOLUTION_Y;
            
            lightPositions[0].x = 400.f * x_scale_factor; // moon
            lightPositions[1].x = 400.f * x_scale_factor; // moon
            lightPositions[2].x = 400.f * x_scale_factor; // moon

            lightPositions[0].y = (300 + sky_position_y) * y_scale_factor; // moon
            lightPositions[1].y = (1200.0f + sky_position_y) * y_scale_factor; // sun
            lightPositions[2].y = (2100.0f + sky_position_y) * y_scale_factor; // sun
            lightPositions[3].y = (300.0f + sky_position_y) * y_scale_factor; // sun
            lightPositions[4].y = (533.0f + sky_position_y) * y_scale_factor; // sun
            
            multiLightShader.setUniform("screen_resolution", sf::Vector2f(winSize.x, winSize.y));
            


            // std::cout << "y: " << -sky_position_y << "Moon: " << lightPositions[0].y << " Sun:" << lightPositions[1].y << std::endl;

            float ambient_strength = sunlightIntensity(-sky_position_y);
            //std::cout << "-sky_position_y: " << -sky_position_y << "scaled_y: " << y_scale_factor << std::endl;
            multiLightShader.setUniform("ambientStrength", ambient_strength); // Tweak to taste

            // std::vector<sf::RectangleShape*> occlusion_shapes;
            // occlusion_shapes.push_back(&back_background_shape);
            // sf::Texture occlusion_texture = computeOcclusionTexture(occlusion_shapes, window);
            renderOcclusionTexture.draw(back_background_shape);
            renderOcclusionTexture.display();
            sf::Texture occlusion_texture = renderOcclusionTexture.getTexture();

            // Draw the occlusion mask as a semi-transparent rectangle
            sf::RectangleShape maskShape(sf::Vector2f(occlusion_texture.getSize().x, occlusion_texture.getSize().y));

            maskShape.setPosition(sf::Vector2f(0.f, 0.f));
            maskShape.setTexture(&occlusion_texture);
            // window.draw(maskShape);

            // Now update the shader with these dynamic values:
            multiLightShader.setUniformArray("lightPos", &lightPositions[0], numLights);
            multiLightShader.setUniformArray("lightRadius", &lightRadii[0], numLights);
            multiLightShader.setUniformArray("lightShapeRadius", &lightShapeRadius[0], numLights);
            multiLightShader.setUniformArray("attenuation_factor", &attenuation_factor[0], numLights);
            multiLightShader.setUniformArray("lightColor", &lightColors[0], numLights);
            multiLightShader.setUniform("occlusionTexture", occlusion_texture);
            multiLightShader.setUniform("flareIntensity", 0.15f); // Adjust as needed;

            window.draw(sky_shape, &multiLightShader);
            window.draw(back_background_shape, &multiLightShader);
            window.draw(front_background_shape, &multiLightShader);


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
                    // //window.draw(tunnel_shape);
                    // window.draw(tunnel_shape, &multiLightShader);
                    // window.draw(tunnel_entrace_shape);
                    // occlusion_shapes.push_back(&tunnel_shape);
                    //renderOcclusionTexture.draw(tunnel_shape);
                }
                
                // sf::Texture tunnel_occlusion_texture = computeOcclusionTexture(occlusion_shapes, window);
                renderOcclusionTexture.display();
                sf::Texture tunnel_occlusion_texture = renderOcclusionTexture.getTexture();
                multiLightShader.setUniform("occlusionTexture", tunnel_occlusion_texture);
                
                for (i_tunnel = game.tunnel_list.begin(); i_tunnel != game.tunnel_list.end(); ++i_tunnel) {
                    
                    // Set shape pos.
                    tunnel_shape.setPosition(sf::Vector2f(i_tunnel->position.getX(), i_tunnel->position.getY()));
                    tunnel_entrace_shape.setPosition(sf::Vector2f(i_tunnel->position.getX(), RESOLUTION_Y - i_tunnel->entrance.y - i_tunnel->entrance.height));
                    tunnel_entrace_shape.setSize(sf::Vector2f(i_tunnel->getWidth(), i_tunnel->entrance.height));
                    window.draw(tunnel_shape, &multiLightShader);
                    window.draw(tunnel_entrace_shape, &multiLightShader);
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
                    window.draw(*bird_shapes[i_population], &multiLightShader);

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
                        //drawNN(&window, nn, a.copy(), RESOLUTION_X+5, 470, 200-5, 200);
                        
                        // draw panel bird.

                        window.draw(*bird_shapes[i_population], &multiLightShader); // draw again in front


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
                annga->getGA()->repocreate(population_size / 8, AnnGA::repocreateIndividual);
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
