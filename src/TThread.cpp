#include "TThread.h"

uint8_t TThread_Priority; // Threads with a priority level less than this will not be allowed to run (overrides _pauseallowed)


const TSoftwareVersion TThread_Version(TTHREAD_V_MAJOR, TTHREAD_V_MINOR, TTHREAD_V_PATCH);


// Routines
  TThread::TThread() {
    _name = "";

    // Control
      _initialized  = false;
      _priority     = tp_HIGH;
      _status       = ts_NEVER;
      _paused       = false;
      _pauseallowed = true;
    
    // When should this tread run?
      _trigtype  = tt_NONE;
      _triggered = false;
      _interval  = 0;
      _lastrun   = 0;

      _prog = nullptr;

    // CPU Usage
      _runtime    = 0;
      _calls      = 0;
      _cpuusage   = 0;
  }
  

  bool TThread::init(void (*prog)()) {
    if (_initialized) { return false; };
    _initialized = true;
    _prog = prog;
    _usagestart = millis();
    return true;
  }


  bool TThread::run() { return run(false); }
  bool TThread::run(bool withforce) { 
    // Is there a reason we can't run the program?
    if (!_initialized || _status == ts_ERROR) { return false; };
    if (!withforce) {
      if (_priority < TThread_Priority) { return false; };
      if (getPaused())                  { return false; };
    } 
    
    // Checking trigger parameters
    if (withforce || _trigtype == tt_NONE ||
        ((_trigtype == tt_BOOL     || _trigtype == tt_BOOLR || _trigtype == tt_INTBOOL || _trigtype == tt_INTBOOLR) && _triggered) ||
        ((_trigtype == tt_INTERVAL ||                          _trigtype == tt_INTBOOL || _trigtype == tt_INTBOOLR) && getMillisSinceLastRun() >= _interval)) {
      
      // Run the program & track its cpu usage metrics
      _lastrun = millis();
      if (_usagestart == 0) { _usagestart = _lastrun; _runtime = 0; _calls = 0; _cpuusage = 0; };
      _status = ts_RUN;

    #if defined(ARDUINO_PORTENTA_H7_M7) ||  defined(ARDUINO_PORTENTA_H7_M4) || defined(ARDUINO_GIGA)
      if (_callbackRunner) { _callbackRunner(); }
      else
    #endif
      if (_prog) { _prog(); }; // THIS IS APART OF THAT ELSE RIGHT ABOVE, DONT OVERLOOK THAT JUST CUZ OF THAT #ENDIF

      _status = ts_WAIT;
      if (_trigtype != tt_BOOL && _trigtype != tt_INTBOOL) { _triggered = false; };
      _calls++;
      _runtime += (millis() - _lastrun);
      return true;
    };    

    return false; // if we've gotten here, we didn't run the program
  }


  bool TThread::calcUsage() {
    _cpuusage = 0;

    if (!_initialized) { return false; };

    if (_usagestart > 0) { _cpuusage = double(_runtime) / (double(getMillisSinceUsageStart())/100); };
    _calls = 0; _runtime = 0; _usagestart = millis();

    return true;
  }











String TThread::getName() const             { return _name; }
bool   TThread::setName(const String& name) { if (name.length() > 31) { return false; }; _name = name; return true; }


// Get Details
  uint8_t  TThread::getPriority()     const { return _priority; }
  uint8_t  TThread::getStatus()             { return _status; }
  bool     TThread::getPaused()             { return _paused; }
  bool     TThread::getPauseAllowed() const { return _pauseallowed; }

  uint8_t  TThread::getTrigType()           const { return _trigtype; }
  bool     TThread::getTriggered()          const { return _triggered; }
  uint32_t TThread::getInterval()           const { return _interval; }
  uint32_t TThread::getLastRunMillis()      const { return _lastrun; }
  uint32_t TThread::getMillisSinceLastRun() const{
    if (_lastrun == 0) { return _interval; };

    uint32_t ctime = millis();

    if (ctime < _lastrun) { 
      uint32_t biggestval = 0; biggestval--; // overflowing us backwards to dynamically get the largest value for a uin32_t
      return ctime + (biggestval - _lastrun);
    };

    return ctime - _lastrun;
  }

  uint32_t TThread::getUsageStartMillis()      const { return _usagestart; }
  uint32_t TThread::getMillisSinceUsageStart() const {
    if (_usagestart == 0) { return 0; };

    uint32_t ctime = millis();

    if (ctime < _usagestart) {
      uint32_t biggestval = 0; biggestval--; // overflowing us backwards to dynamically get the largest value for a uin32_t
      return ctime + (biggestval - _usagestart);
    }

    return ctime - _usagestart;
  }
  uint32_t TThread::getRunTime()       const { return _runtime; }
  uint32_t TThread::getCalls()         const { return _calls; }
  double   TThread::getCPUUsage()      const { return _cpuusage; }
  double   TThread::getCPUUsage(bool recalc) { if (recalc) { calcUsage(); }; return _cpuusage; }


// Set/Work With Details
  bool TThread::setPriority    (uint8_t priority)  { if (priority > tp_last)  { return false; }; _priority = priority; return true; }
  bool TThread::setStatus      (uint8_t status)    { if (status   > ts_last)  { return false; }; if (status == ts_PAUSE) { return setPaused(true); }; if (_status == ts_PAUSE) { if (!setPaused(false)) { return false; }; }; _status = status; return true; }
  bool TThread::setPaused      (bool paused)       { if (!_pauseallowed && paused) { if (_status == ts_PAUSE) { _status = ts_WAIT; }; _paused = false; return false; }; _paused = paused; _status = _paused ? ts_PAUSE : (_status == ts_PAUSE ? ts_WAIT : _status); return true; }
  bool TThread::setPauseAllowed(bool pauseallowed) { _pauseallowed = pauseallowed; if ((_paused || _status == ts_PAUSE) && !_pauseallowed) { setPaused(false); }; return true; }

  bool TThread::setTrigType        (uint8_t trigtype)    { if (trigtype > tt_last) { return false; }; _trigtype = trigtype; return true; }
  bool TThread::setTriggered       (bool triggered)      { _triggered  = triggered;  return true; }
  bool TThread::setInterval        (uint32_t interval)   { _interval   = interval;   return true; }
  bool TThread::setLastRunMillis   (uint32_t lastrun)    { _lastrun    = lastrun;    return true; }
  bool TThread::setUsageStartMillis(uint32_t usagestart) { _usagestart = usagestart; return true; }