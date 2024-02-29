# Robot-NPC
In this repository I will design a robot npc character that can walk along a set path, do some kind of work, and return. <br> The character should also be able to set a custom path / working parameters / and such.




## TODO

- [X] Simple Skeletal Mesh Follow Spline

- [X] Stay along the ground not neccessarily spline

- [ ] Change speed running uphill / downhill
  - [X] constant decrease / increase if on hill (binary)
    - [ ] Transition between current speed and Down / Up Speed (Acceleration)
  - [ ] lower / increase based on slope (angle / steepness)

- [ ] Falling downward
  - [X] At Some Speed
  - [ ] Gravity
 
- [X] Running into a wall not through
  - [ ] Path around obsticles back to spline
     
- [ ] Multiple splines
  - [X] Base Spline
  - [ ] Wait for some time (work)
  - [ ] Next Spline
  - [ ] keep doing that

      
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
  
  - [ ] Movement 
    - [X] Basic (Forward / Back /  Rotate (Left / Right))
    - [ ] More advanced movement (jump / grapple / etc. based on future features)
  
  - [ ] Make a Spline
    - [X] Spawn DebugBalls after traveling x distance or pressing a key
       - [X] add points to spline
         - [X] button pres
         - [X] distance
       - [X] Spawn Debug Ball 
    - [X] if current spline int == current direction (1 or -1) add to same spline
    - [X] else add new spline
    - [X] reversing addes new spline with int value of -1
    - [ ] stopping / working has value > 1
    - [ ] visually see path (arrows along ground similar to how bloons td previews show before level)
      - [X] First Sections
      - [ ] All Sections
      - [ ] Reverse direction slightly different
      - [ ] Flag at start
  - [X] Make multiple spline
  - [ ] add non movement actions (wait / work)
  - [ ] Start / Stop Program
    - [ ] Stop making new points after hit flag (start)
      - [ ] Arrow points at start?
      - [ ] reset button starts again (new start and clear all old points)
      - [ ] Save tracks?
   
 

