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

*** Recording Problems ***

I had a lot of trouble with the animation severely lagging and jolting around with extremely slow TIME values. I saw on Piazza that you told someone to just take a video another way if they were having trouble, so after four hours of trying to get the frame dumping to work, I finally decided to just take a video my own way and that is the video attached. I apologize if the quality is bad, I did the best that I could with my less-than-stellar computer specs. The most important aspects for grading are the landing gear (3 sets) folding up after takeoff and folding down after landing, as this is my instance of two-level hierarchical movement like a human arm or bee leg motion. If it is hard to tell what the video looks like, feel free to build my project in Xcode and press "a". Or, see the larger video in the email I sent you on May 21st.

*** In the video I uploaded on CourseWeb, about halfway through the plane starts jittering around. This only happens when recording with O-matic, and does NOT happen when just running the animation with "a". Therefore, it seems to be a graphics card problem, NOT a coding issue. Furthermore, I changed my frame rate to 1/90th of a second because that was the manual times I was getting when running on my own machine (per your Piazza post). If you notice something weird with the plane not landing on the runway in the right spot, taking off too soon, etc., it may be due to the difference in our computers clock speed. In any event, see the videos for the way the animation looks when I do it on my own computer. 

Hope you like it, I put a lot of work into this and I'm really proud of the job I did!