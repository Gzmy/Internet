ROOT_PATH=$(shell pwd)
CLIENT=$(ROOT_PATH)/client
SERVER=$(ROOT_PATH)/server
DATAPOOL=$(ROOT_PATH)/data_pool

SER_SRC=$(shell ls $(SERVER)/*.cpp $(DATAPOOL)/*.cpp | sed 's/\/.*\///g')
CLI_SRC=$(shell ls $(CLIENT)/*.cpp | sed 's/\/.*\///g')

SER_OBJ=$(shell echo $(SER_SRC) | sed 's/.cpp/.o/g')
CLI_OBJ=$(shell echo $(CLI_SRC) | sed 's/.cpp/.o/g')

LDFLAGS=-lpthread

ser=chatSystem
cli=chatClient
cc=g++

.PHONY:all
all:$(ser) $(cli)

$(ser):$(SER_OBJ)
	$(cc) -o $@ $^ $(LDFLAGS)
$(cli):$(CLI_OBJ)
	$(cc) -o $@ $^
%.o:$(CLIENT)/%.cpp
	$(cc) -c $<
%.o:$(SERVER)/%.cpp
	$(cc) -c $<
%.o:$(DATAPOOL)/%.cpp
	$(cc) -c $<

.PHONY:clean
clean:
	rm -rf $(ser) $(cli) *.o

.PHONY:debug
debug:
	@echo $(SER_OBJ)
	@echo $(CLI_OBJ)
