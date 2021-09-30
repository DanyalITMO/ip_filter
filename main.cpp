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
            /*for(std::size_t i = 0; i < 4; i++) {
                if (lhs.b0 != std::stoi(rhs[i]))
            }*/

//        b0 = std::stoi(i)

        auto splitted = split(ip_str, '.');
        if(splitted.size() != 4)
            return false;

        bytes.b0 = std::stoi(splitted[0]);
        bytes.b1 = std::stoi(splitted[1]);
        bytes.b2 = std::stoi(splitted[2]);
        bytes.b3 = std::stoi(splitted[3]);

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
    struct ip_4bytes{
        unsigned char b3;
        unsigned char b2;
        unsigned char b1;
        unsigned char b0;
    };
    union {
        uint32_t ip32 = 0;
        ip_4bytes bytes;
    };
};

std::ostream& operator<<(std::ostream& s, const ip_addr& value)
{
    s << static_cast<unsigned>(value.bytes.b0)<<'.'
      << static_cast<unsigned>(value.bytes.b1)<<'.'
      << static_cast<unsigned>(value.bytes.b2)<<'.'
      << static_cast<unsigned>(value.bytes.b3);
    return s;
}

bool operator<(const ip_addr& lhs, const ip_addr& rhs)
{
//    for(std::size_t i = 0; i < 4; i++)
//    {
//        if(lhs.b0 != std::stoi(rhs[i]))
//            return std::stoi(lhs[i]) > std::stoi(rhs[i]);
//    }
    return lhs.ip32 < rhs.ip32;
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

        std::sort(std::begin(ip_pool), std::end(ip_pool));
        for(auto ip : ip_pool)
        {
            std::cout << ip<<std::endl;
        }

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first byte and output
        // ip = filter(1)

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        // ip = filter(46, 70)

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        // ip = filter_any(46)

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
