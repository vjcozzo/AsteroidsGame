#include <random>
#include <sstream>
/* #define IMG_HEIGHT 827
#define IMG_WIDTH 1599

#define IMG_HEIGHT 683
#define IMG_WIDTH 829 */

/* HALF SIZE */
#define SCREEN_HEIGHT 540
#define SCREEN_WIDTH 960
/* 3/4 SIZE */
/*#define SCREEN_HEIGHT 810
#define SCREEN_WIDTH 1440*/

#define NORM (0.25)
#define TOP_SPEED_X 10000
#define TOP_SPEED_Y 10000
#define BRAKE_VEL 0.80
#define FPS 19.99d
/*            double FPS = 19.9;*/
std::mt19937_64 generator(1559);

class Bullet {
  protected:
    ALLEGRO_BITMAP *img;
    double pos_x, pos_y, vel_x, vel_y;
    double angle;
  
  public:
/*    Bullet(ALLEGRO_BITMAP *appearance) {
      img = appearance;
    }
    Bullet(const char *face) {
      img = al_load_bitmap(face);
    }*/
    Bullet(double x_init, double y_init, double v_x, double v_y, double theta) {
      pos_x = x_init;
      pos_y = y_init;
      vel_x = v_x;
      vel_y = v_y;
      angle = theta;
    }
    double x_pos() {
      return pos_x;
    }
    double y_pos() {
      return pos_y;
    }
    double get_angle() {
      return angle;
    }
    bool update_pos() {
      pos_x += vel_x;
      pos_y += vel_y;
      if (pos_x < 0 || pos_x > SCREEN_WIDTH) {
        return false;
      }
      if (pos_y < 0 || pos_y > SCREEN_HEIGHT) {
        return false;
      }
      return true;
    }
/*  al_draw_scaled_rotated_bitmap(bullet_img, bullet_x_com, bullet_y_com,
      ((int) shot->x_pos), ((int) shot->y_pos), 1.0, 1.0, shot->get_angle(), 0);*/
};

class Asteroid {
  protected:
    ALLEGRO_BITMAP *face;
    double angle, spin_vel;
    double pos_x, pos_y;
    double vel_x, vel_y;
    int com_x, com_y;
    int size;

  public:
    /* TODO develop some SMART algorithm or storage mechanism
     * to get the com_x and com_y based on the image ....
     * hmmmm that'll be a challenge.
     * For now, the primitive alternative is to just use wid/2, height/2 */
    Asteroid(const char *fname) {
      face = al_load_bitmap(fname);
      if (face == NULL) {
        fprintf(stderr, "ERR - Asteroid class - given bitmap filename is invalid!\n");
      } else {
        com_x = al_get_bitmap_width(face) / 2;
        com_y = al_get_bitmap_height(face) / 2;
      }
    }

    Asteroid(ALLEGRO_BITMAP *given_pic) {
      face = given_pic;
      if (face == NULL) {
        fprintf(stderr, "ERR - Asteroid class - given bitmap structure is NULL!\n");
      } else {
        com_x = al_get_bitmap_width(face) / 2;
        com_y = al_get_bitmap_height(face) / 2;
      }
    }

    Asteroid(ALLEGRO_BITMAP *given_pic, double x_i, double y_i, double v_x, double v_y, double theta, double omega, int s) {
      face = given_pic;
      if (face == NULL) {
        fprintf(stderr, "ERR - Asteroid class - given bitmap structure is NULL!\n");
      } else {
        com_x = al_get_bitmap_width(face) / 2;
        com_y = al_get_bitmap_height(face) / 2;
      }
      pos_x = x_i;
      pos_y = y_i;
      vel_x = v_x;
      vel_y = v_y;
      angle = theta;
      spin_vel = omega;
      size = s;
    }

    Asteroid(const char *fname, double x_i, double y_i, double v_x, double v_y, double theta, double omega, int s) {
      face = al_load_bitmap(fname);
      if (face == NULL) {
        fprintf(stderr, "ERR - Asteroid class - given bitmap filename is invalid!\n");
      } else {
        com_x = al_get_bitmap_width(face) / 2;
        com_y = al_get_bitmap_height(face) / 2;
      }
      pos_x = x_i;
      pos_y = y_i;
      vel_x = v_x;
      vel_y = v_y;
      angle = theta;
      spin_vel = omega;
      size = s;
    }

    ALLEGRO_BITMAP *sprite() {
      return face;
    }

    int x_com() {
      return com_x;
    }

    int y_com() {
      return com_y;
    }

    double x_pos() {
      return pos_x;
    }

    double y_pos() {
      return pos_y;
    }

    double x_vel() {
      return vel_x;
    }

    double y_vel() {
      return vel_y;
    }

    double spin_angle() {
      return angle;
    }

    double rot_vel() {
      return spin_vel;
    }

    int get_width() {
      return size*com_x;
    }

    int get_height() {
      return size*com_y;
    }

    int get_size() {
      return size;
    }

    bool update_pos() {
      double del_x = vel_x / FPS;
      double del_y = vel_y / FPS;
      /* update x_component position */
      pos_x += del_x;
/*    pos_x = (pos_x + del_x) % SCREEN_WIDTH;*/
      if (pos_x < 0) {
        return false;
/*        pos_x += SCREEN_WIDTH;*/
      } else if (pos_x > SCREEN_WIDTH) {
        return false;
/*        pos_x -= SCREEN_WIDTH;*/
      }
      /* update y_component position */
      pos_y += del_y;
/*    pos_y = (pos_y + del_y) % SCREEN_HEIGHT;*/
      if (pos_y < 0) {
        return false;
/*        pos_y += SCREEN_HEIGHT;*/
      }
      if (pos_y > SCREEN_HEIGHT) {
        return false;
/*        pos_y -= SCREEN_HEIGHT;*/
      }
      angle += spin_vel;
      return true;
    }
};
