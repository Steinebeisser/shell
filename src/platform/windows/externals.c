#ifdef __WIN32

#include "commands/externals.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "config/config.h"
#include "core/shell_print.h"

#include <windows.h>
#include <io.h>
#define F_OK 0

wchar_t *quote_cmd(int argc, char **argv) {
    int total_len = 0;
    for (int i = 0; i < argc; ++i) {
        total_len += MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, NULL, 0) + 2 + 1; // 2 for possible quotes, 1 for space if there
    }

    wchar_t *quoted_cmd = (wchar_t*)malloc(sizeof(wchar_t) * total_len);
    if (!quoted_cmd) return NULL;

    quoted_cmd[0] = L'\0';

    for (int i = 0; i < argc; i++) {
        if (i > 0) wcscat(quoted_cmd, L" ");

        int arg_len = MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, NULL, 0);
        wchar_t *warg = (wchar_t*)malloc(sizeof(wchar_t) * arg_len);
        MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, warg, arg_len);

        if (wcschr(warg, L' ')) {
            wcscat(quoted_cmd, L"\"");
            wcscat(quoted_cmd, warg);
            wcscat(quoted_cmd, L"\"");
        } else {
            wcscat(quoted_cmd, warg);
        }

        free(warg);
    }

    return quoted_cmd;
}

wchar_t *resolve_command_path(const char *command, const char *path, const char *pathext) {

    char *dup_pathext = strdup(pathext);
    size_t ext_count = 0;
    char *exttoken = strtok(dup_pathext, ";");
    char *exts[32];
    while (exttoken) {
        exts[ext_count++] = exttoken;
        // shell_print(SHELL_WARN, "%s:%llu\n", exttoken, ext_count);
        if (ext_count >= 32) {
            // shell_print(SHELL_ERROR, "Pathext is too long, MAX: %d, GOT: %llu", 32, ext_count);
            return NULL;
        }
        exttoken = strtok(NULL, ";");
    }



    char *dup_path = strdup(path);
    char *token = strtok(dup_path, ";");
    size_t buf_len = 1024;
    while (token != NULL) {
        // shell_print(SHELL_WARN, "%s\n", token);
        for (size_t i = 0; i < ext_count; ++i) {

            wchar_t wbuf[buf_len];
            swprintf(wbuf, buf_len, L"%hs\\%hs%hs", token, command, exts[i]);

            // shell_print(SHELL_ERROR, "%ls\n", wbuf);

            if (_waccess(wbuf, F_OK) == 0) {
                return _wcsdup(wbuf);
            }
        }

        token = strtok(NULL, ";");
    }

    return NULL;
}

char *wchar_to_char(const wchar_t *wstr) {
    if (!wstr) return NULL;

    int size_needed = WideCharToMultiByte(
      CP_UTF8, //[in]            UINT                               CodePage,
      0, //[in]            DWORD                              dwFlags,
      wstr, //[in]            _In_NLS_string_(cchWideChar)LPCWCH lpWideCharStr,
      -1, //[in]            int                                cchWideChar,
      NULL, //[out, optional] LPSTR                              lpMultiByteStr,
      0, //[in]            int                                cbMultiByte,
      NULL, //[in, optional]  LPCCH                              lpDefaultChar,
      NULL //[out, optional] LPBOOL                             lpUsedDefaultChar
    );

    if (size_needed <= 0) return NULL;

    char *str = malloc(size_needed);
    if (!str) return NULL;

    WideCharToMultiByte(
      CP_UTF8, //[in]            UINT                               CodePage,
      0, //[in]            DWORD                              dwFlags,
      wstr, //[in]            _In_NLS_string_(cchWideChar)LPCWCH lpWideCharStr,
      -1, //[in]            int                                cchWideChar,
      str, //[out, optional] LPSTR                              lpMultiByteStr,
      size_needed, //[in]            int                                cbMultiByte,
      NULL, //[in, optional]  LPCCH                              lpDefaultChar,
      NULL //[out, optional] LPBOOL                             lpUsedDefaultChar
    );

    return str;
}

bool execute_external(int argc, char **argv) {
    // TODO: move down after finding full path since windows emo and doesnt do it itself, atleast didnt find aynthing

    char *command = argv[0];
    char *path = getenv( "Path" );
    char *pathext = getenv( "PATHEXT" );
    // shell_print(SHELL_INFO, "%s\n", pathext);
    if (!path || !pathext) {
        shell_print(SHELL_ERROR, "Missing Path or PATHEXT");
        LOG_ERROR("Missing Path or PATHEXT");
        return false;
    }

    wchar_t *full_cmd_path = resolve_command_path(argv[0], path, pathext);
    if (!full_cmd_path) return false;
    if (shell_config.show_full_cmd_path) shell_print(SHELL_INFO, "%ls\n", full_cmd_path);

    argv[0] = wchar_to_char(full_cmd_path);
    wchar_t *quoted_cmd = quote_cmd(argc, argv);
    if (!quoted_cmd) return NULL;
    // TODO: search app registry and path directories to find command with any pathext

    STARTUPINFOW siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(siStartInfo));
    siStartInfo.cb = sizeof(STARTUPINFOW);

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    BOOL ok = CreateProcessW(
            NULL,           //   [in, optional]      LPCWSTR               lpApplicationName,
            quoted_cmd,     //   [in, out, optional] LPWSTR                lpCommandLine,
            NULL,           //   [in, optional]      LPSECURITY_ATTRIBUTES lpProcessAttributes,
            NULL,           //   [in, optional]      LPSECURITY_ATTRIBUTES lpThreadAttributes,
            FALSE,          //   [in]                BOOL                  bInheritHandles,
            0,              //   [in]                DWORD                 dwCreationFlags,
            NULL,           //   [in, optional]      LPVOID                lpEnvironment,
            NULL,           //   [in, optional]      LPCWSTR               lpCurrentDirectory,
            &siStartInfo,   //   [in]                LPSTARTUPINFOW        lpStartupInfo,
            &piProcInfo     //   [out]               LPPROCESS_INFORMATION lpProcessInformation
    );

    free(quoted_cmd);

    if (!ok) {
        LPWSTR buffer = NULL;
        DWORD err = GetLastError();


        DWORD size = FormatMessageW(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,//[in]           DWORD   dwFlags,
          NULL,     //[in, optional] LPCVOID lpSource,
          err,      //[in]           DWORD   dwMessageId,
          0,        //[in]           DWORD   dwLanguageId,
          (LPWSTR)&buffer,  //[out]          LPWSTR  lpBuffer,
          0,        //[in]           DWORD   nSize,
          NULL      //[in, optional] va_list *Arguments
        );

        if (size > 0) {
            if (buffer[size-2] == L'\r') {
                buffer[size-2] = L'\0';
            } else if (buffer[size-1] == L'\n') {
                buffer[size-1] = L'\0';
            }

            int mb_size = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, NULL, 0, NULL, NULL);
            if (mb_size > 0) {
                char *mb_buffer = (char *)malloc(mb_size);
                if (mb_buffer) {
                    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, mb_buffer, mb_size, NULL, NULL);
                    shell_print(SHELL_ERROR, "Failed to Create Process, %d: %s\n", err, mb_buffer);
                    LOG_ERROR("Failed to Create Process, %d: %s", err, mb_buffer);
                    free(mb_buffer);
                } else {
                    shell_print(SHELL_ERROR, "Failed to Create Process, %d (error string conversion failed)", err);
                    LOG_ERROR("Failed to Create Process, %d (error string conversion failed)", err);
                }
            } else {
                shell_print(SHELL_ERROR, "Failed to Create Process, %d (error string conversion failed)", err);
                LOG_ERROR("Failed to Create Process, %d (error string conversion failed)", err);
            }
        } else {
            shell_print(SHELL_ERROR, "Failed to Create Process, %d (failed to get error string)", err);
            LOG_ERROR("Failed to Create Process, %d (failed to get error string)", err);
        }

        if (buffer) {
            LocalFree(buffer);
        }

        return true; // we print msg from ffailed execution, treat as handled
    }

    DWORD wait = WaitForSingleObject(piProcInfo.hProcess, shell_config.timeout <= 0 ? INFINITE : shell_config.timeout);
    if (wait == WAIT_TIMEOUT) {
        shell_print(SHELL_ERROR, "Timed out, terminating. Wait Timeout: %d ms\n", shell_config.timeout);
        LOG_ERROR("Timed out, terminating. Wait Timeout: %d ms", shell_config.timeout);
        TerminateProcess(piProcInfo.hProcess, 1);
        WaitForSingleObject(piProcInfo.hProcess, INFINITE);
    }

    DWORD exit_code = 1;
    if (!GetExitCodeProcess(piProcInfo.hProcess, &exit_code)) {
        LOG_ERROR("GetExitCodeProcess failed: %lu", GetLastError());
        exit_code = 1;
    }

    if (shell_config.show_exit_code) {
        shell_print(SHELL_INFO, "%s exited with Exit Code: %u\n", argv[0], exit_code);
    }

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return true;
}

#endif
