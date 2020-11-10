#include "CommandTask.h"

using namespace SFAXCommunications;

std::string CommandTask::command_and_wait(const Command & cmd, size_t timeout, std::vector<std::string> * args)
{
    YAT_INFO << "CommandTask::command_and_wait: " << cmd.name() << std::endl;
    try
    {
        m_comms->gpib_write(cmd.str(args));
        cmd.sleep();
        return m_comms->gpib_blocking_read(timeout);
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::command_and_wait " + cmd.name() + " failed!\n");
        RETHROW_EXCEPTION(ex);
    }
}

std::string CommandTask::command_and_read(const Command & cmd, std::vector<std::string> * args)
{
    YAT_INFO << "CommandTask::command_and_read: " << cmd.name() << std::endl;
    try
    {
        m_comms->gpib_write(cmd.str(args));
        cmd.sleep();
        return m_comms->gpib_read();
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::command_and_read " + cmd.name() + " failed!\n");
        RETHROW_EXCEPTION(ex);
    }
}

std::string CommandTask::command_and_flush(const Command & cmd, std::vector<std::string> * args)
{
    YAT_INFO << "CommandTask::command_and_flush: " << cmd.name() << std::endl;
    try
    {
        m_comms->gpib_write(cmd.str(args));
        cmd.sleep();
        return m_comms->gpib_flush();
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::command_and_flush " + cmd.name() + " failed!\n");
        RETHROW_EXCEPTION(ex);
    }
}


std::string CommandTask::reset(size_t number_retries)
{
    std::string result;
    size_t retries = 0;

    try
    {
        m_comms->gpib_flush();

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

            command_and_flush(REBOOT_IF_HUNG);
            result = command_and_flush(WHERE_AM_I);

        } while(IS_NOT_EQUAL(result, "B") && IS_NOT_EQUAL(result, "F"));
        return result;
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::reset failed on retry " +
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

        for(i=0 ; i < values.size(); i++)
        {
            std::vector<std::string> args;
            args.reserve(3);

            args.push_back(TO_STRING(i));
            args.push_back(TO_STRING(offset));
            args.push_back(TO_STRING(values[i].size()));

            result = command_and_wait(WRITE_DATA, 5000, &args);

            if(WRITE_DATA.is_ack(result))
            {
                m_comms->gpib_write(values[i], false);
            }
            else
            {
                THROW_EXCEPTION(
                    "NOT ACKNOWLEDGED",
                    "Received: " + result,
                    "CommandTask::send_tables");
            }
        }
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::send_tables failed for table: " + table + "\n");
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


void CommandTask::config_CCD()
{
    report_info("INIT SEQUENCE: Configuring CCD, this process can take a few minutes...\n");

    int mode = 0;
    size_t i;
    std::string path = m_tables_path; // + "/544/";
    std::string suffix;

    switch (mode)
    {
        case 0: suffix = "1601"; break;
        case 1: suffix = "1602"; break;
        case 2: suffix = "1603"; break;
        case 3: suffix = "1604"; break;
        case 4: suffix = "1401"; break;
        case 5: suffix = "1402"; break;
    }

    suffix += ".TAB";

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

    for(i=0; i<6; i++)
    {
        send_table(path + tab_names[i] + suffix, offsets[i]);
    }

    yat::Thread::sleep(10000);
    m_comms->gpib_flush();
    report_info("INIT SEQUENCE: Data loaded!\n");

    m_comms->gpib_write("Z328,0,848,1024,256,8,8,0,0,5,0,29000,0,400000000,0,4,270,270,256,1040\r");
    yat::Thread::sleep(1500);
    m_comms->gpib_flush();

    m_comms->gpib_write("Z330,0,0\r");
    yat::Thread::sleep(1500);
    m_comms->gpib_flush();


    m_comms->gpib_write("Z348,0,0\r");
    yat::Thread::sleep(1500);
    m_comms->gpib_flush();
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
        result = command_and_read(READ_ADC, &args);

        std::istringstream(result.substr(1)) >> a_gnd;

        args[0] = "CB";
        result = command_and_read(READ_ADC, &args);

        std::istringstream(result.substr(1)) >> a_ref;

        args[0] = "C9";
        result = command_and_read(READ_ADC, &args);

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