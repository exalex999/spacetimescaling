# spacetimescaling
The program transforms n input bitmaps to m output ones (scaling “in time”), interpolating “gaps” according to Lanczos resampling algorithm, and additionally adjusts their size (scaling in “space”, i.e. normal scaling) using scaling method specified by user (nearest-neighbor, linear or bicubic). SDL2 library is required to run this application.

USAGE:
spacetimescaling <init_path> [-i <init_template> <init_length>] [-o <final_path>] <final_template> <final_width> <final_height> <final_length> <scaling_method>
1. <init_path> - directory address containing input BMP-images to be scaled
2. <init_template> - input image name template which must contain exactly one contiguous sequence of symbols '/' of at least argument <init_length> decimal representation length, end with ".BMP", ".bmp", ".dib", or ".DIB" and have length max. 255 characters
3. <init_length> - image number to be scaled. It must be natural number, less than 10000
4. <final_path> - output directory address to write out a sequence of scaled images.
5. <final_template> - output image name template which must contain exactly one contiguous sequence of symbols '/' of at least argument <final_length> decimal representation length, end with ".BMP", ".bmp", ".dib", or ".DIB" and have length max. 255 characters
6. <final_width> - output image width (in pixels)
7. <final_height> - output image height (inpixels)
8. <final_length> - output image sequence length
9. <scaling_method> - image scaling method to the required size. It can have one of the following values:
	* NEIGHBOR - nearest neighbor interpolation
	* LINEAR - linear horizontal interpolation
	* BICUBIC - bicubic interpolation

RETURN VALUES:
0 - success (output images are written under <final_path> if stated or <init_path> otherwise)
1 - application call syntax errors
2 - image read-errors
3 - image write-errors
