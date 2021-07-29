#ifndef GPIBINTERFACEEXCEPTION_H
#define GPIBINTERFACEEXCEPTION_H

#include <string>



namespace GpibInterfaceLib
{

/**
 *This class is used to handle GpibInterfaceExceptions.
 */
class GpibInterfaceException {
    
    public:
    
    GpibInterfaceException(std::string device_name, std::string origin, std::string err_msg, std::string sta_msg, int err, int sta);
    std::string getMessage();
    std::string getiberrMessage();
    std::string getibstaMessage();
    std::string getDeviceName();
    int getErrorValue();
    int getStateValue();
    protected:
    
    private:
    
    std::string dname;
    std::string message;
    std::string iberrMessage;
    std::string ibstaMessage;
    int ib_err;
    int ib_sta;
};
}

#endif
