CXX = g++
CXXFLAGS = -std=c++11 -g -DDEBUG -march=native
CXXFLAGS += -DSHOW_IMG_WINDOW
LIBS = -lm -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lgsl
all:hw1 hw3 hw4 hw5 hw7
hw1:hw1.o
	$(CXX) $(CXXFLAGS) -o hw1 hw1.o $(LIBS)
hw3:hw3.o
	$(CXX) $(CXXFLAGS) -o hw3 hw3.o $(LIBS)
hw4:hw4.o
	$(CXX) $(CXXFLAGS) -o hw4 hw4.o $(LIBS)
hw5:hw5.o
	$(CXX) $(CXXFLAGS) -o hw5 hw5.o $(LIBS)
hw7:hw7.o
	$(CXX) $(CXXFLAGS) -o hw7 hw7.o $(LIBS)
clean:
	rm *.o hw1 hw3 hw4
