#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

static const int WHITE = 37;
static const int RED = 31;
static const int YELLOW = 33;
static const int GREEN = 32;
static const int CYAN = 36;
static const int BLUE = 34;
static const int MAGENTA = 35;

void color_print(const std::string& msg, int color)
{
    std::cout << "\033[" << color << "m" << msg << "\033[0m";
}

void err(const std::string& msg)
{
    color_print("LC Error: " + msg + "\n", RED);
}

int main(int argc, char** argv)
{

    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }

    bool recursive = false, noEmpty = false, progress = false;
    std::string dir;
    std::vector<std::string> extensions;

    for (int i = 0; i < args.size(); i++)
    {
        std::string& arg = args[i];

        if (arg.length() >= 2) if (arg.substr(0, 2) == "--")
        {
            if (!(arg == "--r" || arg == "--ne" || arg == "--p" || arg == "--help"))
            {
                err("Unknown argument '" + arg + "' (Use --help for a list of valid arguments)");
                return -1;
            }
            else if (arg == "--help")
            {
                color_print("LC Arguments:\n", WHITE);
                color_print("-ext", CYAN); std::cout << "       extensions (should be followed by a list of space-separated extensions (with '.') surrounded by quotes, LC will count only files with these extensions)\n";
                color_print("--r", CYAN); std::cout << "        recursive (searches subdirectories too)\n";
                color_print("--ne", CYAN); std::cout << "       no empty (don't count empty lines)\n";
                color_print("--p", CYAN); std::cout << "        progress (show progress per-file, will greatly decrease speed if there are many files)\n";
                color_print("--help", CYAN); std::cout << "     help (prints this message)\n\n";
                color_print("Example: ", WHITE); color_print("lc --r --p -ext \".cpp .h\" C:/dev/\n", CYAN);
                color_print("This example will count all the lines of all .cpp or .h files found in the 'C:/dev/' directory and all subdirectories (recursive) and print the progress per-file. It will count empty lines, because --ne was not used.\n", WHITE); 
                return 0;
            }
            else if (arg == "--r") recursive = true;
            else if (arg == "--ne") noEmpty = true;
            else if (arg == "--p") progress = true;
        }
        else if (arg.length() >= 1) if (arg.substr(0, 1) == "-")
        {
            if (arg != "-ext")
            {
                err("Unknown argument '" + arg + "' (Use --help for a list of valid arguments)");
                return -1;
            }

            if (i == args.size() - 1)
            {
                err("The '-ext' argument requires a subsequent argument");
                return -1;
            }

            std::string exts = args[i + 1];
            std::vector<std::string> vExts;
            int start = 0, end = 0;
            for (int i = 0; i <= exts.length(); i++)
            {
                if (i == exts.length())
                {
                    end = i;
                    vExts.push_back(exts.substr(start, end - start));
                }
                else if (exts[i] == ' ')
                {
                    end = i;
                    vExts.push_back(exts.substr(start, end - start));
                    start = i + 1;
                }
            }

            extensions = vExts;

            i++;
        }
        else
        {
            if (!std::filesystem::exists(arg)) 
            {
                err("Path does not exist: '" + arg + "'");
                return -1;
            }
            else if (!std::filesystem::is_directory(arg)) 
            {
                err("Path is not a directory: '" + arg + "'");
                return -1;
            }
            dir = arg;
        }
    }

    color_print("Recursion ------------ " + std::string(recursive ? " ON\n" : "OFF\n"), CYAN);
    color_print("Exclude empty lines -- " + std::string(noEmpty ? " ON\n" : "OFF\n"), CYAN);
    color_print("Progress ------------- " + std::string(progress ? " ON\n" : "OFF\n"), CYAN);

    std::vector<std::string> filepaths;

    try
    {
        if (recursive) for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
        {
            try { entry.path(); }
            catch (const std::filesystem::filesystem_error& e)
            {
                std::cout << "caught fs error: " << e.what() << "\n";
                continue;
            }

            if (std::filesystem::is_directory(entry.path())) continue;
            if (extensions.size() == 0) filepaths.push_back(entry.path().string());
            else
            {
                std::string ext = entry.path().extension().string();
                for (const std::string& _ext : extensions)
                {
                    if (ext == _ext)
                    {
                        filepaths.push_back(entry.path().string());
                        break;
                    }
                }
            }
        }
        else for (const auto& entry : std::filesystem::directory_iterator(dir))
        {
            if (std::filesystem::is_directory(entry.path())) continue;
            if (extensions.size() == 0) filepaths.push_back(entry.path().string());
            else
            {
                std::string ext = entry.path().extension().string();
                for (const std::string& _ext : extensions)
                {
                    if (ext == _ext)
                    {
                        filepaths.push_back(entry.path().string());
                        break;
                    }
                }
            }
        }
    }
    catch (std::filesystem::filesystem_error& e)
    {
        err("Encountered internal error: " + std::string(e.what()) + "\n");
        return -1;
    }

    color_print("\nFound " + std::to_string(filepaths.size()) + " files\n", YELLOW);

    int totalLines = 0;
    for (std::string& path : filepaths)
    {
        for (char& c : path)
        {
            if (c == '\\') c = '/';
        }

        if (progress) color_print(path + " - ", WHITE);
        std::ifstream file(path);
        if (!file.is_open())
        {
            if (progress) color_print("file not found or permission denied", RED);
        }
        else
        {
            std::string line;
            int lines = 0;
            while (std::getline(file, line))
            {
                if (!noEmpty) lines++;
                else if (line != "") lines++;
            }
            totalLines += lines;
            if (progress) color_print(std::to_string(lines) + " lines", YELLOW);

            file.close();
        }
        if (progress) printf("\n");
    }

    color_print("Total: " + std::to_string(totalLines) + " lines\n", GREEN);

    return 0;
}
