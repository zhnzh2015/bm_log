#include <iostream>
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include <gtest/gtest.h>

#include "log.h"

using namespace std;

class LoggerTest : public ::testing::Test {
protected:
    LoggerTest() : _m_log_path("../log") {}

    virtual ~LoggerTest() {};
    virtual void SetUp() {
        int ret = mkdir(_m_log_path.c_str(), 0755);
        if (ret != 0) {
            if (ret == -1 && errno == EEXIST) {
                DIR *dirp = opendir(_m_log_path.c_str());
                if (dirp == NULL) {
                    fprintf(stderr, "Create directory [../log] failed! ERROR [%s]\n",
                                                    strerror(errno));
                    return;
                }
                closedir(dirp);
            }
        }
    };

    virtual void TearDown() {
        unlink((_m_log_path + "/test_logger").c_str());
        unlink((_m_log_path + "/test_logger.wf").c_str());
    };

    std::string _m_log_path;
};

TEST_F(LoggerTest, write_log) {
    EXPECT_EQ(false, bm::log::Logger::get_instance()->is_init());
    bm::log::Logger::get_instance()->init(BM_LOG_LEVEL_DEBUG, _m_log_path, "test_logger");
    EXPECT_EQ(true, bm::log::Logger::get_instance()->is_init());

    bm_log_init(BM_LOG_LEVEL_DEBUG, _m_log_path, "test_logger");
    bm_log_write(BM_LOG_LEVEL_NOTICE, "Testing BM_LOG_LEVEL_NOTICE");
    bm_log_write(BM_LOG_LEVEL_WARNING, "Testing BM_LOG_WARNING");
    bm_log_write(BM_LOG_LEVEL_FATAL, "Testing BM_LOG_FATAL");
    bm_log_write(BM_LOG_LEVEL_DEBUG, "Testing BM_LOG_DEBUG");
    bm_log_write(BM_LOG_LEVEL_TRACE, "Testing BM_LOG_TRACE");

    bm_log_set_level(BM_LOG_LEVEL_WARNING);

    bm_log_write(BM_LOG_LEVEL_NOTICE, "Testing BM_LOG_LEVEL_NOTICE");
    bm_log_write(BM_LOG_LEVEL_WARNING, "Testing BM_LOG_WARNING");
    bm_log_write(BM_LOG_LEVEL_FATAL, "Testing BM_LOG_FATAL");
    bm_log_write(BM_LOG_LEVEL_DEBUG, "Testing BM_LOG_DEBUG");
    bm_log_write(BM_LOG_LEVEL_TRACE, "Testing BM_LOG_TRACE");

    char buffer[4096];
    int file = open((_m_log_path + "/test_logger.wf").c_str(), O_RDONLY);
    ssize_t read_bytes = read(file, buffer, sizeof(buffer));
    uint32_t line_cnt = 0;
    for (ssize_t k = 0; k < read_bytes; ++k) {
        if (buffer[k] == '\n') {
            line_cnt += 1;
        }
    }
    close(file);
    EXPECT_EQ(4u, line_cnt);
}
