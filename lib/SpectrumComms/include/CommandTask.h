#ifndef COMMANDTASK_H
#define COMMANDTASK_H

#include <yat/threading/Task.h>
#include <yat/threading/Utilities.h>
#include <yat/utils/String.h>
#include <yat/memory/UniquePtr.h>
#include <yat/utils/Callback.h>
#include <yat/utils/Logging.h>

#include "GpibComms.h"
#include "Commands.hpp"
#include "Common.h"
#include "CommandInterface.h"

#define CONNECT_SOCKET  (yat::FIRST_USER_MSG + 101)
#define INIT_SEQUENCE   (yat::FIRST_USER_MSG + 102)
#define SET_EXP_TIME    (yat::FIRST_USER_MSG + 103)
#define SET_GAIN        (yat::FIRST_USER_MSG + 104)
#define DEFINE_AREA     (yat::FIRST_USER_MSG + 105)
#define SNAP            (yat::FIRST_USER_MSG + 106)
#define GET_TEMPERATURE (yat::FIRST_USER_MSG + 107)
#define RE_CONFIG       (yat::FIRST_USER_MSG + 108)
#define PREPARE         (yat::FIRST_USER_MSG + 109)
#define SET_FLUSHES     (yat::FIRST_USER_MSG + 110)
#define GET_GAIN        (yat::FIRST_USER_MSG + 111)

namespace SpectrumComms
{
class GpibComms;


class CommandTask : public yat::Task
{
public:
    enum State {
        Init, Config, Idle, Fault, Busy,
    };

    struct CamData
    {
        bool modified;
        int x_origin;
        int y_origin;
        int x_size;
        int y_size;
        int x_bin;
        int y_bin;
        int num_flushes;
        int gain;
        int exp_time;
    };

    struct FrameInfo
    {
        int x_origin;
        int y_origin;
        int x_size;
        int y_size;
        int x_bin;
        int y_bin;
    };

    struct CommandConfig
    {
        bool invert_x;
        std::string expert_config;
        std::string tables_path;
        std::string tables_mode;
    };

    struct SnapInfo
    {
        int x_size;
        int y_size;
        void* buffer_ptr;
    };


    CommandTask(GpibComms::GpibConfig gpib_config, CommandConfig command_config, Listener* listener);
    virtual ~CommandTask();
    
    void connect();
    void init_sequence(const bool & force_config = false);
    void set_exp_time(const int & exp_time);
    void set_gain(const int & gain);
    void set_num_flushes(const int & num);
    void prepare(const FrameInfo & frame);
    void snap(void* buffer_ptr, const int & x_size, const int & y_size);
    void get_temperature();
    void get_gain();
    void re_config();


    State get_state();


protected:
    virtual void handle_message(yat::Message & msg);

private:
    // 
    CamData m_cam_data;

    // Shared:
    yat::Mutex m_data_lock;
    State m_state;

    // Used ONLY by task thread (no protection needed):
    CommandConfig m_config;
    CommandInterface m_interface;
    Listener* m_listener;
    int m_col_size;

    void send_table(const std::string & table, unsigned offset);
    static void get_table(const std::string & tab_name, std::vector<std::string> & output_vect);
    void reset(std::string & reply, const size_t & number_retries);
    double get_adc_temperature();
    void send_all_tables();
    void config_CCD();
    void get_config(const std::string & config_str, std::vector<std::string>& args);

    // CommandMessages.cpp:
    void t_connect_gpib();
    void t_init_sequence(const bool & force_config);
    void t_set_gain(const int & gain);
    void t_set_num_flushes(const int & gain);
    void t_set_exp_time(const int & exp_time);
    void t_prepare(const CamData & frame);
    void t_snap(const SnapInfo & frame);
    void t_get_temperature();
    void t_get_gain();
    void t_re_config();

    // State
    void set_state(State state)
    {
        yat::AutoMutex<> guard(m_data_lock);
        m_state = state; 
    }

}; // Class


}
#endif  // COMMANDTASK_H