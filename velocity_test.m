clc
clear
close all
warning("on")

%position          = zeros(200);
g                 = 20;
%speed             = zeros(200);
speed_diff        = 0.05;
xPos              = 50;
  
x_axis = 0:0.1:20;
position(1,1) = 64;
speed(1,1) = 20.0;        


%position = (position + (speed * speed_diff)) - (0.5 * g * speed_diff * speed_diff);
%speed = speed + (g * speed_diff);

for index = 1:200 
    position(1:index+1) = ((position(1:index) + (speed(1:index) * speed_diff)) - (0.5 * g * speed_diff * speed_diff));
    speed(1:index+1) = speed(1:index) + (g * speed_diff);
end