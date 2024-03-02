#include "functions.h"
#include "conversions.h"

//represents the total present files in the directory
uint64_t filecount = 0;
//represents the total present folders in the directory
uint64_t foldercount = 0;


//represents the final amount of searched files by all threads
uint64_t processedFiles = 0;
//represents the final amount of searched folders by all threads
uint64_t processedFolders = 0;


//At any point in time this represents the total number of skipped files combined by all threads.
uint64_t skipped_files = 0;
//At any point in time this represents the total number of skipped folders combined by all threads.
uint64_t skipped_folders = 0;


//At any point in time this represents the total number of searched and skipped files combined by all threads.
uint64_t currentfilecount = 0;
//At any point in time this represents the total number of searched and skipped folders combined by all threads.
uint64_t currentfoldercount = 0;


//At any point in time this represents the ratio between the currentfilecount and the totalfilecount in percent
double percentage = 0;






//bool skipFiles(std::filesystem::recursive_directory_iterator& iter, std::error_code& ec, uint64_t& p_free_threads)
//{
//	for (int i = 0; i < p_free_threads - 1; i++) //skips x files every turn so all files only get processesed by one thread ever
//		iter.increment(ec);
//	return true;
//}



//given a percentage it prints a nice formatted progressbar to the console

//TODO	Let it track a static uint64_t of how many chars have been written the last time, then just set position to 0,0 and overwrite the locations to whatever string to write
//		is longer the old one or the new one
//void printProgress(double& percentage)
//{
//	auto start = std::chrono::high_resolution_clock::now();
//	auto stop = std::chrono::high_resolution_clock::now();
//	auto duration = duration_cast<std::chrono::microseconds>(stop - start);
//
//	while (percentage < 98)
//	{
//		stop = std::chrono::high_resolution_clock::now();
//		duration = duration_cast<std::chrono::microseconds>(stop - start);
//		if (duration.count() >= 300000)
//		{
//			start = std::chrono::high_resolution_clock::now();
//		}
//		else
//			continue;
//
//		double maxPrint = 20;
//		double toPrint = percentage / 5;
//		system("CLS");
//		std::cout << "[";
//		for (uint64_t i = 0; i < toPrint; i++)
//		{
//			std::cout << '\|';
//			maxPrint--;
//		}
//		for (uint64_t i = 0; i < maxPrint; i++)
//		{
//			std::cout << ' ';
//		}
//		std::cout << "]" << percentage << "%\n";
//	}
//	system("CLS");
//	std::cout << "Listing...\n";
//}

//void printProgress(double& percentage)
//{
//	while (percentage < 100)
//	{
//		double maxPrint = 20;
//		double toPrint = percentage / 5;
//		system("CLS");
//		std::cout << "[";
//		for (uint64_t i = 0; i < toPrint; i++)
//		{
//			std::cout << '\|';
//			maxPrint--;
//		}
//		for (uint64_t i = 0; i < maxPrint; i++)
//		{
//			std::cout << ' ';
//		}
//		std::cout << "]" << percentage << "%\n";
//		std::this_thread::sleep_for(std::chrono::milliseconds(500));
//	}
//	system("CLS");
//}




bool validateInputStringForInitialInput(std::string input, std::vector<std::string>& output)
{
	if (input.empty())
		return true;
	else
		output.push_back(to_lower_string(input));
	return false;
}



std::string getFolderPath(std::string pathOfFile)
{
	std::string pathOfFolder = "";
	for (uint64_t i = 0; i < pathOfFile.find_last_of("\\"); i++)
	{
		pathOfFolder += pathOfFile.at(i);
	}
	return pathOfFolder;
}



void BrowseToFile(LPCWSTR filename)
{
	ITEMIDLIST* pidl = ILCreateFromPathW(filename);
	if (pidl) {
		SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
		ILFree(pidl);
	}
}

void BrowseToFolder(LPCWSTR filename) //thx to "Jonathan Potter" on stackoverflow
{
	PIDLIST_ABSOLUTE pidl;
	if (SUCCEEDED(SHParseDisplayName(filename, 0, &pidl, 0, 0)))
	{
		// we don't want to actually select anything in the folder, so we pass an empty
		// PIDL in the array. if you want to select one or more items in the opened
		// folder you'd need to build the PIDL array appropriately
		ITEMIDLIST idNull = { 0 };
		LPCITEMIDLIST pidlNull[1] = { &idNull };
		SHOpenFolderAndSelectItems(pidl, 1, pidlNull, 0);
		ILFree(pidl);
	}
}



//validates inputstring (returns false if wrong input) and transfers input string to vector
bool validateInputStringForOpeningFinalEntriesInExplorer(std::string input, std::vector<uint64_t>& output)
{
	std::string set = "";
	for (uint64_t i = 0; i < input.length(); i++)
	{
		if (isdigit(input.at(i)))
		{
			set += (input.at(i));
		}
		else
			if (input.at(i) == ',' || input.at(i) == ' ')
			{
				output.push_back(stoi(set));
				set = "";
				continue;
			}
			else
				return false;
	}
	if (set != "")
	{
		output.push_back(stoi(set));
	}
	return true;
}




void display(
	uint64_t processedFiles,
	uint64_t processedFolders,
	auto seconds,
	std::vector<std::filesystem::directory_entry>& vec_folder_path,
	std::vector<std::filesystem::directory_entry>& vec_content_path,
	std::vector<std::filesystem::directory_entry>& vec_file_path)

{
	std::vector<uint64_t> vec_to_be_opened; //storing the identifier for files/folders to be opened
	system("CLS");
	uint64_t i = 0;

	std::cout << "\nProcessed an astounding " << processedFiles << " files (of " << filecount << ") inside of " << processedFolders << " folders in just " << seconds / 1000000 << " seconds.\nSkipped Files : " << skipped_files << "\nSkipped Folders : " << skipped_folders << "\n\n";



	if (!vec_folder_path.empty())
	{
		std::cout << "Found Folders\n****************************************************************\n";
		for (uint64_t j = 0; j < vec_folder_path.size(); j++, i++)
			std::cout << i + 1 << ": " << wide_string_to_string(vec_folder_path.at(j).path().wstring()) << "\"" << std::endl;
		std::cout << "****************************************************************\n" << std::endl;
	}
	if (!vec_file_path.empty())
	{
		std::cout << "Found Files\n****************************************************************\n";
		for (uint64_t j = 0; j < vec_file_path.size(); j++, i++)
			std::cout << i + 1 << ": " << wide_string_to_string(vec_file_path.at(j).path().wstring()) << "\"" << std::endl;
		std::cout << "****************************************************************\n" << std::endl;
	}
	if (!vec_content_path.empty())
	{
		std::cout << "Found Content\n****************************************************************\n";
		for (uint64_t j = 0; j < vec_content_path.size(); j++, i++)
			std::cout << i + 1 << ": " << wide_string_to_string(vec_content_path.at(j).path().wstring()) << "\"" << std::endl;
		std::cout << "****************************************************************\n" << std::endl;
	}
	std::string to_be_opened = ""; //Stringinput of what files the user wants to be opened

	if (!(vec_content_path.empty() && vec_file_path.empty() && vec_folder_path.empty()))
	{
		do
		{
			std::cout << "What should be opened? Input-Example: \"1,2,3,40,53\"" << std::endl;

			std::getline(std::cin, to_be_opened);

		} while (!validateInputStringForOpeningFinalEntriesInExplorer(to_be_opened, vec_to_be_opened));
	}
	else
	{
		std::cout << "No files match the searching criteria :(" << std::endl;
		std::cout << "Press \"Enter\" to exit.";
		std::cin.ignore();
		exit;
	}


	for (auto& it : vec_to_be_opened)
	{
		if (vec_folder_path.size() >= it)
		{
			auto x = (vec_folder_path.at(it - 1));//	(LPCSTR((char*)(filesystem::path(vec_folder_path.at(it - 1)).c_str())))
			std::cout << x << "\n";
			std::string askdjasd = (wide_string_to_string(std::filesystem::path(x).wstring()) + "\\\\");
			const char* y = askdjasd.c_str();


			ShellExecuteA(NULL, "open", y, NULL, NULL, SW_SHOWDEFAULT);
		}
		else
			if (vec_file_path.size() + vec_folder_path.size() >= it)
			{
				BrowseToFile(LPCTSTR((char*)(std::filesystem::path(vec_file_path.at(it - vec_folder_path.size() - 1)).c_str())));
			}
			else
				if (vec_content_path.size() + vec_file_path.size() + vec_folder_path.size() >= it)
				{
					BrowseToFile(LPCTSTR((char*)(std::filesystem::path(vec_content_path.at(it - 1 - vec_folder_path.size() - vec_file_path.size())).c_str())));
				}


	}

}




void startWinXSearch(const std::filesystem::path pathToFolder, bool searchFolders, bool searchContent, std::vector<std::string> vecSearchValue)
{
	auto start = std::chrono::high_resolution_clock::now();

	uint64_t current_thread_count = 1;
	uint64_t processor_count = std::thread::hardware_concurrency();

	debug_log << "Found " << processor_count << " CPUs.\n";


	if (!(std::filesystem::directory_entry(pathToFolder.c_str()).is_directory()))
	{
		std::cout << "PATH IS NOT A DIRECTORY!" << std::endl;
		std::cin.ignore();
		abort;
	}



	std::vector<std::filesystem::directory_entry> vec_folder_path; //stores folder paths of folders including atleast one of the searched strings
	std::vector<std::filesystem::directory_entry> vec_content_path; //stores file paths of files whichs content includes atleast one of the searched strings
	std::vector<std::filesystem::directory_entry> vec_file_path; //stores file paths of files including atleast one of the searched strings


	std::vector<std::thread>thread_list;


	auto ec = std::error_code();

	//Calculates Amount of Files
	std::cout << "Scanning files\n";
	for (std::filesystem::recursive_directory_iterator iter(pathToFolder); iter != std::filesystem::end(iter); iter.increment(ec))
	{
		if (ec)
		{
			continue;
		}
		if ((*iter).is_regular_file())
			filecount++;
		else if ((*iter).is_directory())
			foldercount++;
	}


	//	//Starts ProgressBar Thread
	//#ifndef NDEBUG
	//	std::thread t1(printProgress, std::ref(percentage));
	//	thread_list.push_back(std::move(t1));
	//	current_thread_count++;
	//#endif

	debug_log << "Starting big_for_loop\n";

	//std::thread tnew(big_for_loop,
	//	1,
	//	1,
	//	std::ref(currentfilecount),
	//	std::ref(pathToFolder),
	//	std::ref(percentage),
	//	std::ref(filecount),
	//	std::ref(searchFolders),
	//	std::ref(searchContent),
	//	std::ref(vecSearchValue),
	//	std::ref(vec_folder_path),
	//	std::ref(vec_content_path),
	//	std::ref(vec_file_path));

	//thread_list.push_back(std::move(tnew));
	//current_thread_count++;




	for (uint64_t i = 1; i <= processor_count; i++)
	{
		std::thread tnew(big_for_loop,
			i,
			std::ref(processor_count),
			std::ref(currentfilecount),
			std::ref(pathToFolder),
			std::ref(percentage),
			std::ref(filecount),
			std::ref(searchFolders),
			std::ref(searchContent),
			std::ref(vecSearchValue),
			std::ref(vec_folder_path),
			std::ref(vec_content_path),
			std::ref(vec_file_path));

		thread_list.push_back(std::move(tnew));
		current_thread_count++;
	}



	for (uint64_t i = 1; i < thread_list.size(); i++)
	{
		if (thread_list.at(i).joinable())
		{
			thread_list.at(i).join();
			percentage = (processedFiles + skipped_files) / (filecount) * 100;
		}
	}
	percentage = 100;
	thread_list.at(0).join();



	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = duration_cast<std::chrono::microseconds>(stop - start);

	display(processedFiles, processedFolders, duration.count(), vec_folder_path, vec_content_path, vec_file_path);
}


void big_for_loop(uint64_t p_i, uint64_t total_threads, uint64_t& currentfilecount, const std::filesystem::path pathToFolder, double& percentage, uint64_t& total_files_to_search, bool& searchFolders, bool& searchContent,
	std::vector<std::string>& vecSearchValue,
	std::vector<std::filesystem::directory_entry>& vec_folder_path,
	std::vector<std::filesystem::directory_entry>& vec_content_path,
	std::vector<std::filesystem::directory_entry>& vec_file_path)
{
	//tracks this threads processed files

	//tracks this threads skipped files


	bool onetime_skipped = false;
	// Iterates over every file/folder in the path of the executable and its subdiretories

	debug_log << "Started new Thread with: p_i:" << p_i << ", Total_Threads:" << total_threads << ".\n";




	//file iterator loop

		// Check if the given path exists and is a directory
	if (std::filesystem::exists(pathToFolder) && std::filesystem::is_directory(pathToFolder))
	{
		for (std::filesystem::recursive_directory_iterator dirIter(pathToFolder), end; dirIter != end; ++dirIter)
		{

			// Print the path of the current file or directory
			try
			{
				//the current file
				const auto& entry = *dirIter;

				/*
					This if/else block implements a logic based parallelism.
					For X threads introduced every thread skipps X files continously. By that every file gets processed excactly once.
					At the very beginning the first thread doesn't skip at all, the second thread skips 1 file ... the last created thread skips X-1 files.
					This approach supersedes runtime assinging of files to specific threads, but can lead to some threads having a bigger load to bear than others (randomly gets assigned 2 of the largest files).
					This downside is only a potential issue when filecontentsearch is enabled.
				*/
				if (onetime_skipped)
				{	//Continued Skipping for each Iteration (every thread skips the threadsamount of files, so that each file only is searched by one thread during the whole runtime
					uint64_t x = 1;
					for (; x < total_threads; x++)
					{
						++dirIter;
						if (dirIter == end) //we reached the End of this threads file iteration, gotta do a final update of processed files etc
						{
							return;
						}
					}
					if (x > 1) debug_log << "Thread " << p_i << " skipped " << x << " files at this iteration.\n";
				}
				else
				{	//onetime skip at the start for each thread (first thread doesnt skip a file ; the 5. does skip 4 files
					uint64_t counter = 0;
					for (uint64_t i = 0; i < (p_i - 1); i++)
					{
						++dirIter;
						if (dirIter == end) //we reached the End of this threads file iteration, gotta do a final update of processed files etc
						{
							return;
						}
						counter++;
					}
					debug_log << "Thread Nr.:" << p_i << " skipped " << counter << " files at the start.\n";
					//Now we did our one time skip, gotta let the loop know that.
					onetime_skipped = true;
					//we gotta keep going, because otherwise the first (total_threads) files would get ignored.
				}



				debug_log << "Thread Nr.:" << p_i << " is processing File: \"" << entry.path().string() << "\"\n";



				//Skip if it isnt a file or a folder
				if (!is_regular_file(entry) && !is_directory(entry))
				{
					debug_log << "Thread Nr.:" << p_i << " skipped Item: \"" << entry.path().string() << "\" Reason: Symlink\n";

					continue;
				}


				bool found = false;

				if (std::filesystem::is_directory(entry)) // optional: Comparing Folder Name
				{
					if (searchFolders == false)
					{
						debug_log << "Skipped Folder: \"" << entry.path().string() << "\" Reason: FolderNameSearch deactivated\n";
						continue;
					}

					debug_log << "Processing Folder: \"" << entry.path().string() << "\"\n";

					for (auto& it : vecSearchValue)
					{
						if (std::string::npos != to_lower_string(entry.path().filename().string()).find(to_lower_string(it)))
						{
							vec_folder_path.push_back(entry);
							found = true;
							break;
						}
					}
					continue;
				}
				else //since its not a directory, it has to be a file
				{
					debug_log << "Processing File: \"" << entry.path().string() << "\"\n";

					//Comparing File Name
					for (auto& it : vecSearchValue)
					{
						if (std::string::npos != to_lower_string(entry.path().filename().string()).find(it)) {
							vec_file_path.push_back(entry);
							found = true;
							break;
						}
					}

					if (found || searchContent == false) //continue iterating if search on filename was successfull or if content search is disabled
						continue;

					//Open File
					std::ifstream  inputfile;
					inputfile.open(entry, std::ifstream::in);

					// Check File State
					if (!inputfile || inputfile.rdstate() != std::ios_base::goodbit)
					{
						debug_log << "Skipped Content Search on File: \"" << entry.path().string() << "\" Reason: File was not ready\n";

						continue;
					}

					//Compare File Content
					if (inputfile.good() && inputfile) // optional: filecontent as target
					{

						std::string checkedString;

						while (std::getline(inputfile, checkedString)) // Every line of the inputfile gets its own string in the vector
						{
							for (auto& searchIt : vecSearchValue)
							{
								if (std::string::npos != to_lower_string(checkedString).find(searchIt))
								{
									vec_content_path.push_back(entry);
									found = true;
									break;
								}
							}
							if (found)
								break;
						}
						inputfile.close();

					}
					else
					{
						try { inputfile.close(); }
						catch (const std::exception&) { std::cout << "Couldnt close file!" << std::endl; }
					}


					continue;
				}
			}
			catch (const std::filesystem::filesystem_error& e) {
				std::cerr << e.what() << std::endl;
				continue;
			}
			catch (const std::exception& e) {
				std::cerr << "An error occurred: " << e.what() << std::endl;
				continue;
			}

		}
	}
	else {
		std::cout << "The path does not exist or is not a directory." << std::endl;
	}
}