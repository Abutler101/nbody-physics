/*
 * This is an inefficient, O(N^2), N-body simulation in 3D, visualised in 2D.
 * The viewer is looking top down at Z=0, with Z extending positively to infinity
 * 'into the screen'. Z axis position is visualised through scaling of the drawn
 * bodies, as Z increases, size decreases. When at a display border, graphical
 * position is wrapped around however physics positions are kept absolute.
 */
#include <iostream>
#include <cstring>
#include <deque>
#include <tuple>
#include <numeric>
#include <cmath>

#include "SFML/Graphics.hpp"

#include "nbody/bodies.cpp"

// Graphics Settings
const uint16_t Width = 1500u;
const uint16_t Height = 900u;
const int FrameRate = 500;
const int FrameAverage = 500;

//Physics Settings
const sf::Vector3f GlobalForce = {0, 0, 0};
const uint16_t MaxBodies = 100u;
const float GravitationalConst = 1;

//---------------------------------------------------------
// N-Body procession
void update_velocities(RenderedBody **tracked_bodies, uint16_t body_count, float dt){
    for(uint16_t i = 0; i < body_count; i++){
        RenderedBody *target_body = tracked_bodies[i];
        sf::Vector3f net_force = GlobalForce; // Even if no other bodies, there'll still be global force
        for(uint16_t j = 0; j < body_count; j++){
            RenderedBody *active_body = tracked_bodies[j];
            if(active_body==target_body){ continue;}
            const sf::Vector3f vec_dist = active_body->get_position() - target_body->get_position();
            const float lin_dist_sqr = vec_dist.x * vec_dist.x + vec_dist.y * vec_dist.y + vec_dist.z * vec_dist.z;
            const float attraction = GravitationalConst * target_body->get_mass() * active_body->get_mass() / lin_dist_sqr;
            net_force += sf::Vector3f(
                    attraction * vec_dist.x,
                    attraction * vec_dist.y,
                    attraction * vec_dist.z
                    );
        }
        const sf::Vector3f acceleration = net_force / target_body->get_mass();
        target_body->accelerate(acceleration, dt);
    }
}

//---------------------------------------------------------
// Scenarios

// ----- Single Plain ----- //
void two_equal_mass_equal_dist(RenderedBody **tracked_bodies, uint16_t *body_count){
    tracked_bodies[0] = new RenderedBody(10, 10, {500, 400, 0}, {2,-0.75,0});
    tracked_bodies[1] = new RenderedBody(10, 10, {1000, 400, 0}, {-2,0.75,0});
    *body_count = 2;
}

void three_equal_mass_equal_dist(RenderedBody **tracked_bodies, uint16_t *body_count){
    tracked_bodies[0] = new RenderedBody(10, 5, {750, 300, 0}, {2,-0.75,0});
    tracked_bodies[1] = new RenderedBody(10, 5, {900, 600, 0}, {-2,0.75,0});
    tracked_bodies[2] = new RenderedBody(10, 5, {600, 600, 0}, {-2,0.75,0});
    *body_count = 3;
}

void three_equal_mass_equal_dist_hvy_cent(RenderedBody **tracked_bodies, uint16_t *body_count){
    tracked_bodies[0] = new RenderedBody(30, 300, {750, 450, 0}, {0, 0, 0}, sf::Color::White);
    tracked_bodies[1] = new RenderedBody(10, 2, {750, 300, 0}, {10,0,0});
    tracked_bodies[2] = new RenderedBody(10, 2, {900, 600, 0}, {-10,0,0});
    tracked_bodies[3] = new RenderedBody(10, 2, {600, 600, 0}, {-10,0,0});
    *body_count = 4;
}

void n_equal_mass_equal_dist_no_cent(RenderedBody **tracked_bodies, uint16_t *body_count, uint16_t target_count = 30){
    sf::Vector2i center = {750, 450};
    float radius = 100;

    for(uint16_t i=0; i<target_count; i++){
        double rand_angle = 6.28318 * (double(rand())/RAND_MAX);
        sf::Vector3f pos = {float(cos(rand_angle) * radius) + center.x,float(sin(rand_angle) * radius) + center.y, 0};
        float v_x;
        if(pos.y > float(center.y)){v_x=-3;}else{v_x=3;}
        tracked_bodies[i] = new RenderedBody(10, 2, pos, {v_x,0,0});
    }
    *body_count = target_count;
}

// ----- Multi Plain ----- //
void mp_two_equal_mass_equal_dist(RenderedBody **tracked_bodies, uint16_t *body_count){
    // Fake centre at 750, 450, 250
    tracked_bodies[0] = new RenderedBody(10, 10, {500, 450, 0}, {2,-0.75,0});
    tracked_bodies[1] = new RenderedBody(10, 10, {1000, 450, -500}, {-2,0.75,0});
    *body_count = 2;
}
void mp_three_equal_mass_equal_dist(RenderedBody **tracked_bodies, uint16_t *body_count){
    // Fake centre at 750, 450, 250
    tracked_bodies[0] = new RenderedBody(10, 10, {750, 300, 250}, {2,-0.75,0});
    tracked_bodies[1] = new RenderedBody(10, 10, {900, 600, 0}, {-2,0.75,0});
    tracked_bodies[2] = new RenderedBody(10, 10, {600, 600, -500}, {-2,0.75,0});
    *body_count = 3;
}

int main(){
//---------------------------------------------------------
// Window Setup
    auto window = sf::RenderWindow{{Width, Height}, "3D Physics In 2D"};
    window.setFramerateLimit(FrameRate);
    uint32_t frame_count = 0;
    std::deque<int32_t> frame_times;
    sf::Clock frame_timer;

//---------------------------------------------------------
// Simulations Setup
    uint16_t body_count = 0u;
    RenderedBody *tracked_bodies[MaxBodies];
    memset(tracked_bodies, 0, sizeof(tracked_bodies));
    // ----- Single Plain Scenarios ----- //
    // two_equal_mass_equal_dist(tracked_bodies, &body_count);
    // three_equal_mass_equal_dist(tracked_bodies, &body_count);
    // three_equal_mass_equal_dist_hvy_cent(tracked_bodies, &body_count);
    // n_equal_mass_equal_dist_no_cent(tracked_bodies, &body_count, 80);

    // ----- Multi Plain Scenarios ----- //
     mp_two_equal_mass_equal_dist(tracked_bodies, &body_count);
    // mp_three_equal_mass_equal_dist(tracked_bodies, &body_count);

//---------------------------------------------------------
// Draw Loop
    while(window.isOpen()){
        frame_timer.restart();
        // Window Events
        for(auto event = sf::Event{}; window.pollEvent(event);){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        float dt = 1.0;
        // Call an N-Body method to set the velocs
        update_velocities(tracked_bodies, body_count, dt);

        for(uint16_t i = 0; i < body_count and i < MaxBodies; i++) {
            tracked_bodies[i]->update_position(dt);
        }

        // Draw the Bodies
        window.clear();
        for(int i = 0; i < body_count and i < MaxBodies; i++) {
            window.draw(tracked_bodies[i]->get_shape(Width, Height));
        }
        window.display();

        // Frame Time Tracking
        frame_count ++;
        int32_t frame_duration = frame_timer.getElapsedTime().asMilliseconds();
        frame_times.emplace_front(frame_duration);
        if(frame_count % FrameAverage == 0){
            frame_times.pop_back();
            auto count = static_cast<float>(frame_times.size());
            auto avg_duration =  std::reduce(frame_times.begin(), frame_times.end()) / count;
            printf("Avg %f ms/frame over last %d frames\n", avg_duration, FrameAverage);
        }
    }
    return 0;
}
