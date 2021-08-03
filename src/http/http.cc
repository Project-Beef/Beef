#if 0
#include "http.h"

#include <cpr/cpr.h>
#include <sandbird.h>
#include <fmt/core.h>

#include <atomic>
#include <exception>
#include <filesystem>

#include <string.h>

#include "constants/config.hh"
#include "utils/io.hh"

namespace beef::http
{
    int handler(sb_Event* e) {
        if (e->type == SB_EV_REQUEST) {
            if (strlen(e->path) >= 512)
            {
                sb_send_status(e->stream, 401, "oi, ya send too big message");
                return SB_RES_OK;
            }

            fmt::print("{} - {} {}\n", e->address, e->method, e->path);

            if (strcmp(e->path, "/growtopia/server_data.php") == 0 && strcmp(e->method, "POST") == 0)
            {
                sb_send_status(e->stream, 200, "OK");
                sb_send_header(e->stream, "Content-Type", "text/plain");

                sb_writef(e->stream, "server|%s\nport|%d\ntype|1\n%s\nbeta_server|127.0.0.1\nbeta_port|6969\nbeta_type|1\nmeta|undefined\nRTENDMARKERBS1001", config::address.data(), config::enet::server_port, "#maint|`5Server is under maintenance.");
            }
            else if (strcmp(e->method, "GET") == 0)
            {
                if (strstr(e->path, "/game/") != 0 || strstr(e->path, "/social/") != 0 || strstr(e->path, "/interface/") != 0 || strstr(e->path, "/audio/") != 0)
                {
                    if (strlen(e->path) > 512)
                        return SB_RES_CLOSE;

                    sb_send_status(e->stream, 200, "OK");
                    sb_send_header(e->stream, "Content-Type", "text/plain");

                    char path[554] = "data/cache";
#ifdef _WIN32
                    strcat_s(path, e->path);
#else
                    strcat(path, e->path);
#endif

                   /* if (!std::filesystem::is_regular_file(path))
                    {
                        fmt::print("Not regular file: {}\n", path);
                        return SB_RES_CLOSE;
                    }*/ // BROKEN - KEVZ

                    fmt::print("Get Path: {}\n", path);

                    if (!std::filesystem::exists(path))
                    {
                        fmt::print("oi my fucktard: you got a file missing - {}, but since ya are my mate i'll do it for ya\n", path);

                        std::string str_path = path;

                        try
                        {
                            std::filesystem::create_directory(str_path.substr(0, str_path.find_last_of('/')));

                            cpr::Response r = cpr::Get(cpr::Url{ fmt::format("http://{}/{}{}", config::cdn::ubisoft::server_address, config::cdn::ubisoft::server_path, e->path) });
                            io::write_all_bytes(path, r.text.data(), r.text.size());
                        }
                        catch (const std::exception& e)
                        {
                            fmt::print("k, i failed u need to get it!!\n");
                        }
                    }

                    uintmax_t filesize = 0;
                    void* file = io::read_all_bytes(path, filesize);

                    std::string format = fmt::format("{}", filesize);
                    sb_send_header(e->stream, "Content-Length", format.c_str());

                    sb_send_header(e->stream, "Connection", "keep-alive");
				    sb_send_header(e->stream, "Accept-Ranges", "bytes");


                    sb_write(e->stream, file, filesize);
                }
            }
            else
            {
                return SB_RES_CLOSE;
            }
        }

        return SB_RES_OK;
    }

           /* string address = evt->address;
		string request = evt->method;
		if (strstr(evt->path, "/growtopia/server_data.php") != nullptr && request == "POST") {
			try {
				long long current_time = GetCurrentTimeInternalSeconds();
				sb_send_status(evt->stream, 200, "OK");
				sb_send_header(evt->stream, "Content-Type", "text/plain");
				string req_address = address;
				char reply[1024];
				string info = "server|" + header_server_ip + "\nport|" + to_string(header_configPort) + "\ntype|1\n#maint|`4Oh no! `oWait 0 seconds before logging in again!\nbeta_server|%s\nbeta_port|1945\nbeta_type|1\nmeta|ni.com\nRTENDMARKERBS1001\n";
				if (cooldown.find(address) == cooldown.end()) {
					goto ok;
				} else if (cooldown.at(address) > current_time) {
					info = "server|" + header_server_ip + "\nport|" + to_string(header_configPort) + "\ntype|1\nmaint|`4Oh no! `oWait " + to_string(cooldown.at(address) - current_time) + " seconds before logging in again!\nbeta_server|%s\nbeta_port|1945\nbeta_type|1\nmeta|ni.com\nRTENDMARKERBS1001\n";
					memcpy(reply, info.c_str(), info.size() + 1);
					sb_writef(evt->stream, format(reply, http_ip.c_str(), http_port.c_str(), http_ip.c_str()).c_str());
				} else {
				ok:
					if (cooldown.find(address) == cooldown.end()) {
						cooldown.insert({ address, (GetCurrentTimeInternalSeconds() + (6)) });
					} else {
						cooldown.at(address) = (GetCurrentTimeInternalSeconds() + (6));
					}
					memcpy(reply, info.c_str(), info.size() + 1);
					sb_writef(evt->stream, format(reply, http_ip.c_str(), http_port.c_str(), http_ip.c_str()).c_str());
				}
			} catch (const std::out_of_range& e) {
				cout << e.what() << endl;
			}
		} else if ((strstr(evt->path, "/game/") != NULL && request == "GET" || strstr(evt->path, "/social/") != NULL && request == "GET" || strstr(evt->path, "/interface/") != NULL && request == "GET" || strstr(evt->path, "/audio/") != NULL) && request == "GET") {
			uint32_t size = 0;
			const char* path = evt->path + 1;
			uint8_t* content = read_file(path, &size);
			if (content) {
				sb_send_status(evt->stream, 200, "OK");
				sb_send_header(evt->stream, "Content-Type", "application/x-www-form-urlencoded");
				sb_send_header(evt->stream, "Content-Length", format("%d", size).c_str());
				sb_send_header(evt->stream, "beserver", "06");
				sb_send_header(evt->stream, "Connection", "keep-alive");
				sb_send_header(evt->stream, "Accept-Ranges", "bytes");
				sb_write(evt->stream, content, size);
			} 
		} else if (strstr(evt->path, "/render/") != NULL && request == "GET") {
			string path = evt->path + 1;
			ifstream read_map(path);
			if (!read_map.is_open()) {
				return SB_RES_CLOSE;
			}
			std::stringstream sstr;
			while (read_map >> sstr.rdbuf());
			string test = sstr.str();
			sb_send_status(evt->stream, 200, "OK");
			sb_send_header(evt->stream, "Content-Type", "text/html");
			sb_writef(evt->stream, test.c_str());
		} else {
			return SB_RES_CLOSE;
		}*/

           /* const char* path;
            struct stat s;
            int err;
*/
            /* Log request */
  /*          printf("%s - %s %s\n", e->address, e->method, e->path);
*/
            /* Is the requested path valid? */
  /*          if (e->path[1] == '/' || strstr(e->path, ":/") || strstr(e->path, "..")) {
            sb_send_status(e->stream, 400, "Bad request");
            write_error_page(e->stream, "bad request");
            return SB_RES_OK;
            }
*/
            /* Convert to filesystem path */
  /*          if (e->path[1] == '\0') {
            path = ".";
            } else {
            path = e->path + 1;
            }
*/          /* Get file info */
  /*          err = stat(path, &s);
    */        /* Does file / dir exist? */
      /*      if (err == -1) {
            sb_send_status(e->stream, 404, "Not found");
            write_error_page(e->stream, "page not found");
            return SB_RES_OK;
            }
*/
            /* Handle directory */
  /*          if (S_ISDIR(s.st_mode)) {
            DIR *dir;
            struct dirent *ent;
    */        /* Send page header */
      /*      sb_writef(e->stream, 
                        "<html><head><title>Directory listing for %s</title></head>"
                        "<body><h2>Directory listing for %s</h2><hr><ul>",
                        e->path, e->path);
        */    /* Send file list */
          /*  dir = opendir(path);
            while ((ent = readdir (dir)) != NULL) {
                if (!strcmp(ent->d_name, ".")) continue;
                if (!strcmp(ent->d_name, "..")) continue;
                sb_writef(e->stream, "<li><a href=\"/%s/%s\">%s%s</a></li>",
                        path, ent->d_name, ent->d_name,
                        ent->d_type == DT_DIR ? "/" : "");
            }
            closedir(dir);
            *//* Send page footer */
            /*sb_writef(e->stream, "</ul></body></html>");
            return SB_RES_OK;
            }
*/
            /* Handle file */
  /*          err = sb_send_file(e->stream, path);
            if (err) {
            sb_send_status(e->stream, 500, "Internal server error"); 
            write_error_page(e->stream, sb_error_str(err));
            }
        }
*/
    //}

    /*int handler(sb_Event* e)
    {
        fmt::print("sb_event - {}\n", e->type);

        if (e->type == SB_EV_REQUEST)
        {
            fmt::print("{} - {} {}\n", e->address, e->method, e->path);

            sb_send_status(e->stream, 200, "OK");
            sb_send_header(e->stream, "Content-Type", "text/plain");
            sb_writef(e->stream, "Hello world");
        }

        return SB_RES_OK;
    }*/


    void serve(std::atomic<bool>& running)
    {
        sb_Options opt;
        sb_Server *server;

        memset(&opt, 0, sizeof(opt));
        opt.host = "0.0.0.0";
        opt.port = "80";
        opt.handler = handler;

        server = sb_new_server(&opt);

        if (!server)
        {
            fmt::print(stderr, "failed to initialize http server\n");
            exit(EXIT_FAILURE);
        }

        fmt::print("Server running at http://localhost:{}\n", opt.port);

        while (running.load())
        {
            sb_poll_server(server, 1000);
        }

        sb_close_server(server);
    }
}
#endif
