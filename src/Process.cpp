/*************************************************************************
** Process.cpp                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/


#ifdef __WIN32__
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#endif

#include <cstdlib>
#include "FileSystem.h"
#include "Process.h"
#include "SignalHandler.h"
#include "macros.h"

using namespace std;

Process::Process (const string &cmd, const string &paramstr)
	: _cmd(cmd), _paramstr(paramstr)
{
}


#ifndef __WIN32__
/** Extracts whitespace-sparated parameters from a string.
 *  @param[in] paramstr the parameter string
 *  @param[out] params vector holding the extracted parameters */
static void split_paramstr (string paramstr, vector<const char*> &params) {
	size_t left=0, right=0;  // index of first and last character of current parameter
	char quote=0;            // current quote character, 0=none
	const size_t len = paramstr.length();
	while (left <= right && right < len) {
		while (left < len && isspace(paramstr[left]))
			++left;
		if (left < len && (paramstr[left] == '"' || paramstr[left] == '\''))
			quote = paramstr[left++];
		right = left;
		while (right < len && (quote || !isspace(paramstr[right]))) {
			if (quote && paramstr[right] == quote) {
				quote=0;
				break;
			}
			else
				++right;
		}
		if (right < len)
			paramstr[right]=0;
		if (left < len)
			params.push_back(&paramstr[left]);
		left = ++right;
	}
}
#endif


/** Runs the process and waits until it's finished.
 *  @param[in] quiet if true, output to stdout/stderr is suppressed
 *  @return true if process terminated properly
 *  @throw SignalException if CTRL-C was pressed during execution */
bool Process::run (bool quiet) {
#ifdef __WIN32__
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = true;

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	HANDLE devnull = CreateFile("nul", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdOutput = quiet ? devnull : GetStdHandle(STD_OUTPUT_HANDLE);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	string cmdline = _cmd+" "+_paramstr;
	CreateProcess(NULL, (LPSTR)cmdline.c_str(), NULL, NULL, true, 0, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD exitcode = (DWORD)-1;
	GetExitCodeProcess(pi.hProcess, &exitcode);
	CloseHandle(devnull);
	return exitcode == 0;
#else
	pid_t pid = fork();
	if (pid == 0) {   // child process
		if (quiet) {
			int devnull = open(FileSystem::DEVNULL, O_WRONLY);
			if (devnull >= 0) {
				dup2(devnull, STDOUT_FILENO);
				dup2(devnull, STDERR_FILENO);
				close(devnull);
			}
		}
		vector<const char*> params;
		params.push_back(_cmd.c_str());
		split_paramstr(_paramstr, params);
		params.push_back(0);     // trailing NULL marks end
		execvp(_cmd.c_str(), const_cast<char* const*>(&params[0]));
		exit(1);
	}
	if (pid > 0) {    // main process
		int status;
		for (;;) {
			waitpid(pid, &status, WNOHANG);
			if (WIFEXITED(status)) // child process exited normally
				return WEXITSTATUS(status) == 0;

			try {
				SignalHandler::instance().check();
			}
			catch (SignalException &e) { // caught ctrl-c
				kill(pid, SIGKILL);
				throw e;
			}
		}
	}
	return false;
#endif
}

