# Robot-NPC
In this repository I will design a robot npc character that can walk along a set path, do some kind of work, and return. <br> The character should also be able to set a custom path / working parameters / and such.




## TODO

- [X] Simple Skeletal Mesh Follow Spline

- [X] Stay along the ground not neccessarily spline

- [X] Movement Speed
  - [X] Basic set speed
  - [X] Accelleration

- [ ] Change speed running uphill / downhill
  - [X] constant decrease / increase if on hill (binary)
    - [ ] Transition between current speed and Falling Speed (Acceleration)
      - [X] and direction 
    - [ ] lower / increase based on slope (angle / steepness)
      - [ ] Increase / Decrease Friction

- [ ] Falling downward
  - [X] At Some Speed
  - [ ] Gravity
 
- [X] Running into a wall not through
  - [ ] Path around obsticles back to spline
     
- [ ] Running Into a Wall not Through
  - [ ] around
- [ ] falling
- [ ] Jumping? maybe just slopes and such 
     
- [ ] Multiple splines
  - [X] Base Spline
  - [ ] Wait for some time (work)
  - [X] Next Spline
  - [ ] keep doing that
     
      
- [ ] Change Properties to an enum representing what the robot is doing
    - [X] Forward / Back
    - [X] Rotate in Place
    - [ ] Work
- [ ] add an int array or something linked to enum representing how to do the thing
    - [X] what spline to follow
    - [X] how much to rotate
    - [ ] specifics of work (# of items to collect, # of items to deposit, etc.)
      - [ ] make more dynamic, what to collect + how many, 

      
- [ ] New Mesh
  - [ ] Robot (see ipad sketches)
  - [ ] Animations
    - [ ] Running tracks moving
    - [ ] falling (parachute open then swing side to side and land)
    - [ ] Working (based on its job)

- [ ] Document Here

- [ ] Change Actor to Pawn
  - [X] Pawn With a Camera
  - [ ] Line Trace Posses
  - [ ] UnPosses
    - [ ] Just get out
    - [ ] Start runnign program
  
  - [ ] Movement 
    - [X] Basic (Forward / Back /  Rotate (Left / Right))
    - [ ] More advanced movement (jump / grapple / etc. based on future features)
  
  - [X] Make a Spline
    - [X] Spawn DebugBalls after traveling x distance or pressing a key
       - [X] add points to spline
         - [X] button pres
         - [X] distance
       - [X] Spawn Debug Ball 
    - [X] if current spline int == current direction (1 or -1) add to same spline
    - [X] else add new spline
    - [X] reversing addes new spline with int value of -1
    - [ ] visually see path (arrows along ground similar to how bloons td previews show before level)
      - [X] First Sections
      - [X] All Sections
      - [X] Reverse direction slightly different
      - [ ] Rotate
      - [ ] Flag at start
      - [X] Change so it spawns based on distance or speed not number of points, this should ensure a consistant size for preview arrows
      - [ ] make it dots not arrows
  - [X] Make multiple spline
  - [ ] add non movement actions (wait / work)
  - [ ] Start / Stop Program
    - [ ] Stop making new points after hit flag (start)
      - [ ] Arrow points at start?
      - [ ] reset button starts again (new start and clear all old points)
      - [ ] Save tracks?
    - [X] Fix first instruction is always spline forward
         
    - [ ] Play Program
      - [X] Follow Splines
        - [X] Forward
        - [X] Backward
          - [X] Fix Yaw 
      - [X] Rotate
        - [X] Rotate Backwards
      - [X] Fix transition between instructions (maybe wait a second) (prob was positions were diff)
      - [X] Add acceleration
        - [X] Changing Speed
        - [X] Decelerate to stop at end of spline (some math to figure out speed decrease that equals stopping at end)
          - [X] use points not distance (there is some math that get # of points between each) 
      - [ ] Next Point is "Target" to try and get to (path around stuff)
        - [ ] Maze solving algorithms
          - [ ] BFS
           - [ ] DFS
          - [ ] A*
          - [ ] Others
          - [ ] 
      - [X] Reverse Direction on slope gives wrong angle
      - [ ] add friction for slopes up / down
      - [X] Running into walls then rotating should give "rotate" command but doesnt, instead since w or s keys are held it give the forward or backward move which gives weird play
        - [X] stop forward and rotate together if running into wall
        - [X] fix holding w and rotating near wall
          - [X] if hit wall bounce backwards off the wall.
      - [ ] Loop Option 
