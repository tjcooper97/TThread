#ifndef TTHREAD_H
#define TTHREAD_H
  #include <Arduino.h>
  #include <TSoftwareVersion.h>
    #define TTHREAD_V_MAJOR 2
    #define TTHREAD_V_MINOR 1
    #define TTHREAD_V_PATCH 2
    extern const TSoftwareVersion TThread_Version;

  #ifdef ARDUINO_PORTENTA_H7_M7
    #define FullTThread
  #else
    #ifdef ARDUINO_PORTENTA_H7_M4
      #define FullTThread
    #else
      #ifdef ARDUINO_GIGA
        #define FullTThread
      #endif
    #endif
  #endif


  #ifdef FullTThread
    #include <functional>
  #endif

  extern uint8_t TThread_Priority; // Threads with a priority level less than this will not be allowed to run (overrides _pauseallowed)

  // Priority Levels
  #define tp_last 3 // Default priority is always this
    #define tp_NONE 0 // Lowest level priority
    #define tp_LOW  1
    #define tp_MED  2
    #define tp_HIGH 3 // Highest level priority

  // Statuses
  #define ts_last 4 // Default status is always this
    #define ts_NEVER 0 // Not yet ran during this runtime
    #define ts_RUN   1 // Running
    #define ts_PAUSE 2 // Paused
    #define ts_WAIT  3 // Waiting to run (indicates it has ran at least once during this runtime)
    #define ts_ERROR 4 // Failed to run correctly & a critical piece is preventing this from running

  // Trigger Types
  #define tt_last 5 // Default type is always this
    #define tt_INTERVAL 0 // Thread will be triggered to auto run based on a time interval
    #define tt_BOOL     1 // Thread will be triggered to auto run based on the status of _triggered
    #define tt_BOOLR    2 // Thread will be triggered to auto run based on the status of _triggered (which will auto reset to false after the thread has ran)
    #define tt_INTBOOL  3 // Mix of tt_INTERVAL & tt_BOOL  (whichever comes first)
    #define tt_INTBOOLR 4 // Mix of tt_INTERVAL & tt_BOOLR (whichever comes first)
    #define tt_NONE     5 // Thread will run every time it is called & not paused

class TThread {
  private:
    String _name;

    // Control
      bool    _initialized;
      uint8_t _priority;
      uint8_t _status;
      bool    _paused;
      bool    _pauseallowed;
    
    // When should this tread run?
      uint8_t  _trigtype;
      bool     _triggered;
      uint32_t _interval;
      uint32_t _lastrun;

    // What should this thread run?
      void (*_prog)();
      #ifdef FullTThread
        std::function<void()> _callbackRunner;
      #endif

    // CPU Usage
      uint32_t _usagestart;
      uint32_t _runtime;
      uint32_t _calls;
      double   _cpuusage;

  public:
    // Routines
      TThread();

      bool init(void (*prog)());

      #ifdef FullTThread
        template <typename T>
        bool init(T* obj, void (T::*method)()) {
          _initialized = true;
          _callbackRunner = [=]() { (obj->*method)(); };
          _usagestart = millis();
          return true;
        }
      #endif

      bool run();
      bool run(bool withforce);
      bool calcUsage();

      String getName() const;
      bool   setName(const String& name);

    // Get Details
      uint8_t getPriority() const;
      uint8_t getStatus();
      bool    getPaused();
      bool    getPauseAllowed() const;

      uint8_t  getTrigType()           const;
      bool     getTriggered()          const;
      uint32_t getInterval()           const;
      uint32_t getLastRunMillis()      const;
      uint32_t getMillisSinceLastRun() const;

      uint32_t getUsageStartMillis()      const;
      uint32_t getMillisSinceUsageStart() const;
      uint32_t getRunTime()  const;
      uint32_t getCalls()    const;
      double   getCPUUsage() const;
      double   getCPUUsage(bool recalc);

    // Set/Work With Details
      bool setPriority(uint8_t priority);
      bool setStatus(uint8_t status);
      bool setPaused(bool paused);
      bool setPauseAllowed(bool pauseallowed);
      bool setTrigType(uint8_t trigtype);
      bool setTriggered(bool triggered);
      bool setInterval(uint32_t interval);
      bool setLastRunMillis(uint32_t lastrun);
      bool setUsageStartMillis(uint32_t usagestart);
};
#endif