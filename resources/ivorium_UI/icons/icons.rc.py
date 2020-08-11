import tools
import helpers

for file in helpers.each_file( rexp=r'\.png$' ):
	tools.texture( file, density=2.0, filtering="Smooth2D" );
