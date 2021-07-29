#include "GpibInterface/GpibInterfaceException.h"

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
std::string GpibInterfaceException::getMessage() const
{
    return message;
}


/**
 * Get gpib library low level error message.
 */ 
std::string GpibInterfaceException::getiberrMessage() const
{
    return iberrMessage;
}

/**
 * Get gpib ibsta in string form.
 */ 
std::string GpibInterfaceException::getibstaMessage() const
{
    return ibstaMessage;
}

/**
 * Get device name on which exception occurs.
 */ 
std::string GpibInterfaceException::getDeviceName() const
{
    return dname;
}

/**
 * Get iberr value from the device on which error occurs.
 */ 
int GpibInterfaceException::getErrorValue() const
{
    return ib_err;
}

/**
 * Get ibsta value from the device on which error occurs.
 */ 
int GpibInterfaceException::getStateValue() const
{
    return ib_sta;
}

