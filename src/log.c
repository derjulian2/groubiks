
#include <groubiks/log.h>

define_vector(log_t);

vector_t(log_t) g_logs = vector_default(log_t);

int log_init() {
    if (vector_construct(log_t, &GROUBIKS_LOGS_CONTAINER, GROUBIKS_MAX_LOGS) != 0) {
        return -1;
    }
    if ((log_new(INFO_LOG_DEFAULT_FNO, 
                INFO_LOG_DEFAULT_PREFIX,
                INFO_LOG_DEFAULT_USE_TIMESTAMP) != 0) ||
        (log_new(DEBUG_LOG_DEFAULT_FNO, 
                DEBUG_LOG_DEFAULT_PREFIX,
                DEBUG_LOG_DEFAULT_USE_TIMESTAMP) != 1) ||
        (log_new(WARNING_LOG_DEFAULT_FNO, 
                WARNING_LOG_DEFAULT_PREFIX,
                WARNING_LOG_DEFAULT_USE_TIMESTAMP) != 2) ||
        (log_new(ERROR_LOG_DEFAULT_FNO, 
                ERROR_LOG_DEFAULT_PREFIX,
                ERROR_LOG_DEFAULT_USE_TIMESTAMP) != 3))
    { return -1; }
    return 0;
}

int log_flush() {
    for (int i = 0; i < GROUBIKS_LOGS_CONTAINER.size; ++i) {
        fflush(vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, i).m_fno);
    }
}

void log_end() {
    log_flush();
    for (int i = 0; i < GROUBIKS_LOGS_CONTAINER.size; ++i) {
        free((char*)vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, i).m_prefix);
    }
    vector_destroy(log_t, &GROUBIKS_LOGS_CONTAINER);
}

int log_new(FILE* fno, const char* prefix, int use_timestamp) {
    assert(GROUBIKS_LOGS_CONTAINER.size < GROUBIKS_LOGS_CONTAINER.capacity);
    log_t tmp = (log_t) {
        .m_fno = fno,
        .m_prefix = strdup(prefix),
        .m_use_timestamp = use_timestamp
    };
    if ((tmp.m_prefix == NULL) || 
        (vector_push_back(log_t, &GROUBIKS_LOGS_CONTAINER, tmp) == 
         vector_end(log_t, &GROUBIKS_LOGS_CONTAINER))) 
    { return -1; }
    return GROUBIKS_LOGS_CONTAINER.size - 1;
}

void _log_make_timestamp(char* buf) {
    time_t tm;
    struct tm* tm_info;
    tm = time(NULL);
    tm_info = localtime(&tm);
    strftime(buf, 26, "%Y-%m-%d %H:%M:%S;", tm_info);
}

void _log_make_msg(int lognum, const char* msg) {
    assert(GROUBIKS_LOGS_CONTAINER.size > lognum);
    log_t* log = &vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, lognum);
    
    char timestamp_str[26];
    memset(&timestamp_str[0], 0, sizeof(timestamp_str));
    if (log->m_use_timestamp) {
        _log_make_timestamp(&timestamp_str[0]);
        fputs(&timestamp_str[0], log->m_fno);
    }
    if (log->m_prefix != NULL) {
        fputs(log->m_prefix, log->m_fno);
        fputc(':', log->m_fno);
    }
    fputs(msg, log->m_fno);
    fputc('\n', log->m_fno);
#ifdef GROUBIKS_LOGS_ALWAYS_FLUSH
    log_flush();
#endif
}

void _log_make_fmsg(int lognum, const char* fmt, ...) {
    assert(GROUBIKS_LOGS_CONTAINER.size > lognum);
    log_t* log = &vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, lognum);
    
    char timestamp_str[26];
    memset(&timestamp_str[0], 0, sizeof(timestamp_str));
    if (log->m_use_timestamp) {
        _log_make_timestamp(&timestamp_str[0]);
        fputs(&timestamp_str[0], log->m_fno);
    }
    if (log->m_prefix != NULL) {
        fputs(log->m_prefix, log->m_fno);
        fputc(':', log->m_fno);
    }
    va_list arglist;
    va_start(arglist, fmt);
    vfprintf(log->m_fno, fmt, arglist);
    va_end(arglist);
    fputc('\n', log->m_fno);
#ifdef GROUBIKS_LOGS_ALWAYS_FLUSH
    log_flush();
#endif
}