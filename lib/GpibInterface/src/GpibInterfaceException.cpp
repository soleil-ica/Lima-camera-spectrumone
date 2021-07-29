#include "GpibInterfaceException.h"

using namespace GpibInterfaceLib;

/**
 * This class is used to handle GpibInterfaceExceptions.
 * The constructor parameters are:
 * Device name, origin, gpib error message, gpib ibsta string representation.
 */
GpibInterfaceException::GpibInterfaceException(std::string device_name,std::string origin,std::string err_msg,std::string sta_msg,int err,int sta)
{
    message = origin;
    iberrMessage = err_msg;
    ibstaMessage = sta_msg;
    dname = device_name;
    ib_sta = sta;
    ib_err = err;
}

/**
 * Get GpibInterface Class message / origin.
 */ 
std::string GpibInterfaceException::getMessage()
{
    return message;
}


/**
 * Get gpib library low level error message.
 */ 
std::string GpibInterfaceException::getiberrMessage()
{
    return iberrMessage;
}

/**
 * Get gpib ibsta in string form.
 */ 
std::string GpibInterfaceException::getibstaMessage()
{
    return ibstaMessage;
}

/**
 * Get device name on which exception occurs.
 */ 
std::string GpibInterfaceException::getDeviceName()
{
    return dname;
}

/**
 * Get iberr value from the device on which error occurs.
 */ 
int GpibInterfaceException::getErrorValue()
{
    return ib_err;
}

/**
 * Get ibsta value from the device on which error occurs.
 */ 
int GpibInterfaceException::getStateValue()
{
    return ib_sta;
}

