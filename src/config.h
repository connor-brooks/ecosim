#ifndef CONFIG_H
#define CONFIG_H

/* Main World config */
#define DEV_AGENT_COUNT (90)
#define DEV_GAME_FPS (60)
#define DEV_GAME_FOOD_SPAWN_FREQ (2)

/* Input config */
#define INPUT_SPAWN_DELAY (0.5)
#define INPUT_SCROLL_AMT (0.02)


/* Agent config */
#define AGENT_RGB_MAX (1.0)
#define AGENT_RGB_MIN (1.0)
#define AGENT_RGB_ALPHA (0.9)

#define AGENT_VIS_ALPHA (0.2)

#define AGENT_MAX_VELOCITY (1.0)
#define AGENT_MIN_VELOCITY (-1.0)

#define AGENT_ENERGY_DEFAULT (1.0)
#define AGENT_METAB_MAX (0.5)
#define AGENT_METAB_MIN (0.05)

#define AGENT_FEAR_MAX (1.0)
#define AGENT_FEAR_MIN (-1.0)

#define AGENT_VISION_MAX (0.1)
#define AGENT_VISION_MIN (0.2)

#define AGENT_REBIRTH_MAX (3.00)
#define AGENT_REBIRTH_MIN (1.00)

#define AGENT_AGGRESION_MAX (1.00)
#define AGENT_AGGRESION_MIN (-1.00)

#define AGENT_DIET_MAX (1.00)
#define AGENT_DIET_MIN (-1.00)

#define AGENT_FLOCK_MAX (1.00)
#define AGENT_FLOCK_MIN (0.00)

#define AGENT_WOBBLE_MAX (15.00)
#define AGENT_WOBBLE_MIN (3.00)

#define AGENT_METAB_ENERGY_SCALE(x) (0.0015 * x)
#define AGENT_ENERGY_SIZE_SCALE(x) ((10 * x) + 2)

#define AGENT_MAX_SPEED (0.0015)

#define AGENTS_ENERGY_DEAD (0.3)

#define AGENTS_TIME_FACTOR (0.5)

#define AGENTS_DNA_MUTATE_RATE (0.075)

#define AGENT_ARRAY_DEFAULT_SIZE (16)
#define AGENT_DEBUG_SHOW_VISION (0)

#define AGENT_VIS_VERTS_DEFAULT (16)

/* Quadtree config */
#define QUADTREE_MAX_PER_CELL (4)

/* Engine config: Do not touch */
#define AGENT_VERT_DIMS (4)
#define AGENT_DIMENTIONS (4)
#define AGENT_DRAWABLE_FEATURES (2)
#define QUADTREE_VERT_LEN (3 * 4)
#define QUADTREE_QUERY_SIZE (16)
#define QUAD_COUNT (4)
#define QUADTREE_DIMS (2)
#define WORLD_MIN_COORD (-1.0)
#define WORLD_MAX_COORD (1.0)
#endif
