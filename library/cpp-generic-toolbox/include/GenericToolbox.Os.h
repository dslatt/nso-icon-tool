//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_OS_H
#define CPP_GENERIC_TOOLBOX_OS_H


#ifdef __SWITCH__
#include <switch.h>
#endif

#include <fstream>
#include <string>
#include <vector>
#include <array>

#include <sys/statvfs.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>


// ***************************
//! Operating System Tools
// ***************************


// Declaration section
namespace GenericToolbox{

  static std::string getHomeDirectory();
  static std::string getCurrentWorkingDirectory();
  static std::string expandEnvironmentVariables(const std::string &filePath_);
  static std::string getExecutableName(); // untested on windows platform

  // hardware
  static size_t getProcessMemoryUsage();
  static size_t getProcessMaxMemoryUsage();
  static double getCpuUsageByProcess();
  static long getProcessMemoryUsageDiffSinceLastCall();
  static double getFreeDiskSpacePercent( const std::string& path_ );
  static unsigned long long getFreeDiskSpace( const std::string& path_ );
  static unsigned long long getTotalDiskSpace( const std::string& path_ );
  static int getTerminalWidth();
  static int getTerminalHeight();
  static std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_);

}


// Implementation section
#if defined(_WIN32)
// Windows
#include <windows.h>
#include <psapi.h>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
// MacOS
#include <sys/resource.h>
#include <sys/statvfs.h>
#include <sys/ioctl.h>
#include <sys/times.h>
#include <mach/mach.h>
#include <unistd.h>
#include <array>

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
// Linux
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/times.h>
#include <sys/statvfs.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
// AIX and Solaris
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <procfs.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <sys/times.h>

#else
// Unsupported
#endif

extern char* __progname;


namespace GenericToolbox{

  namespace Internals {
    static char * getEnvironmentVariable(char const envVarName_[]){
#if defined _WIN32 // getenv() is deprecated on Windows
      char *buf{nullptr};
      size_t sz;
      std::string val;
      if (_dupenv_s(&buf, &sz, envVarName_) || buf == nullptr) return val;
      val = buf;
      free(buf);
      return val;
#else
      return getenv(envVarName_);
#endif
    }
    static bool expandEnvironmentVariables(const char *fname, char *xname) {
      int n, ier, iter, lx, ncopy;
      char *inp, *out, *x, *t, *buff;
      const char *b, *c, *e;
      const char *p;
      int kBufSize{8196};
      buff = new char[kBufSize * 4];

      iter = 0; xname[0] = 0; inp = buff + kBufSize; out = inp + kBufSize;
      inp[-1] = ' '; inp[0] = 0; out[-1] = ' ';
      c = fname + strspn(fname, " \t\f\r");
      //VP  if (isalnum(c[0])) { strcpy(inp, WorkingDirectory()); strcat(inp, "/"); } // add $cwd

      strncat(inp, c, kBufSize);

      again:
      iter++; c = inp; ier = 0;
      x = out; x[0] = 0;

      p = nullptr; e = nullptr;
      if (c[0] == '~' && c[1] == '/') { // ~/ case
        std::string hd = GenericToolbox::getHomeDirectory();
        p = hd.c_str();
        e = c + 1;
        if (p) {                         // we have smth to copy
          strncpy(x, p, kBufSize);
          x += strlen(p);
          c = e;
        } else {
          ++ier;
          ++c;
        }
      }
//      else if (c[0] == '~' && c[1] != '/') { // ~user case
//        n = int(strcspn(c+1, "/ "));
//        // There is no overlap here as the buffer is segment in 4 strings of at most kBufSize
//        strncpy(buff, c+1, n+1); // strncpy copy 'size-1' characters.
//        std::string hd = GenericToolbox::getHomeDirectory(); // TO FIX this?
//        e = c+1+n;
//        if (!hd.empty()) {                   // we have smth to copy
//          p = hd.c_str();
//          strncpy(x, p, kBufSize);
//          x += strlen(p);
//          c = e;
//        } else {
//          x++[0] = c[0];
//          //++ier;
//          ++c;
//        }
//      }

      for ( ; c[0]; c++) {

        p = nullptr; e = nullptr;

        if (c[0] == '.' && c[1] == '/' && c[-1] == ' ') { // $cwd
          std::string wd = GenericToolbox::getCurrentWorkingDirectory();
          strncpy(buff, wd.c_str(), kBufSize);
          p = buff;
          e = c + 1;
        }
        if (p) {                          // we have smth to copy */
          strncpy(x, p, kBufSize); x += strlen(p); c = e-1; continue;
        }

        if (c[0] != '$') {                // not $, simple copy
          x++[0] = c[0];
        } else {                          // we have a $
          b = c+1;
          if (c[1] == '(') b++;
          if (c[1] == '{') b++;
          if (b[0] == '$')
            e = b+1;
          else
            for (e = b; isalnum(e[0]) || e[0] == '_'; e++) ;
          buff[0] = 0; strncat(buff, b, e-b);
          p = GenericToolbox::Internals::getEnvironmentVariable(buff);
          if (!p) {                      // too bad, try UPPER case
            for (t = buff; (t[0] = toupper(t[0])); t++) ;
            p = GenericToolbox::Internals::getEnvironmentVariable(buff);
          }
          if (!p) {                      // too bad, try Lower case
            for (t = buff; (t[0] = tolower(t[0])); t++) ;
            p = GenericToolbox::Internals::getEnvironmentVariable(buff);
          }
          if (!p && !strcmp(buff, "cwd")) { // it is $cwd
            std::string wd = GenericToolbox::getCurrentWorkingDirectory();
            strncpy(buff, wd.c_str(), kBufSize);
            p = buff;
          }
          if (!p && !strcmp(buff, "$")) { // it is $$ (replace by GetPid())
            snprintf(buff,kBufSize*4, "%d", getpid());
            p = buff;
          }
          if (!p) {                      // too bad, nothing can help
#ifdef WIN32
            // if we're on windows, we can have \\SomeMachine\C$ - don't
            // complain about that, if '$' is followed by nothing or a
            // path delimiter.
            if (c[1] && c[1]!='\\' && c[1]!=';' && c[1]!='/')
               ier++;
#else
            ier++;
#endif
            x++[0] = c[0];
          } else {                       // It is OK, copy result
            int lp = int(strlen(p));
            if (lp >= kBufSize) {
              // make sure lx will be >= kBufSize (see below)
              strncpy(x, p, kBufSize);
              x += kBufSize;
              break;
            }
            strcpy(x,p);
            x += lp;
            c = (b==c+1) ? e-1 : e;
          }
        }
      }

      x[0] = 0; lx = x - out;
      if (ier && iter < 3) { strncpy(inp, out, kBufSize); goto again; }
      ncopy = (lx >= kBufSize) ? kBufSize-1 : lx;
      xname[0] = 0; strncat(xname, out, ncopy);

      delete[] buff;

      if (ier || ncopy != lx) {
        return true;
      }

      return false;
    }
    namespace Hardware{
      static size_t lastProcessMemoryUsage = 0;
    }
  }

  static std::string getHomeDirectory(){
    struct passwd *pw = getpwuid(getuid());
    return {pw->pw_dir};
  }
  static std::string getCurrentWorkingDirectory(){
#ifdef PATH_MAX
    char cwd[PATH_MAX];
#else
    char cwd[1024];
#endif
    if( getcwd(cwd, sizeof(cwd)) == nullptr ){
      throw std::runtime_error("getcwd() returned an invalid value.");
    }
    std::string output_cwd(cwd);
    return output_cwd;
  }
  static std::string expandEnvironmentVariables(const std::string &filePath_){
//#define USE_BASH_TO_EXPAND // VERY SLOW IN FACT...
#ifdef USE_BASH_TO_EXPAND
    std::array<char, PATH_MAX> buffer{};
    std::string result;

    std::string command = "echo " + filePath_;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if( pipe == nullptr ){ return filePath_; }

    std::FILE* pipe_ptr = pipe.get();
    while (std::size_t n = std::fread(buffer.data(), sizeof(char), buffer.size(), pipe_ptr)) {
      result.append(buffer.data(), n-1);
    }

    return result;
#else
    //    char outputName[PATH_MAX];
    char outputName[8192];
    Internals::expandEnvironmentVariables(filePath_.c_str(), outputName);

    return {outputName};
#endif
  }
  static std::string getExecutableName(){
    std::string outStr;
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) //check defines for your setup
    std::ifstream("/proc/self/comm") >> outStr;
#elif defined(_WIN32)
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    outStr = buf;
#else
    outStr = __progname;
#endif
    return outStr;
  }

  struct CpuStat{
    inline CpuStat(){ this->getCpuUsageByProcess(); }
    clock_t lastCPU{}, lastSysCPU{}, lastUserCPU{};
    inline double getCpuUsageByProcess(){
      double percent{0};
#if defined(_WIN32)
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) || defined(__APPLE__) && defined(__MACH__)
      struct tms timeSample{};
      clock_t now;

      now = times(&timeSample);
      if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
          timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this value.
        percent = -1.0;
      }
      else{
        percent = double(timeSample.tms_stime - lastSysCPU) +
                  double(timeSample.tms_utime - lastUserCPU);
        percent /= double(now - lastCPU);
        percent *= 100;
      }
      lastCPU = now;
      lastSysCPU = timeSample.tms_stime;
      lastUserCPU = timeSample.tms_utime;
#endif
      return percent;
    }
  };
  static CpuStat cs{};

  static size_t getProcessMemoryUsage(){
    /**
     * Returns the current resident set size (physical memory use) measured
     * in bytes, or zero if the value cannot be determined on this OS.
     */
#if defined(_WIN32)
    // Windows
    PROCESS_MEMORY_COUNTERS memCounter;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof memCounter))
        return (size_t)memCounter.WorkingSetSize;
    return (size_t)0; /* get process mem info failed */

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOS
    struct mach_task_basic_info info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
      return (size_t)info.resident_size;
    return (size_t)0; /* query failed */


#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
//    long rss = 0L;
//    FILE* fp = NULL;
//    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
//        return (size_t)0L;      /* Can't open? */
//    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
//    {
//        fclose( fp );
//        return (size_t)0L;      /* Can't read? */
//    }
//    fclose( fp );
//    return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);
    // Physical Memory currently used by current process
    // https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    FILE* file = fopen("/proc/self/status", "r");
    size_t result{0};
    char line[128];

    while (fgets(line, 128, file) != nullptr){
      if (strncmp(line, "VmRSS:", 6) == 0){
        result = strlen(line);
        const char* p = line;
        while (*p <'0' || *p > '9') p++;
        line[result-3] = '\0';
        result = size_t(atol(p));
        break;
      }
    }
    fclose(file);
    return result*1000;
#else
    // AIX, BSD, Solaris, and Unknown OS
    return (size_t)0L;          /* Unsupported. */

#endif
  }
  static size_t getProcessMaxMemoryUsage(){
    /**
     * Returns the peak (maximum so far) resident set size (physical
     * memory use) measured in bytes, or zero if the value cannot be
     * determined on this OS.
     */
#if defined(_WIN32)
    // Windows
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return (size_t)info.PeakWorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOS
    struct mach_task_basic_info info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
      return (size_t)info.resident_size_max;
    return (size_t)0; /* query failed */

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
    struct rusage rusage;
    if (!getrusage(RUSAGE_SELF, &rusage))
        return (size_t)rusage.ru_maxrss;
    return (size_t)0; /* query failed */

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    // AIX and Solaris
    struct psinfo psinfo;
    int fd = -1;
    if ( (fd = open( "/proc/self/psinfo", O_RDONLY )) == -1 )
        return (size_t)0L;      /* Can't open? */
    if ( read( fd, &psinfo, sizeof(psinfo) ) != sizeof(psinfo) )
    {
        close( fd );
        return (size_t)0L;      /* Can't read? */
    }
    close( fd );
    return (size_t)(psinfo.pr_rssize * 1024L);
#else
    // Unknown OS
    return (size_t)0L;          /* Unsupported. */
#endif
  }
  static double getCpuUsageByProcess(){
    return cs.getCpuUsageByProcess();
  }
  static long getProcessMemoryUsageDiffSinceLastCall(){
    size_t currentProcessMemoryUsage = getProcessMemoryUsage();
    long outVal = static_cast<long>(currentProcessMemoryUsage) - static_cast<long>(Internals::Hardware::lastProcessMemoryUsage);
    Internals::Hardware::lastProcessMemoryUsage = currentProcessMemoryUsage;
    return outVal;
  }
  static double getFreeDiskSpacePercent( const std::string& path_ ){
    return double( getFreeDiskSpace(path_) )/double( getTotalDiskSpace(path_) );
  }
  static unsigned long long getFreeDiskSpace( const std::string& path_ ){
    struct statvfs stat{};
    if( statvfs(path_.c_str(), &stat) != 0 ){ return 0; }
    return stat.f_bsize * static_cast<unsigned long long>(stat.f_bfree);
  }
  static unsigned long long getTotalDiskSpace( const std::string& path_ ){
    struct statvfs stat{};
    if( statvfs(path_.c_str(), &stat) != 0 ){ return 0; }
    return stat.f_bsize * static_cast<unsigned long long>(stat.f_blocks);
  }
  static int getTerminalWidth(){
    int outWith = 0;
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    outWith = (int)(csbi.dwSize.X);
//    outWith = (int)(csbi.dwSize.Y);
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) \
    || (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__))) \
    || ( defined(__APPLE__) && defined(__MACH__) )
    struct winsize winSize{};
    ioctl(fileno(stdout), TIOCGWINSZ, &winSize);
    outWith = (int)(winSize.ws_col);
//    outWith = (int)(winSize.ws_row);
#elif defined(__SWITCH__)
    outWith = consoleGetDefault()->consoleWidth;
#endif // Windows/Linux
    return outWith;
  }
  static int getTerminalHeight(){
    int outWith = 0;
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//    outWith = (int)(csbi.dwSize.X);
    outWith = (int)(csbi.dwSize.Y);
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) \
    || (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__))) \
    || ( defined(__APPLE__) && defined(__MACH__) )
    struct winsize w{};
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
//    outWith = (int)(w.ws_col);
    outWith = (int)(w.ws_row);
#elif defined(__SWITCH__)
    outWith = consoleGetDefault()->consoleHeight;
#endif // Windows/Linux
    return outWith;
  }
  static std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_) {
    // Inspired from: https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
    std::array<char, 128> buffer{};
    std::vector<std::string> output;
#if defined(_WIN32)
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd_.c_str(), "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd_.c_str(), "r"), pclose);
#endif
    if( pipe == nullptr ){
//      throw std::runtime_error("popen() failed!");
    }
    else{
      while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        output.emplace_back( buffer.data() );
      }
    }
    return output;
  }


}


#endif // CPP_GENERIC_TOOLBOX_OS_H
