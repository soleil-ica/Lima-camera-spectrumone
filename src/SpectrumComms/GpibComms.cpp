//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2021
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################

#include <iostream>
#include <sstream>
#include <string>
#include "SpectrumComms/GpibComms.h"


#include <windows.h>
#include "ni4882.h"    

#define EOTMODE               1     // Enable the END message
#define EOSMODE               0     // Disable the EOS mode

using namespace SpectrumComms;

/**
 * Standard GPIB errors strings.
 */
const std::string error_array[] = {
    "EDVR - A system call has failed. ibcnt/ibcntl will be set to the value of errno.",
    "ECIC - Your interface board needs to be controller-in-charge, but is not.",
    "ENOL - You have attempted to write data or command bytes, but there are no listeners currently addressed.",
    "EADR - The interface board has failed to address itself properly before starting an io operation.",
    "EARG - IOne or more arguments to the function call were invalid.",
    "ESAC - The interface board needs to be system controller, but is not.",
    "EABO - A read or write of data bytes has been aborted, possibly due to a timeout or reception of a device clear command.",
    "ENEB - The GPIB interface board does not exist, its driver is not loaded, or it is not configured properly.",
    "EDMA - DMA hardware problem",
    "Idx9 - Not defined",
    "EOIP - Function call can not proceed due to an asynchronous IO operation (ibrda(), ibwrta(), or ibcmda()) in progress.",
      "ECAP - Incapable of executing function call, due the GPIB board lacking the capability, or the capability being disabled in software.",
    "EFSO - File system error",
    "Idx13 - Index error invalid : error not listed",
    "EBUS - An attempt to write command bytes to the bus has timed out.",
    "ESTB - One or more serial poll status bytes have been lost. This can occur due to too many status bytes accumulating (through automatic serial polling) without being read.",
    "ESRQ - The serial poll request service line is stuck on. This can occur if a physical device on the bus requests service, but its GPIB address has not been opened (via ibdev() for example) by any process. Thus the automatic serial polling routines are unaware of the device's existence and will never serial poll it.",
    "Idx17 - Index error invalid : error not listed",
    "Idx18 - Index error invalid : error not listed",
    "Idx19 - Index error invalid : error not listed",
    "ETAB - The return buffer is full.",
    "ELCK - Address or board is locked.",
    "EARM - The ibnotify Callback failed to rearm.",
    "EHDL - The input handle is invalid.",
    "WCFG - Configuration warning",
    "Idx25 - Index error invalid : error not listed",
    "EWIP - Wait already in progress on input ud",
    "ERST - The event notification was cancelled due to a reset of the interface",
    "EPWR - The system or board has lost power or gone to standby",
};


/**
 * This is the second constructor for the GpibComms class.
 * The parameter passed to this constructor, is the device address 
 * configured on your device hardware. 
 * Warning: using this constructor assumes that the device name defined in
 * the gpib driver is the original one. Default gpib device name are
 * composed like this:  dev + address ->  hardware at address 4 will use
 * dev4 as defaultname. If you have change this name with ibconf, you will
 * have to use the previous constructor.
 * VERY IMPORTANT NOTES:
 * If method doesn't throw exception this not necessarly mean that the gpib
 * device exists. This mean that the name devX exists in the gpib driver !
 * This constructor assumes your gpib device to be on board 0 !
 * This constructor SHOULD NOT BE USE ANYMORE since it simply concat dev to
 * the address, to build the device name (store in driver). Instead of doing
 * GpibComms( 4 ) use GpibComms ("dev4") or, for THE BEST SOLUTION:
 * GpibComms( "dev4", "gpib0").
 *
 */

GpibComms::GpibComms(GpibConfig config):
    m_config(config),
    m_is_initialized(false)
{
}

void GpibComms::delay(int t) // les valeurs transmises doivent �tre compatibles avec ibtmo()
{
    int OldTmo;
    int i;
    ibask(0,0x0003,&OldTmo);  //ibaTMO
    ibtmo(0,t); 
    do 
    i=ibwait(0,TIMO);    
    while ((i & 0x4000)==0);
    ibtmo(0,OldTmo);
}


void GpibComms::connect() 
{
    m_is_initialized = false;
    m_buffer.capacity(DATA_SIZE);
   std::string ss;
   device_ID=-1;
   
    std::ostringstream os;
    os << m_config.gpib_address;

    //- Get Device by name + addr.
    device_name = "dev" + os.str();
    devAddr = m_config.gpib_address;

    //- Save board id.
    std::string boardname {"gpib0"};
    ss = boardname.substr(4);
    gpib_board = atoi( ss.c_str() );
    
    if ((gpib_board < 0) || (gpib_board> MAX_BOARD_INDEX) )
    {
        throw yat::Exception("Error occured while getting device Addr", "Board index is out of range - Value must be between 0 and 7", device_name, iberr);
    }
    /*
     *  The Initialization portion consists of initializing the bus and the
     *  GPIB interface board so that the GPIB board is Controller-In-Charge
     *  (CIC).
    */
//    SendIFC(gpib_board);

    //- by default EOT is true and EOS is false 
    resetState();

    device_ID = ibdev(gpib_board,  devAddr, NO_SAD, T10s , EOTMODE ,EOSMODE);
    saveState();

    if (dev_ibsta & ERR)
    {
        throw yat::Exception("Error occured while connecting to GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    }

    ibconfig(device_ID,IbcEOSrd,0);
    // ibwait(device_ID,0x100);

    // ibtmo(device_ID,T1s);
    // ibdma(0,1); //enable la DMA
    
    
    
    // ibrd(device_ID, m_buffer.base(), DATA_SIZE); //flush buffer

    unsigned char rst = 222;

    //ibwrt(device_ID,&rst,1L);  // reboote si en attente d'une entr�e
    //delay(T300ms);
    // ibtmo(device_ID,T3s);




    if ( device_ID == -1) 
    {
        throw yat::Exception("Error occured while connecting to GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    }

    m_is_initialized = true;
    //std::cout << "gpib_board=" << gpib_board<< ",device_ID=" << device_ID << ",devAddr=" << devAddr <<std::endl;

};

/**
 * ibsta register string conversion.
 * This method returns string describing ibsta register meaning.
 */
std::string GpibComms::ibstaToString() 
{
    std::string ret;
    
   if (dev_ibsta & ERR)  ret  = "GPIB error \n";
   if (dev_ibsta & TIMO) ret += "Time limit exceeded \n";
   if (dev_ibsta & END)  ret += "END or EOS detected \n";
   if (dev_ibsta & SRQI) ret += "SRQ Interrupt received \n";
   if (dev_ibsta & RQS)  ret += "Device requesting service \n";
   if (dev_ibsta & CMPL) ret += "I/O completed \n";
   if (dev_ibsta & LOK)  ret += "Lockout state \n";
   if (dev_ibsta & REM)  ret += "Remote state \n";
   if (dev_ibsta & CIC)  ret += "Controller-In-Charge \n";
   if (dev_ibsta & ATN)  ret += "Attention is asserted \n";
   if (dev_ibsta & TACS) ret += "Talker \n";
   if (dev_ibsta & LACS) ret += "Listener \n";
   if (dev_ibsta & DTAS) ret += "Device trigger state \n";
   if (dev_ibsta & DCAS) ret += "Device clear state \n";
   return ret;    
}

/**
 * ibserr register string conversion.
 * This method returns error string corresponding to err register.
 */
std::string GpibComms::iberrToString() {
   
    std::string ret("UNHANDLED ERROR ID !");
    
    if (dev_iberr>=0 && dev_iberr<=28) 
    {   
        return error_array[dev_iberr];
    }
    
    return ret;
}

/**
 * This method returns device's iberr register.
 */
int GpibComms::getiberr()
{
    return dev_iberr;
}

yat::Buffer<char>& GpibComms::getBuffer()
{
    return m_buffer;
}

/**
 * This method returns device's ibsta register.
 */
int GpibComms::getibsta()
{
    return dev_ibsta;
}

/**
 * This method returns device's ibcnt register.
 */
unsigned int GpibComms::getibcnt()
{
    return dev_ibcnt;
}    

/**
 * This method returns device's ID. This is usefull for controlling.
 * device from a gpib board. For example : Method BCclr needs this value
 * to clear a device.
 */
int GpibComms::getDeviceID()
{
    return device_ID;
}

/**
 * This method returns device's name. 
 * This mean, the name passed to the constructor as string, or the name 
 * built with the constructor using address e.g. dev4
 */
int GpibComms::getDeviceAddr()
{
    return devAddr;
}
/**
 * This method is for internal class use. 
 * GPIB iberr, ibsta and ibcnt are global to all devices. The GpibComms 
 * class will call this method after all NI488 / NI488.2 function call, 
 * to save a specific device state. This method is must not be used from
 * outside the GpibComms class.
 */
void GpibComms::saveState()
{
    dev_iberr = iberr;
    dev_ibsta = ibsta;
    dev_ibcnt = ibcntl;
    return;

#ifdef WIN32
    dev_iberr = ThreadIberr ();
    dev_ibsta = ThreadIbsta ();
    dev_ibcnt = ThreadIbcntl ();
#else
    dev_iberr = iberr;
    dev_ibsta = ibsta;
    dev_ibcnt = ibcntl;
#endif
}

/**
 * This method is for internal class use. 
 * GPIB iberr, ibsta and ibcnt are global to all devices. The GpibComms 
 * class will call this method before all NI488 / NI488.2 function call, 
 * to reset a specific device state. This method must not be used from
 * outside the GpibComms class.
 */
void GpibComms::resetState()
{
    dev_iberr = 0;
    dev_ibsta = 0;
    dev_ibcnt = 0;
}

/**
 * This method test if the device is alive on the bus.
 * If returns 0: nobody there, else ok.
 */
bool GpibComms::isAlive() {

    if (device_ID  >0)
    {
        int tmp = ibln( gpib_board , devAddr, NO_SAD, &alive);
        if (alive !=0)
            return true;
        else
            return false;
    }
    else
        return false;

}

/**
 * This method send a string to the encapsulated device. 
 * Return the number of data written. 
 */
void GpibComms::write(const std::string & m) 
{
      resetState();
    ibwrt(device_ID,(char *) m.c_str(),m.length() );  
    saveState();

    if (dev_ibsta & ERR)
    {
        throw yat::Exception("Error occured while writing to GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    } 
    
    //return dev_ibcnt;    /* Return saved incnt value */
}


void GpibComms::write_and_read(const std::string& argin, std::string & result, int size)
{
    write(argin);
    read(result, size);
}

/*
 * This method reads a string from the encapsulated device.
 * Read a specified size string from the device. Return the string readed. 
 * A byte read value is store in private int 'dev_ibcnt' and is valid 
 * until another call to a NI488 function is made. It should not be accessed
 * throught get_ibcnt, because string.length() already do that.
 */
void GpibComms::read(std::string & result, int size)
{
    read_raw(m_buffer, size);

    result.assign(m_buffer.base(), ibcntl);
}

int GpibComms::read_raw(yat::Buffer<char>& buff, int size)
{
    resetState();

    if(size <= 0 || size > buff.capacity()) size = buff.capacity();
    
    ibrd(device_ID, buff.base(), size);
    saveState();

    if (dev_ibsta & ERR)
    {
        throw yat::Exception("Error occured while reading to GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    }

    return ibcntl;
}

/******************************************************************************************/
/*
 *    These methods perform a write or a read of binary data on the GPIB device
 *
 *
/******************************************************************************************/
void GpibComms::sendData(const char *argin, long count)
{
    resetState();
    Send ( gpib_board , MakeAddr(devAddr, 0),(char *)argin, count, NLend);
    saveState();
    if (dev_ibsta & ERR)
    {
        throw yat::Exception("Error occured while writing binary data to GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    } 
}

char *GpibComms::receiveData(long count)
{
// allocate buffer for reading data on the GPIB bus
char* buffer = new char [count+1];

if( !buffer )
    {
        throw yat::Exception("Cannot allocate memory for receiveData", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    }
  
    memset(buffer,0, count+1);

    resetState();
    Receive ( gpib_board, MakeAddr(devAddr, 0), buffer, count, STOPend);
    // The actual number of bytes transferred is returned in the  variable, ibcntl.
    // we use ThreadIbcntl for thread safety
    saveState();

    if (dev_ibsta & ERR)
    {
        delete [] buffer;
        throw yat::Exception("Error occured while reading binary data from GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    } 

    //long bytes_read=ThreadIbcntl ();
    // CAUTION/TODO : If long bytes_read != count what should we do : 
    // a) throw an exception: but it may be too much to take such a decision at low level
    // b) reallocate a data buffer of the right size (long bytes_read), copy the buffer data into it, 
    // and return this buffer of the rigth size to the caller . this last opion is our prefered but 
    // for now we prefer to avoid this potential extracopy of data 
    // 

    return buffer;
}
/******************************************************************************************/

/**
 * This method sends a trigger signal to the gpib device. The device was
 * previously set to wait for a trigger command. When it receivces the signal,
 * the device sends its measurement. You can get it with read Command.
 */
void GpibComms::trigger()
{
    resetState();
    ibtrg(device_ID);  
    saveState();
    if (dev_ibsta & ERR)
    {
        throw yat::Exception("Error occured while triggering GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    } 
}

void GpibComms::wait_events(int mask)
{
    resetState();
    ibwait(device_ID, mask);
    saveState();
    if (dev_ibsta & ERR)
    {
        throw yat::Exception("Error occured while waiting for GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    } 

}


/**
 * Set the device time out value. 
 * Warning: These values are predefined. With GPIB you can only choose
 * between 16 differents time out values. #defined values can be found 
 * in GpibComms.h.
 */
void GpibComms::setTimeOut(int v)
{
    resetState();
    if ( (v >= 0) && (v <= 17) )
    {
        ibtmo(device_ID,v);  
        saveState();

    }
    else 
    {
        throw yat::Exception("Error occured while writing to GPIB", "Value out of range - [0-15] value expected", device_name, iberr);
    }

    // v contains correct value, does the cmd terminate correctly ?
    if ( dev_ibsta & ERR )
    {
        throw yat::Exception("Error occured while setting time out on GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    } 
}

/**
 * Clear a specified device.
 * This method sends a clear signal on the GPIB bus for a specified device.
 */
void GpibComms::clr() 
{
    resetState();
    ibclr( getDeviceID() );  
    saveState();
    if (dev_ibsta & ERR)
    {
        throw yat::Exception("Error occured while ibclr on GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
    } 
}

/**
 * Get a list of devices connected on the bus.
 * This method returns a reference on vector of GpibCommsInfo, containing 
 * information on all GpibComms found on the gpib bus. The referenced variable
 * is  just below. Provided info are device *IDN string, primary address,
 * secondary address. See GpibCommsInfo for more information on these fields.
 * Board must be CIC to perform FindLstn !
 */
std::vector<GpibCommsInfo>& GpibComms::getConnectedDeviceList()
{
  char idn_buffer[MAX_DEV_IDN_STR];
  int loop, nb_listener;
  Addr4882_t scanlist[MAX_DEV_ON_BOARD+1]; // +1 for terminal NOADDR
  Addr4882_t result[MAX_DEV_ON_BOARD];
  
  
  inf.clear(); // Empty vector first.
  // Build address list to scan, add NOADDR list terminator 
  for (loop = 0; loop < MAX_DEV_ON_BOARD; loop++)
    scanlist[loop] = loop;
  scanlist[MAX_DEV_ON_BOARD] = NOADDR;
  
  resetState();
  FindLstn(gpib_board, scanlist, result, MAX_DEV_ON_BOARD);
  saveState();
  
  if (dev_ibsta & ERR)
  {
    throw yat::Exception("Error occured while FindLstn on GPIB", iberrToString() + " - " + ibstaToString(), device_name, iberr);
  } 
  
  nb_listener = dev_ibcnt -1;//-1; MODIF
  // Start from 1 to avoid gpib board 0 who does not answer to "*IDN?"
  for (loop = 1; loop <= nb_listener; loop++ )    
  {
    GpibCommsInfo *t = new GpibCommsInfo();
    
    t->dev_pad = GetPAD( result[loop] );
    t->dev_sad = GetSAD( result[loop] );
    t->dev_idn = "Device does not support *IDN? command.\n";
    
    resetState();
    Send(gpib_board, result[loop],(char *)"*IDN?", 5L, NLend);
    saveState();
    if (! (dev_ibsta & ERR) )
    {
      memset(idn_buffer,0, MAX_DEV_IDN_STR);
      Receive(gpib_board, result[loop], idn_buffer, MAX_DEV_IDN_STR, STOPend);
      
      // Most of gpib device understand '*IDN?' command, and return a string
      // of identification. Some old device does implement this command, like
      // Tektronik 2440 who implements his own ID command: 'ID?'. On *IDN?
      // cmd the 2440 return char 255, as bad command.
      // Thats why if a device return an ID string < 5 byte, or finish in 
      //Time Out error, we admit that it does not implement command.
      if ( (!(dev_ibsta & ERR)) && (ibcnt > 5) ) // Ibcnt = nb of byte received.
      {
        t->dev_idn = idn_buffer;
      } 
    } 
    inf.push_back( *t );
  }
 
  return inf;
}


