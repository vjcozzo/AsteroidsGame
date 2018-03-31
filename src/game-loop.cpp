#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <random>
#include "game-loop.hpp"

class Game {
    protected:
        ALLEGRO_DISPLAY *disp;
        ALLEGRO_BITMAP *bg;
        ALLEGRO_BITMAP *asteroid_layer;
        ALLEGRO_BITMAP *overlay;
        ALLEGRO_BITMAP *bullet_img;
        ALLEGRO_FONT *font_luxirb_large;
        ALLEGRO_FONT *font_luxirb_med;
        std::vector<std::string *> *stock_options;
        std::vector<Asteroid *> *all_asteroids;
        std::vector<Bullet *> *shots;
        unsigned int health;
        int bullet_x_com, bullet_y_com;
    
        double cosine(double input) {
            double result = 1.0;
            double scalar = -1.0/2.0;
            int ind = 2;
            double last_poly = 1;
            while (ind < 62) {
                last_poly *= (input*input);
                result += (scalar * last_poly);
                scalar /= (0-ind-1);
                scalar /= (ind+2);
                ind += 2;
/*            printf("\tintermediate result of cosine is %f\n", result);*/
            }
/*        printf("\t\tcosine of %f is about %f\n\n", input, result);*/
            return result;
        }
    
        double sine(double input) {
            double result = input;
            double scalar = -1.0/6.0;
            int ind = 3;
            double last_poly = input;
            while (ind < 62) {
                last_poly *= (input*input);
                result += (scalar * last_poly);
/*            printf("Now adding term %f*%f (degree = index = %d)\n", scalar, last_poly, ind);*/
                scalar /= (0-ind-1);
                scalar /= (ind+2);
                ind += 2;
/*            printf("\tintermediate result of sine is %f\n", result);*/
            }
/*        printf("\t\tsine of %f is about %f\n\n", input, result);*/
            return result;
        }
    
        void update_stars() {
            unsigned int num_stars, ind;
            ALLEGRO_BITMAP *star = al_load_bitmap("res/star.png");
            al_set_target_bitmap(bg);
            num_stars = generator() % 12;
            al_set_target_bitmap(bg);
            for (ind=0; ind < num_stars; ind ++) {
                int x_pos = (generator()*generator() % SCREEN_WIDTH);
                int y_pos = (generator()*generator() % SCREEN_HEIGHT);
                al_draw_bitmap(star, (double)x_pos, (double)y_pos, 0);
            }
/*        al_draw_text(font_luxirb_med, al_map_rgb(190, 190, 190),
            5, 5, ALLEGRO_ALIGN_LEFT, stock_options->at(0)->c_str());
        al_draw_text(font_luxirb_med, al_map_rgb(190, 190, 190),
            SCREEN_WIDTH-180, 5, ALLEGRO_ALIGN_LEFT, stock_options->at(1)->c_str());*/
/*        al_set_target_backbuffer(disp);*/
        }
    
        /* Precondition: x1, y1, x2, y2 are nonnegative integers
         * Postcondition: returns the Euclidean distance,
         *      which will always be a nonnegative integer!
         *      between the two points.
         */
        int rect_dist(int x1, int y1, int x2, int y2) {
            if (x1 > x2) {
                if (y1 > y2) {
                    return (y1 - y2 + x1 - x2);
                } else {
                    return (y2 - y1 + x1 - x2);
                }
            } else {
                if (y1 > y2) {
                    return (y1 - y2 + x2 - x1);
                } else {
                    return (y2 - y1 + x2 - x1);
                }
            }
        }
        void update_overlay() {
            std::stringstream health_display;
            health_display << health;
/*            int ind;*/
            al_set_target_bitmap(overlay);
            al_draw_text(font_luxirb_med, al_map_rgb(250, 250, 250),
                5, 5, ALLEGRO_ALIGN_LEFT, stock_options->at(0)->c_str());
/*            for (ind = 0; ind < health; ind ++) {
 *            draw the player health icon a certain  number of times,
 *            proportional to the actual player health.
                al_draw_bitmap();
            }*/
            al_draw_text(font_luxirb_med, al_map_rgb(250, 250, 250),
                200, 5, ALLEGRO_ALIGN_LEFT, health_display.str().c_str());
            al_draw_text(font_luxirb_med, al_map_rgb(250, 250, 190),
                SCREEN_WIDTH-540, 5, ALLEGRO_ALIGN_LEFT, stock_options->at(1)->c_str());
            al_set_target_backbuffer(disp);
        }

        /* Precondition:
         *    | x and y are nonnegative integers representing a pixel location
         *    | the pixel is an overlap between a projectile and an asteroid.
         *    -
         * Postcondition:
         *    returns -1 if there is no asteroid matching the given pixel;
         *      otherwise returns the INDEX of the asteroid that overlaps with
         *      the pixel.
         */
        int find_asteroid_overlap(int x, int y) {
            /* Cycle through each asteroid in existence ... */
            unsigned int ind = 0;
            for (ind = 0; ind < all_asteroids->size(); ind ++) {
                /* If it's too far away from the pixel in question, then don't even bother
                 * doing any other checks. */
                Asteroid *next_asteroid = all_asteroids->at(ind);
                int dist = rect_dist(x, y, next_asteroid->x_pos(), next_asteroid->y_pos());
                int comp = next_asteroid->get_width() + next_asteroid->get_height();
                if (dist < comp) {
/*                    find_path();*/
                    /* The answer is "yes", i.e., this asteroid
                     * IS IN FACT the asteroid with which the projectile collided,
                     * if and only if
                     * all paths from the center of the asteroid to the 
                     * given pixel in question lie entirely WITHIN
                     * the asteroid being considered.
                     */
                    /* so... TODO, make this hit detection 
                     * or, "path detection" if you will, more accurate */
                    return ind;
                }
            }
            return -1;
        }
    
    public:
        Game(ALLEGRO_DISPLAY *display) {
            if (display != NULL) {
                disp = display;
            }
            stock_options = new std::vector<std::string *>();
            stock_options->push_back(new std::string("HP"));
            stock_options->push_back(new std::string("SCORE"));
            stock_options->push_back(new std::string("VOL"));
            stock_options->push_back(new std::string("HEH"));
            font_luxirb_large = al_load_font("./res/fonts/luxirb.ttf", 64, 0);
            if (font_luxirb_large == NULL) {
                fprintf(stderr, "Failed to load the font luxirb.ttf\n");
                fprintf(stderr, "Perhaps the font addon or TTF addon was not loaded in advance?\n");
            }
            font_luxirb_med = al_load_font("./res/fonts/luxirb.ttf", 36, 0);
            if (font_luxirb_med == NULL) {
                fprintf(stderr, "Failed to load the font luxirb.ttf in medium size\n");
                fprintf(stderr, "Perhaps the font addon or TTF addon was not loaded in advance?\n");
            }
            all_asteroids = new std::vector<Asteroid *>();
            shots = new std::vector<Bullet *>();
        }
        Game() {
            disp = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
            stock_options = new std::vector<std::string *>();
            stock_options->push_back(new std::string("HP"));
            stock_options->push_back(new std::string("SCORE"));
            stock_options->push_back(new std::string("VOL"));
            stock_options->push_back(new std::string("HEH"));
            font_luxirb_large = al_load_font("./res/fonts/luxirb.ttf", 64, 0);
            if (font_luxirb_large == NULL) {
                fprintf(stderr, "Failed to load the font luxirb.ttf\n");
            }
            font_luxirb_med = al_load_font("./res/fonts/luxirb.ttf", 36, 0);
            if (font_luxirb_med == NULL) {
                fprintf(stderr, "Failed to load the font luxirb.ttf in medium size\n");
            }
            all_asteroids = new std::vector<Asteroid *>();
            shots = new std::vector<Bullet *>();
        }
        
        ~Game() {
            unsigned int ind, upper=stock_options->size();
            for (ind=0; ind < upper; ind ++) {
                delete stock_options->at(ind);
            }
            stock_options->clear();
            delete stock_options;
            upper=shots->size();
            for (ind=0; ind < upper; ind ++) {
                delete shots->at(ind);
            }
            shots->clear();
            delete shots;
            al_destroy_font(font_luxirb_large);
            al_destroy_font(font_luxirb_med);
        }
        
        int launch() {
            int game_state = 1, result;
            bool redraw = false;
            ALLEGRO_TIMER *ticker_timer = NULL;
            ALLEGRO_TIMER *asteroid_spawn_timer = NULL;
/*            double FPS = 19.9;*/
/*            unsigned int ind;*/
            
            /*** INIT TIMERS ***/
            ticker_timer = al_create_timer(1.0 / FPS);
            if (ticker_timer == NULL) {
                fprintf(stderr, "ERR: failed to load FPS timer :-(\n");
                al_destroy_display(disp);
                return -1;
            }
            
            asteroid_spawn_timer = al_create_timer(5);
            if (asteroid_spawn_timer == NULL) {
                fprintf(stderr, "ERR: failed to load FPS timer :-(\n");
                al_destroy_display(disp);
                return -1;
            }
            
            /*** INIT FONTS ***/
            /* Note to developers:
             * original font files were in /usr/share/fonts/TTF/
             * in case you need to know that drectory. */
            ALLEGRO_FONT *font_amok = al_load_font("./res/fonts/ComputerAmok.ttf", 54, 0);
            if (font_amok == NULL) {
                fprintf(stderr, "Failed to load the font ComputerAmok.ttf\n");
                al_destroy_display(disp);
                al_destroy_timer(ticker_timer);
                al_destroy_timer(asteroid_spawn_timer);
                return -1;
            }
            ALLEGRO_FONT *font_luxirb = al_load_font("./res/fonts/luxirb.ttf", 54, 0);
            if (font_luxirb == NULL) {
                fprintf(stderr, "Failed to load the font luxirb.ttf\n");
                al_destroy_display(disp);
                al_destroy_timer(ticker_timer);
                al_destroy_timer(asteroid_spawn_timer);
                return -1;
            }
            ALLEGRO_FONT *font_luxirb_small = al_load_font("./res/fonts/luxirb.ttf", 20, 0);
            if (font_luxirb_small == NULL) {
                fprintf(stderr, "Failed to load the font luxirb.ttf in small size\n");
                al_destroy_display(disp);
                al_destroy_timer(ticker_timer);
                al_destroy_timer(asteroid_spawn_timer);
                return -1;
            }
            
            bg = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
            overlay = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
            asteroid_layer = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
            bullet_img = al_load_bitmap("./res/bullet0.png");
            if (bullet_img == NULL) {
                fprintf(stderr, "ERR - cannot find find file bullet0.png in resource directory\n");
            }
            bullet_x_com = al_get_bitmap_width(bullet_img);
            bullet_y_com = al_get_bitmap_height(bullet_img);
        /*    al_lock_bitmap(img, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);*/
        /*    img_display = al_load_bitmap("res/bg-map-1-provinces.png");*/
            
            ALLEGRO_EVENT_QUEUE *decisions_response_queue = NULL;
            decisions_response_queue = al_create_event_queue();
        
            result = al_install_keyboard();
            if (result == 0) {
                fprintf(stderr, "ERR: failed to install the keyboard    :(\n");
                al_destroy_display(disp);
                al_destroy_timer(ticker_timer);
                al_destroy_timer(asteroid_spawn_timer);
                return -1;
            }
        
            result = al_install_mouse();
            if (result == 0) {
                fprintf(stderr, "ERR: failed to install the mouse       :(\n");
                al_destroy_display(disp);
                al_destroy_timer(ticker_timer);
                al_destroy_timer(asteroid_spawn_timer);
                return -1;
            }
        
            al_register_event_source(decisions_response_queue, al_get_display_event_source(disp));
            al_register_event_source(decisions_response_queue, al_get_timer_event_source(ticker_timer));
            al_register_event_source(decisions_response_queue, al_get_timer_event_source(asteroid_spawn_timer));
            al_register_event_source(decisions_response_queue, al_get_keyboard_event_source());
            al_register_event_source(decisions_response_queue, al_get_mouse_event_source());
            
            al_clear_to_color(al_map_rgb(5, 126, 200));
            al_draw_text(font_luxirb, al_map_rgb(2, 2, 2),
                SCREEN_WIDTH/2, SCREEN_HEIGHT/4, ALLEGRO_ALIGN_CENTER, "\'Asteroids\'");
            al_draw_text(font_luxirb_med, al_map_rgb(2, 2, 2),
                SCREEN_WIDTH/2, 18.0f*SCREEN_HEIGHT/32, ALLEGRO_ALIGN_CENTER, "Eventually will put a twist on it...");
            al_draw_text(font_luxirb_med, al_map_rgb(2, 2, 2),
                SCREEN_WIDTH/2, 22.0f*SCREEN_HEIGHT/32, ALLEGRO_ALIGN_CENTER, "SPACE - Start game ");
            al_draw_text(font_luxirb_med, al_map_rgb(2, 2, 2),
                SCREEN_WIDTH/2, 26.0f*SCREEN_HEIGHT/32, ALLEGRO_ALIGN_CENTER, "ESC/Q - Quit to desktop");
            al_flip_display();
            
            /* start timers */
            al_start_timer(ticker_timer);
            al_start_timer(asteroid_spawn_timer);
            
            while (game_state == 1) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(decisions_response_queue, &ev);
                
                if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                        /* */
                        game_state = 2;
        /*                printf("\tEVENT PROCESSED, paused == %d\n", paused);*/
                    } else if (ev.keyboard.keycode == ALLEGRO_KEY_Q ||
                                ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                       /* */
                       game_state = 0;
                       printf("\"Q\"/ESC Key has been pressed (exiting)\n");
                    }
                } else if (ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
                    al_clear_to_color(al_map_rgb(5, 126, 200));
                    al_draw_text(font_luxirb, al_map_rgb(2, 2, 2),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/4, ALLEGRO_ALIGN_CENTER, "\'Asteroids\'");
                    al_draw_text(font_luxirb_med, al_map_rgb(2, 2, 2),
                        SCREEN_WIDTH/2, 18.0f*SCREEN_HEIGHT/32, ALLEGRO_ALIGN_CENTER, "Eventually will put a twist on it...");
                    al_draw_text(font_luxirb_med, al_map_rgb(2, 2, 2),
                        SCREEN_WIDTH/2, 22.0f*SCREEN_HEIGHT/32, ALLEGRO_ALIGN_CENTER, "SPACE - Start game ");
                    al_draw_text(font_luxirb_med, al_map_rgb(2, 2, 2),
                        SCREEN_WIDTH/2, 26.0f*SCREEN_HEIGHT/32, ALLEGRO_ALIGN_CENTER, "ESC/Q - Quit to desktop");
                    al_flip_display();
                } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    game_state = 0;
                }
            }
            
            /* Set up data that needs to be PRINTED over top the game sprites */
            /* e.g., HP, scores, upgrades? */
/*            std::stringstream str_buffer;*/
            ALLEGRO_BITMAP *space_ship_img = al_load_bitmap("res/space_ship.png");
            /* may replace the following with small positive constants 
             * ... this will allow us to call al_draw_scaled_bitmap()
             * with simpler parameters */
            int space_ship_wid = al_get_bitmap_width(space_ship_img);
            int space_ship_hgt = al_get_bitmap_height(space_ship_img);
            double ship_angle = 0.0f;
            bool turn_left = false;
            bool turn_right = false;
            bool speed_up = false;
            bool speed_down = false;
            bool brake = false;
/*            bool speed_down = false;*/
            
/*            ALLEGRO_BITMAP *large_asteroid_img = al_create_bitmap(90, 90);*/
            ALLEGRO_BITMAP *star = al_load_bitmap("res/star.png");
            
/*            unsigned int num_stars = generator() % 12;*/
            double pos_x = (SCREEN_WIDTH)/2;
            double pos_y = (SCREEN_HEIGHT)/2;
            double vel_x = 0;
            double vel_y = 0;

            std::stringstream stream;
            int threshold = 10;
            /* MAIN GAME LOOP (MGL) (mgl) */
            /* which branches on the current game state variable */
            while (game_state > 0) {
                if (game_state == 2) {
                    al_set_target_bitmap(bg);
                    al_clear_to_color(al_map_rgb(2, 2, 2));
                    /*for (ind=0; ind < num_stars; ind ++) {
                        int x_pos = (generator()*generator() % SCREEN_WIDTH);
                        int y_pos = (generator()*generator() % SCREEN_HEIGHT);
                        al_draw_bitmap(star, (double)x_pos, (double)y_pos, 0);
                    }*/
                    for (int i=0; i < 4; i++) {
                        update_stars();
                    }
                    update_overlay();
                    
                    al_draw_bitmap(bg, 0, 0, 0);
                    printf("Drew text to the bitmap.\n");
/*                    al_draw_bitmap(space_ship_img, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0);*/
                    al_draw_scaled_rotated_bitmap(space_ship_img, space_ship_wid/2, space_ship_hgt/2,
                        ((int)pos_x), ((int)pos_y), 0.8, 0.8, ship_angle, 0);
                    al_flip_display();
                    game_state = 3;
                }
                if (game_state == 3) {
                    /* */
                    ALLEGRO_EVENT ev;
                    al_wait_for_event(decisions_response_queue, &ev);
                    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                        if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                            /* Note: THIS calculation IS ACTUALLY CORRECT!!!
                             * It uses the CURRENT ship_angle value,
                             * which need not equal the direction of the ship's
                             * current velocity vector!!
                             * Don't delete it unless you REALLY
                             * know what you're doing! */
                            double hypotenuse = space_ship_hgt / 2.0;
                            double del_x = hypotenuse*sine(ship_angle);
                            double del_y = /*(-1)**/hypotenuse*sine(0-1.57089-ship_angle);
                            double b_vel_x = 5*sine(0-3.14159-ship_angle)/NORM;
                            double b_vel_y = 5*cosine(0-3.14159-ship_angle)/NORM;
/*                            printf("Ok, x,y shift for bullet init pos is (%f, %f)\n", del_x, del_y);*/
                            Bullet *new_sh = new Bullet(pos_x+del_x, pos_y+del_y, b_vel_x, b_vel_y, ship_angle);
                            shots->push_back(new_sh);
/*                            redraw = true;*/
                        } else if (ev.keyboard.keycode == ALLEGRO_KEY_Q) {
                            game_state = 0;
                        } else if (ev.keyboard.keycode == ALLEGRO_KEY_B) {
                            brake = true;
                            if (vel_x > BRAKE_VEL) {
                                vel_x -= BRAKE_VEL;
                            } else if (vel_x < (-1)*BRAKE_VEL) {
                                vel_x += BRAKE_VEL;
                            } else {
                                vel_x /= 2;
                            }
                            if (vel_y > BRAKE_VEL) {
                                vel_y -= BRAKE_VEL;
                            } else if (vel_y < (-1)*BRAKE_VEL) {
                                vel_y += BRAKE_VEL;
                            } else {
                                vel_y /= 2;
                            }
/*                            printf("`brake` is now true.\n");*/
                        } else if (ev.keyboard.keycode == ALLEGRO_KEY_R) {
/*                            update_stars();*/
                        } else if (ev.keyboard.keycode == ALLEGRO_KEY_E) {
                            int det = generator() /* * generator() */% 2;
                            double enter_x=0.0, enter_y=0.0, vi_x=0.0, vi_y=0.0;
                            double ang=0.0;
                            generator();
                            double spin = generator() % 80;
                            if (det == 0) {
                                det = generator() % 4;
                                if (det < 3) {
                                    enter_x = generator() % SCREEN_WIDTH;
                                    enter_y = 1;
                                    vi_x = 0.01;
                                    vi_y = 4.01;
                                } else {
                                    enter_x = 1;
                                    enter_y = generator() % SCREEN_HEIGHT;
                                    vi_x = 4.01;
                                    vi_y = 0.01;
                                }
                            } else {
                                det = generator() % 4;
                                if (det < 3) {
                                    enter_x = generator() % SCREEN_WIDTH;
                                    enter_y = SCREEN_HEIGHT-2;
                                    vi_x = -0.01;
                                    vi_y = -4.01;
                                } else {
                                    enter_x = SCREEN_WIDTH-2;
                                    enter_y = generator() % SCREEN_HEIGHT;
                                    vi_x = -4.01;
                                    vi_y = -0.01;
                                }
                            }
                            spin = 0.08 + (double) (spin / 100.0);
                            ang = (double) (generator() % 6);
                            stream << "./res/asteroid" << (generator() % 6) << ".png";
                            printf("[    ] Data: pos (%f, %f); vel (%f, %f); angl (%f); omega %f\n", enter_x, enter_y, vi_x, vi_y, ang, spin);
                            Asteroid *next_obj = new Asteroid(
                                stream.str().c_str(), enter_x, enter_y,
                                vi_x, vi_y, ang, spin, 2);
                            printf("%s\n", stream.str().c_str());
                            stream.str("");
                            all_asteroids->push_back(next_obj);
                        } else if ((ev.keyboard.keycode == ALLEGRO_KEY_A) ||
                            (ev.keyboard.keycode == ALLEGRO_KEY_LEFT)) {
                            ship_angle += 0.04;
                            if (ship_angle > 6.283) {
                                ship_angle -= 6.283;
                            }
                            turn_left = true;
                        } else if ((ev.keyboard.keycode == ALLEGRO_KEY_D) ||
                            (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT)) {
                            ship_angle -= 0.04;
                            if (ship_angle < 0 /*6.283*/) {
                                ship_angle += 6.283;
                            }
                            turn_right = true;
                        } else if ((ev.keyboard.keycode == ALLEGRO_KEY_W) ||
                            (ev.keyboard.keycode == ALLEGRO_KEY_UP)) {
                            /*if (vel_x*vel_x + vel_y*vel_y < TOP_SPEED_QUAD) {*/
                            double del_x = sine(0-3.14159-ship_angle) / NORM;
                            double del_y = cosine(0-3.14159-ship_angle) / NORM;
                            if (((vel_x+del_x)*(vel_x+del_x)) < TOP_SPEED_X) {
                                vel_x += del_x;
                            }
                            if (((vel_y+del_y)*(vel_y+del_y)) < TOP_SPEED_Y) {
                                vel_y += del_y;
                            }
                            /*}*/
                            speed_up = true;
                        } else if ((ev.keyboard.keycode == ALLEGRO_KEY_S) ||
                            (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)) {
                            /*if (vel_x*vel_x + vel_y*vel_y < TOP_SPEED_QUAD) {*/
                            double del_x = sine(0-ship_angle) / NORM;
                            double del_y = cosine(0-ship_angle) / NORM;
                            if (((vel_x+del_x)*(vel_x+del_x)) < TOP_SPEED_X) {
                                vel_x += del_x;
                            }
                            if (((vel_y+del_y)*(vel_y+del_y)) < TOP_SPEED_Y) {
                                vel_y += del_y;
                            }
                            /*}*/
                            speed_down = true;
                        }
                    } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                        if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                            /* No-op for now... */
                        } else if (ev.keyboard.keycode == ALLEGRO_KEY_Q) {
                            game_state = 0;
                        } else if (ev.keyboard.keycode == ALLEGRO_KEY_R) {
                            update_stars();
                        } else if ((ev.keyboard.keycode == ALLEGRO_KEY_A) ||
                            (ev.keyboard.keycode == ALLEGRO_KEY_LEFT)) {
                            turn_left = false;
                        } else if ((ev.keyboard.keycode == ALLEGRO_KEY_D) ||
                            (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT)) {
                            turn_right = false;
                        } else if ((ev.keyboard.keycode == ALLEGRO_KEY_W) ||
                            (ev.keyboard.keycode == ALLEGRO_KEY_UP)) {
                            speed_up = false;
                        } else if ((ev.keyboard.keycode == ALLEGRO_KEY_S) ||
                            (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)) {
                            speed_down = false;
                        }
                        if (ev.keyboard.keycode == ALLEGRO_KEY_B) {
                            brake = false;
/*                            printf("`brake` is now turned off\n");*/
                        }
                    } else if (ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
/*                        redraw = true;*/
                    } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                        game_state = 0;
                    } else if (ev.type == ALLEGRO_EVENT_TIMER) {
                        if (ticker_timer == ev.timer.source) {
                            if (turn_left) {
                                ship_angle -= 0.04;
                                if (ship_angle < 0 /*6.283*/) {
                                    ship_angle += 6.283;
                                }
                            }
                            if (turn_right) {
                                ship_angle += 0.04;
                                if (ship_angle > 6.283) {
                                    ship_angle -= 6.283;
                                }
                            }
                            if (speed_up /*&& ((vel_x*vel_x + vel_y*vel_y) < TOP_SPEED_QUAD)*/) {
                                double del_x = sine(0-3.14159-ship_angle) / NORM;
                                double del_y = cosine(0-3.14159-ship_angle) / NORM;
                                if (((vel_x+del_x)*(vel_x+del_x)) < TOP_SPEED_X) {
                                    vel_x += del_x;
                                }
                                if (((vel_y+del_y)*(vel_y+del_y)) < TOP_SPEED_Y) {
                                    vel_y += del_y;
                                }
                            } else if (speed_down) {
                                double del_x = sine(0-ship_angle) / NORM;
                                double del_y = cosine(0-ship_angle) / NORM;
                                if (((vel_x+del_x)*(vel_x+del_x)) < TOP_SPEED_X) {
                                    vel_x += del_x;
                                }
                                if (((vel_y+del_y)*(vel_y+del_y)) < TOP_SPEED_Y) {
                                    vel_y += del_y;
                                }
                            }
                            if (brake) {
                                if (vel_x > BRAKE_VEL) {
                                    vel_x -= BRAKE_VEL;
                                } else if (vel_x < (-1)*BRAKE_VEL) {
                                    vel_x += BRAKE_VEL;
                                } else {
                                    vel_x /= 2;
                                }
                                if (vel_y > BRAKE_VEL) {
                                    vel_y -= BRAKE_VEL;
                                } else if (vel_y < (-1)*BRAKE_VEL) {
                                    vel_y += BRAKE_VEL;
                                } else {
                                    vel_y /= 2;
                                }
    /*                            printf("Now `brake`ing !!!\n");*/
                            }
                            redraw = true;
                        } else if (asteroid_spawn_timer == ev.timer.source) {
                            int spawn_rand = generator() % 101;
                            if (spawn_rand < threshold) {
                                printf("[INFO] New asteroid being spawned!!\n");
                                generator();
                                int det = generator() /* * generator() */% 2;
                                double enter_x=0.0, enter_y=0.0, vi_x=0.0, vi_y=0.0;
                                double ang=0.0;
                                double spin = generator() % 80;
                                if (det == 0) {
                                    det = generator() % 4;
                                    if (det < 3) {
                                        enter_x = generator() % SCREEN_WIDTH;
                                        enter_y = 1;
                                        vi_x = 0.01;
                                        vi_y = 4.01;
                                    } else {
                                        enter_x = 1;
                                        enter_y = generator() % SCREEN_HEIGHT;
                                        vi_x = 4.01;
                                        vi_y = 0.01;
                                    }
                                } else {
                                    det = generator() % 4;
                                    if (det == 0) {
                                        enter_x = generator() % SCREEN_WIDTH;
                                        enter_y = SCREEN_HEIGHT-2;
                                        vi_x = -0.01;
                                        vi_y = -4.01;
                                    } else {
                                        enter_x = SCREEN_WIDTH-2;
                                        enter_y = generator() % SCREEN_HEIGHT;
                                        vi_x = -4.01;
                                        vi_y = -0.01;
                                    }
                                }
                                ang = (double) (generator() % 6);
                                stream << "./res/asteroid" << (generator() % 6) << ".png";
                                spin = 0.08 + (double) (spin / 100.0);
                                printf("[    ] Data: pos (%f, %f); vel (%f, %f); angl (%f); omega %f\n", enter_x, enter_y, vi_x, vi_y, ang, spin);
                                Asteroid *next_obj = new Asteroid(
                                    stream.str().c_str(), enter_x, enter_y,
                                    vi_x, vi_y, ang, spin, 2);
                                printf("%s\n", stream.str().c_str());
                                stream.str("");
                                all_asteroids->push_back(next_obj);
                            }
/*                            redraw = true;*/
                        } else {
                            
                        }
                    }
                    if (redraw) {
                        /* Updating space ship location */
                        double del_x = vel_x / FPS;
                        double del_y = vel_y / FPS;
                        /* update x_component position */
                        pos_x += del_x;
/*                        pos_x = (pos_x + del_x) % SCREEN_WIDTH;*/
                        if (pos_x < 0) {
                            pos_x += SCREEN_WIDTH;
                        } else if (pos_x > SCREEN_WIDTH) {
                            pos_x -= SCREEN_WIDTH;
                        }
                        /* update y_component position */
                        pos_y += del_y;
/*                        pos_y = (pos_y + del_y) % SCREEN_HEIGHT;*/
                        if (pos_y < 0) {
                            pos_y += SCREEN_HEIGHT;
                        }
                        if (pos_y > SCREEN_HEIGHT) {
                            pos_y -= SCREEN_HEIGHT;
                        }
                        
                        /* 1: draw background bitmap */
                        al_draw_bitmap(bg, 0, 0, 0);

                        /* 2: set up and draw the middle layer */

                        /* TODO maybe fill with transparent background color if needed */
                        
                        /* Here, for the middle layer,
                         * Compute new positions for each entity...
                         * the code is separated by each CATEGORY of entity. */
                        redraw = false;
            /*            ALLEGRO_BITMAP *lit = al_create_bitmap(210, 210);
                        al_draw_bitmap(lit, 1060, 599, 0);*/
/*                        printf("Current velocity (%f, %f)\n", vel_x, vel_y);*/
                        /* Control variables,
                         * for iterating through each asteroid obj */
                        unsigned int ind;
                        unsigned int upper_bound = all_asteroids->size();
                        
                        al_set_target_bitmap(asteroid_layer);
                        al_clear_to_color(al_map_rgba(1, 1, 1, 1));
                        /* Iterate through asteroids, to DRAW them */
                        for (ind=0; ind < upper_bound; ind ++) {
                            Asteroid *ast = all_asteroids->at(ind);
                            if (ast->update_pos()) {
                                al_draw_scaled_rotated_bitmap(ast->sprite(), ast->x_com(), ast->y_com(),
                                    ((int) ast->x_pos()), ((int) ast->y_pos()), 1.0, 1.0, /*ast->spin_angle()*/0, 0);
                            } else {
                                delete ast;
                                all_asteroids->erase(all_asteroids->begin()+ind);
                                printf("[INFO] Deleting asteroid number %d\n", ind);
                                upper_bound --;
                                ind--;
                            }
                        }
                        al_set_target_backbuffer(disp);
                        al_draw_bitmap(asteroid_layer, 0, 0, 0);
                        
                        /* Iterate through projectiles, to DRAW them */
                        upper_bound = shots->size();
                        for (ind=0; ind < upper_bound; ind ++) {
                            Bullet *shot = shots->at(ind);
                            if (shot->update_pos()) {
                                int x_px = shot->x_pos();
                                int y_px = shot->y_pos();
                                al_draw_scaled_rotated_bitmap(bullet_img, bullet_x_com, bullet_y_com,
                                    x_px, y_px, 1.0, 1.0, shot->get_angle(), 0);
                                /* Check for impact, and if so, respond to that!!
                                 * This will be rather complex (splitting up
                                 * the Asteroid object into more asteroids) but
                                 * the important point is to 
                                 * delete & erase the shot object! */
                                unsigned char r, g, b/*, a*/;
                                ALLEGRO_COLOR asteroid_color = al_get_pixel(asteroid_layer, x_px, y_px);
                                al_unmap_rgb(asteroid_color, &r, &g, &b/*, a*/);
                                if ((r == 128) && (g == 128) && (b == 128)) { /* cgp GREY */
                                    /* Collision detected! */
                                    int match_num = find_asteroid_overlap(x_px, y_px);
                                    if (ind >= 0) {
                                        Asteroid *match = all_asteroids->at(match_num);
/*                                        match->break_up();*/
                                        /* TODO condition on the size of the broken asteroid
                                         * to see how many smaller asteroids should be made */
                                        
                                        /* TODO make it look like the first asteroid broke up along
                                         * an actual line through the bitmap */
/*                                        double ;*/
                                        stream.str("");
                                        stream << "res/asteroid_small" <<
                                          (generator() % 4) << ".png";
                                        if (match->get_size() > 1) {
                                            Asteroid *spawn_1 = new Asteroid(stream.str().c_str(),
                                                match->x_pos(), match->y_pos(), 0-match->y_vel(), match->x_vel(),
                                                match->spin_angle(), match->rot_vel(), match->get_size()-1);
                                            stream.str("");
                                            stream << "res/asteroid_small" <<
                                              (generator() % 4) << ".png";
                                            Asteroid *spawn_2 = new Asteroid(stream.str().c_str(),
                                                match->x_pos(), match->y_pos(), match->y_vel(), 0-match->x_vel(),
                                                match->spin_angle(), match->rot_vel(), match->get_size()-1);
                                            stream.str("");
                                            
                                            /* In with the new */
                                            all_asteroids->push_back(spawn_1);
                                            all_asteroids->push_back(spawn_2);
                                        }
                                        /* Out with the old */
                                        delete match;
                                        all_asteroids->erase(all_asteroids->begin()+match_num);
                                        delete shot;
                                        shots->erase(shots->begin()+ind);
                                        ind --;
                                        upper_bound --;
                                    }
                                }
                            } else {
                                delete shot;
                                shots->erase(shots->begin()+ind);
                                upper_bound --;
                                ind--;
                            }
                        }
                        al_draw_scaled_rotated_bitmap(space_ship_img, space_ship_wid/2, space_ship_hgt/2,
                            ((int)pos_x), ((int)pos_y), 0.8, 0.8, ship_angle, 0);
                        /* TODO have a hit box for the player spaceship...
                         * then when it gets hit, process that by
                         * decrementing health (if zero, then change game_state; if not,
                         * then activate an immunity timer for 1.5 seconds... bonus points
                         * for having the player space ship flashing?) */
                        /* 3: draw overlay with player stats */
                        al_draw_bitmap(overlay, 0, 0, 0);
                        al_flip_display();
                    }
                }
            }
            al_stop_timer(ticker_timer);
            al_stop_timer(asteroid_spawn_timer);
            al_destroy_timer(ticker_timer);
            al_destroy_timer(asteroid_spawn_timer);
            al_destroy_bitmap(bg);
            al_destroy_bitmap(overlay);
            al_destroy_bitmap(bullet_img);
            al_destroy_bitmap(space_ship_img);
            al_destroy_bitmap(star);
            al_destroy_font(font_amok);
            al_destroy_font(font_luxirb);
            al_destroy_font(font_luxirb_small);
            al_destroy_display(disp);
            return 0;
        }
};

int main(int argc, char **argv) {
    ALLEGRO_DISPLAY *display = NULL;
    int result;
/*    int visual_type = 0;*/
    result = al_init();
    if (result == 0) {
        fprintf(stderr, "Failed to start Allegro game engine!\n");
        al_show_native_message_box(display, "error", "error", "Failed to initialize Allegro game engine",
            NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    
    result = al_init_image_addon();
    if (result == 0) {
        fprintf(stderr, "Failed to start Allegro img addon!\n");
        al_show_native_message_box(display, "error", "error", "Failed to initialize Allegro img addon",
            NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    
    result = al_init_primitives_addon();
    if (result == 0) {
      fprintf(stderr, "Failed to start Allegro primitives addon!\n");
      al_show_native_message_box(display, "error", "error", "Failed to initialize Allegro primitives addon",
        NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return -1;
    }
    al_init_font_addon();
    al_init_ttf_addon();
    
    /*** INIT DISPLAY ***/
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (display == NULL) {
        fprintf(stderr, "Failed to start Allegro display struct!\n");
        al_show_native_message_box(display, "error", "error", "Failed to start the display structure",
            NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    
    Game *instance = new Game(display);
    instance->launch();
    delete instance;
    return 0;
}
