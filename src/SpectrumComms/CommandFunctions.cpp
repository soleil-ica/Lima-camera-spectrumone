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

void CommandTask::reset(std::string & reply, const size_t & number_retries)
{
    size_t retries = 0;

    try
    {
        //m_interface.m_comms.gpib_flush(reply);

        do
        {
            if(retries == number_retries)
            {
                THROW_EXCEPTION(
                    "MAX RETRIES",
                    "No answer received!",
                    "CommandTask::reset");
            }

            retries ++;

            reply = m_interface.command_and_read(CCD_WHERE_AM_I);

        } while(IS_NOT_EQUAL(reply, "B") && IS_NOT_EQUAL(reply, "F"));
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("CommandTask::reset failed on retry " +
            TO_STRING(retries) + "\n");
        RETHROW_EXCEPTION(ex);
    }
}

void CommandTask::send_table(const std::string & table, unsigned offset)
{
    std::vector<std::string> values;
    std::string result;
    size_t i;

    try
    {
        get_table(table, values);

        for(i=0; i < values.size(); i++)
        {
            std::vector<std::string> args;
            args.reserve(3);

            args.push_back(TO_STRING(i));
            args.push_back(TO_STRING(offset));
            args.push_back(TO_STRING(values[i].size()));

            result = m_interface.command_and_read(CCD_WRITE_DATA, &args);

            // send raw data
            m_interface.m_comms.write(values[i]);
        }
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("CommandTask::send_tables failed for table: " + table + "\n");
        RETHROW_EXCEPTION(ex);
    }
}


void CommandTask::get_table(const std::string & tab_name, std::vector<std::string> & output_vect)
{
    try
    {
        size_t n_rows, i, j;
        std::string temp_string;
        std::vector<std::string> values_vector;

        {
            yat::File f_table(tab_name);
            f_table.load(&temp_string);
        }
        
        yat::StringUtil::split(&temp_string, '\n', &values_vector);

        output_vect.clear();
        output_vect.resize(4);

        std::istringstream(values_vector[0]) >> std::hex >> n_rows;

        for(i=0; i<4; i++)
        {
            output_vect[i].reserve(n_rows);
        }
        for(i=1; i < values_vector.size(); i++)
        {
            temp_string = Utils::hex_digits_to_string(values_vector[i]);
            if(temp_string.size() >= 4)
            {
                for(j=0; j<4; j++)
                {
                    output_vect[j].push_back(temp_string[4-j-1]);
                }
            }
        }
    }
    catch(const yat::Exception & ex)
    {
        RETHROW_EXCEPTION(ex);
    }
    catch(const std::exception & ex)
    {
        THROW_EXCEPTION(
            "STANDARD ERROR",
            ex.what(),
            "CommandTask::getTable");
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR",
            "Unknown error during table reading!",
            "CommandTask::getTable");
    }
}


void CommandTask::get_config(const std::string & config_str, std::vector<std::string>& args)
{
    try
    {
        yat::CfgFile cfg_file = yat::CfgFile();

        cfg_file.load_from_string(config_str);
        cfg_file.set_section("CCD_config");

        args.clear();
        args.reserve(18);
        args.push_back(cfg_file.get_param_value("port", true));
        args.push_back(cfg_file.get_param_value("total_active_x_pixels", true));
        args.push_back(cfg_file.get_param_value("total_active_y_pixels", true));
        args.push_back(cfg_file.get_param_value("num_serial_pixels_before_active", true));
        args.push_back(cfg_file.get_param_value("num_serial_pixels_after_active", true));
        args.push_back(cfg_file.get_param_value("num_parallel_rows_before_active", true));
        args.push_back(cfg_file.get_param_value("num_parallel_rows_after_active", true));
        args.push_back(cfg_file.get_param_value("register_loc_and_direction", true));
        args.push_back(cfg_file.get_param_value("min_temperature", true));
        args.push_back(cfg_file.get_param_value("max_temperature", true));
        args.push_back(cfg_file.get_param_value("min_shutter_time", true));
        args.push_back(cfg_file.get_param_value("max_shutter_time", true));
        args.push_back(cfg_file.get_param_value("min_gain", true));
        args.push_back(cfg_file.get_param_value("max_gain", true));
        args.push_back(cfg_file.get_param_value("h_pixel_spacing", true));
        args.push_back(cfg_file.get_param_value("v_pixel_spacing", true));
        args.push_back(cfg_file.get_param_value("total_parallel_pixels", true));
        args.push_back(cfg_file.get_param_value("total_serial_pixels", true));
    }
    catch(const yat::Exception & ex)
    {
        RETHROW_EXCEPTION(ex);
    }
    catch(const std::exception & ex)
    {
        THROW_EXCEPTION(
            "STANDARD ERROR",
            ex.what(),
            "CommandTask::getTable");
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR",
            "Unknown error during table reading!",
            "CommandTask::getTable");
    }
}

void CommandTask::send_all_tables()
{
    m_interface.report_info("INIT SEQUENCE: Configuring CCD, this process can take a few minutes...\n");

    size_t i;
    std::string suffix, result;

    suffix = m_config.tables_mode + ".TAB";

    std::vector<std::string> tab_names;
    std::vector<unsigned> offsets;

    tab_names.reserve(8);
    tab_names.push_back("STID");
    tab_names.push_back("SERW");
    tab_names.push_back("SERC");
    tab_names.push_back("SERB");
    tab_names.push_back("PART");
    tab_names.push_back("BCON");
    tab_names.push_back("ECON");
    tab_names.push_back("NIDL");

    offsets.reserve(8);
    offsets.push_back(0XD000);
    offsets.push_back(0XD400);
    offsets.push_back(0XD800);
    offsets.push_back(0XDC00);
    offsets.push_back(0XE000);
    offsets.push_back(0XE400);
    offsets.push_back(0XE800);
    offsets.push_back(0XEC00);

    for(i=0; i<8; i++)
    {
        send_table(m_config.tables_path + tab_names[i] + suffix, offsets[i]);
    }

    m_interface.report_info("INIT SEQUENCE: Data loaded!\n");
}

void CommandTask::config_CCD()
{
    std::vector<std::string> args;

    try
    {
        get_config(m_config.expert_config, args);
    }
    catch(const yat::Exception & ex)
    {
        m_interface.report_error("CommandTask::config_CCD failed when parsing expert config!\n");
        RETHROW_EXCEPTION(ex);
    }

    // Write config
    m_interface.command_and_read(CCD_WRITE_CONFIG, &args, true);

    m_interface.command_and_read(CCD_UNKNOWN_1, true);

    m_interface.command_and_read(CCD_UNKNOWN_2, true);

   // m_interface.command_and_read(CCD_READ_CONFIG, true);

    m_interface.command_and_read(CCD_UNKNOWN_3, true);

    //m_interface.command_and_read(CCD_READ_GAIN, true);

    // Set numb flushes
    args.resize(1);
    args[0] = "0";
    m_interface.command_and_read(CCD_SET_SHUTTER, &args, true);
    if(m_listener) m_listener->shutter_callback(false);

    m_interface.command_and_read(CCD_STOP_ACQ);


    // Set exp time
    args[0] = "1000";
    m_interface.command_and_read(CCD_SET_EXP_TIME, &args, true);

    // Set numb flushes
    args[0] = "2";
    m_interface.command_and_read(CCD_SET_FLUSHES, &args, true);
    if(m_listener) m_listener->num_flushes_callback(2);

    // Define format
    args.resize(2);
    args[0] = "0";
    args[1] = "1";
    m_interface.command_and_read(CCD_DEFINE_FORMAT, &args, true);
}


double CommandTask::get_adc_temperature()
{
    std::vector<std::string> args(1, "");
    std::string result;

    int a_gnd, a_ref, a_temperature;

    try
    {
        args[0] = "207";
        result = m_interface.command_and_read(CCD_READ_ADC, &args);

        std::istringstream(result.substr(1)) >> a_gnd;

        args[0] = "203";
        result = m_interface.command_and_read(CCD_READ_ADC, &args);

        std::istringstream(result.substr(1)) >> a_ref;

        args[0] = "201";
        result = m_interface.command_and_read(CCD_READ_ADC, &args);

        std::istringstream(result.substr(1)) >> a_temperature;

        if(a_ref - a_gnd != 0)
        {
            return static_cast<double>((a_temperature - a_gnd) * 3000) / static_cast<double>(a_ref - a_gnd);
        }
        else
        {
            return static_cast<double>((a_temperature - a_gnd) * 3000) / static_cast<double>(4915);
        }


    }
    catch(const yat::Exception & ex)
    {
        RETHROW_EXCEPTION(ex);
    }
    catch(const std::exception & ex)
    {
        THROW_EXCEPTION(
            "STANDARD ERROR",
            ex.what(),
            "CommandTask::get_adc_temperature");
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR",
            "Unknown error when getting temperature!",
            "CommandTask::get_adc_temperature");
    }
}
