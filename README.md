# LavaFlow
lava flow visualization on a real topographic surface

The projects allow to visualizate a real topographic surface, represented
through three datasets: one for the ALTITUDE, one for the THICKNESS and one for 
the TEMPERATURE.

The visuazzation could be:

-graphic representation based on straightforward color mapping with color black
	for lower altitude and white color for the highest.

-visualizzation with openGL transformation, light and texture for real appearance.
	In this case wi can have three different type:
	-topographic represtation with only light
	-topographic representation with light and lava
	-topographic representation with light, lava and texture
	
	
The command that is possible use in application are:

A:	translate the view on left
S:	move the camera back
D:	translate the view on right
W:	move the camera forward
UP,DOWN,LEFT,RIGHT:	 rotate camera

alt+A: 	translate the light on left
alt+S: 	move the light back
alt+D: 	translate the light on right
alt+W: 	move the light forward

Q,E: rotate the plan on x (slightly)
alt+Q,alt+E: rotate the plan on y (slightly)

Left_Contorl: reset angle of rotation
R: 	reset initial camera position
T: 	reset initial light position
I: 	print current info of light and position

1: 	active straightforward color mapping (not work with altitudes dataset, 
	with test yes. Not understand why. The colors are correct but it draw 
	all fragment white. If you resolve, please contact me)
2: 	active topographic represtation with only light
3: 	active topographic representation with light and lava
4: 	active topographic representation with light, lava and texture

