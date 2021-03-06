#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#include <filesystem>
#include <set>

#include "MD5.h"

namespace fs = std::filesystem;

// md5 hashes are stored in $HOME/.watch/MD5OFTHEDIRNAME.watch
// skips .files. 

auto getHomePath()
{
#if __GNUC__ 
    return fs::path(getenv("HOME")); 
#else
    return fs::path(getenv("USERPROFILE"));
#endif 
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage  " << argv[0] << " directory to watch. (skips .dirs)  \n";
        return -1;
    }

    //
    auto dir_to_watch = fs::path(argv[1]);

    if (!fs::is_directory(dir_to_watch))
    {
        throw std::logic_error(dir_to_watch.string() + std::string(" not a directory "));
    }

    auto watch_dir = getHomePath() / fs::path(".watch");
    fs::create_directory(watch_dir);

    auto cache_file = watch_dir / fs::path(md5(dir_to_watch.string().c_str()));

    if (!fs::exists(cache_file))
    {
        std::ofstream output(cache_file);
    }

    std::fstream               cache_stream(cache_file, std::ios::in);

    std::set<std::string>    old_cache;
    std::set<std::string>    new_cache;

    std::set<std::string>    old_cache_files;
    std::set<std::string>    new_cache_files;


    for (std::string line; std::getline(cache_stream, line); )
    {
         std::string name_file = line.substr(0, line.find("+"));
         old_cache.insert(line + "\n");
         old_cache_files.insert(name_file);
    }

    using directory_iterator = std::filesystem::recursive_directory_iterator;

    for (auto entry =  directory_iterator(dir_to_watch) ; entry != fs::recursive_directory_iterator() ; entry ++ )
    {
        fs::file_status status = fs::status ( entry->path().string() ); 

        try {
            if (fs::is_directory(status) &&  entry->path().filename().string()[0] == '.' ) 
            {   
                    entry.disable_recursion_pending();
            }
            if (fs::is_regular_file(status)) 
            {

                // read it all.
                std::fstream         file_reader(entry->path(), std::ios::in);
                std::string          raw_data(std::istreambuf_iterator<char>{file_reader}, {});

                new_cache.insert(entry->path().string() + std::string("+") + md5(raw_data) + "\n");
                new_cache_files.insert( entry->path().string() );
            }

        }
        catch (std::exception& ex)
        {
        }
    }

    // changed files. ( or added )
    std::vector<std::string> changed_files;

    // deleted files. 
    std::vector<std::string> deleted_files;

    std::set_difference(
                          new_cache.begin(), new_cache.end(),
                          old_cache.begin(), old_cache.end(),
                          std::back_inserter(changed_files)
                       );

    for (auto& cache_line : changed_files)
    {
        std::string name_file = cache_line.substr(0, cache_line.find("+"));
        std::cout << name_file << "\n";
    }

    std::fstream cache_out_stream(cache_file, std::ios::out | std::ios::trunc);

    for (auto& cache_line : new_cache)
    {
        cache_out_stream << cache_line << " m\n";
    }

    cache_stream.close();

    std::set_difference(
                      old_cache_files.begin(), old_cache_files.end(),
                      new_cache_files.begin(), new_cache_files.end(),
                      std::back_inserter(deleted_files)
                   );

    for (auto& deleted_file : deleted_files)
    {
        std::cout << deleted_file << " d\n";
    }

    // find the deleted files. 



    // update the cache.

    return 0;

}
