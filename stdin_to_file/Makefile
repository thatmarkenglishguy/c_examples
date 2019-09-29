#Makefile
OBJ = stdin_to_file.o
TEST_DIR = ./tests
TEST_OBJ = $(TEST_DIR)/test_main.o $(TEST_DIR)/test_stdin_to_file.o

CXXFLAGS = -std=c++1z -g
CATCH_DIRECTORY = ../../../../thirdparty/catch/Catch2/single_include/catch2

# If CATCH_DIRECTORY is empty
ifeq ($(wildcard $(CATCH_DIRECTORY)),)
  CATCH_DIRECTORY = ../../thirdparty/catch/Catch2.git/catch2/single_include
endif

all: stdin_to_file

test: test_stdin_to_file run_test

stdin_to_file: $(OBJ)
	g++ -o $@ $^ $(CXXFLAGS)

cleanbuildobj:
	rm -f *.o

cleantestobj:
	rm -f $(TEST_DIR)/*.o

cleantest: cleantestobj
	rm -f test_stdin_to_file

cleanobj: cleanbuildobj cleantestobj

clean: cleanobj cleantest
	rm -f stdin_to_file

test_stdin_to_file: EXTRA_DIRS = -I $(CATCH_DIRECTORY)
run_test:
	./test_stdin_to_file -r compact -s

test_stdin_to_file: $(TEST_OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

#test_stdin_to_file: $(TEST_OBJ)
#	$(CXX) -o $@ $^ $(CXXFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(EXTRA_DIRS) -c -o $@ $<

