#include "PrintToString.hpp"



PrintToString::PrintToString()
{
    data = new String();
}

PrintToString::~PrintToString()
{
    delete data;
    data = NULL;
}


String* PrintToString::results()
{
    return data;
}

void PrintToString::clear()
{
    delete data;
    data = new String();
}

size_t PrintToString::write(uint8_t buff)
{
    data->concat(buff);
    return 1;
}

size_t PrintToString::write(const uint8_t* buffer, size_t size)
{
    size_t n = 0;

    while (size--)
    {
        if (data->concat(*buffer++))
            n++;
        else
            break;
    }

    return n;
}
