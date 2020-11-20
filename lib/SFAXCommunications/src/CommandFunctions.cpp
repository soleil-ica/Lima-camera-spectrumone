#include "CommandTask.h"

using namespace SFAXCommunications;

void CommandTask::reset(std::string & reply, const size_t & number_retries)
{
    size_t retries = 0;

    try
    {
        m_comms->gpib_flush(reply);

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

            command_and_flush(CCD_REBOOT_IF_HUNG);
            reply = command_and_flush(CCD_WHERE_AM_I);

        } while(IS_NOT_EQUAL(reply, "B") && IS_NOT_EQUAL(reply, "F"));
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

            result = command_and_wait(CCD_WRITE_DATA, 5000, &args, true);

            // send raw data
            m_comms->gpib_write(values[i], false);
        }
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::send_tables failed for table: " + table + "\n");
        RETHROW_EXCEPTION(ex);
    }
}

void CommandTask::config_CCD()
{
    report_info("INIT SEQUENCE: Configuring CCD, this process can take a few minutes...\n");

    int mode = 0;
    size_t i;
    std::string suffix, result;
    std::vector<std::string> config_args;

    switch (mode)
    {
        case 0: suffix = "1401"; break;
        case 1: suffix = "1402"; break;
        case 2: suffix = "1601"; break;
        case 3: suffix = "1602"; break;
        case 4: suffix = "1603"; break;
        case 5: suffix = "1604"; break;

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

    result = command_and_read(CCD_INIT);

    for(i=0; i<8; i++)
    {
        send_table(m_tables_path + tab_names[i] + suffix, offsets[i]);
    }

    yat::Thread::sleep(5000);

    report_info("INIT SEQUENCE: Data loaded!\n");

    try
    {
        config_args = get_config(m_expert_config);
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandTask::config_CCD failed when parsing expert config!\n");
        RETHROW_EXCEPTION(ex);
    }

    command_and_read(CCD_WRITE_CONFIG, &config_args, true);

    command_and_read(CCD_UNKNOWN_1);

    command_and_read(CCD_UNKNOWN_2);

    command_and_read(CCD_READ_CONFIG);
}
