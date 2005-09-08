/**
 * @file logger.h
 * A simple file and console logger.
 *
 * Logs messages with different log levels, writes output to console and / or
 * file. You can use printf-like formatting of your message and do not have to
 * worry about whether to put a linebreak at the end of it or not. Different
 * options increase the amount of output made. The logger is also thread-safe.
 *
 * @author Matthias Liebig
 */

/**
 * @defgroup utils Utilities
 * Utility collection
 * @}
 */

/**
 * @defgroup logging Logging
 * @ingroup utils
 * @{
 */

#ifndef HLOGGER_USED
#define HLOGGER_USED

/**
 * A log level is meant for filtering and prioritising log messages. A higher
 * log level is more likely to be displayed.
 */
typedef enum {
	/** 
	 * LOG_DEBUG is for developers only and may contain internal variable 
	 * values or function names.
	 */
	LOG_DEBUG,
	/**
	 * LOG_INFO is meant for advanced users or developers that intend to use, 
	 * not debug the module. More information are given, for example some 
	 * statistics like a peak number of active threads or the display of
	 * loaded configuration data.
	 */
	LOG_INFO,
	/**
	 * LOG_MESSAGE is used for standard output messages a user is supposed to 
	 * see. For example that the server initialization is finished.
	 */
	LOG_MESSAGE,
	/**
	 * LOG_WARNING should be used if a minor error occurs that can be 
	 * automatically corrected but may lead to unintended behaviour. Another
	 * example is missing configuration data, but defaults may work also.
	 */
	LOG_WARNING,
	/**
	 * LOG_ERROR is used if an action can not be performed because of wrong
	 * inputs, external causes etc. Other non-dependant actions must not be
	 * affected.
	 */
	LOG_ERROR,
	/**
	 * LOG_FAILURE is used when a halt of the program is imminent. Even 
	 * non-dependant actions can not be guaranteed to work.
	 */
	LOG_FAILURE
} loglevel;

/**
 * Initialize logging. Create lock, reserve memory and read configuration 
 * data. Call this before calling any other function of the logger.
 * @return if initialization was successfull (boolean)
 */
int logger_init();

/**
 * Writes a message to the log. If it is written to console or file is defined
 * by the configuration in the same manner detail levels and timestamp use are. 
 * Formatting known from printf is supported.
 * @param lvl the loglevel for this message
 * @param fmt the format string or plain message
 * @param ... an optional argument list
 */
void log_message(loglevel lvl, const char *fmt, ...);

/**
 * Destroy lock and free memory. Call this function when you are done using
 * logging.
 * @return if releasing logging was successfull (boolean)
 */
int logger_destroy();

#endif

/**
 * @}
 */
