#This file for track

# Define source file lists to SRC_LIST

SRC_LIST +=track_agps\src\track_fun_compress.c\
           track_agps\src\track_fun_des.c

# Only need to be increased .c file in the above

#  Define include path lists to INC_DIR
INC_DIR =  hal\peripheral\inc \
			drv\include \
			inc \
			interface\hwdrv \
			interface\hal\L1 \
			interface\hal\
			track\drv\drv_inc \
			track_agps\inc \
			track\drv\drv_inc \
			track\at\at_inc \
			ELL\ell-common \
			track\fun\fun_inc  \
			track\at\at_inc \
			track\os\os_inc \
			gps\mnl\include

# Define the specified compile options to COMP_DEFS
COMP_DEFS = __GPS_TRACK__

# Define the source file search paths to SRC_PATH
SRC_PATH = track_agps\src
