#include "CommandTask.h"

using namespace SFAXCommunications;

std::string CommandTask::command_and_wait(const Command & cmd, size_t timeout,
    std::vector<std::string> * args, bool ack)
{
    YAT_INFO << "CommandTask::command_and_wait: " << cmd.name() << std::endl;
    try
    {
        std::string result;
        m_comms->gpib_write(cmd.str(args));
        cmd.sleep();
        m_comms->gpib_blocking_read(result, timeout);
        if(ack)
        {
            if(!cmd.is_ack(result))
            {
                THROW_EXCEPTION(
                    "NOT ACKNOWLEDGED",
                    "Received: " + Utils::make_string_readable(result) +
                    " for command " + cmd.name(),
                    "CommandTask::command_and_wait");
            }
        }
        return result;
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::command_and_wait " + cmd.name() + " failed!\n");
        RETHROW_EXCEPTION(ex);
    }
}

std::string CommandTask::command_and_read(const Command & cmd,
    std::vector<std::string> * args, bool ack)
{
    YAT_INFO << "CommandTask::command_and_read: " << cmd.name() << std::endl;
    try
    {
        std::string result;
        m_comms->gpib_write(cmd.str(args));
        cmd.sleep();
        m_comms->gpib_read(result);
        if(ack)
        {
            if(!cmd.is_ack(result))
            {
                THROW_EXCEPTION(
                    "NOT ACKNOWLEDGED",
                    "Received: " + Utils::make_string_readable(result) +
                    " for command " + cmd.name(),
                    "CommandTask::command_and_read");
            }
        }
        return result;
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::command_and_read " + cmd.name() + " failed!\n");
        RETHROW_EXCEPTION(ex);
    }
}

std::string CommandTask::command_and_flush(const Command & cmd,
    std::vector<std::string> * args, bool ack)
{
    YAT_INFO << "CommandTask::command_and_flush: " << cmd.name() << std::endl;
    try
    {
        std::string result;
        m_comms->gpib_write(cmd.str(args));
        cmd.sleep();
        m_comms->gpib_flush(result);
        if(ack)
        {
            if(!cmd.is_ack(result))
            {
                THROW_EXCEPTION(
                    "NOT ACKNOWLEDGED",
                    "Received: " + Utils::make_string_readable(result) +
                    " for command " + cmd.name(),
                    "CommandTask::command_and_flush");
            }
        }
        return result;
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::command_and_flush " + cmd.name() + " failed!\n");
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

std::string CommandTask::read_adc_temperature()
{
    std::vector<std::string> args;
    std::string result;

    int a_gnd, a_ref, ccd_temp;

    double r;

    try
    {
        args.resize(1);

        args[0] = "CF";
        result = command_and_read(CCD_READ_ADC, &args);

        std::istringstream(result.substr(1)) >> a_gnd;

        args[0] = "CB";
        result = command_and_read(CCD_READ_ADC, &args);

        std::istringstream(result.substr(1)) >> a_ref;

        args[0] = "C9";
        result = command_and_read(CCD_READ_ADC, &args);

        std::istringstream(result.substr(1)) >> ccd_temp;

        if(a_ref - a_gnd != 0)
        {
            r = static_cast<double>((ccd_temp - a_gnd) * 3000) / static_cast<double>(a_ref - a_gnd);
        }
        else
        {
            r = static_cast<double>((ccd_temp - a_gnd) * 3000) / static_cast<double>(4915);
        }


        return TO_STRING(r);
    }
    catch(const std::exception & ex)
    {
        THROW_EXCEPTION(
            "STANDARD ERROR",
            ex.what(),
            "CommandTask::read_adc_temperature");
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR",
            "Unknown error!",
            "CommandTask::read_adc_temperature");
    }

}


std::vector<std::string> CommandTask::get_config(const std::string & config_str)
{
    try
    {
        yat::CfgFile cfg_file = yat::CfgFile();
        std::vector<std::string> result_vect;

        cfg_file.load_from_string(config_str);
        cfg_file.set_section("CCD_config");

        result_vect.reserve(18);
        result_vect.push_back(cfg_file.get_param_value("port", true));
        result_vect.push_back(cfg_file.get_param_value("total_active_x_pixels", true));
        result_vect.push_back(cfg_file.get_param_value("total_active_y_pixels", true));
        result_vect.push_back(cfg_file.get_param_value("num_serial_pixels_before_active", true));
        result_vect.push_back(cfg_file.get_param_value("num_serial_pixels_after_active", true));
        result_vect.push_back(cfg_file.get_param_value("num_parallel_rows_before_active", true));
        result_vect.push_back(cfg_file.get_param_value("num_parallel_rows_after_active", true));
        result_vect.push_back(cfg_file.get_param_value("register_loc_and_direction", true));
        result_vect.push_back(cfg_file.get_param_value("min_temperature", true));
        result_vect.push_back(cfg_file.get_param_value("max_temperature", true));
        result_vect.push_back(cfg_file.get_param_value("min_shutter_time", true));
        result_vect.push_back(cfg_file.get_param_value("max_shutter_time", true));
        result_vect.push_back(cfg_file.get_param_value("min_gain", true));
        result_vect.push_back(cfg_file.get_param_value("max_gain", true));
        result_vect.push_back(cfg_file.get_param_value("h_pixel_spacing", true));
        result_vect.push_back(cfg_file.get_param_value("v_pixel_spacing", true));
        result_vect.push_back(cfg_file.get_param_value("total_parallel_pixe", true));
        result_vect.push_back(cfg_file.get_param_value("total_serial_pixels", true));

        return result_vect;
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

std::string CommandTask::error_code(int val)
{
    switch (val)
    {
    case 1:
    return "Hardware problem";

    case 2:
    return "Not available";

    case 3:
    return "Parameter problem";

    case 4:
    return "Not initialized";

    case 20:
    return "CCD: NULL user pointer";

    case 21:
    return "CCD: Not enough memory";

    case 22:
    return "CCD: Altparam";

    case 23:
    return "CCD: Load";

    case 24:
    return "CCD: Read program";

    case 25:
    return "CCD: Timeout";

    case 26:
    return"CCD: Zeroloop";

    case 30:
    return "PDA: Multiscan error";

    case 31:
    return "Remote: Not enough memory";

    case 32:
    return "Remote: No data available";

    case 33:
    return "Remote: Binary transfer error";

    case 34:
    return "Remote: Illegal call sequence";

    default:
    return "Unknown error";
    }
}