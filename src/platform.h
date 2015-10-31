////////////////////////////////////////////////////////////////////////////////
//
// SmokeSignal - LocalChat standard compliant application
//
// Copyright (C) 2010-2013 Joseph Kuziel and Micca Osmar
//
// This file is part of SmokeSignal.
//
// SmokeSignal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SmokeSignal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SmokeSignal.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////


#if defined(__gnu_linux__) || defined(__linux__) || defined(__unix__) || \
	defined(__unix)

#define SMOKESIGNAL_PLATFORM_UNIX

#elif defined(__APPLE__) && defined(__MACH__)

#define SMOKESIGNAL_PLATFORM_MACOS

#elif defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__) || \
	defined(__MINGW32__)

#define SMOKESIGNAL_PLATFORM_WINDOWS

#endif
