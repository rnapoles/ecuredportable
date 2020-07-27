/**
  @file
  @author Stefan Frings
*/

#ifndef STARTUP_H
#define STARTUP_H

#include <QtCore/QCoreApplication>
#include "qtservice.h"
#include <QApplication>

/**
  Helper class to install and run the application as a windows
  service.
*/
class Startup : public QtService<QApplication>
{
public:

    /** Constructor */
    Startup(int argc, char *argv[]);
    ~Startup();

protected:

    /** Start the service */
    void start();

    /**
      Stop the service gracefully.
      @warning This method is not called when the program terminates
      abnormally, e.g. after a fatal error, or when killed from outside.
    */
    void stop();
private:


};

#endif // STARTUP_H
