#ifndef CONFIG_H
#define CONFIG_H

/* Main World config */
/* How many agents to initaly spawn */
#define DEV_AGENT_COUNT (90)
/* Frames per second */
#define DEV_GAME_FPS (60)
/* How often (in seconds) to spawn food */
#define DEV_GAME_FOOD_SPAWN_FREQ (4)
#define DEV_GAME_FOOD_SPAWN_INIT (15)
/* Max/min: How many foods to spawn each feeding time */
#define DEV_GAME_FOOD_SPAWN_MIN (5)
#define DEV_GAME_FOOD_SPAWN_MAX (7)
#define DEV_GAME_FOOD_ENERGY (0.5)
/* End world config */

/* Input config */
/* The delay between re-spawn of agents when the mouse is held down */
#define INPUT_SPAWN_DELAY (0.5)
/* Scroll sensitivity (also used for zoom */
#define INPUT_SCROLL_AMT (0.02)
/* End input config */

/* Agent general config */
/* Agent transparency */
#define AGENT_RGB_ALPHA (0.9)
/* Agent vision field transparency */
#define AGENT_VIS_ALPHA (0.2)
/* Maximum agent velocity */
#define AGENT_MAX_VELOCITY (1.0)
#define AGENT_MIN_VELOCITY (-1.0)
/* Default energy for new-spawned agents */
#define AGENT_ENERGY_DEFAULT (1.0)
/* The rate energy is burned over time, with respect to metabolism rates 
 * (where x is agents metabolism */
#define AGENT_METAB_ENERGY_SCALE(x) (0.0015 * x)
/* How large agents are, with respect to their energy (Where x is energy) */
#define AGENT_ENERGY_SIZE_SCALE(x) ((10 * x) + 4)
/* The maximum speed any agents can move at */
#define AGENT_MAX_SPEED (0.0015)
/* The energy level at which an agent dies */
#define AGENT_ENERGY_DEAD (0.3)
/* How quickly ageing effects the agents */
#define AGENT_TIME_FACTOR (0.3)

#define AGENT_DIET_BOUNDARY (0.5)
/* End agent general config */

/* Agent DNA config */
/* The amount a DNA trait changes if mutation occurs */
#define AGENT_DNA_MUTATE_RATE (0.1)
/* Metabolism trait max/min 
 * How quickly an agent can move. Faster moving agents burn energy a lot
 * quicker */
#define AGENT_METAB_MAX (0.5)
#define AGENT_METAB_MIN (0.05)
/* Vision trait max/min 
 * How wide the agents field of view is */
#define AGENT_VISION_MAX (0.1)
#define AGENT_VISION_MIN (0.2)
/* Rebirth trait max/min 
 * How much energy is stored within an agent until it splits, creating a 
 * possibly mutated clone of itself, halving it's energy */
#define AGENT_REBIRTH_MAX (3.00)
#define AGENT_REBIRTH_MIN (1.00)
/* Diet trait max/min
 * If greater or equal to zero, the agent eats other agents, if less than 
 * zero, the agent eats only dead agents */
#define AGENT_DIET_MAX (1.00)
#define AGENT_DIET_MIN (0.00)
/* Flock max/min
 * How strong flocking behaviours influence the movement of an agent */
#define AGENT_FLOCK_MAX (1.00)
#define AGENT_FLOCK_MIN (0.00)
/* Wobble max/min
 * How many times per second the agent "wobbles" This is a movement based on
 * a sin wav, which gives a temporary boost of speed, followed by a equal
 * period of slower movement */
#define AGENT_WOBBLE_MAX (3.0)
#define AGENT_WOBBLE_MIN (1.5)

/*Log config */
#define LOGGER_ENABLE (0)
#define LOGGER_FILE "logger_data.py"
#define LOGGER_FREQ (2)

/* Engine config: 
 * DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING! */
#define QUADTREE_MAX_PER_CELL (8)
#define AGENT_VERT_DIMS (4)
#define AGENT_DIMENTIONS (4)
#define AGENT_ARRAY_DEFAULT_SIZE (16)
#define AGENT_DEBUG_SHOW_VISION (0)
#define AGENT_VIS_VERTS_DEFAULT (16)
#define AGENT_DRAWABLE_FEATURES (2)
#define QUADTREE_VERT_LEN (3 * 4)
#define QUADTREE_QUERY_SIZE (16)
#define QUAD_COUNT (4)
#define QUADTREE_DIMS (2)
#define WORLD_MIN_COORD (-1.0)
#define WORLD_MAX_COORD (1.0)
#define LOG_DEFAULT_SIZE (12)
#define QUAD_HEAD_POS (-1.0f)
#define QUAD_HEAD_SIZE (2.0f)
#define WINDOW_DEFAULT_X (1600)
#define WINDOW_DEFAULT_Y (900)
#define AGENT_ARRAY_PRUNE_TIME (2)
/* End engine config */
#endif
