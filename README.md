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
 Install dependacies

`$ sudo apt-get install libglfw3 libglew2.0 libglfw3-dev libglew-dev ffmpeg`

 Grab the code:

`$ git clone https://github.com/connor-brooks/ecosim`

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







## Notes 
* If you'd like some background noise to play whilst running the simulation, install ffmpeg, otherwise it's not required


