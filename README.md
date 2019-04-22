**NOT CURRENTLY FIT FOR HUMAN CONSUMPTION**
# ecosim
*An ecosystem simulator*

Ecosim is an ecosystem simulator:

![Screenshot of ecosim](screenshot.png)
## Requirements
* GNU/Linux
* make
* gcc
* libglfw3
* libglew2.0
* libglfw3-dev
* libglew-dev
* ffplay (optional)
## How to use
 Install dependencies

`$ sudo apt-get install libglfw3 libglew2.0 libglfw3-dev libglew-dev ffmpeg`

 Grab the code:

`$ git clone https://github.com/connor-brooks/ecosim.git`

 Enter source directory:

`$ cd ecosim/src`

 Build:

`$ make`

 Run the simulation: 

`$ ./ecosim`
## Controls 
* Zoom: `ctrl` + `scroll wheel`
* Pan: `scroll wheel`
* Pause: `space`
* Quit: `q`
* Insert agent: `left click`
* Cycle agents: `left click` (hold)

## Behaviour of agents
Agents within the simulation constantly loose energy over time, as moving around in the environment causes energy to be burned. The only way energy levels can be increased is via consumption.

Dietary preferences of agents vary, but fall into two main categories: 
* Primary consumers - Agents which only take energy from non-living entities.
* Secondary consumers - Agents which only take energy from living agents, killing them in the process.

If an agents energy drops below a threshold, death occurs. In contrast, passing above a another threshold causes the agent to split into two copies of itself (asexual reproduction).

When an agent splits, it doesn't always make an exact copy of itself. Sometimes mutation occurs. This means that the genetic code (DNA) that dictates the agents behaviours is slightly modified. 

Over time, only the most successful agents are able to pass on their DNA, causing the population of agents to slowly evolve.

## Tweaking the simulation 
Ecosim provides a way to configure the mechanics of the simulation via editing the `config.h` file. The process of doing so is as follows:

* Enter the `src` directory
* Edit the `config.h` file with the text editor of your choice
* Run `make clean`
* Run `make`

### What to change
This is entirely down to personal preference, however, most people will only be concerned with two sections of the configuration: The general agent settings, and the agent DNA settings.

### List of config settings
#### Main world settings
* `DEV_AGENT_COUNT` - The amount of agents to spawn when the simulation runs
* `DEV_GAME_FPS` - Frames per second (FPS) of the simulation
* `DEV_GAME_FOOD_SPAWN_FREQ` - How often (in seconds) to spawn food
#### Input settings
* `INPUT_SPAWN_DELAY` - How often to respawn agents when the mouse is held down
* `INPUT_SCROLL_AMT` - Input sensitivity (Effects scroll and zoom)
#### Agent general settings
* `AGENT_RGB_ALPHA` - Transparency of agent's inner cell
* `AGENT_VIS_ALPHA` - Transparaney of agent's vision field
* `AGENT_MAX_VELOCITY`/`AGENT_MIN_VELOCITY` - Agent velocity maximum / minimum
* `AGENT_ENERGY_DEFAULT` - Default energy of agents spawned at startup
* `AGENT_METAB_ENERGY_SCALE(x)` - The rate of which energy is burned over time, with respect to the metabolic rate of an agent (where x is metabolism)
* `AGENT_ENERGY_SIZE_SCALE(x)` - How large an agent is, with respect to their energy (where x is energy)
* `AGENT_MAX_SPEED` - The maximum speed any agent can move
* `AGENT_TIME_FRACTOR` - How fast the process of ageing occurs
#### Agent DNA settings
* `AGENT_DNA_MUTATE_RATE` - The maximum amount a trait can change if mutation occurs
* `AGENT_METAB_MAX`/`AGENT_METAB_MIN` - The maximum and minimum metabolic rates allowed
* `AGENT_VISION_MAX`/`AGENT_VISION_MIN` - The maximum and minimum vision field sizes allowed
* `AGENT_REBIRTH_MAX`/`AGENT_REBIRTH_MIN` - The maximum and minimum amount of energy stored in an agent before splitting occurs
* `AGENT_DIET_MAX`/`AGENT_DIET_MIN` - The maximum and minimum diet values allowed
* `AGENT_FLOCK_MAX`/`AGENT_FLOCK_MIN` - The maximum and minimum influence flocking has on agents
* `AGENT_WOBBLE_MAX`/`AGENT_WOBBLE_MIN` - The maximum and minimum amount an agent can wobble per second


## Notes 
* If you'd like some background noise to play whilst running the simulation, install ffmpeg, otherwise it's not required


