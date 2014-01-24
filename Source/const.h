#ifndef CONST_INCLUDED
#define CONST_INCLUDED

//ToDo move stuff out of here and into other .h files

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

using namespace std;

#define AREA_IMAGE_SIZE             800    //How many pixels the area images are (both width and height; they're square).
#define AUTO_PLUCK_INPUT_INTERVAL   0.5    //How long the player has to press the pluck button again, to make the captain auto-pluck.
#define AUTO_PLUCK_MAX_RADIUS       160    //How far a leader can go to auto-pluck the next Pikmin.
#define CAM_TRANSITION_DURATION     0.5    //How many seconds a camera transition lasts for.
#define CURSOR_MAX_DIST             200    //The cursor can only be these many units away from the captain.
#define DEF_PIKMIN_SIZE             24     //Default Pikmin size.
#define DEF_ROTATION_SPEED          M_PI * 2 //The default rotation speed of a mob type.
#define DEF_WHISTLE_RANGE           80     //The whistle can't go past this radius, by default.
#define DISMISS_DISTANCE            64     //Dismissed Pikmin go these many units away from the captain.
#define GRAVITY_ADDER               -2.0f  //Accelerate the Z speed of mobs affected by gravity by this amount per second.
#define IDLE_GLOW_SPIN_SPEED        M_PI_2 //The idle glow spins these many radians per second.
#define INFO_SPOT_TRIGGER_RANGE     64     //If the current captain is at this distance or closer from an info spot, it gets triggered.
#define LEADER_MOVE_SPEED           100    //Max speed at which a leader can move.
#define MESSAGE_CHAR_INTERVAL       0.02   //These many seconds until a new character of the message is drawn.
#define MIN_GRAB_RANGE              60     //The leader needs to be at least this close to a Pikmin to grab it.
#define MIN_ONION_CHECK_RANGE       64     //The minimum distance a leader must be from the onion in order to check it.
#define MIN_PLUCK_RANGE             30     //The leader needs to be at least this close to a buried Pikmin to pluck it.
#define MOUSE_CURSOR_MOVE_SPEED     500    //How many pixels the mouse cursor moves, per second, when using an analog stick.
#define MOVE_GROUP_ARROW_SPEED      400    //"Move group" arrows move these many units per second.
#define MOVE_GROUP_ARROWS_INTERVAL  0.1    //Seconds that need to pass before another "move group" arrow appears.
#define NECTAR_AMOUNT               5      //A drop of nectar starts with this amount.
#define PARTY_SPOT_INTERVAL         1      //Pikmin must be at least these many units away from one another; used when calculating group spots.
#define PIKMIN_MAX_HEALTH           300    //Maximum Pikmin HP.
#define PIKMIN_MIN_TASK_RANGE       20     //If there's this gap between a Pikmin and a task, the Pikmin will take the task.
#define SHADOW_MAX_WIDTH            100    //The shadows can't be any wider than this.
#define SHADOW_Y_MULTIPLIER         150    //For every unit above the ground that the mob is on, the shadow goes these many units to the side.
#define SHIP_BEAM_RANGE             30     //The center of a ship's beam reaches this far.
#define SHIP_BEAM_RING_COLOR_SPEED  255    //Red color's index moves these many units per second. (Green is fast and blue is faster still).
#define SUN_METER_SUN_SPIN_SPEED    0.5    //The Sun Meter's sun spins these many radians per second.
#define THROW_DISTANCE_MULTIPLIER   0.49   //When a leader throws a Pikmin, multiply their strength by this.
#define UNCALLABLE_PERIOD           1      //A mob cannot be called to a party during this period.
#define WHISTLE_DOT_INTERVAL        0.03   //Seconds that need to pass before another dot is added.
#define WHISTLE_DOT_SPIN_SPEED      M_PI_2 //A whistle dot spins these many radians a second.
#define WHISTLE_FADE_TIME           0.1    //Time the whistle animations take to fade out.
#define WHISTLE_MAX_HOLD_TIME       1.5    //After the whistle reaches its maximum size, hold it for these many seconds until it stops by itself.
#define WHISTLE_RADIUS_GROWTH_SPEED 180    //The whistle's radius grows these many units per second.
#define WHISTLE_RING_SPEED          600    //Whistle rings move these many units per second.
#define WHISTLE_RINGS_INTERVAL      0.1    //Seconds that need to pass before another whistle ring appears.
#define ZOOM_MAX_LEVEL              2      //Maximum zoom level possible.
#define ZOOM_MAX_LEVEL_EDITOR       4      //Maximum zoom level possible on the area editor.
#define ZOOM_MIN_LEVEL              0.5    //Minimum zoom level possible.
#define ZOOM_MIN_LEVEL_EDITOR       0.05   //Minimum zoom level possible on the area editor.

#define DEF_FPS 30
#define DEF_SCR_W 640
#define DEF_SCR_H 480

#define AUDIO_FOLDER           GAME_DATA_FOLDER "/Audio"
#define AREA_FOLDER            GAME_DATA_FOLDER "/Areas"
#define CONFIGURATIONS_FOLDER  GAME_DATA_FOLDER "/Configurations"
#define ENEMIES_FOLDER         TYPES_FOLDER "/Enemies"
#define GAME_DATA_FOLDER       "Game_data"
#define GRAPHICS_FOLDER        GAME_DATA_FOLDER "/Graphics"
#define LEADERS_FOLDER         TYPES_FOLDER "/Leaders"
#define ONIONS_FOLDER          TYPES_FOLDER "/Onions"
#define OTHER_TYPES_FOLDER     TYPES_FOLDER "/Others"
#define PELLETS_FOLDER         TYPES_FOLDER "/Pellets"
#define PIKMIN_FOLDER          TYPES_FOLDER "/Pikmin"
#define TREASURES_FOLDER       TYPES_FOLDER "/Treasures"
#define TYPES_FOLDER           GAME_DATA_FOLDER "/Types"
#define WEATHER_FILE           CONFIGURATIONS_FOLDER "/Weather.txt"

enum BUTTONS {
    BUTTON_NONE,
    BUTTON_PUNCH,
    BUTTON_WHISTLE,
    BUTTON_MOVE_RIGHT,
    BUTTON_MOVE_UP,
    BUTTON_MOVE_LEFT,
    BUTTON_MOVE_DOWN,
    BUTTON_MOVE_CURSOR_RIGHT,
    BUTTON_MOVE_CURSOR_UP,
    BUTTON_MOVE_CURSOR_LEFT,
    BUTTON_MOVE_CURSOR_DOWN,
    BUTTON_MOVE_GROUP_RIGHT,
    BUTTON_MOVE_GROUP_UP,
    BUTTON_MOVE_GROUP_LEFT,
    BUTTON_MOVE_GROUP_DOWN,
    BUTTON_MOVE_GROUP_TO_CURSOR,
    BUTTON_SWITCH_CAPTAIN_RIGHT,
    BUTTON_SWITCH_CAPTAIN_LEFT,
    BUTTON_DISMISS,
    BUTTON_USE_SPRAY_1,
    BUTTON_USE_SPRAY_2,
    BUTTON_USE_SPRAY,
    BUTTON_SWITCH_SPRAY_RIGHT,
    BUTTON_SWITCH_SPRAY_LEFT,
    BUTTON_SWITCH_ZOOM,
    BUTTON_ZOOM_IN,
    BUTTON_ZOOM_OUT,
    BUTTON_SWITCH_TYPE_RIGHT,
    BUTTON_SWITCH_TYPE_LEFT,
    BUTTON_SWITCH_MATURITY_UP,
    BUTTON_SWITCH_MATURITY_DOWN,
    BUTTON_LIE_DOWN,
    BUTTON_PAUSE,
};

enum CONTROL_TYPES {
    CONTROL_TYPE_NONE,
    CONTROL_TYPE_KEYBOARD_KEY,
    CONTROL_TYPE_MOUSE_BUTTON,
    CONTROL_TYPE_MOUSE_WHEEL_UP,
    CONTROL_TYPE_MOUSE_WHEEL_DOWN,
    CONTROL_TYPE_MOUSE_WHEEL_LEFT,
    CONTROL_TYPE_MOUSE_WHEEL_RIGHT,
    CONTROL_TYPE_MOUSE_MOVE,
    CONTROL_TYPE_JOYSTICK_BUTTON,
    CONTROL_TYPE_JOYSTICK_AXIS_POS,
    CONTROL_TYPE_JOYSTICK_AXIS_NEG,
};

enum EDITOR_MODES {
    EDITOR_MODE_NORMAL,
    EDITOR_MODE_NEW_SECTOR = 1,
    EDITOR_MODE_NEW_HITBOX = 1,
    EDITOR_MODE_DELETE_HITBOX = 2,
};

enum MOB_TYPES {
    MOB_TYPE_PIKMIN,
    MOB_TYPE_ONION,
    MOB_TYPE_LEADER,
    MOB_TYPE_ENEMY,
    MOB_TYPE_TREASURE,
    MOB_TYPE_PELLET,
};

enum PERCIPITATION_TYPES {
    PERCIPITATION_TYPE_NONE,
    PERCIPITATION_TYPE_RAIN,
    PERCIPITATION_TYPE_WIND,
};

enum SCREENS {
    SCREEN_MAIN_MENU,
    SCREEN_GAME,
    SCREEN_AREA_EDITOR,
    SCREEN_ANIMATION_EDITOR,
};

//ToDo these colors aren't right. The purple is pink, the cyan is light green...
#define N_WHISTLE_RING_COLORS 8;
const unsigned char WHISTLE_RING_COLORS[8][3] = {
    {255, 255, 0},
    {255, 0, 0},
    {255, 0, 255},
    {128, 0, 255},
    {0, 0, 255},
    {0, 255, 255},
    {0, 255, 0},
    {128, 255, 0}
};

#endif //ifndef CONST_INCLUDED