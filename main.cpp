#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
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

auto str_vector_to_int_vector(const std::vector<std::vector<std::string>>& ip_pool)
{
    auto width = ip_pool.size();
    auto result = std::vector<std::vector<int>>(width);
    
    for(size_t x = 0; x<width; ++x)
    {
        auto height = ip_pool[x].size();
        result[x] = std::vector<int>(height);
        for(size_t y = 0; y<height; ++y)
        {
            result[x][y] = stoi(ip_pool[x][y]);
        }
    }
    return result;
}
auto int_vector_to_str_vector(const std::vector<std::vector<int>>& ip_pool)
{
    auto width = ip_pool.size();
    auto result = std::vector<std::vector<std::string>>(width);
    
    for(size_t x = 0; x<width; ++x)
    {
        auto height = ip_pool[x].size();
        result[x] = std::vector<std::string>(height);
        for(size_t y = 0; y<height; ++y)
        {
            result[x][y] = std::to_string(ip_pool[x][y]);
        }
    }
    return result;
}

//filter_helper с одним аргументом сортирует вектор <int>
inline auto filter_helper(std::vector<std::vector<int>>& ip_pool)
{
    std::sort(ip_pool.rbegin(),ip_pool.rend());
    return int_vector_to_str_vector(ip_pool);
}

//filter_helper с одним аргументом сортирует вектор.
//Эта функция вызывается ровно один раз - при начальной сортировке ip_pool
inline auto filter_helper(std::vector<std::vector<std::string>>& ip_pool)
{
    return filter_helper(str_vector_to_int_vector(ip_pool));
}

inline auto filter_helper(const std::vector<std::vector<int>>& ip_pool,int)
{
    auto tmp = ip_pool;
    return filter_helper(tmp);
}
template<class... Args>
inline auto filter_helper(const std::vector<std::vector<int>>& ip_pool,int index,int byte,Args... args)
{
    auto width = ip_pool.size();

    //На всякий пожарный
    if(!ip_pool.empty() && (index >= ip_pool[0].size()))
        return filter_helper(ip_pool,index);

    std::vector<std::vector<int>> result;
    result.reserve(width);

    for(size_t x = 0; x<width; ++x)
    {
        auto &ip = ip_pool[x];
        if(ip[index] == byte)
        {
            result.push_back(ip);
        }
    }
    return filter_helper(result,++index,args...);
}

void print_ips(const std::vector<std::vector<std::string> >& ip_pool)
{
    for(std::vector<std::vector<std::string> >::const_iterator ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
    {
        for(std::vector<std::string>::const_iterator ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
        {
            if (ip_part != ip->cbegin())
            {
                std::cout << ".";

            }
            std::cout << *ip_part;
        }
        std::cout << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    try
    {
        std::vector<std::vector<std::string>> ip_pool;

        //Ввод
        for(std::string line; std::getline(std::cin, line) && (line.size()>0);)
        {
            std::vector<std::string> v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        //Сортировка
        ip_pool = filter_helper(ip_pool);
        std::cout<<"--------------------------"<<std::endl;
        print_ips(ip_pool);

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8


        // <Лямбды>
        auto filter = [&ip_pool](auto... args) mutable
        {
            auto int_vector = str_vector_to_int_vector(ip_pool);
            return filter_helper(int_vector,0,args...);
        };

        auto filter_any = [&ip_pool](int byte) mutable
        {
            auto length = ip_pool.size();
            auto result = std::vector<std::vector<std::string>>();
            result.reserve(length);
            std::string byte_string = std::to_string(byte);

            for(size_t i = 0;i<length;++i)
            {  
                for(auto &ip_byte: ip_pool[i])
                {
                    if (ip_byte == byte_string)
                    {
                        result.push_back(ip_pool[i]);
                        break;
                    }
                }
            }
            return result;
        };
        // </Лямбды>

        //filter by first byte and output
        
        auto ip = filter(1);
        std::cout<<"--------------------------"<<std::endl<<"filter(1):"<<std::endl;
        print_ips(ip);

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        //filter by first and second bytes and output
        ip = filter(46, 70);
        std::cout<<"--------------------------"<<std::endl<<"filter(46,70):"<<std::endl;
        print_ips(ip);

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        //filter by any byte and output
        ip = filter_any(46);
        std::cout<<"--------------------------"<<std::endl<<"filter_any(46):"<<std::endl;
        print_ips(ip);

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