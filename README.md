# Robot-NPC
In this repository I will design a robot npc character that can walk along a set path, do some kind of work, and return. <br> The character should also be able to set a custom path / working parameters / and such.




## TODO

- [X] Simple Skeletal Mesh Follow Spline
- [X] Stay along the ground not neccessarily spline
- [ ] Change speed running uphill / downhill
  - [X] constant decrease / increase if on hill (binary)
    - [ ] Transition between current speed and Down / Up Speed
  - [ ] lower / increase based on slope (angle)
    - [ ] transition to this
- [ ] Falling downward
- [ ] Running into a wall not through
- [ ] Multiple splines
  - [ ] Base Spline
  - [ ] Wait for some time (work)
  - [ ] Next Spline
  - [ ] keep doing that
- [ ] Path around obsticles back to spline 
