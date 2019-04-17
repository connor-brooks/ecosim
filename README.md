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

## Notes 
* If you'd like some background noise to play whilst running the simulation, install ffmpeg, otherwise it's not required


