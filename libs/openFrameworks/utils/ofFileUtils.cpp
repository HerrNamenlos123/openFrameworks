#include "ofFileUtils.h"
#include "ofLog.h"

#ifndef TARGET_WIN32
	#include <pwd.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

#ifdef TARGET_OSX
	#include <mach-o/dyld.h>       /* _NSGetExecutablePath */
	#include <limits.h>        /* PATH_MAX */
#endif

// FIXME: better explicit declaration
using std::string;
using std::vector;
using std::fstream;
using std::istream;
using std::ostream;
using std::ios;

namespace{
	bool enableDataPath = true;

	//--------------------------------------------------
//	MARK: - near future
//	of::filesystem::path defaultDataPath(){
	std::string defaultDataPath(){
	#if defined TARGET_OSX
		try{
			return of::filesystem::canonical(ofFilePath::getCurrentExeDir() / of::filesystem::path("../../../data/")).string();
		}catch(...){
			return (ofFilePath::getCurrentExeDir() / of::filesystem::path("../../../data/")).string();
		}
	#elif defined TARGET_ANDROID
		return string("sdcard/");
	#else
		try{
            return of::filesystem::canonical(ofFilePath::join(ofFilePath::getCurrentExeDir(),  "data/")).make_preferred().string();
        }catch(...){
			return (ofFilePath::getCurrentExeDir() / of::filesystem::path("data/")).string();
		}
	#endif
	}

	//--------------------------------------------------
	of::filesystem::path & defaultWorkingDirectory(){
		static auto * defaultWorkingDirectory = new of::filesystem::path(ofFilePath::getCurrentExeDir());
		return * defaultWorkingDirectory;
	}

	//--------------------------------------------------
	of::filesystem::path & dataPathRoot(){
		static auto * dataPathRoot = new of::filesystem::path(defaultDataPath());
		return *dataPathRoot;
	}
}

namespace of{
	namespace priv{
		void initfileutils(){
			defaultWorkingDirectory() = of::filesystem::absolute(of::filesystem::current_path());
		}
	}
}


//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofBuffer
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
ofBuffer::ofBuffer()
:currentLine(end(),end()){
}

//--------------------------------------------------
ofBuffer::ofBuffer(const char * buffer, std::size_t size)
:buffer(buffer,buffer+size)
,currentLine(end(),end()){
}

//--------------------------------------------------
ofBuffer::ofBuffer(istream & stream, std::size_t ioBlockSize)
:currentLine(end(),end()){
	set(stream, ioBlockSize);
}

//--------------------------------------------------
bool ofBuffer::set(istream & stream, std::size_t ioBlockSize){
	if(stream.bad()){
		clear();
		return false;
	}else{
		buffer.clear();
	}

	vector<char> aux_buffer(ioBlockSize);
	while(stream.good()){
		stream.read(&aux_buffer[0], ioBlockSize);
		append(aux_buffer.data(), stream.gcount());
	}
	return true;
}

//--------------------------------------------------
void ofBuffer::setall(char mem){
	buffer.assign(buffer.size(), mem);
}

//--------------------------------------------------
bool ofBuffer::writeTo(ostream & stream) const {
	if(stream.bad()){
		return false;
	}
	stream.write(buffer.data(), buffer.size());
	return stream.good();
}

//--------------------------------------------------
void ofBuffer::set(const char * buffer, std::size_t size){
	this->buffer.assign(buffer, buffer+size);
}

//--------------------------------------------------
void ofBuffer::set(const std::string & text){
	set(text.c_str(), text.size());
}

//--------------------------------------------------
void ofBuffer::append(const std::string& buffer){
	append(buffer.c_str(), buffer.size());
}

//--------------------------------------------------
void ofBuffer::append(const char * buffer, std::size_t size){
	this->buffer.insert(this->buffer.end(), buffer, buffer + size);
}

//--------------------------------------------------
void ofBuffer::reserve(std::size_t size){
	buffer.reserve(size);
}

//--------------------------------------------------
void ofBuffer::clear(){
	buffer.clear();
}

//--------------------------------------------------
void ofBuffer::allocate(std::size_t size){
	resize(size);
}

//--------------------------------------------------
void ofBuffer::resize(std::size_t size){
	buffer.resize(size);
}


//--------------------------------------------------
char * ofBuffer::getData(){
	return buffer.data();
}

//--------------------------------------------------
const char * ofBuffer::getData() const{
	return buffer.data();
}

//--------------------------------------------------
char * ofBuffer::getBinaryBuffer(){
	return getData();
}

//--------------------------------------------------
const char * ofBuffer::getBinaryBuffer() const {
	return getData();
}

//--------------------------------------------------
string ofBuffer::getText() const {
	if(buffer.empty()){
		return "";
	}
	return std::string(buffer.begin(), buffer.end());
}

//--------------------------------------------------
ofBuffer::operator std::string() const {
	return getText();
}

//--------------------------------------------------
ofBuffer & ofBuffer::operator=(const std::string & text){
	set(text);
	return *this;
}

//--------------------------------------------------
std::size_t ofBuffer::size() const {
	return buffer.size();
}

//--------------------------------------------------
string ofBuffer::getNextLine(){
	if(currentLine.empty()){
		currentLine = getLines().begin();
	}else{
		++currentLine;
	}
	return currentLine.asString();
}

//--------------------------------------------------
string ofBuffer::getFirstLine(){
	currentLine = getLines().begin();
	return currentLine.asString();
}

//--------------------------------------------------
bool ofBuffer::isLastLine(){
	return currentLine == getLines().end();
}

//--------------------------------------------------
void ofBuffer::resetLineReader(){
	currentLine = getLines().begin();
}

//--------------------------------------------------
vector<char>::iterator ofBuffer::begin(){
	return buffer.begin();
}

//--------------------------------------------------
vector<char>::iterator ofBuffer::end(){
	return buffer.end();
}

//--------------------------------------------------
vector<char>::const_iterator ofBuffer::begin() const{
	return buffer.begin();
}

//--------------------------------------------------
vector<char>::const_iterator ofBuffer::end() const{
	return buffer.end();
}

//--------------------------------------------------
vector<char>::reverse_iterator ofBuffer::rbegin(){
	return buffer.rbegin();
}

//--------------------------------------------------
vector<char>::reverse_iterator ofBuffer::rend(){
	return buffer.rend();
}

//--------------------------------------------------
vector<char>::const_reverse_iterator ofBuffer::rbegin() const{
	return buffer.rbegin();
}

//--------------------------------------------------
vector<char>::const_reverse_iterator ofBuffer::rend() const{
	return buffer.rend();
}

//--------------------------------------------------
ofBuffer::Line::Line(vector<char>::iterator _begin, vector<char>::iterator _end)
	:_current(_begin)
	,_begin(_begin)
	,_end(_end){

	if(_begin == _end){
		line =  "";
		return;
	}

	_current = std::find(_begin, _end, '\n');
	if(_current - 1 >= _begin && *(_current - 1) == '\r'){
		line = string(_begin, _current - 1);
	}else{
		line = string(_begin, _current);
	}
	if(_current != _end){
		_current+=1;
	}
}

//--------------------------------------------------
const std::string & ofBuffer::Line::operator*() const{
	return line;
}

//--------------------------------------------------
const std::string * ofBuffer::Line::operator->() const{
	return &line;
}

//--------------------------------------------------
const std::string & ofBuffer::Line::asString() const{
	return line;
}

//--------------------------------------------------
ofBuffer::Line & ofBuffer::Line::operator++(){
	*this = Line(_current,_end);
	return *this;
}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Line::operator++(int) {
	Line tmp(*this);
	operator++();
	return tmp;
}

//--------------------------------------------------
bool ofBuffer::Line::operator!=(Line const& rhs) const{
	return rhs._begin != _begin || rhs._end != _end;
}

//--------------------------------------------------
bool ofBuffer::Line::operator==(Line const& rhs) const{
	return rhs._begin == _begin && rhs._end == _end;
}

bool ofBuffer::Line::empty() const{
	return _begin == _end;
}



//--------------------------------------------------
ofBuffer::RLine::RLine(vector<char>::reverse_iterator _rbegin, vector<char>::reverse_iterator _rend)
	:_current(_rbegin)
	,_rbegin(_rbegin)
	,_rend(_rend){

	if(_rbegin == _rend){
		line =  "";
		return;
	}
	_current = std::find(_rbegin+1, _rend, '\n');
	line = string(_current.base(), _rbegin.base() - 1);
	if(_current < _rend-1 && *(_current + 1) == '\r'){
		_current+=1;
	}
}

//--------------------------------------------------
const std::string & ofBuffer::RLine::operator*() const{
	return line;
}

//--------------------------------------------------
const std::string * ofBuffer::RLine::operator->() const{
	return &line;
}

//--------------------------------------------------
const std::string & ofBuffer::RLine::asString() const{
	return line;
}

//--------------------------------------------------
ofBuffer::RLine & ofBuffer::RLine::operator++(){
	*this = RLine(_current,_rend);
	return *this;
}

//--------------------------------------------------
ofBuffer::RLine ofBuffer::RLine::operator++(int) {
	RLine tmp(*this);
	operator++();
	return tmp;
}

//--------------------------------------------------
bool ofBuffer::RLine::operator!=(RLine const& rhs) const{
	return rhs._rbegin != _rbegin || rhs._rend != _rend;
}

//--------------------------------------------------
bool ofBuffer::RLine::operator==(RLine const& rhs) const{
	return rhs._rbegin == _rbegin && rhs._rend == _rend;
}

bool ofBuffer::RLine::empty() const{
	return _rbegin == _rend;
}

//--------------------------------------------------
ofBuffer::Lines::Lines(vector<char>::iterator begin, vector<char>::iterator end)
:_begin(begin)
,_end(end){}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Lines::begin(){
	return Line(_begin,_end);
}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Lines::end(){
	return Line(_end,_end);
}


//--------------------------------------------------
ofBuffer::RLines::RLines(vector<char>::reverse_iterator rbegin, vector<char>::reverse_iterator rend)
:_rbegin(rbegin)
,_rend(rend){}

//--------------------------------------------------
ofBuffer::RLine ofBuffer::RLines::begin(){
	return RLine(_rbegin,_rend);
}

//--------------------------------------------------
ofBuffer::RLine ofBuffer::RLines::end(){
	return RLine(_rend,_rend);
}

//--------------------------------------------------
ofBuffer::Lines ofBuffer::getLines(){
	return ofBuffer::Lines(begin(), end());
}

//--------------------------------------------------
ofBuffer::RLines ofBuffer::getReverseLines(){
	return ofBuffer::RLines(rbegin(), rend());
}

//--------------------------------------------------
ostream & operator<<(ostream & ostr, const ofBuffer & buf){
	buf.writeTo(ostr);
	return ostr;
}

//--------------------------------------------------
istream & operator>>(istream & istr, ofBuffer & buf){
	buf.set(istr);
	return istr;
}

//--------------------------------------------------
ofBuffer ofBufferFromFile(const of::filesystem::path & path, bool binary){
	ofFile f(path,ofFile::ReadOnly, binary);
	return ofBuffer(f);
}

//--------------------------------------------------
bool ofBufferToFile(const of::filesystem::path & path, const ofBuffer& buffer, bool binary){
	ofFile f(path, ofFile::WriteOnly, binary);
	return buffer.writeTo(f);
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofFile
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
ofFile::ofFile()
:mode(Reference)
,binary(true){
}

ofFile::ofFile(const of::filesystem::path & path, Mode mode, bool binary)
:mode(mode)
,binary(true){
	open(path, mode, binary);
}

//-------------------------------------------------------------------------------------------------------------
ofFile::~ofFile(){
	//close();
}

//-------------------------------------------------------------------------------------------------------------
ofFile::ofFile(const ofFile & mom)
:basic_ios()
,fstream()
,mode(Reference)
,binary(true){
	copyFrom(mom);
}

//-------------------------------------------------------------------------------------------------------------
ofFile & ofFile::operator=(const ofFile & mom){
	copyFrom(mom);
	return *this;
}

//-------------------------------------------------------------------------------------------------------------
void ofFile::copyFrom(const ofFile & mom){
	if(&mom != this){
		Mode new_mode = mom.mode;
		if(new_mode != Reference && new_mode != ReadOnly){
			new_mode = ReadOnly;
			ofLogWarning("ofFile") << "copyFrom(): copying a writable file, opening new copy as read only";
		}
		open(mom.myFile.string(), new_mode, mom.binary);
	}
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::openStream(Mode _mode, bool _binary){
	mode = _mode;
	binary = _binary;
	ios_base::openmode binary_mode = binary ? ios::binary : (ios_base::openmode)0;
	switch(_mode) {
		case WriteOnly:
		case ReadWrite:
		case Append:
			if(!ofDirectory(ofFilePath::getEnclosingDirectory(path())).exists()){
				ofFilePath::createEnclosingDirectory(path());
			}
			break;
		case Reference:
		case ReadOnly:
			break;
	}
	switch(_mode){
		case Reference:
			return true;
			break;

		case ReadOnly:
			if(exists() && isFile()){
				fstream::open(path().c_str(), ios::in | binary_mode);
			}
			break;

		case WriteOnly:
			fstream::open(path().c_str(), ios::out | binary_mode);
			break;

		case ReadWrite:
			fstream::open(path().c_str(), ios_base::in | ios_base::out | binary_mode);
			break;

		case Append:
			fstream::open(path().c_str(), ios::out | ios::app | binary_mode);
			break;
	}
	return fstream::good();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::open(const of::filesystem::path & _path, Mode _mode, bool binary){
	close();
	myFile = ofToDataPath(_path);
	return openStream(_mode, binary);
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::openFromCWD(const of::filesystem::path & _path, Mode _mode, bool binary){
	close();
	myFile = _path;
	return openStream(_mode, binary);
}

//-------------------------------------------------------------------------------------------------------------
bool ofFile::changeMode(Mode _mode, bool binary){
	if(_mode != mode){
		auto _path = path();
		close();
		myFile = _path;
		return openStream(_mode, binary);
	}
	else{
		return true;
	}
}

//-------------------------------------------------------------------------------------------------------------
bool ofFile::isWriteMode(){
	return mode != ReadOnly;
}

//-------------------------------------------------------------------------------------------------------------
void ofFile::close(){
	myFile = of::filesystem::path();
	if(mode!=Reference) fstream::close();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::create(){
	return create(path());
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::create(const of::filesystem::path & path){
	bool success = false;

	auto oldmode = this->mode;
	auto oldpath = this->path();
	success = open(path,ofFile::WriteOnly,binary);
	close();

	if( !oldpath.empty() ){
		open(oldpath,oldmode,binary);
	}

	return success;
}

//------------------------------------------------------------------------------------------------------------
ofBuffer ofFile::readToBuffer(){
	if(myFile.string().empty() || !of::filesystem::exists(myFile)){
		return ofBuffer();
	}

	return ofBuffer(*this);
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::writeFromBuffer(const ofBuffer & buffer){
	if(myFile.string().empty()){
		return false;
	}
	if(!isWriteMode()){
		ofLogError("ofFile") << "writeFromBuffer(): trying to write to read only file \"" << myFile.string() << "\"";
	}
	return buffer.writeTo(*this);
}

//------------------------------------------------------------------------------------------------------------
std::filebuf *ofFile::getFileBuffer() const {
	return rdbuf();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::exists() const {
	if(path().empty()){
		return false;
	}
	return of::filesystem::exists(myFile);
}

//------------------------------------------------------------------------------------------------------------
//	MARK: - near future
//of::filesystem::path ofFile::path() const {
//return myFile;
std::string ofFile::path() const {
	return myFile.string();
}

//------------------------------------------------------------------------------------------------------------
string ofFile::getExtension() const {
	auto dotext = myFile.extension().string();
	// FIXME: probably not needed;
	if(!dotext.empty() && dotext.front()=='.'){
		return std::string(dotext.begin()+1,dotext.end());
	}else{
		return dotext;
	}
}

//------------------------------------------------------------------------------------------------------------
string ofFile::getFileName() const {
	return myFile.filename().string();
}

//------------------------------------------------------------------------------------------------------------
string ofFile::getBaseName() const {
	return myFile.stem().string();
}

//------------------------------------------------------------------------------------------------------------
//	MARK: - near future
//of::filesystem::path ofFile::getEnclosingDirectory() const {
std::string ofFile::getEnclosingDirectory() const {
	return ofFilePath::getEnclosingDirectory(path());
}

//------------------------------------------------------------------------------------------------------------
//	MARK: - near future
//of::filesystem::path ofFile::getAbsolutePath() const {
std::string ofFile::getAbsolutePath() const {
	return ofFilePath::getAbsolutePath(path());
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::canRead() const {
	
#ifdef TARGET_WIN32
	DWORD attr = GetFileAttributes(myFile.native().c_str());
	if (attr == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	return true;
#else
	struct stat info;
	stat(path().c_str(), &info);  // Error check omitted
	auto perm = of::filesystem::status(myFile).permissions();
#if OF_USING_STD_FS
	if(geteuid() == info.st_uid){
		return (perm & of::filesystem::perms::owner_read) != of::filesystem::perms::none;
	}else if (getegid() == info.st_gid){
		return (perm & of::filesystem::perms::group_read) != of::filesystem::perms::none;
	}else{
		return (perm & of::filesystem::perms::others_read) != of::filesystem::perms::none;
	}
#else
	if(geteuid() == info.st_uid){
		return perm & of::filesystem::perms::owner_read;
	}else if (getegid() == info.st_gid){
		return perm & of::filesystem::perms::group_read;
	}else{
		return perm & of::filesystem::perms::others_read;
	}
#endif
#endif
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::canWrite() const {
#ifdef TARGET_WIN32
	DWORD attr = GetFileAttributes(myFile.native().c_str());
	if (attr == INVALID_FILE_ATTRIBUTES){
		return false;
	}else{
		return (attr & FILE_ATTRIBUTE_READONLY) == 0;
	}
#else
	struct stat info;
	stat(path().c_str(), &info);  // Error check omitted
	auto perm = of::filesystem::status(myFile).permissions();
#if OF_USING_STD_FS
	if(geteuid() == info.st_uid){
		return (perm & of::filesystem::perms::owner_write) != of::filesystem::perms::none;
	}else if (getegid() == info.st_gid){
		return (perm & of::filesystem::perms::group_write) != of::filesystem::perms::none;
	}else{
		return (perm & of::filesystem::perms::others_write) != of::filesystem::perms::none;
	}
#else
	if(geteuid() == info.st_uid){
		return perm & of::filesystem::owner_write;
	}else if (getegid() == info.st_gid){
		return perm & of::filesystem::group_write;
	}else{
		return perm & of::filesystem::others_write;
	}
#endif
#endif
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::canExecute() const {
#ifdef TARGET_WIN32
	return getExtension() == "exe";
#else
	struct stat info;
	stat(path().c_str(), &info);  // Error check omitted
	auto perm = of::filesystem::status(myFile).permissions();
#if OF_USING_STD_FS
	if(geteuid() == info.st_uid){
		return (perm & of::filesystem::perms::owner_exec) != of::filesystem::perms::none;
	}else if (getegid() == info.st_gid){
		return (perm & of::filesystem::perms::group_exec) != of::filesystem::perms::none;
	}else{
		return (perm & of::filesystem::perms::others_exec) != of::filesystem::perms::none;
	}
#else
	if(geteuid() == info.st_uid){
		return perm & of::filesystem::owner_exe;
	}else if (getegid() == info.st_gid){
		return perm & of::filesystem::group_exe;
	}else{
		return perm & of::filesystem::others_exe;
	}
#endif
#endif
}
//------------------------------------------------------------------------------------------------------------
bool ofFile::isFile() const {
	return of::filesystem::is_regular_file(myFile);
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isLink() const {
	return of::filesystem::is_symlink(myFile);
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isDirectory() const {
	return of::filesystem::is_directory(myFile);
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isDevice() const {
#ifdef TARGET_WIN32
	return false;
#else
#if OF_USING_STD_FS
	return of::filesystem::is_block_file(of::filesystem::status(myFile));
#else
	return of::filesystem::status(myFile).type() == of::filesystem::block_file;
#endif
#endif
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isHidden() const {
#ifdef TARGET_WIN32
	return false;
#else
	return myFile.filename() != "." && myFile.filename() != ".." && myFile.filename().string()[0] == '.';
#endif
}

//------------------------------------------------------------------------------------------------------------
void ofFile::setWriteable(bool flag){
	try{
#if !OF_USING_STD_FS || (OF_USING_STD_FS && OF_USE_EXPERIMENTAL_FS)
		if(flag){
			of::filesystem::permissions(myFile,of::filesystem::perms::owner_write | of::filesystem::perms::add_perms);
		}else{
			of::filesystem::permissions(myFile,of::filesystem::perms::owner_write | of::filesystem::perms::remove_perms);
		}
#else
		if(flag){
			of::filesystem::permissions(myFile,
										 of::filesystem::perms::owner_write,
										 of::filesystem::perm_options::add);
		}else{
			of::filesystem::permissions(myFile,
										 of::filesystem::perms::owner_write,
										 of::filesystem::perm_options::remove);
		}
#endif
	}catch(std::exception & e){
		ofLogError() << "Couldn't set write permission on " << myFile << ": " << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
// deprecated
void ofFile::setReadOnly(bool flag){
	setWriteable(!flag);
}

//------------------------------------------------------------------------------------------------------------
void ofFile::setReadable(bool flag){
	try{
#if !OF_USING_STD_FS || (OF_USING_STD_FS && OF_USE_EXPERIMENTAL_FS)
		if(flag){
			of::filesystem::permissions(myFile,of::filesystem::perms::owner_read | of::filesystem::perms::add_perms);
		}else{
			of::filesystem::permissions(myFile,of::filesystem::perms::owner_read | of::filesystem::perms::remove_perms);
		}
#else
		if(flag){
			of::filesystem::permissions(myFile,
										 of::filesystem::perms::owner_read,
										 of::filesystem::perm_options::add);
		}else{
			of::filesystem::permissions(myFile,
										 of::filesystem::perms::owner_read,
										 of::filesystem::perm_options::remove);
		}
#endif
	}catch(std::exception & e){
		ofLogError() << "Couldn't set read permission on " << myFile << ": " << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
void ofFile::setExecutable(bool flag){
	try{
#if OF_USING_STD_FS
#   if OF_USE_EXPERIMENTAL_FS
		if(flag){
			of::filesystem::permissions(myFile, of::filesystem::perms::owner_exec | of::filesystem::perms::add_perms);
		} else{
			of::filesystem::permissions(myFile, of::filesystem::perms::owner_exec | of::filesystem::perms::remove_perms);
		}
#   else
		if(flag){
			of::filesystem::permissions(myFile,
										 of::filesystem::perms::owner_exec,
										 of::filesystem::perm_options::add);
		} else{
			of::filesystem::permissions(myFile,
										 of::filesystem::perms::owner_exec,
										 of::filesystem::perm_options::remove);
		}
#   endif
#else
		if(flag){
			of::filesystem::permissions(myFile, of::filesystem::perms::owner_exe | of::filesystem::perms::add_perms);
		} else{
			of::filesystem::permissions(myFile, of::filesystem::perms::owner_exe | of::filesystem::perms::remove_perms);
		}
#endif
	}catch(std::exception & e){
		ofLogError() << "Couldn't set executable permission on " << myFile << ": " << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::copyTo(const of::filesystem::path& _path, bool bRelativeToData, bool overwrite) const{
	auto path = _path;

	if(path.empty()){
		ofLogError("ofFile") << "copyTo(): destination path " << _path << " is empty";
		return false;
	}
	if(isDirectory()){
		ofDirectory tmp;
		//don't want to add ofToDataPath to myFile path as it was already done in ofFile::open
		tmp.openFromCWD(myFile);
		return tmp.copyTo(path,bRelativeToData,overwrite);
	}
	if(!exists()){
		ofLogError("ofFile") << "copyTo(): source file " << this->path() << " does not exist";
		return false;
	}

	//bRelativeToData is handled here for the destination path - so we pass false to static functions below
	if(bRelativeToData){
		path = ofToDataPath(path);
	}

	if(ofFile::doesFileExist(path, false)){
		if(isFile()){
			ofFile tmp;
			tmp.openFromCWD(path,ofFile::Reference);
			if(tmp.isDirectory()){
				path = path / getFileName();
			}
		}
		if(ofFile::doesFileExist(path, false)){
			if(overwrite){
				ofFile::removeFile(path, false);
			}else{
				ofLogWarning("ofFile") << "copyTo(): destination file \"" << path << "\" already exists, set bool overwrite to true if you want to overwrite it";
			}
		}
	}

	try{
		ofDirectory destDir;
		auto p = ofFilePath::getEnclosingDirectory(path,false);
		destDir.openFromCWD(p);
		if(!destDir.exists()){
			ofFilePath::createEnclosingDirectory(path, false);
		}
		of::filesystem::copy_file(myFile,path);
	}catch(std::exception & except){
		ofLogError("ofFile") <<  "copyTo(): unable to copy \"" << path << "\": " << except.what();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::moveTo(const of::filesystem::path& _path, bool bRelativeToData, bool overwrite){
	auto path = _path;

	if(path.empty()){
		ofLogError("ofFile") << "moveTo(): destination path is empty";
		return false;
	}
	if(!exists()){
		ofLogError("ofFile") << "moveTo(): source file does not exist";
		return false;
	}

	if(bRelativeToData){
		path = ofToDataPath(path);
	}
	if(ofFile::doesFileExist(path, false)){

		if(isFile()){
			ofFile tmp;
			tmp.openFromCWD(path,ofFile::Reference);
			if(tmp.isDirectory()){
				path = path / getFileName();
			}
		}
		if(ofFile::doesFileExist(path, false)){
			if(overwrite){
				ofFile::removeFile(path, false);
			}else{
				ofLogWarning("ofFile") << "copyTo(): destination file \"" << path << "\" already exists, set bool overwrite to true if you want to overwrite it";
			}
		}
	}

	try{
		auto mode = this->mode;
		if(mode != ofFile::Reference){
			changeMode(ofFile::Reference, binary);
		}
		ofDirectory destDir;
		destDir.openFromCWD(ofFilePath::getEnclosingDirectory(path,false));
		if(!destDir.exists()){
			ofFilePath::createEnclosingDirectory(path,false);
		}
		of::filesystem::rename(myFile,path);
		myFile = path;
		if(mode != ofFile::Reference){
			changeMode(mode, binary);
		}
	}
	catch(std::exception & except){
		ofLogError("ofFile") << "moveTo(): unable to move \"" << path << "\": " << except.what();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::renameTo(const of::filesystem::path& path, bool bRelativeToData, bool overwrite){
	return moveTo(path,bRelativeToData,overwrite);
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::remove(bool recursive){
	if(myFile.string().empty()){
		ofLogError("ofFile") << "remove(): file path is empty";
		return false;
	}
	if(!exists()){
		ofLogError("ofFile") << "remove(): file does not exist";
		return false;
	}

	try{
		if(mode!=Reference){
			open(path(),Reference,binary);
		}
		if(recursive){
			of::filesystem::remove_all(myFile);
		}else{
			of::filesystem::remove(myFile);
		}
	}catch(std::exception & except){
		ofLogError("ofFile") << "remove(): unable to remove \"" << myFile << "\": " << except.what();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
uint64_t ofFile::getSize() const {
	try{
		return of::filesystem::file_size(myFile);
	}catch(std::exception & except){
		ofLogError("ofFile") << "getSize(): unable to get size of \"" << myFile << "\": " << except.what();
		return 0;
	}
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator==(const ofFile & file) const {
	return getAbsolutePath() == file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator!=(const ofFile & file) const {
	return getAbsolutePath() != file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator<(const ofFile & file) const {
	return getAbsolutePath() < file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator<=(const ofFile & file) const {
	return getAbsolutePath() <= file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator>(const ofFile & file) const {
	return getAbsolutePath() > file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator>=(const ofFile & file) const {
	return getAbsolutePath() >= file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
// ofFile Static Methods
//------------------------------------------------------------------------------------------------------------

bool ofFile::copyFromTo(const of::filesystem::path& pathSrc, const of::filesystem::path& pathDst, bool bRelativeToData,  bool overwrite){
	ofFile tmp;
	if( bRelativeToData ){
		tmp.open(pathSrc,ofFile::Reference);
	}else{
		tmp.openFromCWD(pathSrc,ofFile::Reference);
	}
	return tmp.copyTo(pathDst,bRelativeToData,overwrite);
}

//be careful with slashes here - appending a slash when moving a folder will causes mad headaches
//------------------------------------------------------------------------------------------------------------
bool ofFile::moveFromTo(const of::filesystem::path& pathSrc, const of::filesystem::path& pathDst, bool bRelativeToData, bool overwrite){
	ofFile tmp;
	if( bRelativeToData ){
		tmp.open(pathSrc,ofFile::Reference);
	}else{
		tmp.openFromCWD(pathSrc,ofFile::Reference);
	}
	return tmp.moveTo(pathDst, bRelativeToData, overwrite);
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::doesFileExist(const of::filesystem::path& _fPath, bool bRelativeToData){
	ofFile tmp;
	if(bRelativeToData){
		tmp.open(_fPath,ofFile::Reference);
	}else{
		tmp.openFromCWD(_fPath,ofFile::Reference);
	}
	return !_fPath.empty() && tmp.exists();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::removeFile(const of::filesystem::path& _path, bool bRelativeToData){
	ofFile tmp;
	if(bRelativeToData){
		tmp.open(_path,ofFile::Reference);
	}else{
		tmp.openFromCWD(_path,ofFile::Reference);
	}
	return tmp.remove();
}


//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofDirectory
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
ofDirectory::ofDirectory(){
	showHidden = false;
}

//------------------------------------------------------------------------------------------------------------
ofDirectory::ofDirectory(const of::filesystem::path & path){
	showHidden = false;
	open(path);
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::open(const of::filesystem::path & path){
	originalDirectory = ofFilePath::getPathForDirectory(path.string());
	files.clear();
	myDir = of::filesystem::path(ofToDataPath(originalDirectory));
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::openFromCWD(const of::filesystem::path & path){
	originalDirectory = ofFilePath::getPathForDirectory(path.string());
	files.clear();
	myDir = of::filesystem::path(originalDirectory);
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::close(){
	myDir = of::filesystem::path();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::create(bool recursive){

	if(!myDir.string().empty()){
		try{
			if(recursive){
				of::filesystem::create_directories(myDir);
			}else{
				of::filesystem::create_directory(myDir);
			}
		}
		catch(std::exception & except){
			ofLogError("ofDirectory") << "create(): " << except.what();
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::exists() const {
	return (myDir == "" || of::filesystem::exists(myDir));
}

//------------------------------------------------------------------------------------------------------------
//	MARK: - near future
//of::filesystem::path ofDirectory::path() const {
//	return myDir;
std::string ofDirectory::path() const {
	return myDir.string();
}

//------------------------------------------------------------------------------------------------------------
//	MARK: - near future
//of::filesystem::path ofDirectory::getAbsolutePath() const {
//	try{
//		return of::filesystem::canonical(of::filesystem::absolute(myDir));
//	}catch(...){
//		return of::filesystem::absolute(myDir);
//	}
//}
std::string ofDirectory::getAbsolutePath() const {
	try{
		return of::filesystem::canonical(of::filesystem::absolute(myDir)).string();
	}catch(...){
		return of::filesystem::absolute(myDir).string();
	}
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::canRead() const {
	return ofFile(myDir,ofFile::Reference).canRead();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::canWrite() const {
	return ofFile(myDir,ofFile::Reference).canWrite();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::canExecute() const {
	return ofFile(myDir,ofFile::Reference).canExecute();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::isHidden() const {
	return ofFile(myDir,ofFile::Reference).isHidden();
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::setWriteable(bool flag){
	return ofFile(myDir,ofFile::Reference).setWriteable(flag);
}

//------------------------------------------------------------------------------------------------------------
// deprecated
void ofDirectory::setReadOnly(bool flag){
	setWriteable(!flag);
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::setReadable(bool flag){
	return ofFile(myDir,ofFile::Reference).setReadable(flag);
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::setExecutable(bool flag){
	return ofFile(myDir,ofFile::Reference).setExecutable(flag);
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::setShowHidden(bool showHidden){
	this->showHidden = showHidden;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::isDirectory() const {
	return of::filesystem::is_directory(myDir);
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::copyTo(const of::filesystem::path& _path, bool bRelativeToData, bool overwrite){
	auto path = _path;

	if(myDir.string().empty()){
		ofLogError("ofDirectory") << "copyTo(): source path is empty";
		return false;
	}
	if(!of::filesystem::exists(myDir)){
		ofLogError("ofDirectory") << "copyTo(): source directory does not exist";
		return false;
	}
	if(!of::filesystem::is_directory(myDir)){
		ofLogError("ofDirectory") << "copyTo(): source path is not a directory";
		return false;
	}

	if(bRelativeToData){
		path = ofToDataPath(path, bRelativeToData);
	}

	if(ofDirectory::doesDirectoryExist(path, false)){
		if(overwrite){
			ofDirectory::removeDirectory(path, true, false);
		}else{
			ofLogWarning("ofDirectory") << "copyTo(): dest \"" << path << "\" already exists, set bool overwrite to true to overwrite it";
			return false;
		}
	}

	//our path is bRelativeToData handled from above - so can't open via the constructor approach
	ofDirectory dir;
	dir.openFromCWD(path);
	dir.create(true);

	// Iterate through the source directory
	for(of::filesystem::directory_iterator file(myDir); file != of::filesystem::directory_iterator(); ++file){
		auto currentPath = of::filesystem::absolute(file->path());
		auto dst = of::filesystem::path(path) / currentPath.filename();
		if(of::filesystem::is_directory(currentPath)){
			ofDirectory current(currentPath);
			// Found directory: Recursion
			if(!current.copyTo(dst,false,overwrite)){
				return false;
			}
		}else{
			ofFile tmp;
			tmp.openFromCWD(file->path(),ofFile::Reference);
			tmp.copyTo(dst.string(),false,overwrite);
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::moveTo(const of::filesystem::path& path, bool bRelativeToData, bool overwrite){
	if(copyTo(path,bRelativeToData,overwrite)){
		return remove(true);
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::renameTo(const of::filesystem::path& path, bool bRelativeToData, bool overwrite){
	return moveTo(path, bRelativeToData, overwrite);
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::remove(bool recursive){
	if(path().empty() || !of::filesystem::exists(myDir)){
		return false;
	}

	try{
		if(recursive){
			of::filesystem::remove_all(of::filesystem::canonical(myDir));
		}else{
			of::filesystem::remove(of::filesystem::canonical(myDir));
		}
	}catch(std::exception & except){
		ofLogError("ofDirectory") << "remove(): unable to remove file/directory: " << except.what();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::allowExt(const std::string& extension){
	if(extension == "*"){
		ofLogWarning("ofDirectory") << "allowExt(): wildcard extension * is deprecated";
	}
	extensions.push_back(ofToLower(extension));
}

//------------------------------------------------------------------------------------------------------------
std::size_t ofDirectory::listDir(const of::filesystem::path & directory){
	open(directory);
	return listDir();
}

//------------------------------------------------------------------------------------------------------------
std::size_t ofDirectory::listDir(){
	files.clear();
	if(path().empty()){
		ofLogError("ofDirectory") << "listDir(): directory path is empty";
		return 0;
	}
	if(!of::filesystem::exists(myDir)){
		ofLogError("ofDirectory") << "listDir:() source directory does not exist: \"" << myDir << "\"";
		return 0;
	}

	of::filesystem::directory_iterator end_iter;
	if ( of::filesystem::exists(myDir) && of::filesystem::is_directory(myDir)){
		for( of::filesystem::directory_iterator dir_iter(myDir) ; dir_iter != end_iter ; ++dir_iter){
			files.emplace_back(dir_iter->path().string(), ofFile::Reference);
		}
	}else{
		ofLogError("ofDirectory") << "listDir:() source directory does not exist: \"" << myDir << "\"";
		return 0;
	}

	if(!showHidden){
		ofRemove(files, [](ofFile & file){
			return file.isHidden();
		});
	}


	if(!extensions.empty() && !ofContains(extensions, (string)"*")){
		ofRemove(files, [&](ofFile & file){
			return std::find(extensions.begin(), extensions.end(), ofToLower(file.getExtension())) == extensions.end();
		});
	}

	if(ofGetLogLevel() == OF_LOG_VERBOSE){
		for(int i = 0; i < (int)size(); i++){
			ofLogVerbose() << "\t" << getName(i);
		}
		ofLogVerbose() << "listed " << size() << " files in \"" << originalDirectory << "\"";
	}

	return size();
}

//------------------------------------------------------------------------------------------------------------
string ofDirectory::getOriginalDirectory() const {
	return originalDirectory;
}

//------------------------------------------------------------------------------------------------------------
string ofDirectory::getName(std::size_t position) const{
	return files.at(position).getFileName();
}

//------------------------------------------------------------------------------------------------------------
string ofDirectory::getPath(std::size_t position) const{
	return originalDirectory + getName(position);
}

//------------------------------------------------------------------------------------------------------------
ofFile ofDirectory::getFile(std::size_t position, ofFile::Mode mode, bool binary) const {
	ofFile file = files[position];
	file.changeMode(mode, binary);
	return file;
}

ofFile ofDirectory::operator[](std::size_t position) const {
	return getFile(position);
}

//------------------------------------------------------------------------------------------------------------
const vector<ofFile> & ofDirectory::getFiles() const{
	if(files.empty() && !myDir.empty()){
		const_cast<ofDirectory*>(this)->listDir();
	}
	return files;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::getShowHidden() const{
	return showHidden;
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::reset(){
	close();
}

//------------------------------------------------------------------------------------------------------------
static bool natural(const ofFile& a, const ofFile& b) {
	string aname = a.getBaseName(), bname = b.getBaseName();
	int aint = ofToInt(aname), bint = ofToInt(bname);
	if(ofToString(aint) == aname && ofToString(bint) == bname) {
		return aint < bint;
	} else {
		return a < b;
	}
}


//------------------------------------------------------------------------------------------------------------
struct StringSort{
    of::filesystem::path path;
    string basename;
    int nameInt;
    string stringInt;
};

//------------------------------------------------------------------------------------------------------------
static bool naturalStr(const StringSort& a, const StringSort& b) {
    if(a.stringInt == a.basename && b.stringInt == b.basename) {
        return a.nameInt < b.nameInt;
    } else {
        return a.path < b.path;
    }
}

//------------------------------------------------------------------------------------------------------------
static bool byDate(const ofFile& a, const ofFile& b) {
	auto ta = of::filesystem::last_write_time(a);
	auto tb = of::filesystem::last_write_time(b);
	return ta < tb;
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::sortByDate() {
	if (files.empty() && !myDir.empty()) {
		listDir();
	}
	ofSort(files, byDate);
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::sort(const SortMode & mode){
	if(files.empty() && !myDir.empty()){
		listDir();
	}

    if( mode == ofDirectory::SORT_NATURAL ){
        vector <StringSort> sort;
        sort.reserve(files.size());

        for( auto & f : files ){
            StringSort ss;
            ss.path = f.path();
            ss.basename = f.getBaseName();
            ss.nameInt = ofToInt(ss.basename);
            ss.stringInt = ofToString(ss.nameInt);
            sort.push_back(ss);
        }
        
        ofSort(sort, naturalStr);
        files.clear();
        files.reserve(sort.size());
        for( auto & s : sort ){
            files.emplace_back( s.path , ofFile::Reference);
        }
    }
    else if(mode == ofDirectory::SORT_FAST){
        std::vector <string> sort;
        sort.reserve(files.size());
        
        for( auto & f : files ){
            string ss = f.getFileName();
            sort.push_back(ss);
        }

        std::sort(sort.begin(), sort.end());
        files.clear();
        files.reserve(sort.size());
        for( auto & s : sort ){
            files.emplace_back( myDir / of::filesystem::path(s), ofFile::Reference);
        }
    }else if(mode == ofDirectory::SORT_BY_DATE){
        sortByDate();
    }
}

//------------------------------------------------------------------------------------------------------------
ofDirectory ofDirectory::getSorted(){
	ofDirectory sorted(*this);
	sorted.listDir();
	sorted.sort();
	return sorted;
}

//------------------------------------------------------------------------------------------------------------
std::size_t ofDirectory::size() const{
	return files.size();
}

//------------------------------------------------------------------------------------------------------------
int ofDirectory::numFiles(){
	return static_cast<int>(size());
}

//------------------------------------------------------------------------------------------------------------
// ofDirectory Static Methods
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::removeDirectory(const of::filesystem::path& _path, bool deleteIfNotEmpty, bool bRelativeToData){
	auto path = _path;

	ofFile dirToRemove;
	if(bRelativeToData){
		dirToRemove.open(path,ofFile::Reference);
	}else{
		dirToRemove.openFromCWD(path,ofFile::Reference);
	}

	return dirToRemove.remove(deleteIfNotEmpty);
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::createDirectory(const of::filesystem::path& _dirPath, bool bRelativeToData, bool recursive){
	auto dirPath = _dirPath;

	if(bRelativeToData){
		dirPath = ofToDataPath(dirPath);
	}

	// on OSX,of::filesystem::create_directories seems to return false *if* the path has folders that already exist
	// and true if it doesn't
	// so to avoid unnecessary warnings on OSX, we check if it exists here:

	bool bDoesExistAlready = ofDirectory::doesDirectoryExist(dirPath,false);

	if (!bDoesExistAlready){

		bool success = false;
		try{
			if(!recursive){
				success = of::filesystem::create_directory(dirPath);
			}else{
				success = of::filesystem::create_directories(dirPath);
			}
		} catch(std::exception & except){
			ofLogError("ofDirectory") << "createDirectory(): couldn't create directory \"" << dirPath << "\": " << except.what();
			return false;
		}
		return success;
	}

	// no need to create it - it already exists.
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::doesDirectoryExist(const of::filesystem::path& _dirPath, bool bRelativeToData){
	auto dirPath = _dirPath;
	try {
		if (bRelativeToData) {
			dirPath = ofToDataPath(dirPath);
		}
		return of::filesystem::exists(dirPath) && of::filesystem::is_directory(dirPath);
	}
	catch (std::exception & except) {
		ofLogError("ofDirectory") << "doesDirectoryExist(): couldn't find directory \"" << dirPath << "\": " << except.what() << std::endl;
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::isDirectoryEmpty(const of::filesystem::path& _dirPath, bool bRelativeToData){
	auto dirPath = _dirPath;
	if(bRelativeToData){
		dirPath = ofToDataPath(dirPath);
	}

	if(!dirPath.empty() && of::filesystem::exists(dirPath) && of::filesystem::is_directory(dirPath)){
		return of::filesystem::directory_iterator(dirPath) == of::filesystem::directory_iterator();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator==(const ofDirectory & dir) const{
	return getAbsolutePath() == dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator!=(const ofDirectory & dir) const{
	return getAbsolutePath() != dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator<(const ofDirectory & dir) const{
	return getAbsolutePath() < dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator<=(const ofDirectory & dir) const{
	return getAbsolutePath() <= dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator>(const ofDirectory & dir) const{
	return getAbsolutePath() > dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator>=(const ofDirectory & dir) const{
	return getAbsolutePath() >= dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::const_iterator ofDirectory::begin() const{
	return getFiles().begin();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::const_iterator ofDirectory::end() const{
	return files.end();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::const_reverse_iterator ofDirectory::rbegin() const{
	return getFiles().rbegin();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::const_reverse_iterator ofDirectory::rend() const{
	return files.rend();
}


//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofFilePath
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------
string ofFilePath::addLeadingSlash(const of::filesystem::path& _path){
	auto path = _path.string();
	auto sep = of::filesystem::path("/").make_preferred();
	if(!path.empty()){
		if(ofToString(path[0]) != sep.string()){
			path = (sep / path).string();
		}
	}
	return path;
}

//------------------------------------------------------------------------------------------------------------
//	MARK: - near future
//of::filesystem::path ofFilePath::addTrailingSlash(const of::filesystem::path & _path){
std::string ofFilePath::addTrailingSlash(const of::filesystem::path & _path){
#if OF_USING_STD_FS && !OF_USE_EXPERIMENTAL_FS
	if(_path.string().empty()) return "";
	// FIXME: Remove .string() here and following
	// return (of::filesystem::path(_path).make_preferred() / "");
	return (of::filesystem::path(_path).make_preferred() / "").string();
#else
	auto path = of::filesystem::path(_path).make_preferred();
	auto sep = of::filesystem::path("/").make_preferred();
	if(!path.empty()){
		if(ofToString(path.string().back()) != sep.string()){
			path = (path / sep);
		}
	}
//	return path;
	return path.string();
#endif
}


//------------------------------------------------------------------------------------------------------------
string ofFilePath::getFileExt(const of::filesystem::path& filename){
	return ofFile(filename,ofFile::Reference).getExtension();
}

//------------------------------------------------------------------------------------------------------------
// FIXME: remove const and copy
// MARK: - near future
// of::filesystem::path ofFilePath::removeExt(const of::filesystem::path& _filename){
std::string ofFilePath::removeExt(const of::filesystem::path& _filename){
	auto filename = _filename;
//	return filename.replace_extension();
	return filename.replace_extension().string();
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::getPathForDirectory(const of::filesystem::path& path){
	// if a trailing slash is missing from a path, this will clean it up
	// if it's a windows-style "\" path it will add a "\"
	// if it's a unix-style "/" path it will add a "/"

	// FIXME: Remove .string() here and following
	// FIXME: this seems over complicated and not useful anymore, using filesystem

#if OF_USING_STD_FS && !OF_USE_EXPERIMENTAL_FS
	if(path.string().empty()) return "";
	return (path / "").string();
#else
	auto sep = of::filesystem::path("/").make_preferred();
	if(!path.empty() && ofToString(path.string().back())!=sep.string()){
		return (path / sep).string();
	}else{
		return path.string();
	}
#endif
}

//------------------------------------------------------------------------------------------------------------
// FIXME: convert to of::filesystem::path
string ofFilePath::removeTrailingSlash(const of::filesystem::path& _path){
	auto path = _path.string();
	if(path.length() > 0 && (path[path.length() - 1] == '/' || path[path.length() - 1] == '\\')){
		path = path.substr(0, path.length() - 1);
	}
	return path;
}


//------------------------------------------------------------------------------------------------------------
// FIXME: is this still useful? if yes convert to of::filesystem::path
string ofFilePath::getFileName(const of::filesystem::path& _filePath, bool bRelativeToData){
	auto filePath = _filePath;

	if(bRelativeToData){
		filePath = ofToDataPath(filePath);
	}

	// FIXME: this is probably over complicated
	return of::filesystem::path(filePath).filename().string();
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::getBaseName(const of::filesystem::path& filePath){
	// FIXME: is this still useful?
	return ofFile(filePath,ofFile::Reference).getBaseName();
}

//------------------------------------------------------------------------------------------------------------
//	MARK: - near future
//of::filesystem::path ofFilePath::getEnclosingDirectory(const of::filesystem::path & _filePath, bool bRelativeToData){
std::string ofFilePath::getEnclosingDirectory(const of::filesystem::path & _filePath, bool bRelativeToData){
	auto fp = _filePath;
	if(bRelativeToData){
		fp = ofToDataPath(fp);
	}
	return addTrailingSlash(fp.parent_path());
}

//------------------------------------------------------------------------------------------------------------
bool ofFilePath::createEnclosingDirectory(const of::filesystem::path& filePath, bool bRelativeToData, bool bRecursive) {
	return ofDirectory::createDirectory(ofFilePath::getEnclosingDirectory(filePath,bRelativeToData), bRelativeToData, bRecursive);
}

//------------------------------------------------------------------------------------------------------------
// FIXME: - near future
//of::filesystem::path ofFilePath::getAbsolutePath(const of::filesystem::path& path, bool bRelativeToData){
std::string ofFilePath::getAbsolutePath(const of::filesystem::path& path, bool bRelativeToData){
	if(bRelativeToData){
		return ofToDataPath(path, true);
	}else{
		try{
//			return of::filesystem::canonical(of::filesystem::absolute(path));
			return of::filesystem::canonical(of::filesystem::absolute(path)).string();
		}catch(...){
//			return of::filesystem::absolute(path);
			return of::filesystem::absolute(path).string();
		}
	}
}

//------------------------------------------------------------------------------------------------------------
bool ofFilePath::isAbsolute(const of::filesystem::path& path){
	return of::filesystem::path(path).is_absolute();
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::getCurrentWorkingDirectory(){
	return of::filesystem::current_path().string();
}

//------------------------------------------------------------------------------------------------------------
// MARK: - near future
//of::filesystem::path ofFilePath::join(const of::filesystem::path& path1, const of::filesystem::path& path2){
//	// FIXME: deprecate when possible. helper function more complex than actual solution
//	return (path1 / path2);
//}
std::string ofFilePath::join(const of::filesystem::path& path1, const of::filesystem::path& path2){
	// FIXME: deprecate when possible. helper function more complex than actual solution
	return (path1 / path2).string();
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::getCurrentExePath(){
	#if defined(TARGET_LINUX) || defined(TARGET_ANDROID)
		char buff[FILENAME_MAX];
		ssize_t size = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
		if (size == -1){
			ofLogError("ofFilePath") << "getCurrentExePath(): readlink failed with error " << errno;
		}
		else{
			buff[size] = '\0';
			return buff;
		}
	#elif defined(TARGET_OSX)
		char path[FILENAME_MAX];
		uint32_t size = sizeof(path);
		if(_NSGetExecutablePath(path, &size) != 0){
			ofLogError("ofFilePath") << "getCurrentExePath(): path buffer too small, need size " <<  size;
		}
		return path;
	#elif defined(TARGET_WIN32)
		vector<char> executablePath(MAX_PATH);
		DWORD result = ::GetModuleFileNameA(nullptr, &executablePath[0], static_cast<DWORD>(executablePath.size()));
		if(result == 0) {
			ofLogError("ofFilePath") << "getCurrentExePath(): couldn't get path, GetModuleFileNameA failed";
		}else{
			return string(executablePath.begin(), executablePath.begin() + result);
		}
	#endif
	return "";
}

//------------------------------------------------------------------------------------------------------------
// MARK: - near future
//of::filesystem::path ofFilePath::getCurrentExeDir(){
std::string ofFilePath::getCurrentExeDir(){
	return ofFilePath::getEnclosingDirectory(ofFilePath::getCurrentExePath(), false);
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::getUserHomeDir(){
	#ifdef TARGET_WIN32
		// getenv will return any Environent Variable on Windows
		// USERPROFILE is the key on Windows 7 but it might be HOME
		// in other flavours of windows...need to check XP and NT...
		return ofGetEnv("USERPROFILE");
	#elif !defined(TARGET_EMSCRIPTEN)
		struct passwd * pw = getpwuid(getuid());
		return pw->pw_dir;
	#else
		return "";
	#endif
}

// MARK: - near future
//of::filesystem::path ofFilePath::makeRelative(const of::filesystem::path & from, const of::filesystem::path & to){
std::string ofFilePath::makeRelative(const of::filesystem::path & from, const of::filesystem::path & to){
	auto pathFrom = of::filesystem::absolute( from );
	auto pathTo = of::filesystem::absolute( to );
	of::filesystem::path ret;
	of::filesystem::path::const_iterator itrFrom( pathFrom.begin() ), itrTo( pathTo.begin() );
	// Find common base
	for( of::filesystem::path::const_iterator toEnd( pathTo.end() ), fromEnd( pathFrom.end() ) ; itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo );
	// Navigate backwards in directory to reach previously found base
	for( of::filesystem::path::const_iterator fromEnd( pathFrom.end() ); itrFrom != fromEnd; ++itrFrom ){
		if( (*itrFrom) != "." ){
			ret /= "..";
		}
	}
	// Now navigate down the directory branch
	for( ; itrTo != pathTo.end() ; ++itrTo ){
		if( itrTo->string() != "."){
			ret /= *itrTo;
		}
	}

//	return ret;
	return ret.string();
}

//--------------------------------------------------
void ofEnableDataPath(){
	enableDataPath = true;
}

//--------------------------------------------------
void ofDisableDataPath(){
	enableDataPath = false;
}

//--------------------------------------------------
bool ofRestoreWorkingDirectoryToDefault(){
	try{
		of::filesystem::current_path(defaultWorkingDirectory());
		return true;
	}catch(...){
		return false;
	}
}

//--------------------------------------------------
void ofSetDataPathRoot(const of::filesystem::path& newRoot){
	dataPathRoot() = newRoot;
}

//--------------------------------------------------
// MARK: - near future
//of::filesystem::path ofToDataPath(const of::filesystem::path & path, bool makeAbsolute){
std::string ofToDataPath(const of::filesystem::path & path, bool makeAbsolute){
	if (makeAbsolute && path.is_absolute()) {
//		return path;
		return path.string();
	}

	if (!enableDataPath) {
//		return path;
		return path.string();
	}

	bool hasTrailingSlash = !path.empty() && path.generic_string().back()=='/';

	// if our Current Working Directory has changed (e.g. file open dialog)
#ifdef TARGET_WIN32
	if (defaultWorkingDirectory() != of::filesystem::current_path()) {
		// change our cwd back to where it was on app load
		bool ret = ofRestoreWorkingDirectoryToDefault();
		if(!ret){
			ofLogWarning("ofUtils") << "ofToDataPath: error while trying to change back to default working directory " << defaultWorkingDirectory();
		}
	}
#endif

	// this could be performed here, or wherever we might think we accidentally change the cwd, e.g. after file dialogs on windows
	const auto & dataPath = dataPathRoot();
	of::filesystem::path inputPath(path);
	of::filesystem::path outputPath;

	// if path is already absolute, just return it
	if (inputPath.is_absolute()) {
		try {
			auto outpath = of::filesystem::canonical(inputPath).make_preferred();
			if(of::filesystem::is_directory(outpath) && hasTrailingSlash){
				return ofFilePath::addTrailingSlash(outpath);
			}else{
				return outpath.string();
				// return outpath;
			}
		}
		catch (...) {
			return inputPath.string();
			// return inputPath;
		}
	}

	// here we check whether path already refers to the data folder by looking for common elements
	// if the path begins with the full contents of dataPathRoot then the data path has already been added
	// we compare inputPath.toString() rather that the input var path to ensure common formatting against dataPath.toString()
	auto dirDataPath = dataPath;
	// also, we strip the trailing slash from dataPath since `path` may be input as a file formatted path even if it is a folder (i.e. missing trailing slash)
	dirDataPath = ofFilePath::addTrailingSlash(dataPath);

	auto relativeDirDataPath = ofFilePath::makeRelative(of::filesystem::current_path(), dataPath);
	relativeDirDataPath = ofFilePath::addTrailingSlash(relativeDirDataPath);

	// FIXME: this can be simplified without using string conversion
	// if (inputPath.string().find(dirDataPath.string()) != 0 && inputPath.string().find(relativeDirDataPath.string())!=0) {
	if (inputPath.string().find(dirDataPath.string()) != 0 && inputPath.string().find(relativeDirDataPath)!=0) {
		// inputPath doesn't contain data path already, so we build the output path as the inputPath relative to the dataPath
		if(makeAbsolute){
			outputPath = dirDataPath / inputPath;
		}else{
			outputPath = relativeDirDataPath / inputPath;
		}
	} else {
		// inputPath already contains data path, so no need to change
		outputPath = inputPath;
	}

	// finally, if we do want an absolute path and we don't already have one
	if(makeAbsolute){
		// then we return the absolute form of the path
		try {
			auto outpath = of::filesystem::canonical(of::filesystem::absolute(outputPath)).make_preferred();
			if(of::filesystem::is_directory(outpath) && hasTrailingSlash){
				return ofFilePath::addTrailingSlash(outpath);
			}else{
//				return outpath;
				return outpath.string();
			}
		}
		catch (std::exception &) {
		   return of::filesystem::absolute(outputPath).string();
			// return of::filesystem::absolute(outputPath);
		}
	}else{
		// or output the relative path
//		return outputPath;
		return outputPath.string();
	}
}
