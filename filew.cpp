#include <FileWatcher/FileWatcher.h>
#include <map>
#include <vector>

extern "C"
{

#include "filew.h"
#include <malloc.h>
#include <string.h>
#include <assert.h>


#ifdef _WIN32
# include <windows.h>
# include <io.h>
#endif


#include <sys/stat.h>
#ifdef _WIN32
# define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
# define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif


#ifndef _WIN32
# include <dirent.h>
#endif

#ifdef __ANDROID__
# include <unistd.h>
#endif


#ifdef ARYLEN
# undef ARYLEN
#endif
#define ARYLEN(a) (sizeof(a) / sizeof((a)[0]))


#ifdef max
# undef max
#endif
#ifdef min
# undef min
#endif
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))



static char* stzncpy(char* dst, char const* src, size_t len)
{
    assert(len > 0);
#ifdef _WIN32
    char* p = (char*)_memccpy(dst, src, 0, len - 1);
#else
    char* p = memccpy(dst, src, 0, len - 1);
#endif
    if (p) --p;
    else
    {
        p = dst + len - 1;
        *p = 0;
    }
    return p;
}


double getNow(void)
{
#ifdef _WIN32
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    assert(freq.QuadPart > 0);
    return (double)count.QuadPart / freq.QuadPart;
#elif defined(__ANDROID__)
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (double)(now.tv_sec * 1000000000LL + now.tv_nsec) / 1000000000.0;
#elif defined(__EMSCRIPTEN__)
    double now = (double)clock();
    return now / CLOCKS_PER_SEC;
#else
    struct timeval tv;
    int r = gettimeofday(&tv, 0);
    if (0 != r)
    {
        return NaN;
    }
    return (double)(tv.tv_sec * 1000 * 1000 + tv.tv_usec) / 1000000.0;
#endif
}

}


using namespace FW;



struct FILEW_Op
{
    char dir[FILEW_PATH_BUF_MAX];
    char filename[FILEW_PATH_BUF_MAX];
    FILEW_Change change;
    FILEW_Callback cb;

    bool operator==(const FILEW_Op& _b) const
    {
        const FILEW_Op* a = this;
        const FILEW_Op* b = &_b;
        return
            (0 == strncmp(a->dir, b->dir, FILEW_PATH_BUF_MAX)) &&
            (0 == strncmp(a->filename, b->filename, FILEW_PATH_BUF_MAX)) &&
            (a->change == b->change) &&
            (a->cb == b->cb);
    }
};





class FILEW_Listener : public FileWatchListener
{
    FILEW_Context* context;
    std::map<WatchID, FILEW_Callback> m_cbMap;

public:
    FILEW_Listener(FILEW_Context* ctx) : context(ctx) {}

    void addCB(WatchID watchid, FILEW_Callback cb)
    {
        m_cbMap.insert(std::pair<WatchID, FILEW_Callback>(watchid, cb));
    }
    void handleFileAction(WatchID watchid, const String& dir, const String& filename, Action action);
};


struct FILEW_Context
{
    double interval;
    double lastUpdateTime;
    FileWatcher* hander;
    FILEW_Listener* listener;
    std::vector<FILEW_Op> fileOPs;

    FILEW_Context()
        : interval(1.0)
        , lastUpdateTime(0.0)
        , hander(new FileWatcher())
        , listener(new FILEW_Listener(this))
    {
    }
    ~FILEW_Context()
    {
        delete listener;
        delete hander;
    }
};




void FILEW_Listener::handleFileAction(WatchID watchid, const String& dir, const String& filename, Action action)
{
    FILEW_Change change;
    switch (action)
    {
    case Actions::Add:
    {
        change = FILEW_Change_Add;
        break;
    }
    case Actions::Delete:
    {
        change = FILEW_Change_Delete;
        break;
    }
    case Actions::Modified:
    {
        change = FILEW_Change_Modified;
        break;
    }
    default:
        assert(false);
        break;
    }
    //switch (change)
    //{
    //case FILEW_Change_Add:
    //{
    //    printf("File (%s/%s) Added!", dir.c_str(), filename.c_str());
    //    break;
    //}
    //case FILEW_Change_Delete:
    //{
    //    printf("File (%s/%s) Deleted!", dir.c_str(), filename.c_str());
    //    break;
    //}
    //case FILEW_Change_Modified:
    //{
    //    printf("File (%s/%s) Modified!", dir.c_str(), filename.c_str());
    //    break;
    //}
    //default:
    //    assert(false);
    //    break;
    //}
    for (std::map<WatchID, FILEW_Callback>::const_iterator it = m_cbMap.begin(); it != m_cbMap.end(); ++it)
    {
        FILEW_Op op = { 0 };
        stzncpy(op.dir, dir.c_str(), dir.size() + 1);
        stzncpy(op.filename, filename.c_str(), filename.size() + 1);
        op.cb = it->second;
        op.change = change;

        if (std::find(context->fileOPs.begin(), context->fileOPs.end(), op) == context->fileOPs.end())
        {
            context->fileOPs.push_back(op);
        }
    }
}




extern "C" FILEW_Context* FILEW_newContext(void)
{
    return new FILEW_Context();
}

extern "C" void FILEW_contextFree(FILEW_Context* ctx)
{
    assert(ctx);
    delete ctx;
}




extern "C" void FILEW_contextAddFile(FILEW_Context* ctx, const char* path, FILEW_Callback cb)
{
    WatchID watchid = ctx->hander->addWatch(path, ctx->listener);
    ctx->listener->addCB(watchid, cb);
}






extern "C" void FILEW_contextUpdate(FILEW_Context* ctx)
{
    ctx->hander->update();

    double time = getNow();
    if ((ctx->lastUpdateTime > 0.0) && ((time - ctx->lastUpdateTime) > ctx->interval))
    {
        ctx->lastUpdateTime = time;
        for (u32 i = 0; i < ctx->fileOPs.size(); ++i)
        {
            FILEW_Op* op = &ctx->fileOPs[i];
            op->cb(op->dir, op->filename, op->change);
        }
        ctx->fileOPs.clear();
    }
}




















































































