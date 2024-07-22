#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#include <shlobj.h>

#include <iostream>
#include <fstream>

#include <string>
#include <vector>

#pragma comment(lib, "Shlwapi.lib")

namespace serialization_utils
{
	enum Priority : int
	{
		High = 2,
		Medium = 1,
		Low = 0
	};

	struct TODO_Element
	{
		std::string content;
		Priority priority;
	};

	// All TODO elements stores here
	inline std::vector<TODO_Element> TODO_elements;

	// Ñhecks if the serialization file exists,
	// if it does not, creates it,
	// then read TODO elements
	inline auto CheckAndReadSerializationFile() -> bool
	{
		char DocumentsPath[MAX_PATH];
		std::string SeriFile_path;

		// Get Documents path
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, DocumentsPath);

		if (!SUCCEEDED(result))
		{
			return false;
		}

		SeriFile_path = DocumentsPath;
		SeriFile_path += "\\TODO-app\\";

		// Checking if "...\Documents\TODO-app\" folder exists
		if (PathFileExists(SeriFile_path.c_str()) && PathIsDirectory(SeriFile_path.c_str()))
		{
			SeriFile_path += "TODO-Serialization.bin";

			// Checking if TODO-Serialization.bin exists
			if (PathFileExists(SeriFile_path.c_str()) && !PathIsDirectory(SeriFile_path.c_str()))
			{
				// Reading serialization file
				std::ifstream SeriFile_r(SeriFile_path, std::ios::binary);

				if (!SeriFile_r.is_open())
				{
					return false;
				}

				uint64_t ArrSize = 0;
				SeriFile_r.read(reinterpret_cast<char*>(&ArrSize), sizeof(ArrSize));

				if (ArrSize > 0)
				{
					//std::vector<TODO_Element> tempArr1(ArrSize);

					for (size_t i = 0; i < ArrSize; ++i)
					{
						TODO_Element element{};

						size_t contentSize;
						SeriFile_r.read(reinterpret_cast<char*>(&contentSize), sizeof(contentSize));
						element.content.resize(contentSize);
						SeriFile_r.read(&element.content[0], contentSize);

						int priorityValue;
						SeriFile_r.read(reinterpret_cast<char*>(&priorityValue), sizeof(priorityValue));
						element.priority = static_cast<Priority>(priorityValue);

						TODO_elements.push_back(element);
					}
					SeriFile_r.close();
				}

				return true;
			}
			else
			{
				// Creating file
				std::ofstream file(SeriFile_path);
				file.close();
			}
		}
		else
		{
			if (!(CreateDirectory(SeriFile_path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS))
			{
				return false;
			}

			SeriFile_path += "TODO-Serialization.bin";

			// Creating file
			std::ofstream file(SeriFile_path);
			file.close();
		}
	}

	inline auto SaveToSerializationFile() -> bool
	{
		/*
		if (TODO_elements.size() <= 0)
		{
			return true;
		}
		*/

		char DocumentsPath[MAX_PATH];
		std::string SeriFile_path;

		// Get Documents path
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, DocumentsPath);

		if (!SUCCEEDED(result))
		{
			return false;
		}

		SeriFile_path = DocumentsPath;
		SeriFile_path += "\\TODO-app\\TODO-Serialization.bin";

		std::ofstream SeriFile_w(SeriFile_path, std::ios::binary);

		if (!SeriFile_w.is_open())
		{
			return false;
		}

		uint64_t ArrSize = TODO_elements.size();
		SeriFile_w.write(reinterpret_cast<char*>(&ArrSize), sizeof(ArrSize));

		for (const TODO_Element& element : TODO_elements)
		{
			uint64_t contentSize = element.content.size();
			SeriFile_w.write(reinterpret_cast<char*>(&contentSize), sizeof(contentSize));
			SeriFile_w.write(element.content.c_str(), contentSize);


			int priorityValue = static_cast<int>(element.priority);
			SeriFile_w.write(reinterpret_cast<char*>(&priorityValue), sizeof(priorityValue));
		}

		SeriFile_w.close();

		return true;
	}
}