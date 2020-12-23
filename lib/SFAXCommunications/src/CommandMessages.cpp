#include "CommandTask.h"

using namespace SFAXCommunications;

void CommandTask::handle_message(yat::Message& msg)
{
    switch(msg.type())
    {
        case yat::TASK_INIT:
        YAT_INFO << "CommandTask::handle_message: TASK_INIT" << std::endl;
        t_connect_gpib();
        break;

        case yat::TASK_PERIODIC:
        YAT_INFO << "CommandTask::handle_message: TASK_PERIODIC" << std::endl;
        break;

        case yat::TASK_EXIT:
        YAT_INFO << "CommandTask::handle_message: TASK_EXIT" << std::endl;
        m_comms->disconnect();
        break;

        case INIT_SEQUENCE:
        YAT_INFO << "CommandTask::handle_message: INIT_SEQUENCE" << std::endl;
        t_init_sequence(msg.get_data<bool>());
        break;

        case SET_EXP_TIME:
        YAT_INFO << "CommandTask::handle_message: SET_EXP_TIME" << std::endl;
        t_set_exp_time(msg.get_data<int>());
        break;

        case SET_GAIN:
        YAT_INFO << "CommandTask::handle_message: SET_GAIN" << std::endl;
        t_set_gain(msg.get_data<int>());
        break;

        case SNAP:
        YAT_INFO << "CommandTask::handle_message: SNAP" << std::endl;
        t_snap();
        break;

        default:
        YAT_INFO << "CommandTask::handle_message: UNKNOWN" << std::endl;
        report_error("CommandTask::handle_message UNKNOWN message received!\n");
        set_state(State::Fault);
    }
}

void CommandTask::t_connect_gpib()
{
    set_state(State::Init);
    try
    {
        try
        {
            yat::Thread::sleep(500);
            m_comms->connect();
        }
        catch(const yat::Exception & ex)
        {
            m_comms->disconnect();
            yat::Thread::sleep(5000);
            m_comms->connect();
        }

        report_info("CONNECT SOCKET: Successful socket connexion!\n");
    }
    catch(const yat::Exception & ex)
    {
        report_fatal("CONNECT SOCKET: Failed to connect socket!\n");
        report_error(ex.to_string());
        set_state(State::Fault);
        return;
    }
    try
    {
        m_comms->gpib_init();
        report_info("CONNECT SOCKET: Successful GPIB initialization!\n");
    }
    catch(const yat::Exception & ex)
    {
        report_fatal("CONNECT SOCKET: Failed to initialize Gpib!\n");
        report_error(ex.to_string());
        set_state(State::Fault);
    }
    yat::Thread::sleep(500); // Waiting before another message
}

void CommandTask::t_init_sequence(const bool & force_config)
{
    bool config_overwrite = force_config;
    if(get_state() == State::Fault)
    {
        report_error("INIT SEQUENCE: Can not launch sequence, fault state!\n");
        return;
    }
    try
    {
        std::string result;

        set_state(State::Init);

        report_info("INIT SEQUENCE: Connecting to Camera...\n");

        reset(result, 5);

        report_info("INIT SEQUENCE: Camera found!\n");

        if(IS_EQUAL(result, "B"))
        {
            config_overwrite = true;
            result = command_and_read(CCD_START_MAIN_PROGRAM, true);
            result = command_and_read(CCD_WHERE_AM_I);
        }
        if(IS_EQUAL(result, "F"))
        {
            result = command_and_read(CCD_INIT);
            if(IS_EQUAL(result, "o1\r"))
            {
                report_info("INIT SEQUENCE: Hardware successfully detected!\n");

            }
            else if(IS_EQUAL(result, "o0\r"))
            {
                report_info("INIT SEQUENCE: No hardware detected: Emulation Mode!\n");
            }
            else
            {
                report_error("INIT SEQUENCE: Failed to init! Try restarting the camera!\n");
                set_state(State::Fault);
                return;
            }
        }
        else
        {
            THROW_EXCEPTION(
                    "UNKNOWN RESPONSE",
                    "Received: " + result,
                    "INIT SEQUENCE");
        }
        if(config_overwrite)
        {
            config_CCD();
        }
        set_state(State::Idle);

        // result = command_and_read(READ_CONFIG);
        // result = command_and_read(READ_TEMP);
        // std::cout << "TEMP: " << read_adc_temperature() << std::endl;
        // result = command_and_read(GET_GAIN);
        // result = command_and_read(GET_DATA_SIZE);
    }
    catch(const yat::Exception & ex)
    {
        report_error("INIT SEQUENCE: Failed to init!\n");
        report_error(ex.to_string());
        set_state(State::Fault);
    }
}

void CommandTask::t_set_exp_time(const int & exp_time)
{
    // TODO: return
    return;
    if(get_state() != State::Idle)
    {
        report_error("SET EXP TIME: Invalid state!\n");
        return;
    }
    set_state(State::Config);

    std::vector<std::string> args;
    args.reserve(1);
    args.push_back(TO_STRING(exp_time));

    try
    {
        command_and_read(CCD_SET_EXP_TIME, &args, true);
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        report_error("SET EXP TIME: Failed to write exposition time!\n");
        report_error(ex.to_string());
        set_state(State::Fault);
    }

}

void CommandTask::t_set_gain(const int & gain)
{
    if(get_state() != State::Idle)
    {
        report_error("SET GAIN: Invalid state!\n");
        return;
    }
    set_state(State::Config);

    std::vector<std::string> args;
    args.reserve(1);
    args.push_back(TO_STRING(gain));

    try
    {
        command_and_read(CCD_SET_GAIN, &args, true);
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        report_error("SET GAIN: Failed to write exposition time!\n");
        report_error(ex.to_string());
        set_state(State::Fault);
    }

}

void CommandTask::t_snap()
{
    if(get_state() != State::Idle)
    {
        report_error("SNAP: Invalid state!\n");
        return;
    }
    set_state(State::Busy);

    std::vector<std::string> args;
    std::string result;
    args.resize(1);

    try
    {
        // Set gain 1
        // args[0] = "1";
        // command_and_read(CCD_SET_GAIN, &args, true);

        // Read gain
        command_and_read(CCD_READ_GAIN);

        // close shutter
        args[0] = "0";
        command_and_read(CCD_SET_SHUTTER, &args, true);

        // Stop acquisition
        command_and_read(CCD_STOP_ACQ, true);

        // Set acq time
        args[0] = "100";
        command_and_read(CCD_SET_EXP_TIME, &args, true);

        // Set numb flushes
        args[0] = "2";
        command_and_read(CCD_SET_FLUSHES, &args, true);

        args.resize(2);
        args[0] = "0";
        args[1] = "1";
        command_and_read(CCD_DEFINE_FORMAT, &args, true);

        args.resize(7);
        args[0] = "0";
        args[1] = "0";
        args[2] = "0";
        args[3] = "2000";
        args[4] = "800";
        args[5] = "1";
        args[6] = "1";
        command_and_read(CCD_DEFINE_AREA, &args, true);


        result = command_and_read(CCD_GET_DATA_SIZE);

        result = command_and_read(CCD_STATUS);

        // if status = 0






        args.resize(1);
        args[0] = "1";
        command_and_read(CCD_START, &args, true);

        std::vector<std::string> image;

        do
        {
            yat::Thread::sleep(800);
            result = command_and_read(CCD_STATUS);

        }while(IS_NOT_EQUAL(result, "o0\r"));

        command_and_read(CCD_RESET_IMAGE, true);

        std::string image_as_str = "";

        m_comms->gpib_write(CCD_READ_IMAGE.str(0));

        size_t count = 0;

        // YAT_ERROR << "BEFORE SLEEP" << count << std::endl;

        // yat::Thread::sleep(100000);

        // YAT_ERROR << "AFTER SLEEP" << count << std::endl;

        while(m_comms->gpib_read_all(image_as_str))
        {
            count += image_as_str.size();
            YAT_ERROR << "Scount =" << count << std::endl;
            //yat::Thread::sleep(500);
        }


        m_comms->gpib_flush(result, true);
        m_comms->gpib_flush(result, true);




        set_state(State::Idle);

        report_info("SNAP: Qca ok!\n");

        YAT_ERROR << "SNap ok size received =" << count << std::endl;

        return;

        yat::Thread::sleep(5000);

        result = command_and_read(CCD_STATUS);

        yat::Thread::sleep(5000);

        result = command_and_read(CCD_STATUS);

        command_and_read(CCD_RESET_IMAGE, true);

        //result = command_and_flush(CCD_READ_IMAGE, true);

        report_info("SNAP: Qca ok!\n");

        //m_comms->gpib_flush(result);
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        report_error("SNAP: Failed!\n");
        report_error(ex.to_string());
        set_state(State::Fault);
    }
}