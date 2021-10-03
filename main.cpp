#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <bitset>
// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]


std::string read_entire_file(const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ios_base::in);
    if ( !file.good() )
    {
        std::clog<< "Can't open file \"" << filename << "\"";
        return std::string();
    }

    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if ( file.fail() )
    {
        std::clog<<"Can't read file \"" << filename << "\" contents";
        return std::string();
    }

    return contents;
}

std::vector<std::string> read_file_lines(const std::string& filename)
{
    const auto content = read_entire_file(filename);
    std::vector<std::string> ret;
    std::istringstream input(content);
    std::string line;
    while (std::getline(input, line)) {
        try
        {
            ret.push_back(line);
        }
        catch (const std::exception& e)
        {
//            std::cerr<<LOCATION<< e.what()<<std::endl;
        }
    }

    return ret;
}

std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}


class ip_addr {
public:
    ip_addr() = default;

    [[nodiscard("reason")]] bool from_string(const std::string &ip_str) //change to string_view
    {
        auto splitted = split(ip_str, '.');
        if(splitted.size() != 4)
            return false;

        b0 = std::stoi(splitted[0]);
        b1 = std::stoi(splitted[1]);
        b2 = std::stoi(splitted[2]);
        b3 = std::stoi(splitted[3]);

        return true;
    }

    friend std::ostream& operator<<(std::ostream& s, const ip_addr& value);

    std::string to_string()
    {
        std::ostringstream ss;
        ss << *this;
        return ss.str();
    }

public:
    //std::byte
    //std::variant
        union {
        uint32_t ip32 = 0;
            struct {
                unsigned char b3;
                unsigned char b2;
                unsigned char b1;
                unsigned char b0;
            };
        };
};

std::ostream& operator<<(std::ostream& s, const ip_addr& value)
{
    s << static_cast<unsigned>(value.b0)<<'.'
      << static_cast<unsigned>(value.b1)<<'.'
      << static_cast<unsigned>(value.b2)<<'.'
      << static_cast<unsigned>(value.b3);
    return s;
}

bool operator<(const ip_addr& lhs, const ip_addr& rhs)
{
    return lhs.ip32 < rhs.ip32;
}

bool operator>(const ip_addr& lhs, const ip_addr& rhs)
{
    return lhs.ip32 > rhs.ip32;
}

std::vector<ip_addr> filter(const std::vector<ip_addr>& ips, const std::function<bool(ip_addr)>& predicat)
{
    std::vector<ip_addr> filtered_ip;
    for(auto&& ip : ips)
    {
        if(predicat(ip))
        {
            filtered_ip.push_back(ip);
        }
    }
    return filtered_ip;
}


std::vector<ip_addr> filter(const std::vector<ip_addr>& ips, unsigned char first)
{
    return filter(ips, [first](const ip_addr& ip){
        return ip.b0 == first;
    });
}

std::vector<ip_addr> filter(const std::vector<ip_addr>& ips, unsigned char first, unsigned char second)
{
    return filter(ips, [first, second](const ip_addr& ip){
        return (ip.b0 == first && ip.b1 == second);
    });
}

std::vector<ip_addr> filter_any(const std::vector<ip_addr>& ips, unsigned char byte)
{
    return filter(ips, [byte](const ip_addr& ip){
        return (ip.b0 == byte || ip.b1 == byte || ip.b2 == byte || ip.b3 == byte);
    });
}

int main(int argc, char const *argv[])
{

    try
    {
        auto lines = read_file_lines("ip_filter.tsv");
        std::vector<ip_addr> ip_pool;

        for(auto&& line : lines)
        {
            std::vector<std::string> v = split(line, '\t');
            ip_addr ip;
            if(!ip.from_string(v.at(0)))
            {
                std::clog<<v.at(0)<<" is not ip"<<std::endl;
                continue;
            }

            ip_pool.push_back(ip);
        }

        std::sort(std::begin(ip_pool), std::end(ip_pool), std::greater<>());
        for(auto ip : ip_pool)
        {
            std::cout << ip<<std::endl;
        }

        auto filtered_by_first = filter(ip_pool, 1);
        std::sort(std::begin(filtered_by_first), std::end(filtered_by_first), std::greater<>());

        for(auto ip : filtered_by_first)
        {
            std::cout << ip<<std::endl;
        }


        auto filtered_by_first_second = filter(ip_pool, 46, 70);
        std::sort(std::begin(filtered_by_first_second), std::end(filtered_by_first_second), std::greater<>());

        for(auto ip : filtered_by_first_second)
        {
            std::cout << ip<<std::endl;
        }

        auto filtered_any = filter_any(ip_pool, 46);
        std::sort(std::begin(filtered_any), std::end(filtered_any), std::greater<>());

        for(auto ip : filtered_any)
        {
            std::cout << ip<<std::endl;
        }

    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
