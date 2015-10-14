CXX = g++
CXXFLAGS = -O3 -march=native
#CXXFLAGS += -DSHOW_IMG_WINDOW
LIBS = -lm -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
all:hw1
hw1:hw1.o
	$(CXX) $(CXXFLAGS) -o hw1 hw1.o $(LIBS)
clean:
	rm *.o hw1
