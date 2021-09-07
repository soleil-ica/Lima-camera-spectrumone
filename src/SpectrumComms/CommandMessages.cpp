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

#include "SpectrumComms/CommandTask.h"

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

        case SET_SHUTTER:
        YAT_INFO << "CommandTask::handle_message: SET_SHUTTER" << std::endl;
        t_set_shutter(msg.get_data<bool>());
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
            //m_interface.m_comms.wait_events(0x100);
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
        m_interface.m_comms.init();
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
            yat::Thread::sleep(800);
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
        if(m_listener) m_listener->num_flushes_callback(num_flushes);
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
    size_t returned_size, count;
    std::vector<std::string> args(1, "");

    try
    {
        m_interface.report_info("Configuring acquisition...");
        size_t expected_size = ((m_col_size)*(frame.frame_info.y_size / frame.frame_info.y_bin))*2;
        yat::Buffer<char> buff(expected_size);

        result = m_interface.command_and_read(CCD_STATUS);

        m_interface.report_info("Starting acquisition!");
        args[0] = "1";
        m_interface.command_and_read(CCD_START, &args, true);

        if(m_listener) m_listener->shutter_callback(true);


        do
        {
            yat::Thread::sleep(800);
            result = m_interface.command_and_read(CCD_STATUS);

            // TODO
            // Break after a number of tries

        }while(IS_NOT_EQUAL(result, "o0\r"));

        m_interface.report_info("Acquisition over, loading data...");

        if(m_listener) m_listener->shutter_callback(false);

        m_interface.command_and_read(CCD_RESET_IMAGE, true);


        m_interface.command_and_read(CCD_READ_IMAGE, true);

        returned_size = m_interface.m_comms.read_raw(buff, expected_size, true);

        if(returned_size != expected_size)
        {
            THROW_EXCEPTION(
                "FAILED",
                "Didn't receive expected snap data after 5 retries!",
                "CommandTask::SNAP");
        }

        m_interface.report_info("Data loaded!");

        unsigned short *ptr = (unsigned short *)frame.buffer_ptr;

        count = 1; // Offset of 1 because "o"
        for(size_t y=0; y < (frame.frame_info.y_size / frame.frame_info.y_bin); y++)
        {
            for(size_t x=0; x<frame.frame_info.x_size; x++)
            {
                // Reinterpret the chars from the buffer as unsigned chars and then convert to short (16 bits)
                *ptr = ((unsigned short)(reinterpret_cast<unsigned char&>(buff.base()[count])) << 8) | (reinterpret_cast<unsigned char&>(buff.base()[count+1]));
                count+=2;
                ptr++;
            }
            count += (m_col_size - frame.frame_info.x_size) * 2;
        }

        if(m_config.invert_x)
        {
            // Invert the image on the x axis
            unsigned short *ptr =  (unsigned short *)frame.buffer_ptr;
            unsigned short tmp;
            for(size_t y=0; y < (frame.frame_info.y_size / frame.frame_info.y_bin); y++)
            {
                for(size_t x=0; x<frame.frame_info.x_size/2; x++)
                {
                    tmp = *(ptr + x);
                    *(ptr + x) = *(ptr + frame.frame_info.x_size - x);
                    *(ptr + frame.frame_info.x_size - x) = tmp;
                }
                ptr += frame.frame_info.x_size;
            }
        }

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

void CommandTask::t_set_shutter(const bool & open_shutter)
{
    if(get_state() != State::Idle)
    {
        // m_interface.report_error("SET SHUTTER: Invalid state!\n");
        return;
    }
    set_state(State::Config);

    std::vector<std::string> args(1, "0");

    if (open_shutter) args[0] = "1";

    try
    {
        m_interface.command_and_read(CCD_SET_SHUTTER, &args, true);
        if(m_listener) m_listener->shutter_callback(open_shutter);
        set_state(State::Idle);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("SET SHUTTER: Failed to set shutter!\n");
        m_interface.report_error(ex.to_string());
        set_state(State::Fault);
    }
}
