import tools
import helpers

for file in helpers.each_file( rexp=r'\.ttf$', recursive=False ):
	tools.font( file, msdf_pixelRange = 32.0, msdf_fontSize = 64 );
