#ifndef GPIBINTERFACE_H
#define GPIBINTERFACE_H

#include <string>
#include <vector>
#include <yat/Exception.h>
#include <yat/memory/DataBuffer.h>
#include <yat/threading/Thread.h>

/**
* Device's default size buffer for read operations.
*/
#define RD_BUFFER_SIZE         128

/**
* Drivers are by default limited to 8 gpibBoard per driver.
*/
#define MAX_BOARD_INDEX        7

/**
 * Maximum size of string received, when identifying devices connected on
 * the bus.( string return by device in answer to "*IDN?"
 */
#define MAX_DEV_IDN_STR        128

/**
 * Maximum number of device on the GPIB bus. 
 * This is used to limit bus scan with getConnectedDeviceList method.
 */
#define MAX_DEV_ON_BOARD    30

#define DATA_SIZE 2048

namespace SpectrumComms
{
/**
 * This class is a container for Device Info.
 * No methods are implemented, it only feature public fields. This class
 * is mainly used with getConnectedDeviceList method.
 */
class GpibCommsInfo {

public:
   virtual ~GpibCommsInfo(){};
  /**
     * This string contains device's answer to "*IDN?"
     */
   std::string dev_idn;        

  /**
     * This int contains device's Primary ADdress.
     */
     int    dev_pad;        

  /**
     * This int contains device's Secondary ADdress.
     */
     int dev_sad;        
};


/**
 * Static vector of GpibCommsInfo.
 * This var is declared here to allow getConnectedDeviceList method to
 * return reference on object. User can avoid his own vector<GpibCommsInfo>
 * declaration.
 */
static std::vector<GpibCommsInfo> inf;


/** 
* This class is designed to handle GpibCommss. It's point of
* view is very device oriented: For example, setting device in remote mode, is
* done with 'goToRemoteMode' device's method. It totaly hides the fact that this
* is done by sending a command to gpib board. Due to ESRF gpib bus usage, this
* class is designed to work on a single CIC (Controler In Charge) bus. In simple
* word this class can't handle multi board buses. The default CIC selected is
* "gpib0".
*
* The usual way to include gpib feature into a Tango device server consists in
* single or multiple instance of a GpibComms / gpibBoard.
* Once simply use read / write functions to handle devices. Note that All 
* operations made on GpibComms or gpibBoard can potentialy throw a 
* GpibCommsException.<link ref ="code example" href "code-example.txt"> 
*/
class GpibComms {

public:

    struct GpibConfig
    {
        std::string host; // Gpib controler Host (Ip address or hostname)
        size_t      port; // Gpib controler Port
        size_t      gpib_address; // Adress of the device to reach on the Gpib Bus
    };

    GpibComms(GpibConfig config);

  void connect();    // class constructor.
  void disconnect() {} //noop
  void init() {} // noop


  std::string    ibstaToString();                            // Get string from ibsta string.    
  std::string    iberrToString();                            // Get string from iberr string.     
  int        getiberr();                                        // Get device iberr value.        
  int        getibsta();                                        // Get device ibsta value.        
  unsigned int getibcnt();                            // Get device ibcnt value.        
  int        getDeviceID();                                  // Get internal device ID.
  int        getDeviceAddr();                              // Get device gpib address;
 void write(const std::string & argin);                                  // Send a string to a gpib device.     
  void read(std::string & result, int size = 0);        // Read a std::string from a gpib device.     
  int read_raw(yat::Buffer<char>& buff, int size = 0);
void write_and_read(const std::string& argin, std::string & result, int size = 0);                    // Perform a write/read operation in a row.


  char*    receiveData(long count);                // Read binary data from a GPIB device
  void    sendData(const char *, long count);                // Write binary data on a GPIB device
  void      wait_events(int mask);

  void    setTimeOut(int tmo);                        // Set Device Time out.
  bool    isAlive();                                        // Check the presence of the device on the bus.
  void    trigger();                                        // Trigger the device.        
    void    clr(void);                                        // Clear specified device.    (Board command).
    yat::Buffer<char>& getBuffer();
  std::vector<GpibCommsInfo>& getConnectedDeviceList();    //- List of connected devices

private:
  void    saveState();                                    // save iberr/ibstat in dev_ibsta/dev_iberr. 
  void    resetState();                                    // reset iberr/ibstat in dev_ibsta/dev_iberr.

  void delay(int t);
  /** 
  * Internal gpib handler.
  */
  int device_ID;        

  /**
  * gpib PAD return by driver at ibask(device_ID,IbaPAD,...).
  */
  int devAddr;        

  /**
  * Internal gpib ibsta copy.
  */
  int dev_ibsta;            

  /**
  * Internal gpib iberr copy.
  */
  int dev_iberr;            

  /**
  * Internal gpib ibcnt copy.
  */
  unsigned int dev_ibcnt;            

  /**
  * gpib device name passed in constructor, or built on address e.g dev4
  */
  std::string device_name;         

  /**
  * Status Byte value, polled by ReadStatusByte funct.
  */
  short alive;

  /**
  * This is the gpib board, where our device is connected to.
  */
  int gpib_board;    

  GpibConfig m_config;
  bool m_is_initialized;

  yat::Buffer<char> m_buffer;
};
}
#endif
