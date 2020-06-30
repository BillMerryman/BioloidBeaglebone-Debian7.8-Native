PROJECT=BioloidBeaglebone

# Directory for C-Source
vpath %.c $(CURDIR)

# Directory for object files
OBJDIR = $(CURDIR)

# OpenCV include path

OPENCV_INC = -I/usr/local/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 

# OpenCV library path

OPENCV_LIBPATH = -L/usr/local/lib 

# OpenCV libraries

OPENCV_LIBS = 	-lopencv_shape \
				-lopencv_stitching \
				-lopencv_objdetect \
				-lopencv_superres \
				-lopencv_videostab \
				-lopencv_calib3d \
				-lopencv_features2d \
				-lopencv_highgui \
				-lopencv_videoio \
				-lopencv_imgcodecs \
				-lopencv_video \
				-lopencv_photo \
				-lopencv_ml \
				-lopencv_imgproc \
				-lopencv_flann \
				-lopencv_core \
				-lopencv_hal

# Other dependencies
DEPS = \
 Makefile \
 pru.h \
 pruss_intc_mapping.h \
 prussdrv.h 
 

# Compiler object files 
COBJ = \
 main.o \
 pru.o \
 prussdrv.o \
 motionManager.o  \
 visionManager.o
 
 CPPOBJ =

# gcc binaries to use
CC = gcc
CP = g++
LD = g++

# rm 
REMOVE = rm -f

# Compiler options
CFLAGS = -marm
CFLAGS += -O4 
CFLAGS += -g 
CFLAGS += -I.
CFLAGS += $(CDEFINE)   

# for a better output
MSG_EMPTYLINE = . 
MSG_COMPILING = ---COMPILE--- 
MSG_LINKING = ---LINK--- 
MSG_SUCCESS = ---SUCCESS--- 

# Our favourite
all: pru_bin $(PROJECT)

#$(PROJECT):
#	$(CP) $(CFLAGS) -o main main.c image.c  pru.c prussdrv.c

# Linker call
$(PROJECT): $(COBJ) $(CPPOBJ)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_LINKING)
	$(LD) -o $@ $^ $(CFLAGS) $(OPENCV_LIBPATH) $(OPENCV_LIBS) -lpthread
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_SUCCESS) $(PROJECT)

# Compiler call
$(COBJ): %.o: %.c $(DEPS)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_COMPILING) $<
	$(CC) -c -o $@ $< $(CFLAGS) $(OPENCV_INC)
	
$(CPPOBJ): %.o: %.cpp
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_COMPILING) $<
	$(CP) -c -o $@ $< $(CFLAGS) $(OPENCV_INC)

clean: pru_clean
	$(REMOVE) ./*.o ./PRUInteropDataLocation.h
	$(REMOVE) $(PROJECT)
	
pru_bin:
	make -C ./PRU_0 TARGET_MODE=Release
	make -C ./PRU_1 TARGET_MODE=Release

pru_clean:
	make -C PRU_0 clean
	make -C PRU_1 clean

