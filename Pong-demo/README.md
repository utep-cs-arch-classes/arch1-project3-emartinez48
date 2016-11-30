## Description
This Program is a Demo Pong game that was made with shape-motion.c and other added features

The Program keeps score above the play area.  The area will draw different depening on the players score.  If the score is in the 10's it will display a different color that if it was in the 20's and so on.  The max score you can get is 999 and after that it will display a character ':' in the hundreds place.  The Program also plays a beep if it touches any wall in the game.  If the ball touches the left or right walls the score will reset to zero in the ones place but will not change in the tens place allowing the player to make it to 999 if they play long enough. With a touch method it will push the ball the opposite direction when it touches a paddle in which case it should count it as a point.  The objective and to be able to get points is by moving the paddle and preventing for the ball to touch the wall.  If you do not move you do not get points.

_Separation of duties:
Since I worked off shapemotion I kept all of the code there to work off.  THe modifications that I have made to the file is adding a buzzer as well as button pushes.  I also added the ability to change color below.  Unfortunatly I have all of my code in one file instead of header files.  The order of the program itself I have all of my shapes and reigons in the begining of my files after I handle how they all move as well as some method provided.  After I have my buzzer code that will handle the beep at certain positions.  Lasty I have my main which calls all of the other methods.

Credit and Collaberation:
To complete this Lab assignment I had help from Cesar Garcia and Luis Cuellar in understanding the collision between the ball and the paddle
