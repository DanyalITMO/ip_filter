#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <charconv>

std::vector<std::string_view> split(const std::string_view str, char d)
{
    std::vector<std::string_view> r;

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

class IPV4 {
public:
    IPV4() = default;

    [[nodiscard]] bool from_string(std::string_view ip_str)
    {
        auto splitted = split(ip_str, '.');
        if(splitted.size() != 4)
            return false;

        auto converter = [](std::string_view str)-> int{
            int number;
            auto result = std::from_chars(str.data(), str.data() + str.size(), number);
            if (result.ec == std::errc::invalid_argument) {
                throw std::logic_error{"Could not convert."};
            }
            return number;
        };

        b0 = {static_cast<std::byte>(converter(splitted[0]))};
        b1 = {static_cast<std::byte>(converter(splitted[1]))};
        b2 = {static_cast<std::byte>(converter(splitted[2]))};
        b3 = {static_cast<std::byte>(converter(splitted[3]))};

        return true;
    }

    friend std::ostream& operator<<(std::ostream& s, const IPV4& value);

    std::string to_string()
    {
        std::ostringstream ss;
        ss << *this;
        return ss.str();
    }

public:
    //NOTE std::variant выглядит здесь как overengineering , потому что type safety не нужен, типы в union совместимы
        union {
        uint32_t ip32 = 0;
            struct {
                std::byte b3;
                std::byte b2;
                std::byte b1;
                std::byte b0;
            };
        };
};

std::ostream& operator<<(std::ostream& s, const IPV4& value)
{
    s << static_cast<unsigned>(value.b0)<<'.'
      << static_cast<unsigned>(value.b1)<<'.'
      << static_cast<unsigned>(value.b2)<<'.'
      << static_cast<unsigned>(value.b3);
    return s;
}

bool operator<(const IPV4& lhs, const IPV4& rhs)
{
    return lhs.ip32 < rhs.ip32;
}

bool operator>(const IPV4& lhs, const IPV4& rhs)
{
    return lhs.ip32 > rhs.ip32;
}

std::vector<IPV4> filter(const std::vector<IPV4>& ips, const std::function<bool(IPV4)>& predicat)
{
    std::vector<IPV4> filtered_ip;
    for(auto&& ip : ips)
    {
        if(predicat(ip))
        {
            filtered_ip.push_back(ip);
        }
    }
    return filtered_ip;
}


std::vector<IPV4> filter(const std::vector<IPV4>& ips, std::byte first)
{
    return filter(ips, [first](const IPV4& ip){
        return ip.b0 == first;
    });
}

std::vector<IPV4> filter(const std::vector<IPV4>& ips, std::byte first, std::byte second)
{
    return filter(ips, [first, second](const IPV4& ip){
        return (ip.b0 == first && ip.b1 == second);
    });
}

std::vector<IPV4> filter_any(const std::vector<IPV4>& ips, std::byte byte)
{
    return filter(ips, [byte](const IPV4& ip){
        return (ip.b0 == byte || ip.b1 == byte || ip.b2 == byte || ip.b3 == byte);
    });
}

int main(int argc, char const *argv[])
{
    try
    {
        std::vector<IPV4> ip_pool;

        std::vector<std::string> lines;
        for(std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string_view> v = split(line, '\t');
            IPV4 ip;
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

        auto filtered_by_first = filter(ip_pool, std::byte{1});
        std::sort(std::begin(filtered_by_first), std::end(filtered_by_first), std::greater<>());

        for(auto ip : filtered_by_first)
        {
            std::cout << ip<<std::endl;
        }

        auto filtered_by_first_second = filter(ip_pool, std::byte{46}, std::byte{70});
        std::sort(std::begin(filtered_by_first_second), std::end(filtered_by_first_second), std::greater<>());

        for(auto ip : filtered_by_first_second)
        {
            std::cout << ip<<std::endl;
        }

        auto filtered_any = filter_any(ip_pool, std::byte{46});
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
