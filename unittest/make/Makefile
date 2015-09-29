all : test_logger

BASE_ROOT=../../../..
LOCAL_ROOT=../..
LIB_PATH=-L$(BASE_ROOT)/public/log/lib \
		 -L$(BASE_ROOT)/third/gtest/1.7.0/lib

INC_PATH=-I$(BASE_ROOT)/public/log/include \
		 -I$(BASE_ROOT)/third/gtest/1.7.0/include

test_logger : $(LOCAL_ROOT)/unittest/case/test_logger.cc
	(cd ../../ && sh build.sh clean && sh build.sh)
	g++ $(LOCAL_ROOT)/unittest/case/test_logger.cc -o test_logger $(INC_PATH) $(LIB_PATH) -lbm_log -lgtest -lgtest_main

clean:
	rm -rf test_logger
