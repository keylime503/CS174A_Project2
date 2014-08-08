Keon Vafai
103-922-513
CS 174A Project 2

Modeling Description:

- TWO airports. EACH airport has a ground plane, runway, control tower base, and control tower top in a hierarchical relationship. 
- One airplane. The airplane has a fuselage, nose, tail, main wings, elevator wings, nose landing gear, and TWO sets of main landing gear, in a hierarchal relationship.
- 3 sets of landing gear. Each landing gear is made of two rod pieces and 2 or 6 wheels on a wheel bar at the bottom (4+ hierarchal pieces each). The motion of each set of landing gear is a two-level hierarchical relationship like the movement of a human arm or the bee legs from project 1, although the nose landing gear folds in the X-Y plane while the two sets of main landing gear fold in the Y-Z plane, in opposite Z directions.

Animation Description:

The scene begins at the origin airport with a 360 degree flyover using LookAt() (beginning to ~7 seconds). Then, the camera moves to a view next to the runway as the plane accelerates down the runway, pitches up, and lifts off. Next the camera moves to under the takeoff-path so that you can see the landing gear fold away (*** 3 sets of two-level hierarchical movement ***) in the fuselage. 

Then, the camera moves again to watch the plane continue to climb and level at cruise altitude. Next, the camera switches to a side view in the air as the plane turns slightly to the right, and then slightly to the left to straighten out. Then the plane begins to descend. 

Finally, the camera switches to a lower view so you can see the landing gear slowly deploy (*** another 3 sets of two-level hierarchical movement ***). Finally, the camera switches to a wide-airport view and follows the plane as it touches down on the runway at the destination airport and slows to a complete halt.
