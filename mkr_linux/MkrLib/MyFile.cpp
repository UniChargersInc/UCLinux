#include "MkrLib.h"
#include <pthread.h>
#ifdef WIN32
#include <Windows.h>
#endif
#include <mutex>
#ifdef WIN32
#include <direct.h>    //_mkdir
#include "dirent.h"   // to make a directory listing //https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
#else
#include <sys/types.h>
#include <dirent.h>
#endif

//==============================================================
static std::mutex sat_w_mutex;
/*
static bool create_dir(std::string &dir_name)
{
#ifdef WIN32
	int rv = _mkdir(dir_name.c_str());
	if (rv == 0)
	{
		return true;   // OK dir created
	}
	else {
		if (errno == EEXIST)
			return true;
	}
	return false;
#else
#endif
}
*/

int getDirectoryListing(std::string &dir_path,std::vector<std::string> &v)
{
	int rv = 0;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(/*"c:\\src\\"*/dir_path.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			
			if (S_ISDIR(ent->d_type))
			{
				//printf("%s\n", ent->d_name);
				std::string name = std::string(ent->d_name);
				v.push_back(name);
				rv++;
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		
	}
	return rv;
}

bool isDirExist(const std::string& path)
{
#if defined(_WIN32)
	struct _stat info;
	if (_stat(path.c_str(), &info) != 0)
	{
		return false;
	}
	return (info.st_mode & _S_IFDIR) != 0;
#else 
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
	{
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
#endif
}
bool makePath(const std::string& path)
{
#if defined(_WIN32)
	int ret = _mkdir(path.c_str());
#else
	mode_t mode = 0755;
	int ret = mkdir(path.c_str(), mode);
#endif
	if (ret == 0)
		return true;

	switch (errno)
	{
	case ENOENT:
		// parent didn't exist, try to create it
	{
		int pos = path.find_last_of('/');
		if (pos == std::string::npos)
#if defined(_WIN32)
			pos = path.find_last_of('\\');
		if (pos == std::string::npos)
#endif
			return false;
		if (!makePath(path.substr(0, pos)))
			return false;
	}
	// now, try to create again
#if defined(_WIN32)
	return 0 == _mkdir(path.c_str());
#else 
	return 0 == mkdir(path.c_str(), mode);
#endif

	case EEXIST:
		// done!
		return isDirExist(path);

	default:
		return false;
	}
}

//https://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux

//https://stackoverflow.com/questions/3071665/getting-a-directory-name-from-a-filename

bool SplitFilename2Dir(const std::string& full_path, std::string &dir, std::string &filename)
{
	bool rv = false;
	size_t found;
	//cout << "Splitting: " << str << endl;
#ifdef WIN32
	found = full_path.find_last_of("/\\");
#else
	found = full_path.find_last_of("/");
#endif
	if (found != string::npos)
	{
		//cout << " folder: " << str.substr(0, found) << endl;
		//cout << " file: " << str.substr(found + 1) << endl;
		dir = full_path.substr(0, found);
		filename = full_path.substr(found + 1);
		rv = true;
	}
	return rv;
}

int write_to_file_dir_check(string &fname, string &payload) {
	int rv = 0;
	std::lock_guard<std::mutex> lock(sat_w_mutex);

	std::string dir, filename;
	bool rv1 = SplitFilename2Dir(fname, dir, filename);
	if (!rv1)
	{
		//printf("could not split flename:%s \r\n", fname.c_str());
		return 0;
	}
	rv1 = makePath(dir);
	if (!rv1)
	{
		//printf("could not make dir:%s \r\n", dir.c_str());
		return 0;
	}

	FILE * pFile;

	pFile = fopen(fname.c_str(), "w+b");
	if (pFile != NULL)
	{
		rv = fwrite(payload.data(), 1, payload.size(), pFile);
		fclose(pFile);
	}
	//printf("wrote ti file:%s  size:%d\r\n", fname.c_str(), rv);
	return rv;

}



//==============================================================
static bool initialized = false;
static pthread_mutex_t lockMutex;
static bool isInitialized()
{
	if (!initialized)
	{
		if (pthread_mutex_init(&lockMutex, NULL) != 0)
		{
			//printf("\n mutex init failed\n");
			return false;
		}
	}
	initialized = true;
	return true;
}
int write_to_file(string &fname, string &payload) {
	int rv = 0;
	if (isInitialized())
	{
		pthread_mutex_lock(&lockMutex);
		FILE * pFile;
		
		pFile = fopen(fname.c_str(), "w+b");
		if (pFile != NULL)
		{
			rv = fwrite(payload.data(), 1, payload.size(), pFile);
			fclose(pFile);
		}
		pthread_mutex_unlock(&lockMutex);
	}
	return rv;

}
int write_to_file(const char* fname,const std::stringstream &payload)
{
	int rv = 0;
	if (isInitialized())
	{
		pthread_mutex_lock(&lockMutex);
		FILE * pFile;

		pFile = fopen(fname, "w+b");
		if (pFile != NULL)
		{
			const string &str = payload.str();
			rv = fwrite(str.data(), 1, str.size(), pFile);
			fclose(pFile);
		}

		
		pthread_mutex_unlock(&lockMutex);
	}
	return rv;
}

//==========================================================================
//======================Read Html file================================
//char* read_html_file(char *full_path) 
//{
//
//	int size = 0;
//	char *pChar =(char*) read_bin_file(full_path, size);
//	if (pChar != 0 && size > 0)
//		pChar[size] = 0;     // MUST be a null-terminated response !!!!
//	return pChar;
//}
int read_html_file(char *full_path, string &str)
{
	//ifstream fs(full_path);
	//	//streamsize size = fs.tellg();
	//	stringstream strStream;
	//	strStream << fs.rdbuf();//read the file
	//	out = strStream.str();//str holds the content of the file
	//	return 1;

	std::ifstream t(full_path);
	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return 1;
}
unsigned char* read_bin_file(const char *full_path, int &length)
{
	streampos size;
	unsigned char * memblock = 0;
	//int maxsize;

	ifstream file(full_path, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		//if (size > maxsize)
		//	size = maxsize;
		length = size;
		memblock = new unsigned char[length];  // +1 to make null-terminated read_html_file response
		file.seekg(0, ios::beg);
		file.read((char*)memblock, size);
		file.close();

	}

	return memblock;
}

int read_bin_file_to_buf(const char *full_path, unsigned char *pBuf, int max_size,int offset)
{
	int rv = 0;
	streampos size;
	ifstream file(full_path, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		if (max_size == 0)
			size = file.tellg();
		else
			size = max_size;
		file.seekg(/*0*/offset, ios::beg);
		file.read((char*)pBuf, size);
		file.close();
		rv = size;
	}

	return rv;
}



int read_bin_file2buf(char *full_path, unsigned char** buf)
{
	streampos size;
	int len = 0;
	ifstream file(full_path, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		//if (size > maxsize)
		//	size = maxsize;
		len = size;
		*buf = new unsigned char[len];  // +1 to make null-terminated read_html_file response
		file.seekg(0, ios::beg);
		file.read((char*)*buf, size);
		file.close();

	}
	return len;
}
/*
int read_html_file2buf(char *full_path, unsigned char** buf)
{
	streampos size;
	int len = 0;
	std::ifstream t(full_path);
	t.seekg(0, std::ios::end);
	size=t.tellg();
	len = size;
	t.seekg(0, std::ios::beg);
	*buf = new unsigned char[len];
	t.read((char*)*buf, size);
	return len;
}
*/
int read_html_file2buf(char *full_path, unsigned char** buf)
{
	unsigned char *p = 0;
	size_t newLen = 0;
	FILE *fp = fopen(full_path, "r");
	if (fp != NULL) {
		/* Go to the end of the file. */
		if (fseek(fp, 0L, SEEK_END) == 0) {
			/* Get the size of the file. */
			long bufsize = ftell(fp);
			if (bufsize == -1) { return -1; /* Error */ }

			/* Allocate our buffer to that size. */
			p = new unsigned char[bufsize+1];

			/* Go back to the start of the file. */
			if (fseek(fp, 0L, SEEK_SET) != 0) { return -1; /* Error */ }

			/* Read the entire file into memory. */
			newLen = fread(p, 1, bufsize, fp);
			if (ferror(fp) != 0) {
				return -1;
			}
			else {
				p[newLen++] = '\0'; /* Just to be safe. */
			}
		}
		fclose(fp);
	}
	*buf = p;
	return newLen;
}

//to comply with WIN32 
int delete_file(const char* path)  // DeleteFile
{
#ifndef WIN32
	return remove(path);  // does not work on windows
#else
	std::string paths = path;
	std::wstring wFileName(paths.begin(), paths.end());
	DeleteFile(wFileName.c_str());
	return 1;
#endif

}

bool M_PathFileExists(char *path, bool remove_file_name)
{
	if (remove_file_name)
	{
		std::string pathf = path;
		const size_t last_slash_idx = pathf.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			pathf = pathf.substr(0, last_slash_idx);
		}

		struct stat buf;
		return (stat(pathf.c_str(), &buf) == 0);

	}

	
	//ifstream my_file(path);
	//if (my_file.good())
	//{
	//	// read away
	//}

	//Note that these function to not check if the file is a normal file. 
	//They just check if something (a normal file, a UNIX domain socket, a FIFO, a device file etc.) with the given name exists
	struct stat buf;
	return (stat(path, &buf) == 0);

}
int getSizeOfFile(const char *full_path)
{
	struct stat buf;
	if (stat(full_path, &buf) == 0)
	{
		return buf.st_size;
	}
	return -1;
}
/*
m_hMsgFile = CreateFile(m_szFileName,
GENERIC_WRITE, FILE_SHARE_READ,
NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
*/

int M_CreateFile(char *path)
{
	int fd = open(path, O_CREAT + O_WRONLY + O_APPEND, 0666);
	return fd;
}

//-------------------------------------------------------------------------------
int cp(const char *to, const char *from)
{
	int fd_to, fd_from;
	char buf[4096];
	ssize_t nread;
	int saved_errno;

	fd_from = open(from, O_RDONLY);
	if (fd_from < 0)
		return -1;

	fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd_to < 0)
		goto out_error;

	while (nread = read(fd_from, buf, sizeof buf), nread > 0)
	{
		char *out_ptr = buf;
		ssize_t nwritten;

		do {
			nwritten = write(fd_to, out_ptr, nread);

			if (nwritten >= 0)
			{
				nread -= nwritten;
				out_ptr += nwritten;
			}
			else if (errno != EINTR)
			{
				goto out_error;
			}
		} while (nread > 0);
	}

	if (nread == 0)
	{
		if (close(fd_to) < 0)
		{
			fd_to = -1;
			goto out_error;
		}
		close(fd_from);

		/* Success! */
		return 0;
	}

out_error:
	saved_errno = errno;

	close(fd_from);
	if (fd_to >= 0)
		close(fd_to);

	errno = saved_errno;
	return -1;
}

bool M_CopyFile(
	char* lpExistingFileName,
	char* lpNewFileName,
	bool    bFailIfExists
)
{
	//bFailIfExists [in]
	//If this parameter is TRUE and the new file specified by lpNewFileName already exists, the function fails.
	//If this parameter is FALSE and the new file already exists, the function overwrites the existing file and succeeds.
	bool new_exists = M_PathFileExists(lpNewFileName);
	if (bFailIfExists&&new_exists)
		return false;
	
	if (new_exists)
		delete_file(lpNewFileName);
	int rv=cp(lpNewFileName, lpExistingFileName);
	return (rv == 0);
}
bool M_WriteFile(
	int       hFile,
	unsigned char*      lpBuffer,
	unsigned int       nNumberOfBytesToWrite,
	unsigned int*      lpNumberOfBytesWritten,
	void* lpOverlapped
)
{
	ssize_t rv=write(hFile, (unsigned char*)lpBuffer, nNumberOfBytesToWrite);
	*lpNumberOfBytesWritten =(int) rv;
	return (rv > 0);
}

/*
WriteFile(m_hMsgFile,buf,dwIn, &dwOut,NULL)
*/