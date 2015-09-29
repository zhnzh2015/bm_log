#include "log.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

namespace bm {
namespace log {

Logger::~Logger() {
    pthread_mutex_destroy(&_m_flog_lock);
    if (_m_flog != stdout && _m_flog != NULL) {
        fclose(_m_flog);
    }

    pthread_mutex_destroy(&_m_ffatal_lock);
    if (_m_ffatal != stderr && _m_ffatal != NULL) {
        fclose(_m_ffatal);
    }
    pthread_mutex_destroy(&_m_init_lock);
}

void Logger::init(int level, const std::string &dir, const std::string &fname) {
    if (_m_is_inited) {
        return;
    }
    pthread_mutex_lock(&_m_init_lock); // Access init lock
    if (!_m_is_inited) {
        _m_log_level = static_cast<BMLogLevel>(level);
        std::string path = dir + "/" + fname;

        int ret = mkdir(dir.c_str(), 0755);
        if (ret != 0) {
            if (ret == -1 && errno == EEXIST) {
                DIR *dirp = opendir(dir.c_str());
                if (dirp == NULL) {
                    fprintf(stderr, "Create directory [%s] failed! ERROR [%s]\n",
                                     dir.c_str(), strerror(errno));
                    return;
                }
                closedir(dirp);
            }
        }

        _m_flog = fopen(path.c_str(), "a");
        if (_m_flog == NULL) {
            fprintf(stderr, "Opening file [%s] failed! ERROR [%s]\n",
                             path.c_str(), strerror(errno));
            exit(1);
        }

        _m_ffatal = fopen((path + ".wf").c_str(), "a");
        if (_m_ffatal == NULL) {
            fprintf(stderr, "Opening file [%s.wf] failed! ERROR [%s]\n",
                             path.c_str(), strerror(errno));
            exit(1);
        }
        _m_is_inited = true;
    }
    pthread_mutex_unlock(&_m_init_lock);
}

bool Logger::write_log(BMLogLevel level, const char *fmt, ...) {
    if (level > _m_log_level) {
        return true;
    }

    time_t t;  
    struct tm *tp;
    char time_str[100];
    time(&t);
    tp = localtime(&t);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tp);

    va_list ap;
    char buffer[4096];

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    switch (level) {
    case BM_LOG_LEVEL_FATAL:
        pthread_mutex_lock(&_m_ffatal_lock);
        fprintf(_m_ffatal, "[%s][FATAL]%s\n", time_str, buffer);
        fflush(_m_ffatal);
        pthread_mutex_unlock(&_m_ffatal_lock);
        break;

    case BM_LOG_LEVEL_WARNING:
        pthread_mutex_lock(&_m_ffatal_lock);
        fprintf(_m_ffatal, "[%s][WARNING]%s\n", time_str, buffer);
        pthread_mutex_unlock(&_m_ffatal_lock);
        break;

    case BM_LOG_LEVEL_NOTICE:
        pthread_mutex_lock(&_m_flog_lock);
        fprintf(_m_flog, "[%s][NOTICE]%s\n", time_str, buffer);
        pthread_mutex_unlock(&_m_flog_lock);
        break;

    case BM_LOG_LEVEL_TRACE:
        pthread_mutex_lock(&_m_flog_lock);
        fprintf(_m_flog, "[%s][TRACE]%s\n", time_str, buffer);
        pthread_mutex_unlock(&_m_flog_lock);
        break;

    case BM_LOG_LEVEL_DEBUG:
        pthread_mutex_lock(&_m_flog_lock);
        fprintf(_m_flog, "[%s][DEBUG]%s\n", time_str, buffer);
        pthread_mutex_unlock(&_m_flog_lock);
        break;

    default:
        fprintf(stderr, "Unknown log level [%d]\n", level);
        return false;
    }
    return true;
}

} // END namespace log
} // END namespace bm
