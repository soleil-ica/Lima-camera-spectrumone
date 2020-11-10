#include "CommandTask.h"

using namespace SFAXCommunications;

void CommandTask::handle_message(yat::Message& msg)
{
    switch(msg.type())
    {
        case yat::TASK_INIT:
        case yat::TASK_EXIT:
        case yat::TASK_PERIODIC:
        break;

        case CONNECT_SOCKET:
        YAT_INFO << "CommandTask::handle_message: CONNECT_SOCKET" << std::endl;
        t_connect_gpib();
        break;

        case INIT_SEQUENCE:
        YAT_INFO << "CommandTask::handle_message: INIT_SEQUENCE" << std::endl;
        t_init_sequence(msg.get_data<bool>());
        break;

        default:
        YAT_INFO << "CommandTask::handle_message: UNKNOWN" << std::endl;
        report_error("CommandTask::handle_message UNKNOWN message received!\n");
        STATE(State::Fault);
    }
}

void CommandTask::t_connect_gpib()
{
    STATE(State::Init);
    try
    {
        m_comms->connect();
        report_info("CONNECT SOCKET: Successful socket connexion!\n");
    }
    catch(const yat::Exception & ex)
    {
        report_fatal("CONNECT SOCKET: Failed to connect socket!\n");
        report_error(ex.to_string());
        STATE(State::Fault);
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
        STATE(State::Fault);
    }
    yat::Thread::sleep(500); // Waiting before another message
}

void CommandTask::t_init_sequence(bool force_config)
{
    try
    {
        std::string result;

        STATE(State::Init);

        report_info("INIT SEQUENCE: Connecting to SFAX...\n");

        result = reset(5);

        report_info("INIT SEQUENCE: SFAX found!\n");

        if(IS_EQUAL(result, "B"))
        {
            result = command_and_read(START_MAIN_PROGRAM);
            if(START_MAIN_PROGRAM.is_ack(result))
            {
                //config_CCD();
                result = command_and_read(WHERE_AM_I);
            }
            else
            {
                THROW_EXCEPTION(
                    "NOT ACKNOWLEDGED",
                    "Received: " + result,
                    "INIT SEQUENCE");
            }
        }
        else if(force_config)
        {
            config_CCD();
            result = command_and_read(WHERE_AM_I);
        }

        if(IS_EQUAL(result, "F"))
        {
            result = command_and_read(CCD_INIT);
            if(IS_EQUAL(result, "o1\r"))
            {
                report_info("INIT SEQUENCE: Camera is ready! Hardware successfully detected!\n");
                STATE(State::Idle);
            }
            else if(IS_EQUAL(result, "o0\r"))
            {
                report_info("INIT SEQUENCE: Camera is ready! No hardware detected: Emulation Mode!\n");
                STATE(State::Idle);
            }
            else
            {
                report_error("INIT SEQUENCE: Failed to init! Try restarting the camera!\n");
                STATE(State::Fault);
                return;
            }
        }
        else
        {
            THROW_EXCEPTION(
                    "NOT ACKNOWLEDGED",
                    "Received: " + result,
                    "INIT SEQUENCE");
        }

        

        result = command_and_read(READ_CONFIG);
        result = command_and_read(READ_TEMP);
        std::cout << "TEMP: " << read_adc_temperature() << std::endl;
        result = command_and_read(GET_GAIN);
        result = command_and_read(GET_DATA_SIZE);
    }
    catch(const yat::Exception & ex)
    {
        report_error("INIT SEQUENCE: Failed to init!\n");
        report_error(ex.to_string());
        STATE(State::Fault);
    }
}

