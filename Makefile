CXX = g++
CXXFLAGS = -std=c++11 -O3 -march=native
CXXFLAGS += -DSHOW_IMG_WINDOW
LIBS = -lm -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
all:hw1 hw3 hw4
hw1:hw1.o
	$(CXX) $(CXXFLAGS) -o hw1 hw1.o $(LIBS)
hw3:hw3.o
	$(CXX) $(CXXFLAGS) -o hw3 hw3.o $(LIBS)
hw4:hw4.o
		$(CXX) $(CXXFLAGS) -o hw4 hw4.o $(LIBS)
clean:
	rm *.o hw1 hw3 hw4
