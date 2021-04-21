#include "CommandTask.h"

using namespace SpectrumComms;

void CommandTask::handle_message(yat::Message& msg)
{
    switch(msg.type())
    {
        case yat::TASK_INIT:
        YAT_INFO << "CommandTask::handle_message: TASK_INIT" << std::endl;
        break;

        case yat::TASK_PERIODIC:
        YAT_INFO << "CommandTask::handle_message: TASK_PERIODIC" << std::endl;
        break;

        case yat::TASK_EXIT:
        YAT_INFO << "CommandTask::handle_message: TASK_EXIT" << std::endl;
        m_interface.m_comms.disconnect();
        break;

        case CONNECT_SOCKET:
        YAT_INFO << "CommandTask::handle_message: TASK_INIT" << std::endl;
        t_connect_gpib();
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

        case SET_FLUSHES:
        YAT_INFO << "CommandTask::handle_message: SET_FLUSHES" << std::endl;
        t_set_num_flushes(msg.get_data<int>());
        break;

        case PREPARE:
        YAT_INFO << "CommandTask::handle_message: PREPARE" << std::endl;
        t_prepare(msg.get_data<CamData>());
        break;

        case SNAP:
        YAT_INFO << "CommandTask::handle_message: SNAP" << std::endl;
        t_snap(msg.get_data<SnapInfo>());
        break;

        case GET_TEMPERATURE:
        YAT_INFO << "CommandTask::handle_message: GET_TEMPERATURE" << std::endl;
        t_get_temperature();
        break;

        case GET_GAIN:
        YAT_INFO << "CommandTask::handle_message: GET_GAIN" << std::endl;
        t_get_gain();
        break;

        case RE_CONFIG:
        YAT_INFO << "CommandTask::handle_message: RE_CONFIG" << std::endl;
        t_re_config();
        break;

        default:
        YAT_INFO << "CommandTask::handle_message: UNKNOWN" << std::endl;
        m_interface.report_error("CommandTask::handle_message UNKNOWN message received!\n");
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
            m_interface.m_comms.connect();
        }
        catch(const yat::Exception & ex)
        {
            m_interface.m_comms.disconnect();
            //yat::Thread::sleep(5000);
            m_interface.m_comms.connect();
        }

        m_interface.report_info("CONNECT SOCKET: Successful socket connexion!\n");
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_fatal("CONNECT SOCKET: Failed to connect socket!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
        return;
    }
    try
    {
        m_interface.m_comms.gpib_init();
        m_interface.report_info("CONNECT SOCKET: Successful GPIB initialization!\n");
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_fatal("CONNECT SOCKET: Failed to initialize Gpib!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
    yat::Thread::sleep(500); // Waiting before another message
}

void CommandTask::t_init_sequence(const bool & force_config)
{
    bool config_overwrite = force_config;
    if(get_state() == State::Fault)
    {
        // The error leading to the fault was already reported
        //m_interface.report_error("INIT SEQUENCE: Can not launch sequence, fault state!\n");
        return;
    }
    try
    {
        std::string result;

        set_state(State::Init);

        m_interface.report_info("INIT SEQUENCE: Connecting to Camera...\n");

        reset(result, 5);

        m_interface.report_info("INIT SEQUENCE: Camera found!\n");

        if(IS_EQUAL(result, "B"))
        {
            config_overwrite = true;
            result = m_interface.command_and_read(CCD_START_MAIN_PROGRAM, true);
            result = m_interface.command_and_read(CCD_WHERE_AM_I);
        }
        if(IS_EQUAL(result, "F"))
        {
            result = m_interface.command_and_read(CCD_INIT);
            if(IS_EQUAL(result, "o1\r"))
            {
                m_interface.report_info("INIT SEQUENCE: Hardware successfully detected!\n");

            }
            else if(IS_EQUAL(result, "o0\r"))
            {
                m_interface.report_info("INIT SEQUENCE: No hardware detected: Emulation Mode!\n");
            }
            else
            {
                m_interface.report_error("INIT SEQUENCE: Failed to init! Try restarting the camera!\n");
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
            send_all_tables();
            yat::Thread::sleep(2000);
            config_CCD();
        }
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("INIT SEQUENCE: Failed to init!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
}

void CommandTask::t_set_exp_time(const int & exp_time)
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("SET EXP TIME: Invalid state!\n");
        return;
    }
    set_state(State::Config);

    std::vector<std::string> args(1, TO_STRING(exp_time));

    try
    {
        m_interface.command_and_read(CCD_SET_EXP_TIME, &args, true);
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("SET EXP TIME: Failed to write exposition time!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }

}


void CommandTask::t_set_num_flushes(const int & num_flushes)
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("SET NUM FLUSHES: Invalid state!\n");
        return;
    }
    set_state(State::Config);

    std::vector<std::string> args(1, TO_STRING(num_flushes));

    try
    {
        m_interface.command_and_read(CCD_SET_FLUSHES, &args, true);
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("SET NUM FLUSHES: Failed to write exposition time!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }

}

void CommandTask::t_set_gain(const int & gain)
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("SET GAIN: Invalid state!\n");
        return;
    }
    set_state(State::Config);

    std::vector<std::string> args(1, TO_STRING(gain));

    try
    {
        m_interface.command_and_read(CCD_SET_GAIN, &args, true);
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("SET GAIN: Failed to write exposition time!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
}

void CommandTask::t_prepare(const CamData & data)
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("SNAP: Invalid state!\n");
        return;
    }
    set_state(State::Config);
    
    std::string result;
    std::vector<std::string> split_result;
    std::vector<std::string> args;

    try
    {
        // args.resize(1);
        // args[0] = "0";
        // m_interface.command_and_read(CCD_SET_SHUTTER, &args, true);

        // m_interface.command_and_read(CCD_STOP_ACQ);

        // args[0] = TO_STRING(data.exp_time);
        // m_interface.command_and_read(CCD_SET_EXP_TIME, &args, true);

        // args[0] = TO_STRING(data.num_flushes);
        // m_interface.command_and_read(CCD_SET_FLUSHES, &args, true);

        // Define format
        // args.resize(2);
        // args[0] = "0";
        // args[1] = "1";
        // m_interface.command_and_read(CCD_DEFINE_FORMAT, &args, true);

        args.resize(7);
        args[0] = "0";
        args[1] = TO_STRING(data.x_origin);
        args[2] = TO_STRING(data.y_origin);
        args[3] = TO_STRING(data.x_size);
        args[4] = TO_STRING(data.y_size);
        args[5] = TO_STRING(data.x_bin);
        args[6] = TO_STRING(data.y_bin);
        m_interface.command_and_read(CCD_DEFINE_AREA, &args, true);

        result = m_interface.command_and_read(CCD_GET_DATA_SIZE);
        yat::StringUtil::split(result, ',', &split_result, true);
        std::istringstream(split_result[0].substr(1)) >> m_col_size;
        m_col_size+=2;
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("PREPARE: Failed to get data size!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
}

void CommandTask::t_snap(const SnapInfo & frame)
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("SNAP: Invalid state!\n");
        return;
    }
    set_state(State::Busy);

    
    std::string result;
    size_t count;
    int retries;
    std::vector<std::string> args(1, "");

    // {
    //     unsigned short *ptr =  (unsigned short *)frame.buffer_ptr;

    //     count = 2; // Offset of 1 because "o"
    //     unsigned short value = 65535;
    //     for(size_t y=0; y<frame.y_size; y++)
    //     {
    //         for(size_t x=0; x<frame.x_size; x++)
    //         {
    //             *ptr = value;
    //             count+=2;
    //             ptr++;
    //         }
    //         count += (m_col_size - frame.x_size) * 2;
    //         value-=10;
    //     }
    //     if(m_listener) m_listener->on_buffer_filled();
    //     set_state(State::Idle);
    //     return;
    // }

    try
    {
        m_interface.report_info("Configuring acquisition...");
        yat::Buffer<unsigned char> buff(4000000);

        // // close shutter
        // args[0] = "0";
        // command_and_read(CCD_SET_SHUTTER, &args, true);

        // // Stop acquisition
        // command_and_read(CCD_STOP_ACQ, true);

        // // // Set numb flushes
        // args[0] = "2";
        // command_and_read(CCD_SET_FLUSHES, &args, true);

        result = m_interface.command_and_read(CCD_STATUS);

        m_interface.report_info("Starting acquisition!");
        args[0] = "1";
        m_interface.command_and_read(CCD_START, &args, true);


        do
        {
            yat::Thread::sleep(800);
            result = m_interface.command_and_read(CCD_STATUS);

            // TODO
            // Break after a number of tries

        }while(IS_NOT_EQUAL(result, "o0\r"));

        m_interface.report_info("Acquisition over, loading data...");

        m_interface.command_and_read(CCD_RESET_IMAGE);


        m_interface.m_comms.gpib_write(CCD_READ_IMAGE.str(0), false);

        // return;
        bool force = true;
        size_t expected_size = ((m_col_size)*frame.y_size)*2;
        //expected_size = 3208000;
        YAT_ERROR << "expected size =" << expected_size << std::endl;

        count = 0;
        retries = 0;

        while(true){
            try{ 
                m_interface.m_comms.gpib_read_raw(force);
                unsigned char* dta = reinterpret_cast<unsigned char*>(m_interface.m_comms.getBuffer().base());
                buff.memcpy(dta, m_interface.m_comms.getBuffer().length(), count);
                count += m_interface.m_comms.getBuffer().length();

                //report_info("Acquisition over, loading data... (" + TO_STRING(count) + " bytes loaded)");

                if(force) force = false;
                //YAT_ERROR << "size received =" << count << std::endl;

                if(buff.size()-2 == expected_size) break;
            }
            catch(const yat::Exception& e)
            {
                YAT_ERROR << "size received =" << count << std::endl;
                force = true;
                retries++;
                if(retries>3)
                {
                    THROW_EXCEPTION(
                        "FAILED",
                        "Didn't receive expected snap data after 5 retries!",
                        "CommandTask::SNAP");
                }
            }
        }

        yat::File data_file("/home/informatique/ica/ica/ah/output.dat");
        std::string giga;

        for(size_t r=0; r<buff.size(); r++)
        {
            Utils::char_to_hex_digits(buff.base()[r], giga);
            giga.push_back(' ');
            if(!(r%16)) giga.push_back('\n');
        }

        data_file.save(giga);



        YAT_ERROR << "Snap ok buff size =" << buff.length() << std::endl;

        m_interface.report_info("Data loaded!");

        unsigned short *ptr =  (unsigned short *)frame.buffer_ptr;

        unsigned short value = 0;

        count = 1; // Offset of 1 because "o"
        for(size_t y=0; y<frame.y_size; y++)
        {
            for(size_t x=0; x<frame.x_size; x++)
            {
                //*ptr = value;
                //*ptr = ((unsigned short)buff.base()[count] << 8) | buff.base()[count+1];
                *ptr = buff.base()[count+1] + buff.base()[count];
                count+=2;
                ptr++;
            }
            value = value + 50;
            count += (m_col_size - frame.x_size) * 2;
            std::cout << "skipox3 " << (m_col_size - frame.x_size) * 2 << std::endl;
        }

        if(m_config.invert_x)
        {
            unsigned short *ptr =  (unsigned short *)frame.buffer_ptr;
            unsigned short tmp;
            for(size_t y=0; y<frame.y_size; y++)
            {
                for(size_t x=0; x<frame.x_size/2; x++)
                {
                    tmp = *(ptr + x);
                    *(ptr + x) = *(ptr + frame.x_size - x);
                    *(ptr + frame.x_size - x) = tmp;
                }
                ptr += frame.x_size;
            }
        }

        std::cout << "CNT COUNT: " << count-1 << std::endl;

        if(m_listener) m_listener->on_buffer_filled();

        m_interface.report_info("Acquisition successfull!!");

        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("SNAP: Failed!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
}

void CommandTask::t_get_temperature()
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("GET TEMPERATURE: Invalid state!\n");
        return;
    }
    set_state(State::Config);

    try
    {
        double result = get_adc_temperature();

        if(m_listener) m_listener->temperature_callback(result);

        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("GET TEMPERATURE: Failed to get temperature!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
}

void CommandTask::t_get_gain()
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("GET GAIN: Invalid state!\n");
        return;
    }
    set_state(State::Config);

    std::string result;
    long read_gain;

    try
    {
        result = m_interface.command_and_read(CCD_READ_GAIN);

        std::istringstream(result.substr(1)) >> read_gain;

        if(m_listener) m_listener->gain_callback(read_gain);

        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("GET GAIN: Failed to get gain!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
}


void CommandTask::t_re_config()
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("RE CONFIG: Invalid state!\n");
        return;
    }
    set_state(State::Config);
    
    std::string result;

    try
    {
        config_CCD();
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("RE CONFIG: Failed to re configure CCD!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
}
