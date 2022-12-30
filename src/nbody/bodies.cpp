//
// Created by Alexis on 26/12/2022.
//
#ifndef PHYSICS_STUFF_BODIES
#define PHYSICS_STUFF_BODIES

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

static const sf::Color color_options[] = {
        sf::Color::Red,
        sf::Color::Blue,
        sf::Color::Green,
        sf::Color::Magenta,
        sf::Color::Yellow
};

class RenderedBody{
    private:
        // Position of Body == Position of center of Body
        sf::Vector3f _position;
        sf::Vector3f _velocity;
        float _radius;
        float _mass;

        // Display properties
        sf::Color _color;
        sf::Vector3f _offset;

    public:
        RenderedBody(
            float radius,
            float mass,
            sf::Vector3f position,
            sf::Vector3f velocity,
            sf::Color color = color_options[rand() % 5]
        ){
            this->_radius = radius;
            this->_mass = mass;
            this->_position = position;
            this->_velocity = velocity;
            this->_color = color;
            // Stored position is Centroid, map to position of shape origin (top left of bounding box)
            this->_offset = {-this->_radius, this->_radius, 0};
        };

        sf::Vector3f get_position(){return this->_position;};
        sf::Vector3f get_velocity(){return this->_velocity;};
        float get_mass() const{return this->_mass;};
        float get_density() const{return this->_mass / (1.333f * 3.14159f * this->_radius);};
        sf::CircleShape get_shape(const uint16_t w_width, const uint16_t w_height){
            float radius = this->_radius * (1-(this->_position.z/800));
            sf::CircleShape shape= sf::CircleShape(radius, 500);
            float graphical_x = this->_position.x + this->_offset.x;
            float graphical_y = this->_position.y + this->_offset.y;

            // Deal with falling off the screen
            if(graphical_x > float(w_width)){
                graphical_x -= float(w_width);
                this->_offset.x -= float(w_width);
            }
            else if(graphical_x < 0){
                graphical_x += float(w_width);
                this->_offset.x += float(w_width);
            }
            if(graphical_x > float(w_width)){
                graphical_x -= float(w_width);
                this->_offset.x -= float(w_width);
            }
            else if(graphical_x < 0){
                graphical_x += float(w_width);
                this->_offset.x += float(w_width);
            }
            if(graphical_y > float(w_height)){
                graphical_y -= float(w_height);
                this->_offset.y -= float(w_height);
            }
            else if(graphical_y < 0){
                graphical_y += float(w_height);
                this->_offset.y += float(w_height);
            }

            shape.setPosition(graphical_x, graphical_y);
            shape.setFillColor(this->_color);
            return shape;
        };

        void accelerate(sf::Vector3f acceleration, float dt){
            this->_velocity += acceleration * dt;
        }

        void update_position(float dt){
            this->_position += sf::Vector3f(
                    this->_velocity.x * dt,
                    this->_velocity.y * dt,
                    this->_velocity.z * dt
                    );
        };
};

#endif