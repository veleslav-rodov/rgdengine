#include "precompiled.h"

#include <rgde/io/file.h>
#include <rgde/io/serialized_object.h>

namespace io
{
	namespace helpers
	{
		//-----------------------------------------------------------------------------------
		std::string get_file_ext(const std::string &file_name)
		{
			size_t pos	= file_name.rfind(".") + 1;
			return file_name.substr(pos, file_name.size() - pos);
		}

		//-----------------------------------------------------------------------------------
		std::string get_shot_filename(const std::string &file_name)
		{
			size_t pos	= file_name.rfind(".");
			return file_name.substr(0, pos);
		}

		////-----------------------------------------------------------------------------------
		//std::string createFullFilePath(const std::string& path)
		//{
		//	std::string fullAddr;
		//	wchar_t buf[MAX_PATH];

		//	GetCurrentDirectory (MAX_PATH, buf);
		//	fullAddr.resize (MAX_PATH);
		//	fullAddr = buf;
		//	fullAddr.append (L"/");
		//	fullAddr.append (path);

		//	return fullAddr;
		//}

		////-------------------------------------------------------------------------------
		//std::string createFullFilePathA (const std::wstring& path)
		//{
		//	return wstr_to_str (createFullFilePath (path));
		//}

		//-------------------------------------------------------------------------------
		std::string wstr_to_str(const std::wstring &wstr)
		{
			return std::string(wstr.begin(), wstr.end());
		}
		//-------------------------------------------------------------------------------
		std::wstring str_to_wstr(const std::string &str)
		{
			return std::wstring(str.begin(), str.end());
		}
	}
	//////////////////////////////////////////////////////////////////////////
	read_stream & operator >>(read_stream &rf, std::string &str)
	{
		unsigned size	= 0;
		rf.read((byte *)&size, sizeof(unsigned));
		char *c_str	= new char[size];
		rf.read((byte *)c_str, size * sizeof(char));
		str = std::string(c_str);
		return rf;
	}
	//-----------------------------------------------------------------------------------
	read_stream & operator >>(read_stream &rf, serialized_object &so)
	{
		so.read(rf);
		return rf;
	}
	//-----------------------------------------------------------------------------------
	read_stream & operator >>(read_stream &rf, math::vec3f &vec)
	{
		rf >> vec[0] >> vec[1] >> vec[2];
		return rf;
	}
	//-----------------------------------------------------------------------------------
	read_stream & operator >>(read_stream &rf, math::vec4f &vec)
	{
		rf >> vec[0] >> vec[1] >> vec[2] >> vec[3];
		return rf;
	}
	//-----------------------------------------------------------------------------------
	read_stream & operator >>(read_stream &rf, math::point3f &point)
	{
		rf >> point[0] >> point[1] >> point[2];
		return rf;
	}
	//-----------------------------------------------------------------------------------
	read_stream & operator >>(read_stream &rf, math::quatf &quat)
	{
		rf >> quat[0] >> quat[1] >> quat[2] >> quat[3];
		return rf;
	}
	//-----------------------------------------------------------------------------------
	read_stream & operator >>(read_stream &rf, std::wstring &str)
	{
		unsigned size	= 0;
		rf.read((byte *)&size, sizeof(unsigned));
		wchar_t *c_str	= new wchar_t[size];
		rf.read((byte *)c_str, size * sizeof(wchar_t));
		str = std::wstring(c_str);
		return rf;
	}
	//////////////////////////////////////////////////////////////////////////
	write_stream & operator <<(write_stream &wf, const std::string &str)
	{
		unsigned size	= (unsigned)str.size();
		size++;
		wf.write((const byte *)&size, sizeof(unsigned));
		wf.write((const byte *)str.c_str(), size * sizeof(char));
		return wf;
	}
	//-----------------------------------------------------------------------------------
	write_stream & operator <<(write_stream &wf, const serialized_object &so)
	{
		so.write(wf);
		return wf;
	}
	//-----------------------------------------------------------------------------------
	write_stream & operator <<(write_stream &wf, const math::vec3f &vec)
	{
		wf << vec[0] << vec[1] << vec[2];
		return wf;
	}
	//-----------------------------------------------------------------------------------
	write_stream & operator <<(write_stream &wf, const math::vec4f &vec)
	{
		wf << vec[0] << vec[1] << vec[2] << vec[3];
		return wf;
	}
	//-----------------------------------------------------------------------------------
	write_stream & operator <<(write_stream &wf, const math::point3f &point)
	{
		wf << point[0] << point[1] << point[2];
		return wf;
	}
	//-----------------------------------------------------------------------------------
	write_stream & operator <<(write_stream &wf, const math::quatf &quat)
	{
		wf << quat[0] << quat[1] << quat[2] << quat[3];
		return wf;
	}
	//-----------------------------------------------------------------------------------
	write_stream & operator <<(write_stream &wf, const std::wstring &str)
	{
		unsigned size	= (unsigned)str.size();
		size++;
		wf.write((const byte *)&size, sizeof(unsigned));
		wf.write((const byte *)str.c_str(), size * sizeof(wchar_t));
		return wf;
	}
	//////////////////////////////////////////////////////////////////////////
	write_file::write_file()
	{
		m_isOpened = false;
		m_isValid = false;
		m_isError = false;
	}
	//-----------------------------------------------------------------------------------
	write_file::write_file(const std::string& file_name)
	{
		m_isOpened = open(file_name);
		m_isError = !m_isOpened;
		m_isValid = m_isOpened;
	}
	//-----------------------------------------------------------------------------------
	write_file::~write_file()
	{
		m_fileStream.flush();
		m_fileStream.close();
	}
	//--------------------------------------------------------------------------------------
	bool write_file::do_open_file(const std::string& fullname, const Path& path)
	{
		//std::string fullname = file_name;// + (std::string)path;
		if (m_isOpened)
			m_fileStream.close();
		if (m_isError)
			m_fileStream.clear();
		m_fileStream.open(fullname.c_str(), std::ios_base::binary);

		m_isOpened = m_fileStream.is_open();
		m_isError = !m_fileStream.good();
		m_isValid = m_isOpened;

		return m_isOpened;
	}
	//--------------------------------------------------------------------------------------
	void write_file::write(const byte *buff, unsigned size)
	{
		m_fileStream.write((const char*)buff, size);
	}
	//////////////////////////////////////////////////////////////////////////
	read_file::read_file()
		: m_size(0)
	{
		m_isOpened = false;
		m_isValid = false;
		m_isError = false;
	}
	//-----------------------------------------------------------------------------------
	read_file::read_file(const std::string& file_name)
		: m_size(0)
	{
		m_isOpened = false;
		m_isError = false;
		m_isOpened = open(file_name);
		m_isError = !m_isOpened;
		m_isValid = m_isOpened;
	}
	//-----------------------------------------------------------------------------------
	read_file::~read_file()
	{
		m_fileStream.close();
	}
	//-----------------------------------------------------------------------------------
	void read_file::read(byte *buff, unsigned size)
	{
		m_fileStream.read((char*)buff, size);
	}
	//-----------------------------------------------------------------------------------
	bool read_file::do_open_file(const std::string& fullname, const Path& path)
	{
		//std::string fullname= file_name;// + (std::string)path;
		if (m_isOpened)
			m_fileStream.close();
		if (m_isError)
			m_fileStream.clear();
		m_fileStream.open(fullname.c_str(), std::ios_base::binary);

		m_isOpened = m_fileStream.is_open();
		m_isError = !m_fileStream.good();
		m_isValid = m_isOpened;

		if (m_isValid)
		{
			m_fileStream.seekg(0, std::ios_base::end);
			m_size = m_fileStream.tellg();
			m_fileStream.seekg(0, std::ios_base::beg);
		}

		return m_isOpened;
	}
	//-----------------------------------------------------------------------------------
	unsigned long read_file::get_size() const
	{
		return m_size;
	}
	//-----------------------------------------------------------------------------------
	unsigned long read_file::get_pos()
	{
		return m_fileStream.tellg();;
	}
	//-----------------------------------------------------------------------------------
	void read_file::set_pos(unsigned long pos)
	{
		m_fileStream.seekg(pos, std::ios_base::beg);
	}
	//-----------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------
	bool base_file::open(const std::string& name, Path path)
	{
		return do_open_file(name, path);
	}
}